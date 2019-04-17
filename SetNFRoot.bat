@echo off
REM SetNFRoot - Find/Create and return short path for current directory
REM DAV 01APR19
setlocal

if %1.==. goto :set_path

:check_opts
if /I %1==-h  goto :help
if /I %1==-d set rmsubst=-d

:set_path
set _mypath=%~dp0
call :setshortpath %_mypath%
rem pushd %shortpath%
if [%rmsubst%]==[-d] goto :remsubst
goto :end

:remsubst
set v=%shortpath:~0,2%
echo Remove subst of %mypath% on %v%
if [%rmsubst%]==[-d] subst /D %v%
endlocal & set "nfRoot=%_mypath%"
setx nfRoot %nfRoot% >nul
echo %nfRoot%
goto :eof

:end
:: Clear %errorlevel%
call set _a=a > nul
endlocal & set "nfRoot=%shortpath%"
setx nfRoot %nfRoot% >nul
echo %nfRoot%
rem  popd
::exit
goto :eof

:: ============ Utility commands =====

:: === Help
:help
echo:
echo Usage: %0 [opt]
echo:
echo opt = -d    Delete subst if exists
echo opt = -h    Display this help message
echo:
echo Command line tool to find or create a short directory path
echo for the current directory using the subst command
echo:
goto :eof

REM  ==========================================================
REM Utility Functions
REM
REM Call to set a short path to try avoid overflowing 250 char file path limit
:setshortpath
set "mypath=%~1"
set shortpath=%mypath%
rem echo mypath = %_mypath%

REM Are we already on short path?
set t=%mypath:~0,3%
if /I %t%==%mypath% goto :haveshortpath

REM Not on short path -  is there already a subst for it?
FOR /F "tokens=1,3" %%G IN ('subst') DO (
	if /I "%%H\"=="%mypath%" (
		set sdg=%%G
		goto :foundsubst
	)
)
REM No subst short path found - create one
call :freedrive driveletter && goto :cont
echo ERROR: No free drive letter found, staying on original path
goto :haveshortpath

:cont
echo Found free drive letter: %driveletter% 1>&2
pushd %mypath%
subst %driveletter% .
popd
set shortpath=%driveletter%\
echo Created new  subst for %mypath% on %driveletter% 1>&2
echo You can remove it with subst %driveletter% /D 1>&2
goto :haveshortpath

:foundsubst
set shortpath=%sdg:~0,3%
echo Using existing subst for %mypath% on %shortpath% 1>&2
goto :haveshortpath

:haveshortpath
echo Using short path %shortpath% for %mypath% 1>&2
REM So now we have a short path if it was possible
goto :eof

rem Finds a free drive letter.
rem
rem Parameters:
rem     %1 = Output variable name.
rem
rem Example:
rem     call :freedrive mydriveletter && goto :cont
rem     echo ERROR: No free drive letter found.
rem     goto :EOF
rem     :cont
rem     echo Found drive letter: %mydriveletter%
:freedrive
setlocal EnableDelayedExpansion
set exitcode=0
set "output_var=%~1"
for %%i in (B,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z) do (
    set "drive=%%i:"
    rem If 'subst' fails, the drive letter is already in use.
    rem This way we can even detect optical drives that have a drive
    rem letter but no media in them, a case that goes undetected when
    rem using 'if exist'.
	subst !drive! %SystemDrive%\ >nul
    if !errorlevel! == 0 (
        subst !drive! /d >nul
        goto :freedrive0
    )
)
set exitcode=1
set drive=
:freedrive0
endlocal & set "%output_var%=%drive%" & exit /b %exitcode%

:exit

:eof
