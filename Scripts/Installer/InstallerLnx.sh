#!/bin/bash

# CQTDEPLOYER BUILDING
qmakeDir=$1
cd ../../3rdparty/CQtDeployer
$qmakeDir/qmake -r
make -j$(nproc)

# QT-INSTALLER-FRAMEWORK BINARY FILES COPYING
cp $qmakeDir/../../../Tools/QtInstallerFramework/4.0/bin CQtDeployer/build/QIF -r

# INSTALLER CREATING
cd ../../Scripts/Installer
CQtDir=../../3rdparty/CQtDeployer
QuasarAppLib=$CQtDir/QuasarAppLib/build/release
DeployLib=$CQtDir/Deploy/build/release
PeParserLib=$CQtDir/pe/pe-parser-library/build/release
ZipLib=$CQtDir/zip/build/release
LibPathes=$QuasarAppLib:$DeployLib:$PeParserLib:$ZipLib
CQtExec=$CQtDir/CQtDeployer/build/release/cqtdeployer
ExecFile=../../build/bin/DS_Control
LD_LIBRARY_PATH=$LibPathes $CQtExec -bin $ExecFile -targetDir Installer qif
