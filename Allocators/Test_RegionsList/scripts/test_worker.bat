set ARCH=%1
set CONF=%2
set SUB_PATH=%3
set FLAGS=%4

mkdir %CD%\..\%ARCH%\%CONF%\%SUB_PATH%\

copy %CD%\..\%ARCH%\%CONF%\Test_RegionsList.exe %CD%\..\%ARCH%\%CONF%\%SUB_PATH%\
copy %CD%\..\%ARCH%\%CONF%\Errors.dll %CD%\..\%ARCH%\%CONF%\%SUB_PATH%\
copy %CD%\..\%ARCH%\%CONF%\Logger.dll %CD%\..\%ARCH%\%CONF%\%SUB_PATH%\

cd %CD%\..\%ARCH%\%CONF%\%SUB_PATH%\
Test_RegionsList.exe %FLAGS%