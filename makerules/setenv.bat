REM Set environment batch file
@echo off

REM Set the top level ROOTDIR where "makerules" directory is found
set ROOTDIR=S:/avmecomp

REM Set path for Cygwin installation
set CYGWINPATH=D:/cygwin

REM CYGWIN options
set CYGWIN=nodosfilewarning

REM Append path for cygwin binaries, as there are a lot of POSIX commands that
REM        are used in the make files
set path=%CYGWINPATH%/bin;

echo Environment setup done. Please ensure environment variables are set correctly
echo =============================================================================
echo ROOTDIR   =%ROOTDIR%
echo CYGWINPATH=%CYGWINPATH%
echo PATH      =%path%
echo =============================================================================

REM Nothing beyond this point
