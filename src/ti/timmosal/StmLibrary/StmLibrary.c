/*
 *  Copyright 2008,2009 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  U.S. Patent Nos. 5,283,900  5,392,448
 */
/***************************************************************************/
/*                                                                         */
/*     StmLibrary.C                                                        */
/*                                                                         */
/*     This STM Library provides user the capability of generating         */
/*     Software messages to perform system level debug                     */
/*                                                                         */
/***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "StmLibrary.h"
#include "StmSupport.h"

extern unsigned long STM_BaseAddress;
extern unsigned long STM_ChannelResolution;
extern unsigned long DMA4_BaseAddress;

static bool STMisOpen = false;
#ifdef _CIO

static char * STM_FrmtStringTop; 
static char * STM_FrmtStringHdr;
static char * STM_FrmtStringMsg1; 
static char * STM_FrmtStringMsg;

static char * STM_FrmtStringTop_STDIO = "Master ID  Chan Num  Status                  Message/Data\n";
static char * STM_FrmtStringHdr_STDIO = "%-10.10s   %-3.3d     %-22.22s  ";
static char * STM_FrmtStringMsg1_STDIO = "0x%8.8X\n";
static char * STM_FrmtStringMsg_STDIO = "                                             0x%8.8X\n";

static char * STM_FrmtStringTop_FILEIO = "Master ID,Chan Num,Status,Message,Data\n";
static char * STM_FrmtStringHdr_FILEIO = "%s,%d,%s,";
static char * STM_FrmtStringMsg1_FILEIO = ",0x%8.8X\n";
static char * STM_FrmtStringMsg_FILEIO = ",,,,0x%8.8X\n";
#endif

#if defined(_COMPACT) || defined(_CIO)
// Compact and CIO builds uses a pre-allocated handle object to avoid malloc 
STMHandle STMHandleObj;
#endif

eSTM_STATUS STMXport_getVersion(STMHandle* pSTMHandle, unsigned int * pSTMLibMajorVersion, unsigned int * pSTMLibMinorVersion)
{
#ifdef _DEBUG
	VALIDATE_NULL_VALUE(pSTMHandle);
	VALIDATE_NULL_VALUE(pSTMLibMajorVersion);
	VALIDATE_NULL_VALUE(pSTMLibMinorVersion);	 
#endif

	*pSTMLibMajorVersion = STMLIB_MAJOR_VERSION;
	*pSTMLibMinorVersion = STMLIB_MINOR_VERSION;	

	return eSTM_SUCCESS;
}

STMHandle* STMXport_open(STMBufObj * pSTMBufObj)
{
	STMHandle *pSTMHandle = NULL;

#ifdef _CIO
	if ( NULL == pSTMBufObj) {
	    //Error - CIO version requires buffer object to define filename and master id, return NULL
		return pSTMHandle;
	}
#endif
#if defined(_COMPACT) && !defined(_CIO)
	if ( NULL != pSTMBufObj) {
	    //Error - Comapct versions do not allow opening for Buffered IO, return NULL
		return pSTMHandle;
	}
#endif
	//If interface already open return NULL handle
	if ( false == STMisOpen )  
	{
#if defined(_STM) || ( defined(_COMPACT) && !defined(_CIO) )
		//Map the pyhsical STM memory map to a virtural address
		void * vSTM_BaseAddress = STM_memMap(  (unsigned)&STM_BaseAddress, 256*(unsigned)&STM_ChannelResolution);
		if ( NULL == vSTM_BaseAddress ) {
			return pSTMHandle;	//Return NULL handle on mapping error
		}
#endif
#ifdef _STM
		//Allocate space for interface handle

		pSTMHandle = (STMHandle *) STM_memAlloc(sizeof(STMHandle));
		if (NULL != pSTMHandle)
		{
			// Save parameters to interface handle
//			pSTMHandle->ulBaseAddress = (unsigned long)&STM_BaseAddress;
			pSTMHandle->ulBaseAddress =(unsigned long)vSTM_BaseAddress;
			pSTMHandle->chResolution = (unsigned long)&STM_ChannelResolution; 
			
			// Decide if blocking or non-blocking
			if ( NULL != pSTMBufObj) {

				//Non-blocking so check pSTMBufObj parameters
				if (    ( 0 == pSTMBufObj->maxBufSize)
					 || ( pSTMBufObj->DMAChan_0 > MAX_DMA_CHANNEL )
					 || ( pSTMBufObj->DMAChan_1 > MAX_DMA_CHANNEL )
					 || ( pSTMBufObj->DMAIrq > MAX_DMA_IRQ ) )
				{
					STM_memFree ( pSTMHandle );
					return NULL;
				} 

				//save non-blocking parameters to interface handle
				pSTMHandle->BufIOEnable = true;
				pSTMHandle->BufIOInfo =  * pSTMBufObj;
				pSTMHandle->BufUsed = 0;
				pSTMHandle->pHeadMsgObj = NULL;

				if ( true == pSTMHandle->BufIOInfo.usingDMAInterrupt )
				{
					//Set the DMA channel's enable interrupt
					* (unsigned long *)DMA_ChIntCntl(pSTMHandle->BufIOInfo.DMAChan_0) = DMA_FRAME_COMPLETE;
					
					//Enable the selected IRQ signal
					* (unsigned long *)DMA_IrqEnable(pSTMHandle->BufIOInfo.DMAIrq) = 1 << pSTMHandle->BufIOInfo.DMAChan_0;

				}

			}
			else 
			{
				//save blocking parameters to interface handle
				pSTMHandle->BufIOEnable = false;
			}


			STMisOpen = true;

		} //End of - if (NULL != pSTMHandle)
#endif
#if defined(_COMPACT) && !defined(_CIO)
		//Utilize pre-allocated handle object
		pSTMHandle = &STMHandleObj;
			
		// Save parameters to interface handle
//		STMHandleObj.ulBaseAddress = (unsigned long)&STM_BaseAddress;
		STMHandleObj.ulBaseAddress = (unsigned long)vSTM_BaseAddress;
		STMHandleObj.chResolution = (unsigned long)&STM_ChannelResolution; 

		//Force to blocking mode
		STMHandleObj.BufIOEnable = false;
		STMisOpen = true;
#endif
#ifdef _CIO
		//Utilize pre-allocated handle object
		pSTMHandle = &STMHandleObj;

		STMHandleObj.BufIOInfo = * pSTMBufObj;
		if ( NULL == pSTMBufObj->pFileName )
		{
			STMHandleObj.pLogFileHdl = stdout;
			
			STM_FrmtStringTop = STM_FrmtStringTop_STDIO; 
			STM_FrmtStringHdr = STM_FrmtStringHdr_STDIO;
			STM_FrmtStringMsg1 = STM_FrmtStringMsg1_STDIO; 
			STM_FrmtStringMsg = STM_FrmtStringMsg_STDIO;	
		}
		else {		     
			char * fileMode = ( false == pSTMBufObj->fileAppend ) ? "w" : "a";
			STMHandleObj.pLogFileHdl = fopen( pSTMBufObj->pFileName, fileMode );
			if ( NULL == STMHandleObj.pLogFileHdl )
			{
				//Could not open file so return NULL
				return NULL;
			}

			STM_FrmtStringTop = STM_FrmtStringTop_FILEIO; 
			STM_FrmtStringHdr = STM_FrmtStringHdr_FILEIO;
			STM_FrmtStringMsg1 = STM_FrmtStringMsg1_FILEIO; 
			STM_FrmtStringMsg = STM_FrmtStringMsg_FILEIO;
		}
			
		if ( 0 > fprintf(STMHandleObj.pLogFileHdl, STM_FrmtStringTop) )
		{
			//Could not write to file so return NULL
			return NULL;
		}
		
		STMisOpen = true;
#endif
					
	} //End of - if ( false == STMisOpen )

	return pSTMHandle;
}

eSTM_STATUS STMXport_printf(STMHandle* pSTMHandle, int chNum, STMXport_callback pCallBack, const char* pMsgString, ... )
{
	eSTM_STATUS returnStatus = eSTM_SUCCESS;
#ifdef _STM
	int iHdrWordCnt = 1;					//Minimum is one word 
	int iBufWordCnt = 1;        			//Minimum is one word for formatted string pointer
	long *pOutBuffer;
	const char* pTemp;

	//////////////////////////////////////////////////////////////////
	// Check all function parameters and return error if necessary  //
	//////////////////////////////////////////////////////////////////
	if (    ( NULL == pSTMHandle ) || ( NULL == pMsgString ) 
	     || ( chNum < 0 ) || ( chNum > STM_MAX_CHANNEL ) )
	{
		RETURN_CALLBACK(pCallBack, eSTM_ERROR_PARM);	
	} 

	//////////////////////////////////////////////////////////////////
	// Figure out the buffer size required and allocate it          //
	//  on the heap.											    //
	//////////////////////////////////////////////////////////////////
	{
		const char * pConvChar = pMsgString;
		const char * pPrevConvChar = NULL;	
		
		//Scan pMsgString for the number of conversion characters (%)
		while (1) {
			pConvChar = strchr(pConvChar, '%');
			if ( NULL != pConvChar ) 
			{
				bool bExitFlag = true;

				// For the single "%" case - we know there is at least 1 word to save off
				pPrevConvChar = pConvChar;
				iBufWordCnt++;

				//Search for the conversion character
				pTemp = pConvChar;
				bExitFlag = true;
				while (bExitFlag)
				{
					pTemp++;
					if (('\0' != *pTemp) && (NULL != pTemp))
					{
						char formatCode = *pTemp;
						bExitFlag = false;
						switch (formatCode)
						{
							case 'h': case 'd': case'u': case'i': case 'c': case 'o': case 'x': case 'X':
									bExitFlag = false;
									break;
							case 'l':
							case 'L':
									if (!( 'l' == (char)(*(pTemp+1))
										|| 'f' == (char)(*(pTemp+1)) 
										|| 'e' == (char)(*(pTemp+1))
										|| 'E' == (char)(*(pTemp+1))
										|| 'g' == (char)(*(pTemp+1))
										|| 'G' == (char)(*(pTemp+1)) ) )
									{
										//Not a leagal case so during the second pass we will error out
										//Don't need to repeat the code here.
										bExitFlag = false;
										break;
									}
									// All valid double and long long cases fall through
							case 'e': case 'f': case 'g': case 'E': case 'F': case 'G': 
									/* Float will be treated as double (64 bits) in va_arg macro */
									iBufWordCnt++;
									bExitFlag = false;
									break;
							case '%':
									if (( pTemp - pPrevConvChar ) == 1 )
									{
										//Double '%' case
										pPrevConvChar = NULL; 
										iBufWordCnt--;	
									}
									bExitFlag = false;
									break;		
						}/* end of switch */
					}/* if  */
					else break; //Reached the end of the conversion string
				}/* while */

				pConvChar = pTemp++;
			}
			else break;		
		}
		 
		//Add room for the header. If number of words for message > OST_SMALL_MSG then 2 words needed, else 1 word needed 
		iHdrWordCnt += ( iBufWordCnt > OST_SMALL_MSG ) ? 1 : 0;
		iBufWordCnt += iHdrWordCnt;  

		//If buffered IO need to check the buffer accounting to make sure there is enough heap
		if ( true == pSTMHandle->BufIOEnable ) 
		{
			if ( (pSTMHandle->BufUsed + (iBufWordCnt * sizeof(long))) > pSTMHandle->BufIOInfo.maxBufSize )
			{
				RETURN_CALLBACK(pCallBack, eSTM_ERROR_BUF_ALLOCATION);
			}	
		}

		//Allocate enough memory for both the header and the message
		pOutBuffer = (long*) STM_memAlloc(iBufWordCnt * sizeof(long));
		if (NULL == pOutBuffer)
		{
			RETURN_CALLBACK(pCallBack, eSTM_ERROR_HEAP_ALLOCATION);
		}

		if ( true == pSTMHandle->BufIOEnable ) 
		{
			pSTMHandle->BufUsed += iBufWordCnt * sizeof(long);
		}

	}
	//////////////////////////////////////////////////////////////////
	// Put format string values in msg buffer                       //
	//////////////////////////////////////////////////////////////////
	{
		const char* pTemp;
		long  *pOutBufTemp, *pBufPtr, *tempPtr;	
		char  formatCode = 0;
		int  tempArgValueInt;
		long  tempArgValueLong;
		long long tempArgValueLongLong;
		double tempArgValueDouble;
		va_list arg_addr;
		char bExitFlag = 1;

		//Offset the header
		pOutBufTemp = pOutBuffer + iHdrWordCnt; 

		pBufPtr = pOutBufTemp;		
		pTemp = pMsgString;

		/* store the address of string pointer as the first element in output buffer */
		*pBufPtr = (long)pMsgString; 
		pBufPtr++;

		va_start(arg_addr, pMsgString);
		/* now walk though the remaining string. The end of string should be a " symbol as well */
		while (('\0' != *pTemp) && (NULL != pTemp))
		{
			if ('%' == *pTemp)
			{
				bExitFlag = 1;
				returnStatus = eSTM_ERROR_STRING_FORMAT;
				while (bExitFlag)
				{
					pTemp++;
					if (NULL != pTemp)
					{
						formatCode = (char)(*pTemp);
						bExitFlag = 0;
						switch (formatCode)
						{
							//int (32-bits) cases
							case 'h':
									//The format conversion promotes shorts to ints
									//Fall through to the next case for processing
									if ( !( 'd' == (char)(*(pTemp+1))
										 || 'i' == (char)(*(pTemp+1))
										 || 'o' == (char)(*(pTemp+1))
									     || 'u' == (char)(*(pTemp+1))
									     || 'x' == (char)(*(pTemp+1))
									     || 'X' == (char)(*(pTemp+1))  ) ) 							
									{
										// Must be an un-supported format so error out
										break;
									}
									//Valid int case so fall through
							case 'd': case'u': case'i': case 'c': case 'o': case 'x': case 'X':
									tempArgValueInt = va_arg(arg_addr,int);
									*pBufPtr = (long)tempArgValueInt;
									pBufPtr++;								
									returnStatus = eSTM_SUCCESS;
									break;
							//long (32-bit) & long long cases
							case 'l':
						
									/* this is the long data type (32 bits) */
									if (    'd' == (char)(*(pTemp+1))
										 || 'i' == (char)(*(pTemp+1))
										 || 'o' == (char)(*(pTemp+1))
									     || 'u' == (char)(*(pTemp+1))
									     || 'x' == (char)(*(pTemp+1))
									     || 'X' == (char)(*(pTemp+1))  )							
									{
										tempArgValueLong = va_arg(arg_addr,long);
										*pBufPtr = tempArgValueLong;
										pBufPtr++;
										returnStatus = eSTM_SUCCESS;
									}
									if ( 'l' == (char)(*(pTemp+1)) )
									{
										tempArgValueLongLong = va_arg(arg_addr,long long);
										tempPtr = (long*)&tempArgValueLongLong;
										*pBufPtr++ = *tempPtr++;
#ifdef C55XP_STM
										//for DSPs the long long type is 40 bits
										*pBufPtr++ = 0x000000FF & *tempPtr;
#else 
										*pBufPtr++ = *tempPtr;
#endif
										returnStatus = eSTM_SUCCESS;											
									} 
									//Fall through and test %lf ... cases which are questionable for ANSI C
							//double ( 64-bits only) cases
							case 'L':
									/* this is the double data type (64 bits)*/
									if ( !( 'f' == (char)(*(pTemp+1))
									     || 'e' == (char)(*(pTemp+1))
									     || 'g' == (char)(*(pTemp+1))
									     || 'E' == (char)(*(pTemp+1))
									     || 'G' == (char)(*(pTemp+1))) )

									{
										// Must be an un-supported format so error out
										break;
									}
									//Valid double case so fall through
							case 'e': case 'f': case 'g': case 'E': case 'G': 
									/* Float will be treated as double (64 bits) in va_arg macro */
									tempArgValueDouble = va_arg(arg_addr,double);
									tempPtr = (long*)&tempArgValueDouble;
									
									//TI Compiler pointing to MS word first
									tempArgValueLong = *tempPtr++;
									*pBufPtr++ = *tempPtr;
									*pBufPtr++ = tempArgValueLong;

									returnStatus = eSTM_SUCCESS;
									break;

							//Cases we skip
							case '.': case '+': case '-': case ' ': case '#': case '*':
							case '0': case '1': case '2': case '3': case '4': 
							case '5': case '6': case '7': case '8': case '9': 
									 /* skip these formatting characters. */
									 bExitFlag = 1;
									 returnStatus = eSTM_SUCCESS;
									 break;
							case '%':
									 bExitFlag = 1;
									 break;		
						}/* end of switch */
					}/* if  */
				}/* while */
				/* return failure immediately if the number of variables do not match */
				if (returnStatus != eSTM_SUCCESS)
				{
					STM_memFree(pOutBuffer);
					RETURN_CALLBACK(pCallBack, returnStatus);
				}
			}
			pTemp++;
		}/* end of while loop */
		va_end(arg_addr);

	}

	//////////////////////////////////////////////////////////////////
	// Add the header and dump to STM module                        //
	//////////////////////////////////////////////////////////////////
	if (returnStatus == eSTM_SUCCESS)
	{
		int bytesInHeader;
		long msgByteCount = (iBufWordCnt - iHdrWordCnt) * STM_WORD_SIZE;	

		Build_OST_Header((long)OST_PROTOCOL_PRINTF, msgByteCount, (long*)pOutBuffer, &bytesInHeader);

		if ( true == pSTMHandle->BufIOEnable ) 
		{
		    //Non-blobking case use the DMA
			bool DMASingleAccessMode = false;
			returnStatus = STM_putDMA(pSTMHandle, chNum, pOutBuffer, iBufWordCnt, eWord, DMASingleAccessMode, pCallBack);
		}
		else
		{
			//Blocking case, ok to use the CPU
			returnStatus = STM_putBufCPU(pSTMHandle, chNum, NULL, 0, pOutBuffer, iBufWordCnt, eWord);
			STM_memFree(pOutBuffer);
//			if ( NULL != pCallBack ) pCallBack(returnStatus);

		}					
	}
	
//	return returnStatus;
	RETURN_CALLBACK(pCallBack, returnStatus);
#endif
#ifdef _COMPACT
	//This code included in Compact Build
	returnStatus =  eSTM_ERROR_INVALID_FUNC;
	RETURN_CALLBACK(pCallBack, returnStatus);
#endif
#if defined(_CIO) && !defined(_COMPACT)
	va_list args;

	va_start(args, pMsgString);
	if (   ( 0 > fprintf (pSTMHandle->pLogFileHdl, STM_FrmtStringHdr, pSTMHandle->BufIOInfo.pMasterId, chNum, "STMXport_printf(...)" ) )
	    || ( 0 > vfprintf(pSTMHandle->pLogFileHdl, pMsgString, args) )
		|| ( 0 > fprintf (pSTMHandle->pLogFileHdl, "\n") ) )
	{
	 	returnStatus = eSTM_ERROR_CIO_ERROR;
	}
	va_end(args);
	 
	RETURN_CALLBACK(pCallBack, returnStatus);
    
#endif
}



eSTM_STATUS STMXport_printfV(STMHandle* pSTMHandle, int chNum, STMXport_callback pCallBack, const char* pMsgString, va_list arg_addr)
{
	eSTM_STATUS returnStatus = eSTM_SUCCESS;
#ifdef _STM
	int iHdrWordCnt = 1;					//Minimum is one word 
	int iBufWordCnt = 1;        			//Minimum is one word for formatted string pointer
	long *pOutBuffer;
	const char* pTemp;

	//////////////////////////////////////////////////////////////////
	// Check all function parameters and return error if necessary  //
	//////////////////////////////////////////////////////////////////
	if (    ( NULL == pSTMHandle ) || ( NULL == pMsgString ) 
	     || ( chNum < 0 ) || ( chNum > STM_MAX_CHANNEL ) )
	{
		RETURN_CALLBACK(pCallBack, eSTM_ERROR_PARM);	
	} 

	//////////////////////////////////////////////////////////////////
	// Figure out the buffer size required and allocate it          //
	//  on the heap.											    //
	//////////////////////////////////////////////////////////////////
	{
		const char * pConvChar = pMsgString;
		const char * pPrevConvChar = NULL;	
		
		//Scan pMsgString for the number of conversion characters (%)
		while (1) {
			pConvChar = strchr(pConvChar, '%');
			if ( NULL != pConvChar ) 
			{
				bool bExitFlag = true;

				// For the single "%" case - we know there is at least 1 word to save off
				pPrevConvChar = pConvChar;
				iBufWordCnt++;

				//Search for the conversion character
				pTemp = pConvChar;
				bExitFlag = true;
				while (bExitFlag)
				{
					pTemp++;
					if (('\0' != *pTemp) && (NULL != pTemp))
					{
						char formatCode = *pTemp;
						bExitFlag = false;
						switch (formatCode)
						{
							case 'h': case 'd': case'u': case'i': case 'c': case 'o': case 'x': case 'X':
									bExitFlag = false;
									break;
							case 'l':
							case 'L':
									if (!( 'l' == (char)(*(pTemp+1))
										|| 'f' == (char)(*(pTemp+1)) 
										|| 'e' == (char)(*(pTemp+1))
										|| 'E' == (char)(*(pTemp+1))
										|| 'g' == (char)(*(pTemp+1))
										|| 'G' == (char)(*(pTemp+1)) ) )
									{
										//Not a leagal case so during the second pass we will error out
										//Don't need to repeat the code here.
										bExitFlag = false;
										break;
									}
									// All valid double and long long cases fall through
							case 'e': case 'f': case 'g': case 'E': case 'F': case 'G': 
									/* Float will be treated as double (64 bits) in va_arg macro */
									iBufWordCnt++;
									bExitFlag = false;
									break;
							case '%':
									if (( pTemp - pPrevConvChar ) == 1 )
									{
										//Double '%' case
										pPrevConvChar = NULL; 
										iBufWordCnt--;	
									}
									bExitFlag = false;
									break;		
						}/* end of switch */
					}/* if  */
					else break; //Reached the end of the conversion string
				}/* while */

				pConvChar = pTemp++;
			}
			else break;		
		}
		 
		//Add room for the header. If number of words for message > OST_SMALL_MSG then 2 words needed, else 1 word needed 
		iHdrWordCnt += ( iBufWordCnt > OST_SMALL_MSG ) ? 1 : 0;
		iBufWordCnt += iHdrWordCnt;  

		//If buffered IO need to check the buffer accounting to make sure there is enough heap
		if ( true == pSTMHandle->BufIOEnable ) 
		{
			if ( (pSTMHandle->BufUsed + (iBufWordCnt * sizeof(long))) > pSTMHandle->BufIOInfo.maxBufSize )
			{
				RETURN_CALLBACK(pCallBack, eSTM_ERROR_BUF_ALLOCATION);
			}	
		}

		//Allocate enough memory for both the header and the message
		pOutBuffer = (long*) STM_memAlloc(iBufWordCnt * sizeof(long));
		if (NULL == pOutBuffer)
		{
			RETURN_CALLBACK(pCallBack, eSTM_ERROR_HEAP_ALLOCATION);
		}

		if ( true == pSTMHandle->BufIOEnable ) 
		{
			pSTMHandle->BufUsed += iBufWordCnt * sizeof(long);
		}

	}
	//////////////////////////////////////////////////////////////////
	// Put format string values in msg buffer                       //
	//////////////////////////////////////////////////////////////////
	{
		const char* pTemp;
		long  *pOutBufTemp, *pBufPtr, *tempPtr;	
		char  formatCode = 0;
		int  tempArgValueInt;
		long  tempArgValueLong;
		long long tempArgValueLongLong;
		double tempArgValueDouble;
		//va_list arg_addr;
		char bExitFlag = 1;

		//Offset the header
		pOutBufTemp = pOutBuffer + iHdrWordCnt; 

		pBufPtr = pOutBufTemp;		
		pTemp = pMsgString;

		/* store the address of string pointer as the first element in output buffer */
		*pBufPtr = (long)pMsgString; 
		pBufPtr++;
        
        
		//va_start(arg_addr, pMsgString);
		/* now walk though the remaining string. The end of string should be a " symbol as well */
		while (('\0' != *pTemp) && (NULL != pTemp))
		{
			if ('%' == *pTemp)
			{
				bExitFlag = 1;
				returnStatus = eSTM_ERROR_STRING_FORMAT;
				while (bExitFlag)
				{
					pTemp++;
					if (NULL != pTemp)
					{
						formatCode = (char)(*pTemp);
						bExitFlag = 0;
						switch (formatCode)
						{
							//int (32-bits) cases
							case 'h':
									//The format conversion promotes shorts to ints
									//Fall through to the next case for processing
									if ( !( 'd' == (char)(*(pTemp+1))
										 || 'i' == (char)(*(pTemp+1))
										 || 'o' == (char)(*(pTemp+1))
									     || 'u' == (char)(*(pTemp+1))
									     || 'x' == (char)(*(pTemp+1))
									     || 'X' == (char)(*(pTemp+1))  ) ) 							
									{
										// Must be an un-supported format so error out
										break;
									}
									//Valid int case so fall through
							case 'd': case'u': case'i': case 'c': case 'o': case 'x': case 'X':
									tempArgValueInt = va_arg(arg_addr,int);
									*pBufPtr = (long)tempArgValueInt;
									pBufPtr++;								
									returnStatus = eSTM_SUCCESS;
									break;
							//long (32-bit) & long long cases
							case 'l':
						
									/* this is the long data type (32 bits) */
									if (    'd' == (char)(*(pTemp+1))
										 || 'i' == (char)(*(pTemp+1))
										 || 'o' == (char)(*(pTemp+1))
									     || 'u' == (char)(*(pTemp+1))
									     || 'x' == (char)(*(pTemp+1))
									     || 'X' == (char)(*(pTemp+1))  )							
									{
										tempArgValueLong = va_arg(arg_addr,long);
										*pBufPtr = tempArgValueLong;
										pBufPtr++;
										returnStatus = eSTM_SUCCESS;
									}
									if ( 'l' == (char)(*(pTemp+1)) )
									{
										tempArgValueLongLong = va_arg(arg_addr,long long);
										tempPtr = (long*)&tempArgValueLongLong;
										*pBufPtr++ = *tempPtr++;
#ifdef C55XP_STM
										//for DSPs the long long type is 40 bits
										*pBufPtr++ = 0x000000FF & *tempPtr;
#else 
										*pBufPtr++ = *tempPtr;
#endif
										returnStatus = eSTM_SUCCESS;											
									} 
									//Fall through and test %lf ... cases which are questionable for ANSI C
							//double ( 64-bits only) cases
							case 'L':
									/* this is the double data type (64 bits)*/
									if ( !( 'f' == (char)(*(pTemp+1))
									     || 'e' == (char)(*(pTemp+1))
									     || 'g' == (char)(*(pTemp+1))
									     || 'E' == (char)(*(pTemp+1))
									     || 'G' == (char)(*(pTemp+1))) )

									{
										// Must be an un-supported format so error out
										break;
									}
									//Valid double case so fall through
							case 'e': case 'f': case 'g': case 'E': case 'G': 
									/* Float will be treated as double (64 bits) in va_arg macro */
									tempArgValueDouble = va_arg(arg_addr,double);
									tempPtr = (long*)&tempArgValueDouble;
									
									//TI Compiler pointing to MS word first
									tempArgValueLong = *tempPtr++;
									*pBufPtr++ = *tempPtr;
									*pBufPtr++ = tempArgValueLong;

									returnStatus = eSTM_SUCCESS;
									break;

							//Cases we skip
							case '.': case '+': case '-': case ' ': case '#': case '*':
							case '0': case '1': case '2': case '3': case '4': 
							case '5': case '6': case '7': case '8': case '9': 
									 /* skip these formatting characters. */
									 bExitFlag = 1;
									 returnStatus = eSTM_SUCCESS;
									 break;
							case '%':
									 bExitFlag = 1;
									 break;		
						}/* end of switch */
					}/* if  */
				}/* while */
				/* return failure immediately if the number of variables do not match */
				if (returnStatus != eSTM_SUCCESS)
				{
					STM_memFree(pOutBuffer);
					RETURN_CALLBACK(pCallBack, returnStatus);
				}
			}
			pTemp++;
		}/* end of while loop */
		//va_end(arg_addr);

	}

	//////////////////////////////////////////////////////////////////
	// Add the header and dump to STM module                        //
	//////////////////////////////////////////////////////////////////
	if (returnStatus == eSTM_SUCCESS)
	{
		int bytesInHeader;
		long msgByteCount = (iBufWordCnt - iHdrWordCnt) * STM_WORD_SIZE;	

		Build_OST_Header((long)OST_PROTOCOL_PRINTF, msgByteCount, (long*)pOutBuffer, &bytesInHeader);

		if ( true == pSTMHandle->BufIOEnable ) 
		{
		    //Non-blobking case use the DMA
			bool DMASingleAccessMode = false;
			returnStatus = STM_putDMA(pSTMHandle, chNum, pOutBuffer, iBufWordCnt, eWord, DMASingleAccessMode, pCallBack);
		}
		else
		{
			//Blocking case, ok to use the CPU
			returnStatus = STM_putBufCPU(pSTMHandle, chNum, NULL, 0, pOutBuffer, iBufWordCnt, eWord);
			STM_memFree(pOutBuffer);
//			if ( NULL != pCallBack ) pCallBack(returnStatus);

		}					
	}
	
//	return returnStatus;
	RETURN_CALLBACK(pCallBack, returnStatus);
#endif
#ifdef _COMPACT
	//This code included in Compact Build
	returnStatus =  eSTM_ERROR_INVALID_FUNC;
	RETURN_CALLBACK(pCallBack, returnStatus);
#endif
#if defined(_CIO) && !defined(_COMPACT)
	//va_list args;

    
	//va_start(args, pMsgString);
	if (   ( 0 > fprintf (pSTMHandle->pLogFileHdl, STM_FrmtStringHdr, pSTMHandle->BufIOInfo.pMasterId, chNum, "STMXport_printf(...)" ) )
	    || ( 0 > vfprintf(pSTMHandle->pLogFileHdl, pMsgString, arg_addr) )
		|| ( 0 > fprintf (pSTMHandle->pLogFileHdl, "\n") ) )
	{
	 	returnStatus = eSTM_ERROR_CIO_ERROR;
	}
	//va_end(args);
	 
	RETURN_CALLBACK(pCallBack, returnStatus);
    
#endif
}



eSTM_STATUS STMXport_putMsg(STMHandle* pSTMHandle, int chNum, STMXport_callback pCallBack, const char* pMsg, int iMsgByteCount)
{ 
	eSTM_STATUS returnStatus = eSTM_SUCCESS;


#ifdef _DEBUG
	if (    ( (int)NULL == (int)pSTMHandle ) || ( (int)NULL == (int)pMsg ) || ( (int)NULL == iMsgByteCount ) 
	     || ( chNum < 0 ) || ( chNum > STM_MAX_CHANNEL ) )
	{
		RETURN_CALLBACK(pCallBack, eSTM_ERROR_PARM);	
	}
#endif

#ifdef _STM
	if ( true == pSTMHandle->BufIOEnable )
	{

		int iHdrWordCnt = 1;					//Minimum is one word 
		int iBufWordCnt = iMsgByteCount/eWord;  //Minimum is one word for formatted string pointer
		int bytesInHeader = 0;
		long * pOutBuf;
		long * pOutBufTemp;
		bool oddWordFlag = false;
		bool DMASingleAccessMode = false;

		//Check for extra bytes
		if ( ( iMsgByteCount % eWord ) > 0 ) {
			 iBufWordCnt++;
			 oddWordFlag = true;
		}
		iHdrWordCnt += ( iMsgByteCount > OST_SMALL_MSG ) ? 1 : 0;
		iBufWordCnt += iHdrWordCnt;

		//make sure we will not exceed our heap quota
		if ( (pSTMHandle->BufUsed + (iBufWordCnt * sizeof(long))) > pSTMHandle->BufIOInfo.maxBufSize )
		{
			RETURN_CALLBACK(pCallBack,eSTM_ERROR_BUF_ALLOCATION);
		}	

		pOutBuf = (long * )STM_memAlloc(iBufWordCnt*eWord);
		if (NULL == pOutBuf)
		{
			RETURN_CALLBACK(pCallBack,eSTM_ERROR_HEAP_ALLOCATION);
		}

		pSTMHandle->BufUsed += iBufWordCnt * sizeof(long);

		//Initialize the last word in the buffer to zero
		if ( true == oddWordFlag )
		{
			pOutBuf[iBufWordCnt-1] = 0;
		}

		Build_OST_Header((long)OST_PROTOCOL_BYTESTREAM, iMsgByteCount, pOutBuf, &bytesInHeader);
		pOutBufTemp = pOutBuf + iHdrWordCnt;
		Compose_OST_MSG(pMsg, iMsgByteCount, NULL, 0, (char *)pOutBufTemp);

		returnStatus = STM_putDMA(pSTMHandle, chNum, pOutBuf, iBufWordCnt, eWord, DMASingleAccessMode, pCallBack);

	}
	else
	{
		int bytesInHeader = 0;
		long  OST_Header[2]={0};

		Build_OST_Header((long)OST_PROTOCOL_BYTESTREAM, iMsgByteCount, (long*)OST_Header, &bytesInHeader);
		returnStatus = STM_putMsgCPU(pSTMHandle, chNum, (void *)OST_Header, bytesInHeader, (void *)pMsg, iMsgByteCount);

	}

	RETURN_CALLBACK(pCallBack,returnStatus);
#endif
#ifdef _COMPACT 
	//This code included in Compact Build
	returnStatus =  eSTM_ERROR_INVALID_FUNC;
	RETURN_CALLBACK(pCallBack, returnStatus);	
#endif
#if defined(_CIO) && !defined(_COMPACT)
	{
		int fprintfErr = 0;
		unsigned int data;
		bool firstTime = true;
		const char * pMsgTmp = pMsg;

		if ( 0 > fprintf (pSTMHandle->pLogFileHdl, STM_FrmtStringHdr, pSTMHandle->BufIOInfo.pMasterId, chNum, "STMXport_putMsg(...)") )
		{
			returnStatus = eSTM_ERROR_CIO_ERROR;
		}

        do 
		{  
			data = (0 < iMsgByteCount--) ? *pMsgTmp++ : 0;
			data |= (0 < iMsgByteCount--) ? *pMsgTmp++ << 8 : 0;
			data |= (0 < iMsgByteCount--) ? *pMsgTmp++ << 16: 0;
			data |= (0 < iMsgByteCount--) ? *pMsgTmp++ << 24: 0;

			if ( true == firstTime )
			{
				fprintfErr = fprintf (pSTMHandle->pLogFileHdl, STM_FrmtStringMsg1, data );
			}
			else
			{
				fprintfErr = fprintf (pSTMHandle->pLogFileHdl, STM_FrmtStringMsg, data );
			}
			if ( 0 > fprintfErr )
			{
				returnStatus = eSTM_ERROR_CIO_ERROR;
				break;
			}
			firstTime = false;
		}
		while ( 0 < iMsgByteCount); 

	}

	RETURN_CALLBACK(pCallBack,returnStatus);
#endif
}

eSTM_STATUS STMXport_putBuf(STMHandle* pSTMHandle, int chNum, STMXport_callback pCallBack, void* pDataBuf, eSTMElementSize elementSize, int elementCount)
{
	eSTM_STATUS returnStatus = eSTM_SUCCESS;
#ifdef _DEBUG
	unsigned int bufAlignment;

	if (    ( (int)NULL == (int)pSTMHandle ) || ( (int)NULL == (int)pDataBuf ) || ( (int)NULL == elementCount ) 
	     || ( chNum < 0 ) || ( chNum > STM_MAX_CHANNEL ) )
	{
		RETURN_CALLBACK(pCallBack, eSTM_ERROR_PARM);
	}

	//Check address alignment
	bufAlignment = AddressAlignmentChecker((unsigned long)pDataBuf);
	switch (elementSize)
	{
		case eShort:
			if ( ( eByteAlign == bufAlignment) || ( eShortAndByteAlign ==  bufAlignment) )
			{
				RETURN_CALLBACK(pCallBack, eSTM_ERROR_PARM);
			}
			break;
		case eWord:
			if ( ( eByteAlign == bufAlignment) || ( eShortAndByteAlign ==  bufAlignment) || ( eShortAlign == bufAlignment ) )
			{
				RETURN_CALLBACK(pCallBack, eSTM_ERROR_PARM);
			}
			break;
	} 
#endif

#ifdef _STM
	if ( true == pSTMHandle->BufIOEnable )
	{
		int iHdrByteCnt = eWord;
		int msgByteCount = elementSize * elementCount;								//Minimum is one word 
		int iBufByteCnt = msgByteCount;     
		int bytesInHeader = 0;
		int numElements = 0;
		long * pOutBuf;
		long * pOutBufTemp;
		bool DMASingleAccessMode = true;

		iHdrByteCnt += ( iBufByteCnt > OST_SMALL_MSG ) ? eWord : 0;
		iBufByteCnt += iHdrByteCnt;

		//make sure we will not exceed our heap quota
		if ( (pSTMHandle->BufUsed + iBufByteCnt) > pSTMHandle->BufIOInfo.maxBufSize )
		{
			RETURN_CALLBACK(pCallBack, eSTM_ERROR_BUF_ALLOCATION);
		}	

		pOutBuf = (long * )STM_memAlloc(iBufByteCnt);
		if (NULL == pOutBuf)
		{
			RETURN_CALLBACK(pCallBack, eSTM_ERROR_HEAP_ALLOCATION);
		}

		pSTMHandle->BufUsed += iBufByteCnt;

		Build_OST_Header((long)OST_PROTOCOL_MSGSTREAM, msgByteCount, pOutBuf, &bytesInHeader);
		pOutBufTemp = (long *)((char *)pOutBuf + iHdrByteCnt);
		Compose_OST_MSG(pDataBuf, msgByteCount, NULL, 0, (char *)pOutBufTemp);

		numElements = elementCount + iHdrByteCnt/elementSize;		
		returnStatus = STM_putDMA(pSTMHandle, chNum, pOutBuf, numElements, elementSize, DMASingleAccessMode, pCallBack);

	}
	else
#endif
#if defined(_STM) || ( defined(_COMPACT) && !defined(_CIO) )
	{

		register long msgByteCount = elementSize * elementCount;
		register int bytesInHeader = 0;
		register long OST_Header1 = 0;
		long  OST_Header2[2]={0,0};

		if ( msgByteCount < OST_SHORT_HEADER_LENGTH_LIMIT )
		{
			OST_Header1 = Build_CompactOST_Header((long)OST_PROTOCOL_MSGSTREAM, msgByteCount );
			bytesInHeader = 4;
			returnStatus = STM_putBufCPU(pSTMHandle, chNum, (void *) OST_Header1, bytesInHeader, pDataBuf, elementCount, elementSize);		

		}
		else {
			Build_ExtendedOST_Header((long)OST_PROTOCOL_MSGSTREAM, msgByteCount, OST_Header2);
			bytesInHeader = 8;
			returnStatus = STM_putBufCPU(pSTMHandle, chNum, (void *) OST_Header2, bytesInHeader, pDataBuf, elementCount, elementSize);		
		}
	}
#endif
#ifdef _CIO
	{
		int fprintfErr = 0;
		int iElementIndex;
		unsigned int data=0;
		bool firstTime = true;
		unsigned char * pBufByte = pDataBuf;
		unsigned short * pBufShort = pDataBuf;
		unsigned int * pBufInt = pDataBuf;

		if ( 0 > fprintf (pSTMHandle->pLogFileHdl, STM_FrmtStringHdr, pSTMHandle->BufIOInfo.pMasterId, chNum, "STMXport_putBuf(...)") )
		{
			returnStatus = eSTM_ERROR_CIO_ERROR;
		}

		for (iElementIndex = 0; iElementIndex < elementCount; iElementIndex++)
		{
			switch (elementSize)
			{

				case eByte:
					data = *pBufByte++;
					break;

				case eShort:
					data = *pBufShort++;
					break;

				case eWord:
					data = *pBufInt++;
					break;	
			} //End of switch

			if ( true == firstTime )
			{
				fprintfErr = fprintf (pSTMHandle->pLogFileHdl, STM_FrmtStringMsg1, data );
			}
			else
			{
				fprintfErr = fprintf (pSTMHandle->pLogFileHdl, STM_FrmtStringMsg, data );
			}
			if ( 0 > fprintfErr )
			{
				returnStatus = eSTM_ERROR_CIO_ERROR;
				break;
			}
			firstTime = false;


		} //End of for
	}
#endif
	RETURN_CALLBACK(pCallBack, returnStatus);

}


// Warning - Blocking function only, No callback support
// Optimized to send a single word msg (OST Header + 32-bit data) as fast as possible
eSTM_STATUS STMXport_putWord(STMHandle* pSTMHandle, int chNum, STMXport_callback pCallBack, long data)
{

	eSTM_STATUS returnStatus = eSTM_SUCCESS;
#if defined(_STM) || ( defined(_COMPACT) && !defined(_CIO))
	register unsigned long ulMsgAddress = 0;
	register unsigned long ulEndAddress = 0;
	register unsigned long OST_Header = OST_VERSION | OST_ENTITY_ID | (long)OST_PROTOCOL_MSGSTREAM | 4;

#ifdef _DEBUG
	//Check for errors
	if ( ( NULL == pSTMHandle ) || ( chNum < 0 ) || ( chNum > STM_MAX_CHANNEL ) )
	{
		RETURN_CALLBACK(pCallBack, eSTM_ERROR_PARM);
	}
#endif
	//Calculate STM Addresses
    ulMsgAddress = pSTMHandle->ulBaseAddress + (pSTMHandle->chResolution * chNum);
	ulEndAddress = pSTMHandle->ulBaseAddress + (pSTMHandle->chResolution * chNum) + (pSTMHandle->chResolution/2);

	//Xport the OST Header
	*((unsigned long *) ulMsgAddress) = OST_Header;

	//Xport the Data
	*((unsigned long *) ulEndAddress) = data;
#endif
#ifdef _CIO
		if (  ( 0 > fprintf (pSTMHandle->pLogFileHdl, STM_FrmtStringHdr, pSTMHandle->BufIOInfo.pMasterId, chNum, "STMXport_putWord(...)") )
			||( 0 > fprintf (pSTMHandle->pLogFileHdl, STM_FrmtStringMsg1, (unsigned long)data ) ) )
		{
			returnStatus = eSTM_ERROR_CIO_ERROR;
		}	
#endif
	RETURN_CALLBACK(pCallBack, returnStatus);

}

eSTM_STATUS STMXport_putShort(STMHandle* pSTMHandle, int chNum, STMXport_callback pCallBack, short data)
{
	eSTM_STATUS returnStatus = eSTM_SUCCESS;
#if defined(_STM) || ( defined(_COMPACT) && !defined(_CIO))
	register unsigned long ulMsgAddress = 0;
	register unsigned long ulEndAddress = 0;
	register unsigned long OST_Header = OST_VERSION | OST_ENTITY_ID | (long)OST_PROTOCOL_MSGSTREAM | 4;

#ifdef _DEBUG
	//Check for errors
	if ( ( NULL == pSTMHandle ) || ( chNum < 0 ) || ( chNum > STM_MAX_CHANNEL ) )
	{
		RETURN_CALLBACK(pCallBack, eSTM_ERROR_PARM);
	}
#endif
	//Calculate STM Addresses
    ulMsgAddress = pSTMHandle->ulBaseAddress + (pSTMHandle->chResolution * chNum);
	ulEndAddress = pSTMHandle->ulBaseAddress + (pSTMHandle->chResolution * chNum) + (pSTMHandle->chResolution/2);

	//Xport the OST Header
	*((unsigned long *) ulMsgAddress) = OST_Header;

	//Xport the Data
	*((unsigned short *) ulEndAddress) = data;
#endif
#ifdef _CIO
		if (  ( 0 > fprintf (pSTMHandle->pLogFileHdl, STM_FrmtStringHdr, pSTMHandle->BufIOInfo.pMasterId, chNum, "STMXport_putShort(...)") )
			||( 0 > fprintf (pSTMHandle->pLogFileHdl, STM_FrmtStringMsg1, (unsigned short)data ) ) )
		{
			returnStatus = eSTM_ERROR_CIO_ERROR;
		}	
#endif
	RETURN_CALLBACK(pCallBack, returnStatus);


}

eSTM_STATUS STMXport_putByte(STMHandle* pSTMHandle, int chNum, STMXport_callback pCallBack, char data)
{

	eSTM_STATUS returnStatus = eSTM_SUCCESS;

#if defined(_STM) || ( defined(_COMPACT) && !defined(_CIO))
	register unsigned long ulMsgAddress = 0;
	register unsigned long ulEndAddress = 0;
	register unsigned long OST_Header = OST_VERSION | OST_ENTITY_ID | (long)OST_PROTOCOL_MSGSTREAM | 4;

#ifdef _DEBUG
	//Check for errors
	if ( ( NULL == pSTMHandle ) || ( chNum < 0 ) || ( chNum > STM_MAX_CHANNEL ) )
	{
		RETURN_CALLBACK(pCallBack, eSTM_ERROR_PARM);
	}
#endif
	//Calculate STM Addresses
    ulMsgAddress = pSTMHandle->ulBaseAddress + (pSTMHandle->chResolution * chNum);
	ulEndAddress = pSTMHandle->ulBaseAddress + (pSTMHandle->chResolution * chNum) + (pSTMHandle->chResolution/2);

	//Xport the OST Header
	*((unsigned long *) ulMsgAddress) = OST_Header;

	//Xport the Data
	*((unsigned char *) ulEndAddress) = data;
#endif
#ifdef _CIO
		if (  ( 0 > fprintf (pSTMHandle->pLogFileHdl, STM_FrmtStringHdr, pSTMHandle->BufIOInfo.pMasterId, chNum, "STMXport_putByte(...)") )
			||( 0 > fprintf (pSTMHandle->pLogFileHdl, STM_FrmtStringMsg1, (unsigned char)data ) ) )
		{
			returnStatus = eSTM_ERROR_CIO_ERROR;
		}	
#endif
	RETURN_CALLBACK(pCallBack, returnStatus);

}

eSTM_STATUS STMXport_logMsg(STMHandle* pSTMHandle, int chNum, STMXport_callback pCallBack, const char* pMsgString, ...)
{
	eSTM_STATUS returnStatus = eSTM_SUCCESS;
#ifdef _STM
	register volatile unsigned long ulMsgAddress = 0;
	register unsigned long ulEndAddress = 0;
	register unsigned long OST_Header = OST_VERSION | OST_ENTITY_ID | (long)OST_PROTOCOL_PRINTF;

	char firstTime = '0';
	char * pLastConvChar = &firstTime;
	const char * pConvChar = pMsgString;
	int iBufWordCnt = 0;

	//Calculate STM Addresses
    ulMsgAddress = pSTMHandle->ulBaseAddress + (pSTMHandle->chResolution * chNum);
	ulEndAddress = pSTMHandle->ulBaseAddress + (pSTMHandle->chResolution * chNum) + (pSTMHandle->chResolution/2);

	//Figure out the number of variables in the format string
	while ( (pConvChar = strchr(pConvChar, '%')) != NULL) {

		if ( pLastConvChar+1 != pConvChar ) {
			pLastConvChar = (char *)pConvChar;		
			iBufWordCnt++;

		}
		else {
			//Duplicate % so start over
			pLastConvChar = &firstTime;
			//Must remove first % since there is a duplicate
			iBufWordCnt--;
		}
		pConvChar++;

	}

	//Add one for the format string
	iBufWordCnt++;

	//Check that the format string plus the number of variables do not exceed the OST header limit
	if ( iBufWordCnt < OST_SHORT_HEADER_LENGTH_LIMIT) {
		 OST_Header |= iBufWordCnt;
	} 
	else {
		returnStatus = eSTM_ERROR_PARM;
	}

	//Xport the OST Header
	*((unsigned long *) ulMsgAddress) = OST_Header;	


	if ( 1 == iBufWordCnt ) {

		//Xport the Data
		*((unsigned long *) ulEndAddress) = (unsigned long)pMsgString;
	}
	else {
		va_list arg_addr;		
		int argValue;
		 
		va_start(arg_addr, pMsgString);
		
		//Export the pointer to the format string and adjust iBufWordCnt
		*((unsigned long *) ulMsgAddress) = (unsigned long)pMsgString;
		iBufWordCnt--;

		if ( iBufWordCnt > 1 ) {
			do {
				argValue = va_arg(arg_addr,int);
				*((unsigned long *) ulMsgAddress) = argValue;
			} while (--iBufWordCnt != 1);
		}
		
		//One last word to transfer to the end address
		argValue = va_arg(arg_addr,int);
		*((unsigned long *) ulEndAddress) = argValue;

		va_end(arg_addr);
	}
	
	RETURN_CALLBACK(pCallBack, returnStatus);
#endif
#ifdef _COMPACT
	//This code included in Compact Build
	returnStatus =  eSTM_ERROR_INVALID_FUNC;
	RETURN_CALLBACK(pCallBack, returnStatus);	
#endif

#if defined(_CIO) && !defined(_COMPACT)
	va_list args;

	va_start(args, pMsgString);
	if (   ( 0 > fprintf (pSTMHandle->pLogFileHdl, STM_FrmtStringHdr, pSTMHandle->BufIOInfo.pMasterId, chNum, "STMXport_logMsg(...)") )
	    || ( 0 > vfprintf(pSTMHandle->pLogFileHdl, pMsgString, args) )
	    || ( 0 > fprintf (pSTMHandle->pLogFileHdl, "\n") ) )
	{
		returnStatus = eSTM_ERROR_CIO_ERROR;
	}
	va_end(args);
	 
	RETURN_CALLBACK(pCallBack, returnStatus);
    
#endif
}

eSTM_STATUS STMXport_logMsg0(STMHandle* pSTMHandle, int chNum, STMXport_callback pCallBack, const char* pMsgString)
{

	eSTM_STATUS returnStatus = eSTM_SUCCESS;
#if defined(_STM) || ( defined(_COMPACT) && !defined(_CIO))
	register unsigned long ulMsgAddress = 0;
	register unsigned long ulEndAddress = 0;
	register unsigned long OST_Header = OST_VERSION | OST_ENTITY_ID | (long)OST_PROTOCOL_PRINTF | 1;

	//Calculate STM Addresses
    ulMsgAddress = pSTMHandle->ulBaseAddress + (pSTMHandle->chResolution * chNum);
	ulEndAddress = pSTMHandle->ulBaseAddress + (pSTMHandle->chResolution * chNum) + (pSTMHandle->chResolution/2);

	//Xport the OST Header
	*((unsigned long *) ulMsgAddress) = OST_Header;	

	//Xport the Data
	*((unsigned long *) ulEndAddress) = (unsigned long)pMsgString;
#endif
#ifdef _CIO
	fprintf (pSTMHandle->pLogFileHdl, STM_FrmtStringHdr, pSTMHandle->BufIOInfo.pMasterId, chNum, "STMXport_logMsg0(...)");
	fprintf (pSTMHandle->pLogFileHdl, pMsgString );
	fprintf (pSTMHandle->pLogFileHdl, "\n");
#endif	
	RETURN_CALLBACK(pCallBack, returnStatus);

}

eSTM_STATUS STMXport_logMsg1(STMHandle* pSTMHandle, int chNum, STMXport_callback pCallBack, const char* pMsgString, long data)
{
	eSTM_STATUS returnStatus = eSTM_SUCCESS;
#if defined(_STM) || ( defined(_COMPACT) && !defined(_CIO))
	register volatile unsigned long ulMsgAddress = 0;
	register unsigned long ulEndAddress = 0;
	register unsigned long OST_Header = OST_VERSION | OST_ENTITY_ID | (long)OST_PROTOCOL_PRINTF | 2;

	//Calculate STM Addresses
    ulMsgAddress = pSTMHandle->ulBaseAddress + (pSTMHandle->chResolution * chNum);
	ulEndAddress = pSTMHandle->ulBaseAddress + (pSTMHandle->chResolution * chNum) + (pSTMHandle->chResolution/2);

	//Xport the OST Header
	*((unsigned long *) ulMsgAddress) = OST_Header;	

	//Xport the Data
	*((unsigned long *) ulMsgAddress) = (unsigned long)pMsgString;

	*((unsigned long *) ulEndAddress) = (unsigned long)data;
#endif
#ifdef _CIO
	if (   ( 0 > fprintf (pSTMHandle->pLogFileHdl, STM_FrmtStringHdr, pSTMHandle->BufIOInfo.pMasterId, chNum, "STMXport_logMsg1(...)") )
		|| ( 0 > fprintf (pSTMHandle->pLogFileHdl, pMsgString, data ) )
		|| ( 0 > fprintf (pSTMHandle->pLogFileHdl, "\n") ) )
	{
		returnStatus = eSTM_ERROR_CIO_ERROR;
	}
#endif	
	RETURN_CALLBACK(pCallBack, returnStatus);

}

eSTM_STATUS STMXport_logMsg2(STMHandle* pSTMHandle, int chNum, STMXport_callback pCallBack, const char* pMsgString, long data0, long data1)
{
	eSTM_STATUS returnStatus = eSTM_SUCCESS;
#if defined(_STM) || ( defined(_COMPACT) && !defined(_CIO))
	register volatile unsigned long ulMsgAddress = 0;
	register unsigned long ulEndAddress = 0;
	register unsigned long OST_Header = OST_VERSION | OST_ENTITY_ID | (long)OST_PROTOCOL_PRINTF | 3;

	//Calculate STM Addresses
    ulMsgAddress = pSTMHandle->ulBaseAddress + (pSTMHandle->chResolution * chNum);
	ulEndAddress = pSTMHandle->ulBaseAddress + (pSTMHandle->chResolution * chNum) + (pSTMHandle->chResolution/2);

	//Xport the OST Header
	*((unsigned long *) ulMsgAddress) = OST_Header;	

	//Xport the Data
	*((unsigned long *) ulMsgAddress) = (unsigned long)pMsgString;

	*((unsigned long *) ulMsgAddress) = (unsigned long)data0;

	*((unsigned long *) ulEndAddress) = (unsigned long)data1;
#endif
#ifdef _CIO
	if (   ( 0 > fprintf (pSTMHandle->pLogFileHdl, STM_FrmtStringHdr, pSTMHandle->BufIOInfo.pMasterId, chNum, "STMXport_logMsg2(...)") )
		|| ( 0 > fprintf (pSTMHandle->pLogFileHdl, pMsgString, data0, data1 ) )
		|| ( 0 > fprintf (pSTMHandle->pLogFileHdl, "\n") ) )
	{
	 	returnStatus = eSTM_ERROR_CIO_ERROR;
	}
#endif	
	RETURN_CALLBACK(pCallBack, returnStatus);

}

eSTM_STATUS STMXport_getBufInfo(STMHandle* pSTMHandle, unsigned long * msgCnt, unsigned long * curMsgBufSize)
{
#ifdef _STM
	eSTM_STATUS retValue = eSTM_SUCCESS;
#ifdef _DEBUG
	VALIDATE_NULL_VALUE(pSTMHandle);
#endif
	if (true == pSTMHandle->BufIOEnable)
	{
		//Initalize a pointer to head of message object link list 
		STMMsgObj * pTmpMsgObj = pSTMHandle->pHeadMsgObj;
		
		//Set amount of memory allocated 
		*curMsgBufSize = pSTMHandle->BufUsed;

		//Initialize outstanding message count
		*msgCnt = ( (pTmpMsgObj != NULL) && (false == pTmpMsgObj->DMA_posted) ) ? 1 : 0;

		//Walk the message object link list and count the number of outstanding messages
		if ( pTmpMsgObj != NULL )
		{
			while ( pTmpMsgObj->pNextMsg != NULL )
			{
				(*msgCnt)++;
				pTmpMsgObj = pTmpMsgObj->pNextMsg;
			}
		}
		//Get the current DMA state
		retValue = DMA_checkStatus(pSTMHandle);
	}
	else
	{
		*curMsgBufSize = 0;
		*msgCnt = 0;	
	}

	return retValue;
#endif
#if defined(_CIO) || defined(_COMPACT)
	return eSTM_ERROR_INVALID_FUNC;
#endif
}

eSTM_STATUS STMXport_flush(STMHandle* pSTMHandle)
{
	eSTM_STATUS retValue = eSTM_SUCCESS;
#ifdef _DEBUG
	VALIDATE_NULL_VALUE(pSTMHandle);
#endif

#ifdef _STM
	//If interface open for blocking, then there is nothing to flush
	if ( false == pSTMHandle->BufIOEnable )
	{
		return retValue;
	}

	//If DMA is interrupt driven, simply wait until no messages in the link and DMA
	// status is eSTM_SUCCESS
	if ( true == pSTMHandle->BufIOInfo.usingDMAInterrupt )
	{
		while ( NULL != pSTMHandle->pHeadMsgObj){
			//break out if we ever see a DMA error
			retValue = DMA_checkStatus(pSTMHandle);
			if ( retValue <= -1 )
			{
				break;
			}
		};
	}
	else
	{
		//Make sure there is a msg at the head
		while (NULL != pSTMHandle->pHeadMsgObj) 
		{
			//Must wait for the DMA unit to complete the current operation
			do 
			{
				retValue = DMA_checkStatus(pSTMHandle);
			}while( eSTM_PENDING == retValue);

			if ( NULL != pSTMHandle->pHeadMsgObj->pCallBack )
			{
				//If the callback is not NULL call it
				pSTMHandle->pHeadMsgObj->pCallBack(retValue);	
			}
				
			if ( eSTM_SUCCESS == retValue )
			{
				//Start the next message
				DMA_serviceNextMsg(pSTMHandle);

			}
			else
			{
				//Detected an error
				break;
			}
		}
		
		//else retValue has an error that needs to be reported
	}
#endif
#ifdef _CIO
	if ( 0 != fflush(pSTMHandle->pLogFileHdl) )
	{
		retValue = eSTM_ERROR_CIO_ERROR;
	}

#endif

	return retValue;
}

eSTM_STATUS STMXport_close(STMHandle* pSTMHandle)
{
	eSTM_STATUS retValue = eSTM_SUCCESS;
#ifdef _DEBUG
	VALIDATE_NULL_VALUE(pSTMHandle);
#endif

#if defined(_STM) || ( defined(_COMPACT) && !defined(_CIO))
	STM_memUnMap((void *)pSTMHandle->ulBaseAddress, 256*(unsigned int)&STM_ChannelResolution);
#endif

#ifdef _CIO
	if ( 0 != fclose(pSTMHandle->pLogFileHdl) )
	{
		retValue = eSTM_ERROR_CIO_ERROR;
	}

#endif

#ifdef _STM
	if (NULL != pSTMHandle)
	{
		STM_memFree(pSTMHandle);
	}
#endif

	STMisOpen = false;

	return retValue;
}

#if defined(_STM) || ( defined(_COMPACT) && !defined(_CIO))

inline unsigned long Compose_Address(unsigned long ulBaseAddr, int chNum, unsigned long size, eAddressType addrType)
{
	unsigned long tsLocation = 0;

	tsLocation = ( addrType == eTiming ) ? size/2 : 0;
	 
	return (unsigned long) (ulBaseAddr + (size*(unsigned long)chNum)+ tsLocation);		
}

eSTM_STATUS STM_putBufCPU(STMHandle * pSTMHandle, int chNum, void * pHdrBuf, int iHdrByteCnt, void * pMsgBuf, unsigned int iElementCnt, eSTMElementSize iElementSize)
{
	volatile unsigned long ulMsgAddress = 0;
	unsigned long ulEndAddress = 0;
	unsigned int iElementIndex;
	unsigned long * pTempWordPtr;
	unsigned short * pTempShortPtr;
	unsigned char * pTempBytePtr; 

	ulMsgAddress = Compose_Address(pSTMHandle->ulBaseAddress,chNum,pSTMHandle->chResolution,eRegular);
	ulEndAddress = Compose_Address(pSTMHandle->ulBaseAddress,chNum,pSTMHandle->chResolution,eTiming);

	// If the header pointer is not null and the header
	//  word count is greater than 0, send the header.
	// Else (header word count 0 or header pointer null)
	//  then it's assumed the header is contained in the message.

	if (4 == iHdrByteCnt  ) 
	{
		*((unsigned long *) ulMsgAddress) = (long)pHdrBuf;	
	
	}
	if ( 8 == iHdrByteCnt  ) 
	{
		*((unsigned long *) ulMsgAddress) = * (long *)pHdrBuf;
		*((unsigned long *) ulMsgAddress) = * (((long *)pHdrBuf)+1);
	}	

	switch (iElementSize)
	{

		case eByte:
			pTempBytePtr = (unsigned char *)pMsgBuf;
			for (iElementIndex = 0; iElementIndex < (iElementCnt-1); iElementIndex++)
			{	 
				*((unsigned char *) ulMsgAddress) = * pTempBytePtr;
				pTempBytePtr++;
			}

			*((unsigned char *) ulEndAddress) = * pTempBytePtr;
			break;

		case eShort:
			pTempShortPtr = (unsigned short *)pMsgBuf;
			for (iElementIndex = 0; iElementIndex < (iElementCnt-1); iElementIndex++)
			{	 
				*((unsigned short *) ulMsgAddress) = * pTempShortPtr;
				pTempShortPtr++;
			}

			*((unsigned short *) ulEndAddress) = * pTempShortPtr;
			break;

		case eWord:
			pTempWordPtr = (unsigned long *)pMsgBuf;
			for (iElementIndex = 0; iElementIndex < (iElementCnt-1); iElementIndex++)
			{	 
				*((unsigned long *) ulMsgAddress) = * pTempWordPtr;
				pTempWordPtr++;
			}

			*((unsigned long *) ulEndAddress) = * pTempWordPtr;
			break;	
	}

	return eSTM_SUCCESS;
}
#endif

#ifdef _STM
eSTM_STATUS STM_putMsgCPU(STMHandle * pSTMHandle, int chNum, void * pHdrBuf, int iHdrByteCnt, void * pMsgBuf, unsigned int iMsgByteCnt)
{
	volatile unsigned long ulMsgAddress = 0;
	unsigned long ulEndAddress = 0;
	unsigned int iElementIndex;
	unsigned long * pTempWordPtr;
	unsigned short * pTempShortPtr;
	unsigned char * pTempBytePtr;
	void * pTempBuf;
	unsigned int bufByteCnt;
	unsigned int bufAlignment;
	 
	ulMsgAddress = Compose_Address(pSTMHandle->ulBaseAddress,chNum,pSTMHandle->chResolution,eRegular);
	ulEndAddress = Compose_Address(pSTMHandle->ulBaseAddress,chNum,pSTMHandle->chResolution,eTiming);

	// If the header pointer is not null and the header
	//  word count is greater than 0, send the header.
	// Else (header word count 0 or header pointer null)
	//  then it's assumed the header is contained in the message.
	if ( ( 0 != iHdrByteCnt ) && ( NULL != pHdrBuf ) )
	{
		unsigned int iHdrWrdCnt = iHdrByteCnt/eWord;
		for (iElementIndex = 0; iElementIndex < iHdrWrdCnt; iElementIndex++)
		{ 
			*((unsigned long *) ulMsgAddress) = * (long *)pHdrBuf;
		}
	}

	// Process the front end of the message
	bufAlignment = AddressAlignmentChecker((unsigned long)pMsgBuf);
	pTempBuf = pMsgBuf;
	bufByteCnt = iMsgByteCnt;
	switch (bufAlignment)
	{
		//Note on the fron end the alignment enums are reversed from backend
		// eShortAndByte - need to transfer 1 byte to achive word alignment
		// eByte - need to transfer 1 byte and then a short to achive word alignment
		// eShort - same as backend
		// eWord - same as backend
		case eShortAndByteAlign:
			//Fall through to the byte case 
		case eByteAlign:
			if ( bufByteCnt > 1 )
			{
				pTempBytePtr = (unsigned char *)pMsgBuf;
		 
				*((unsigned char *) ulMsgAddress) = * pTempBytePtr;
				pTempBytePtr++;

				pTempBuf = (void *)pTempBytePtr;

				bufByteCnt -= eByte;
			}

			if (eShortAndByteAlign == bufAlignment) break;

			//If eByte case fall through to eShort case
		case eShortAlign:
			if ( bufByteCnt > 2 ) 
			{
				pTempShortPtr = (unsigned short *)pTempBuf;
		 
				*((unsigned short *) ulMsgAddress) = * pTempShortPtr;
				pTempShortPtr++;

				pTempBuf = (void *)pTempShortPtr;

				bufByteCnt -=eShort;
			}

			break;
	}

	// Process the whiole number of eWords
	{
		unsigned int wordCnt;
		unsigned int remBytes = bufByteCnt % eWord;
		if ( 0 == remBytes )
		{
			//Must leave one word to terminate the STM message 
			wordCnt = (bufByteCnt/eWord) - 1;
		}
		else
		{
			//Since there are some byte remainder
			wordCnt = (bufByteCnt/eWord);
		}
		
		pTempWordPtr = (unsigned long *)pTempBuf;
		for (iElementIndex = 0; iElementIndex < wordCnt; iElementIndex++)
		{	 
			*((unsigned long *) ulMsgAddress) = * pTempWordPtr;
			pTempWordPtr++;
			bufByteCnt -= eWord;
		}

		pTempBuf = (void *)pTempWordPtr;
		  
		//Check for the case where there are 3 bytes left on a word boundary
		if ( bufByteCnt == 3 )
		{
				pTempShortPtr = (unsigned short *)pTempBuf;

				*((unsigned short *) ulMsgAddress) = * pTempShortPtr;
				pTempShortPtr++;
				pTempBuf = (void *)pTempShortPtr;
				bufByteCnt -= eShort;	
		}
	}

	//Process the end of the message

	// Byte count must be 1, 2 or 4 at this point
	// and aligned to the proper address.
	switch (bufByteCnt)
	{
		case eByte:
			pTempBytePtr = (unsigned char *)pTempBuf;
 
			*((unsigned char *) ulEndAddress) = * pTempBytePtr;
			break;

		case eShort:
			pTempShortPtr = (unsigned short *)pTempBuf;

			*((unsigned short *) ulEndAddress) = * pTempShortPtr;
			break;

		case eWord:
			pTempWordPtr = (unsigned long *)pTempBuf;

			*((unsigned long *) ulEndAddress) = * pTempWordPtr;
			break;	
	}

	return eSTM_SUCCESS;
}
#endif

#ifdef _STM
// All DMA support is removed in compact build mode
eSTM_STATUS STM_putDMA(STMHandle* pSTMHandle, int chNum, void* pMsgBuf, unsigned int iElementCnt, eSTMElementSize iElementSize, bool DMASingleAccessMode,  STMXport_callback pCallBack)
{
	eSTM_STATUS retValue = eSTM_SUCCESS;

	//Even if the DMA unit is not busy must still put the message in the
	//link list of messages so we know what to do with the message when complete
	STMMsgObj * pMsgObj;
	int msgObjBytes = sizeof(STMMsgObj);
	if ( (pSTMHandle->BufUsed + msgObjBytes) > pSTMHandle->BufIOInfo.maxBufSize )
	{
		return eSTM_ERROR_BUF_ALLOCATION;
	}
	
	pMsgObj = (STMMsgObj *)STM_memAlloc(sizeof(STMMsgObj));
	if (NULL == pMsgObj )
	{
	    return eSTM_ERROR_HEAP_ALLOCATION;
	}

	pSTMHandle->BufUsed += msgObjBytes;
    
    //Fill in the message object  
	pMsgObj->pMsg = pMsgBuf;
	pMsgObj->elementCnt = iElementCnt;
	pMsgObj->elementSize = iElementSize;
	pMsgObj->chNum = chNum;
	pMsgObj->pCallBack = pCallBack;
	pMsgObj->pNextMsg = NULL;
	pMsgObj->DMA_SingleAccessMode = DMASingleAccessMode;
	pMsgObj->DMA_posted = false;

	//Put the message object in the link list
	if ( NULL == pSTMHandle->pHeadMsgObj )
	{
		// Put this message at the head of the link list
		 pSTMHandle->pHeadMsgObj = pMsgObj;
	}
	else {
		// Find the last message in the link list
		STMMsgObj * pTmpMsgObj = pSTMHandle->pHeadMsgObj;
		while ( pTmpMsgObj->pNextMsg != NULL )
		{
			pTmpMsgObj = pTmpMsgObj->pNextMsg;
		}
		//Found the end of the link
		pTmpMsgObj->pNextMsg = pMsgObj;

	}
			 
	//the DMA channel is not busy can post the pending message
	retValue = DMA_checkStatus(pSTMHandle);
	if ( eSTM_SUCCESS == retValue ) 
	{
		//If the message at the head has not been posted
		if ( false == pSTMHandle->pHeadMsgObj->DMA_posted )
		{
			DMA_postMsg(pSTMHandle, pSTMHandle->pHeadMsgObj);
		}
		else if ( false == pSTMHandle->BufIOInfo.usingDMAInterrupt )
		{
		//If the message at the head has been posted

			if ( NULL != pSTMHandle->pHeadMsgObj->pCallBack )
			{
				//If the callback is not NULL call it
				pSTMHandle->pHeadMsgObj->pCallBack(retValue);	
			}
			DMA_serviceNextMsg(pSTMHandle);	

		}

	}
	return retValue;
}


eSTM_STATUS DMA_checkStatus(STMHandle* pSTMHandle)
{

	int dmaCh1 = pSTMHandle->BufIOInfo.DMAChan_0;
	unsigned long DMA_chControl = *(unsigned long * )DMA_ChCntl(dmaCh1);
	unsigned long DMA_chStatus = *(unsigned long * )DMA_ChStatus(dmaCh1);
	unsigned long DMA_chErrors = DMA_chStatus & DMA_ERROR_MASK;
	eSTM_STATUS retValue = eSTM_SUCCESS;

	// If DMA is not enabled then return eSTM_SUCCESS
	// else since DMA is eabled check for error and pending cases
	if ( true == ( ( DMA_chControl & DMA_ENABLE) == DMA_ENABLE ) ) 
	{

		if ( ( 0 != DMA_chErrors ) && ( DMA_chStatus & DMA_FRAME_COMPLETE ) )
		{
			switch ( DMA_chErrors) {
				case DMA_SYSTEM_ERROR:
					retValue = eSTM_ERROR_SYSTEM;
					break;
				case DMA_MISSALIGND_ERROR:
					retValue = eSTM_ERROR_MISSALIGNED_ADDR;
					break;
				case DMA_SUPERVISOR_ERROR:
					retValue = eSTM_ERROR_SUPERVISOR;
					break;
				case DMA_SECURE_ERROR:
					retValue = eSTM_ERROR_SECURE;
					break;
				case DMA_TRASACTION_ERROR:
					retValue = eSTM_ERROR_TRANSACTION;
					break;					
			}
		}
		else if ( false == (DMA_chStatus & DMA_FRAME_COMPLETE))
		{
			// If no errors but DMA is busy return eSTM_PENDING
			retValue = eSTM_PENDING;
		}
		
		//if complete and no errors retValue already set to eSTM_SUCCESS		 
	}

	return retValue;
}

void DMA_serviceNextMsg(STMHandle* pSTMHandle)
{

	// Free the original message
	STM_memFree(pSTMHandle->pHeadMsgObj->pMsg);
	pSTMHandle->BufUsed -= ( pSTMHandle->pHeadMsgObj->elementCnt * pSTMHandle->pHeadMsgObj->elementSize );

	// If the next message is not NULL post it to the DMA unit
	if ( NULL != pSTMHandle->pHeadMsgObj->pNextMsg )
	{
		//Get the next message
		STMMsgObj * pNextMsgObj = pSTMHandle->pHeadMsgObj->pNextMsg;

		//Free the current msg from the head of the link list
		STM_memFree(pSTMHandle->pHeadMsgObj);
		pSTMHandle->BufUsed -= sizeof(STMMsgObj);
		
		//Update the head of the link list
		pSTMHandle->pHeadMsgObj = pNextMsgObj;

		//Post the message to the DMA
		DMA_postMsg(pSTMHandle, pSTMHandle->pHeadMsgObj);
	}
	else
	{
		//Free the current msg from the head of the link list
		STM_memFree(pSTMHandle->pHeadMsgObj);
		pSTMHandle->BufUsed -= sizeof(STMMsgObj);
		//Update the head of the link list
		pSTMHandle->pHeadMsgObj = NULL;	
	}	

}

void DMA_postMsg(STMHandle* pSTMHandle, STMMsgObj * pMsgObj)
{
	unsigned long ulMsgAddress = 0;
	unsigned long ulEndAddress = 0;
	int dmaCh1 = pSTMHandle->BufIOInfo.DMAChan_0;
	int dmaCh2 = pSTMHandle->BufIOInfo.DMAChan_1;
	unsigned long ulDMA_ElemSize = 0;

	char * pTempPtr = (char *)pMsgObj->pMsg; //was char * pTempPtr = (char*)pMsgBuf;
	void * pMsgBuf = pMsgObj->pMsg;
	int chNum = pMsgObj->chNum; 
	unsigned int iElementCnt = pMsgObj->elementCnt;
	eSTMElementSize iElementSize = pMsgObj->elementSize;
	bool DMASingleAccessMode = pMsgObj->DMA_SingleAccessMode;
	

	// This function assumes the calling routine has already determined the DMA is free
	ulMsgAddress = Compose_Address(pSTMHandle->ulBaseAddress,chNum,pSTMHandle->chResolution, eRegular);
	ulEndAddress = Compose_Address(pSTMHandle->ulBaseAddress,chNum,pSTMHandle->chResolution, eTiming);

#if 0
	// Note - will want to eanble DMA to complete when Msuspend(EMU Suspend) set
	if ( DMAFirstTime ) {
		*(unsigned long * )DMA_OCPCntl |= 0x2; // Reset - will want to move this to open, although
			 							   // eventually the DMA reset should come from the calling app.

		*(unsigned long * )DMA_FIFO = (unsigned int)0x01010020; // May ned to move this to open
 		DMAFirstTime = 0;
	}
#endif
	pTempPtr += (iElementCnt -1)*iElementSize;

	//Default for ulDMA_ElemSize is 0 for eByte
	if ( eByte != iElementSize ) 
	{
		ulDMA_ElemSize = (eShort == iElementSize) ? 1: 2; // short is 1, word is 2
	}

	//Program channel for header and body (less one element)
	*(unsigned long * )DMA_ChCntl(dmaCh1) = 0x1000; //Do not increment destination address

	if ( true == DMASingleAccessMode )
	{
		*(unsigned long * )DMA_ChSrcDstPrms(dmaCh1) = 0x10000 | ulDMA_ElemSize; //Single access
	}
	else
	{
		*(unsigned long * )DMA_ChSrcDstPrms(dmaCh1) = 0x1E1C0 | ulDMA_ElemSize; //Burst reads/writes & pack
	}
 
	//	*(unsigned long * )DMA_ChElem(dmaChl) = (unsigned int)iByteWritten/4;
	*(unsigned long * )DMA_ChElem(dmaCh1) = iElementCnt-1;
	*(unsigned long * )DMA_ChFram(dmaCh1) = 1;
	*(unsigned long * )DMA_ChSrcAdd(dmaCh1) = (unsigned long)pMsgBuf;
	*(unsigned long * )DMA_ChDstAdd(dmaCh1) = ulMsgAddress;
	*(unsigned long * )DMA_ChLnkCntl(dmaCh1) = 0x8000 | (dmaCh2); //Link to next channel
	*(unsigned long * )DMA_ChStatus(dmaCh1) = 0xE; //Clear the end of frame status bit

	//Program next channel for end address xfer

	*(unsigned long * )DMA_ChCntl(dmaCh2) = 0x1000; //Do not increment destination address

	if ( true == DMASingleAccessMode )
	{
		*(unsigned long * )DMA_ChSrcDstPrms(dmaCh2) = 0x10000 | ulDMA_ElemSize; //Single access
	}
	else
 	{
		*(unsigned long * )DMA_ChSrcDstPrms(dmaCh2) = 0x1E1C0 | ulDMA_ElemSize; //Burst reads/writes & pack
	}

	*(unsigned long * )DMA_ChElem(dmaCh2) = (unsigned long)1;
	*(unsigned long * )DMA_ChFram(dmaCh2) = 1;
	*(unsigned long * )DMA_ChSrcAdd(dmaCh2) = (unsigned long)pTempPtr;
	*(unsigned long * )DMA_ChDstAdd(dmaCh2) = ulEndAddress;
	*(unsigned long * )DMA_ChLnkCntl(dmaCh2) = 0; //End of link
	*(unsigned long * )DMA_ChStatus(dmaCh2) = 0xE; //Clear the end of frame status bit

	*(unsigned long * )DMA_ChCntl(dmaCh1) |= 0x100; //Enable complete on EMUSuspend
	*(unsigned long * )DMA_ChCntl(dmaCh2) |= 0x100; //Enable complete on EMUSuspend

	*(unsigned long * )DMA_ChCntl(dmaCh1) |= 0x80; //Enable the transfer

	pSTMHandle->pHeadMsgObj->DMA_posted = true;	
}
#endif

eSTM_STATUS STMXport_DMAIntService(STMHandle* pSTMHandle)
{
#ifdef _STM
	eSTM_STATUS retValue = eSTM_ERROR_INTDISABLED;
#ifdef _DEBUG
	VALIDATE_NULL_VALUE(pSTMHandle);
#endif
	//If interface open for blocking, then there should be no interrupts to service
	if ( false == pSTMHandle->BufIOEnable )
	{
		return retValue;
	}

	if ( true == pSTMHandle->BufIOInfo.usingDMAInterrupt )
	{
		//Get the current status
		eSTM_STATUS retValue = DMA_checkStatus(pSTMHandle);
			
		//If the IRQ status bit is not set then exit
		unsigned long dmaIrqStatus = *(unsigned long *)DMA_IrqStatus(pSTMHandle->BufIOInfo.DMAIrq);
		dmaIrqStatus = dmaIrqStatus & ( 1 << pSTMHandle->BufIOInfo.DMAChan_0);
		if (0 == dmaIrqStatus) {
			return (eSTM_ERROR_INTINVALID);
		}

		//Clear the IRQ status bit
		*(unsigned long *)DMA_IrqStatus(pSTMHandle->BufIOInfo.DMAIrq) = 1 << pSTMHandle->BufIOInfo.DMAChan_0;

		//If the message at the head has not been posted yet then this must be invalid
		if ( true != pSTMHandle->pHeadMsgObj->DMA_posted )
		{
			return (eSTM_ERROR_INTINVALID);
		}

		if ( NULL != pSTMHandle->pHeadMsgObj->pCallBack )
		{
			//If the callback is not NULL call it
			pSTMHandle->pHeadMsgObj->pCallBack(retValue);	
		}
			
		// If DMA status is not busy (not pending and no errors)
		if ( eSTM_SUCCESS == retValue ) 
		{
			DMA_serviceNextMsg(pSTMHandle);	
		}		
	}

	return retValue;
#endif
#if defined(_CIO) || defined(_COMPACT)
	return eSTM_ERROR_INVALID_FUNC;
#endif
}

#ifdef _STM
void Build_OST_Header(long protocolID, long numberOfBytes, long *pReturnBuf, int *pBufSizeInBytes)
{
		
	if (numberOfBytes >= OST_SHORT_HEADER_LENGTH_LIMIT)
	{
		*pReturnBuf = OST_VERSION | OST_ENTITY_ID | protocolID | OST_SHORT_HEADER_LENGTH_LIMIT;
		pReturnBuf++;
		*(pReturnBuf) = (numberOfBytes);
		*pBufSizeInBytes = 8; /* 8 bytes for the extended length header */
	}
	else
	{
		*pReturnBuf = OST_VERSION | OST_ENTITY_ID | protocolID | numberOfBytes;
		*pBufSizeInBytes = 4; /* 4 bytes for the normal header */
	}

	return;
}
#endif

#if defined(_STM) || ( defined(_COMPACT) && !defined(_CIO))
inline unsigned long Build_CompactOST_Header(long protocolID, long numberOfBytes )
{
	
	return OST_VERSION | OST_ENTITY_ID | protocolID | numberOfBytes;
	
}

inline Build_ExtendedOST_Header(long protocolID, long numberOfBytes, long *pReturnBuf)
{

	*pReturnBuf = (unsigned long)OST_VERSION | OST_ENTITY_ID | protocolID | OST_SHORT_HEADER_LENGTH_LIMIT;
	pReturnBuf++;
	*(pReturnBuf) = numberOfBytes;
	return;
}


void Compose_OST_MSG(const char *pInputBuf, int iInputBufSize, const char *pOSTHeader, int iHeaderSizeInByte, char *pReturnBuf)
{
	if ( 0 != iHeaderSizeInByte ) {
		memcpy(pReturnBuf, pOSTHeader, iHeaderSizeInByte);
		pReturnBuf += iHeaderSizeInByte;  /* we know that OST header is word aligned */
	}
	memcpy(pReturnBuf, pInputBuf, iInputBufSize); 
}

#endif
eAddrAlignment AddressAlignmentChecker(unsigned long addressValue)
{
	eAddrAlignment addressAlignment;

	if (addressValue % STM_WORD_SIZE == 0)
	{
		addressAlignment = eWordAlign;
	}
	else if (addressValue % STM_WORD_SIZE == eShortAndByteAlign)
	{
		addressAlignment = eShortAndByteAlign;
	}
	else if (addressValue % STM_WORD_SIZE == eShortAlign)
	{
		addressAlignment = eShortAlign;
	}
	else
	{	
		addressAlignment = eByteAlign;
	}

	return addressAlignment;
}


