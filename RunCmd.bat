 @echo off
 REM Call with first parameter as timeout, number of seconds to delay after command finishes
 REM Rest of parameters are the command to run and arguments to be passed to it
 REM If timeout = 0, always wait for user input. timeout < 0, wait for user only if error
 setlocal
 set errwait=0
 set dly=%1
 if %1 lss 0 set errwait=1 & set /A dly=0-%1
 set _tail=%*
 call set _tail=%%_tail:*%1=%%
 %_tail%
 if %ERRORLEVEL% neq 0 goto :cmderror
 if %dly% equ 0 goto :waitforuser

:tick
 timeout %dly%
 exit 0

:cmderror
	Echo "Error %ERRORLEVEL% processing command"
	if %errwait% equ 0 goto :tick
:waitforuser
	pause
	exit 0


