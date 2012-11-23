/*
 *  Copyright (c) 2010-2011, Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  Contact information for paper mail:
 *  Texas Instruments
 *  Post Office Box 655303
 *  Dallas, Texas 75265
 *  Contact information:
 *  http://www-k.ext.ti.com/sc/technical-support/product-information-centers.htm?
 *  DCMP=TIHomeTracking&HQS=Other+OT+home_d_contact
 *  ============================================================================
 *
 */

/*  Include Header Files  */
#include "StmLibrary.h" 
#include <stdio.h>
#include <stdlib.h>

#if _LINUX
#include <sys/mman.h>
#include <fcntl.h>
#endif

#include <ti/timmosal/timm_osal_memory.h>

extern unsigned long STM_BaseAddress;
extern unsigned long STM_ChannelResolution;

extern STMHandle *phandle;
extern int iChannel;
extern STMXport_callback pSTMCallBack;


// The following memory management code must be provided for the STM Library
void * STM_memAlloc(size_t sizeInBytes)
{
    return (void *)TIMM_OSAL_MallocExtn(sizeInBytes,
              TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_EXT, NULL);
}

void STM_memFree(void *p)
{
    TIMM_OSAL_Free(p);
}

//Returns virtual address to region requested
void * STM_memMap(unsigned int phyAddr, unsigned int mapSizeInBytes) 
{
#if _LINUX

	int fd = 0; 
    void * pVirtualAddress = NULL;

	if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) 
	{
#ifdef _DEBUG
		printf("STM_memMap:: Device '/dev/mem' could not be opened with O_RDWR | O_SYNC attributes.\n");
#endif
		return NULL;
	} 

//Note: phyAddr may need to be aligned to a PAGE_SIZE and mapSizeInBytes may need to be
//	    a multiple of PAGE_SIZE. Since the STM physical size is either 256KB or 1MB 
//      this should not be an issue. It is assumed that the Virtual Address returned will
//      map to the first location of physical STM space. 
	pVirtualAddress = mmap(0, mapSizeInBytes, PROT_WRITE, MAP_SHARED, fd, phyAddr);

	close(fd); // close memory device handle
	if (pVirtualAddress == (void *) -1) 
	{
#ifdef _DEBUG
		printf("STM_memMap:: Unable to map physical address 0x%X for %d bytes\n", phyAddr, mapSizeInBytes);
#endif
		// close file handle
        return NULL;
	}

	return pVirtualAddress;

#else
#ifdef _DEBUG
		printf("STM_memMap:: Mapping physical address 0x%X for %d bytes\n", phyAddr, mapSizeInBytes);
#endif
	return (void *)phyAddr;

#endif
}

void STM_memUnMap(void * vAddr, unsigned int mapSizeInBytes)
{
#if _LINUX
	if( munmap(vAddr,  mapSizeInBytes) ==  -1 ) 
	{
#ifdef _DEBUG
		printf("STM_memUnMap:: Unable to unmap address 0x%X for %d bytes\n", (unsigned)vAddr, mapSizeInBytes);
#endif
		return;	
	}
#endif
#ifdef _DEBUG
		printf("STM_memUnMap:: unmap request for address 0x%X for %d bytes\n", (unsigned)vAddr, mapSizeInBytes);
#endif

}

// The following helper code is optional

void STMLog_ErrHandler( eSTM_STATUS stmStatus )
{

#ifdef _DEBUG
	char * errMsg = 0;
    unsigned long msgCnt, heapUsed;

	switch ( stmStatus ) {
		case eSTM_SUCCESS:
			errMsg = "stmStatus is eSTM_SUCCESS - nothing wrong";
			break;
		case eSTM_PENDING:
			errMsg = "stmStatus is eSTM_PENDING - pending waiting for the dma channel";
			break;
		case eSTM_ERROR_PARM:
			errMsg = "stmStatus is eSTM_ERROR_PARM - API parameter error ";
			break;
		case eSTM_ERROR_STRING_FORMAT:
			errMsg = "stmStatus is eSTM_ERROR_STRING_FORMAT - Format string error detected";
			break;
		case eSTM_ERROR_HEAP_ALLOCATION:
			errMsg = "stmStatus is eSTM_ERROR_HEAP_ALLOCATION - Malloc allocation returned null";
			break;
		case eSTM_ERROR_BUF_ALLOCATION:
			errMsg = "stmStatus is eSTM_ERROR_BUF_ALLOCATION - Requested buffer allocation will exceed max provided STMXport_open";
			break;
		case eSTM_ERROR_SYSTEM:
			errMsg = "stmStatus is eSTM_ERROR_SYSTEM - DMA error";
			break;
		case eSTM_ERROR_MISSALIGNED_ADDR:
			errMsg = "stmStatus is eSTM_ERROR_MISSALIGNED_ADDR - DMA error";
			break;
		case eSTM_ERROR_SUPERVISOR:
			errMsg = "stmStatus is eSTM_ERROR_SUPERVISOR - DMA error";
			break;
		case eSTM_ERROR_SECURE:
			errMsg = "stmStatus is eSTM_ERROR_SECURE - DMA error";
			break;
		case eSTM_ERROR_TRANSACTION:
			errMsg = "stmStatus is eSTM_ERROR_TRANSACTION - DMA error";
			break;
		case eSTM_ERROR_INTDISABLED:
			errMsg = "stmStatus is eSTM_ERROR_INTDISABLED - API call to STMXport_DMAIntService without enabling with STMXport_open";
			break;
		case eSTM_ERROR_INTINVALID:
			errMsg = "stmStatus is eSTM_SUCCESS";
			break;
		case eSTM_ERROR_INVALID_FUNC:
			errMsg = "stmStatus is eSTM_ERROR_INVALID_FUNC - API call not support for this build";
			break;
		case eSTM_ERROR_CIO_ERROR:
			errMsg = "stmStatus is eSTM_ERROR_CIO_ERROR - CIO Error";
			break;
		default:
			errMsg = "stmStatus is unknown";
			break;	
	}

	printf("STMLog_ErrHandler: %s\n", errMsg);


	if ( eSTM_ERROR_BUF_ALLOCATION == stmStatus ) {
		stmStatus = STMXport_getBufInfo(phandle, &msgCnt, &heapUsed);
		printf("STMLog_ErrHandler: Outstanding msg count %d heap used is %d\n",msgCnt, heapUsed);	
	}

#endif

	if (   ( eSTM_SUCCESS == stmStatus )
		|| ( eSTM_ERROR_INVALID_FUNC == stmStatus ) ) {
		return;
	}

#ifdef _DEBUG
	printf("STMLog_ErrHandler: Execution Aborted\n");
#endif
	STMXport_close (phandle);
	abort();
}

void STMLog_SendInfo()
{
	unsigned int major_version = (unsigned int)-1, minor_version = (unsigned int)-1;
	eSTM_STATUS stmStatus;

#ifndef _CIO
    stmStatus = STMXport_printf( phandle, iChannel, pSTMCallBack, "Demo::initLog base address 0x%x channel resolution 0x%x\0", (unsigned long)&STM_BaseAddress, (unsigned long)&STM_ChannelResolution);
	if (stmStatus < eSTM_SUCCESS )	STMLog_ErrHandler( stmStatus );
#endif
	stmStatus = STMXport_getVersion( phandle, &major_version, &minor_version );
	if (stmStatus < eSTM_SUCCESS )	STMLog_ErrHandler( stmStatus );
	
	stmStatus = STMXport_printf( phandle, iChannel, pSTMCallBack, "Demo::initLog STM Library revision is %d.%d\0", major_version, minor_version);
	if (stmStatus < eSTM_SUCCESS )	STMLog_ErrHandler( stmStatus );

}

void STMLog_CallBack(eSTM_STATUS stmStatus)
{
#ifdef _DEBUG
	unsigned long msgCnt, heapUsed;
#endif
	
	if (stmStatus < eSTM_SUCCESS ) {
		STMLog_ErrHandler( stmStatus );
	}
	
#ifdef _DEBUG
	if (stmStatus == eSTM_PENDING ) {
		STMXport_getBufInfo(phandle, &msgCnt, &heapUsed);
		if (msgCnt > 16) {
			printf("Pending Threshold Exceeded\n");
		}	
	}
#endif

}

void STMLog_EnableBufMode(unsigned int maxHeapAllowed, int dmaCh_Msg, int dmaCh_Ts, STMXport_callback arg_pSTMCallBack )
{
#if defined(_STM) || (defined(_COMPACT) && !defined(_CIO))
	STMBufObj * pSTMBufInfo;

	STMBufObj STMBufInfo;
	STMBufInfo.maxBufSize = maxHeapAllowed;
	STMBufInfo.DMAChan_0 = dmaCh_Msg;
	STMBufInfo.DMAChan_1 = dmaCh_Ts;
	STMBufInfo.DMAIrq = 0;
	STMBufInfo.usingDMAInterrupt = false;

	//pSTMCallBack is a global
	pSTMCallBack = arg_pSTMCallBack;
	//Using the callback allows us to check status and respond to errors without
	//adding the explicit status test after every API call

	pSTMBufInfo = & STMBufInfo;


	phandle = STMXport_open(pSTMBufInfo);
	if ( phandle == 0 ) exit(EXIT_FAILURE);
#ifdef _DEBUG
	STMXport_printf( phandle, iChannel, pSTMCallBack, "Demo::initSTMLogBuf API initialized for buffered non-blocking calls\0");

	STMLog_SendInfo();
#endif
#endif
#ifdef _CIO
	printf ("In CIO mode STMLog_EnableBufMode() call is not valid\n"); 
#endif
}

void STMLog_EnableBlkMode()
{
#if defined(_STM) || (defined(_COMPACT) && !defined(_CIO))
#ifdef _DEBUG
	eSTM_STATUS stmStatus;
#endif
	STMBufObj * pSTMBufInfo;

	pSTMCallBack = 0;
	pSTMBufInfo = 0;
	pSTMCallBack = 0; //Lets live dangerously and disable external callbacks 

	phandle = STMXport_open(pSTMBufInfo);
	if ( phandle == 0 ) exit(EXIT_FAILURE);
#ifdef _DEBUG
	//This is an error handling example with callbacks diabled - the remainder of this example app assumes callbacks are enabled
    stmStatus = STMXport_printf( phandle, iChannel, pSTMCallBack, "Demo::initSTMLogBlk API initialized for blocking calls\0");
	if (stmStatus < eSTM_SUCCESS )	STMLog_ErrHandler( stmStatus );

	STMLog_SendInfo();
#endif
#endif
#ifdef _CIO
	printf ("In CIO mode STMLog_EnableBlkMode() call is not valid\n"); 
#endif
}

void STMLog_EnableCIOMode()
{
#ifdef _CIO
	STMBufObj STMBufInfo;
	STMBufObj * pSTMBufInfo;

	pSTMCallBack = 0; //Lets live dangerously and disable external callbacks

#ifdef _DEBUG
// The following code is intended as a test of mmap and munmap.
// To port this to your system change the phyAddr and phySize
// to a valid memory area in your system. There is no attempt to 
// actually access the location. 
	{
		unsigned int phyAddr = 0xA0000; //PC's Video Ram Address
		unsigned int phySize = 1024; 
		void * newVAddr = STM_memMap(phyAddr, phySize);
		printf ("STMLog_EnableCIOMode:: STMmemMap Test newVAddr is 0x%X\n", newVAddr);
		STM_memUnMap(newVAddr, phySize);

	}
#endif
	
	STMBufInfo.fileAppend = false;
	STMBufInfo.pFileName = NULL; //"C:\\CCStudio_v3.3\\MyProjects\\STMLog.csv"; // NULL for stdout
	STMBufInfo.pMasterId = "Ducati";
 	pSTMBufInfo = & STMBufInfo;

	phandle = STMXport_open(pSTMBufInfo);
	if ( phandle == 0 ) exit(EXIT_FAILURE);
#ifdef _DEBUG
	//This is an error handling example with callbacks diabled - the remainder of this example app assumes callbacks are enabled
    stmStatus = STMXport_printf( phandle, iChannel, pSTMCallBack, "Demo::initSTMLogBlk API initialized for CIO calls\0");
	if (stmStatus < eSTM_SUCCESS )	STMLog_ErrHandler( stmStatus );


	STMLog_SendInfo();

#endif
#endif
}

void STMLog_checkFlush()
{
#if defined(_STM) || defined (_COMPACT)
	eSTM_STATUS stmStatus;
	unsigned long msgCnt, heapUsed;

	stmStatus = STMXport_getBufInfo(phandle, &msgCnt, &heapUsed);
	if (stmStatus < eSTM_SUCCESS ) STMLog_ErrHandler( stmStatus );
#ifdef _DEBUG
//	stmStatus = STMXport_printf( phandle, iChannel, pSTMCallBack, "Demo::exitSTMlog before flush - outstanding messages %d heap used %d\0", msgCnt, heapUsed);
//	if (stmStatus < eSTM_SUCCESS ) STMLog_ErrHandler( stmStatus );


//	printf("exitSTMlog before flush - outstanding messages %d heap used %d\n", msgCnt, heapUsed);
#endif

	//If there are any outstanding messages or any heap still allocated the flush will clean all this up
	if ( ( msgCnt > 0 ) || (heapUsed != 0) ) {
		STMXport_flush(phandle);
	}
#ifdef _DEBUG
	//This is for test purposes to confirm the flush worked as expected
//	stmStatus = STMXport_getBufInfo(phandle, &msgCnt, &heapUsed);
//	if (stmStatus < eSTM_SUCCESS ) STMLog_ErrHandler( stmStatus );
//	stmStatus = STMXport_printf( phandle, iChannel, pSTMCallBack, "Demo::exitSTMlog after flush - outstanding messages %d heap used %d\0", msgCnt, heapUsed);
//	if (stmStatus < eSTM_SUCCESS ) STMLog_ErrHandler( stmStatus );


//	printf("exitSTMlog after flush - outstanding messages %d heap used %d\n", msgCnt, heapUsed);
#endif
#endif
#ifdef _CIO
	//DO the CIO flush
	STMXport_flush(phandle);
#endif


}



void STMLog_exit()
{

	STMLog_checkFlush();
	STMXport_close (phandle);	

}

