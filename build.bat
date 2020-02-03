@echo off

  find /c "#define SERVER 0" %1
    if %errorlevel% equ 1 goto notfound
    robocopy %2 %3
    goto done
    :notfound
    robocopy %2 %4
    goto done
    :done
	EXIT 0
