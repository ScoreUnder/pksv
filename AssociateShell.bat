@echo off
if exist pksv.exe goto ex
echo There is no PKSV here. Please move this to your PKSV folder.
goto end
:ex
reg add HKCR\.pks /f
reg add HKCR\.pks /d PKSVScript /t REG_SZ /ve /f
reg add HKCR\PKSVScript /f
reg add HKCR\PKSVScript /ve /d "PKSV Compiler Script" /t REG_SZ /f
reg add HKCR\PKSVScript\DefaultIcon /ve /d "%0\..\ball.ico,0" /t REG_SZ /f

REM Batch script is so limiting - look at that pathetic
REM excuse for a path name :(

reg add HKCR\PKSVScript\shell /ve /d edit /t REG_SZ /f
reg add HKCR\PKSVScript\shell\edit /ve /d "&Edit" /t REG_SZ /f
set asd="%%1"
reg add HKCR\PKSVScript\shell\edit\command /ve /d "notepad %asd%" /t REG_SZ /f
reg add HKCR\PKSVScript\shell\comp /ve /d "&Compile" /t REG_SZ /f
reg add HKCR\PKSVScript\shell\comp\command /ve /d "%0\..\pksv.exe -r %asd%" /t REG_SZ /f

:end
echo Press any key to quit.
pause>nul