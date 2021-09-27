:: CQTDEPLOYER BUILDING
set qmakeDir=%1
cd ..\..\3rdparty\CQtDeployer
%qmakeDir%\qmake.exe -r
mingw32-make.exe -j%NUMBER_OF_PROCESSORS%

:: QT-INSTALLER-FRAMEWORK BINARY FILES COPYING
xcopy /s /i /y %qmakeDir%\..\..\..\Tools\QtInstallerFramework\4.0\bin CQtDeployer\build\QIF

:: INSTALLER CREATING
cd ..\..\Scripts\Installer
set cQtDir=..\..\3rdparty\CQtDeployer
set PATH=%PATH%;%cQtDir%\QuasarAppLib\build\release
set PATH=%PATH%;%cQtDir%\Deploy\build\release
set PATH=%PATH%;%cQtDir%\pe\pe-parser-library\build\release
set PATH=%PATH%;%cQtDir%\zip\build\release
set cQtExec=%cQtDir%\CQtDeployer\build\release\cqtdeployer
set execFile=..\..\build\bin\DS_Controll.exe
%cQtExec% -bin %execFile% -targetDir Installer qif
