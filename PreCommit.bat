call "%VS120COMNTOOLS%VsDevCmd.bat"

set FLAG1=/p:Configuration=Debug
set FLAG2=/p:Configuration=Release

msbuild /t:Clean %FLAG1%
msbuild /t:Build %FLAG1%
IF %ERRORLEVEL% NEQ 0 SET ERROR=1

msbuild /t:Clean %FLAG2%
msbuild /t:Build %FLAG2%
IF %ERRORLEVEL% NEQ 0 SET ERROR=1

MSTest /testcontainer:bin\Debug\ProofCheckerTest.dll
IF %ERRORLEVEL% NEQ 0 SET ERROR=1

MSTest /testcontainer:bin\Release\ProofCheckerTest.dll
IF %ERRORLEVEL% NEQ 0 SET ERROR=1

@if DEFINED ERROR (
	call ColorText C "There were errors..."
) else ( call ColorText A "Success!" )
echo.
pause