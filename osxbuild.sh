#!/usr/bin/env bash

cd "$(dirname "$0")"
clear
cat << EOF
********************************************************************************
*                                                                              *
* Raptor build script for macOS                                                *
*                                                                              *
********************************************************************************

Select build:

  1. Release build aarch64
  2. Release build x86-64
  3. Clean all
  0. Exit

EOF

read -p "Select 0-3: " select

case $select in
        0) 
           echo "Abort"
           exit 1;;
        1)
           arch="arm64"
           archname="aarch64";;
        2) 
           arch="x86_64"
           archname="x86-64";;
        3)
           rm -rf build
           rm -rf pkg/osx/build
           rm -rf pkg/osx/DerivedData
           rm *.dmg
           echo "Cleaned all"
           exit 1;;
        *) 
           echo "Incorrect entry"
           exit 1
esac

read -p "Enter version number: " version
volumename="Raptor ${version} ${archname}"
filename="raptor-${version}-${archname}.dmg"

if [ -f $filename ]; then
    rm $filename
fi
rm -rf pkg/osx/build
rm -rf pkg/osx/DerivedData

cd pkg/osx/
xcodebuild -project raptorlauncher.xcodeproj ARCHS=$arch ONLY_ACTIVE_ARCH=No -configuration Release
cd ../../
mkdir build
cd build
cmake -DCMAKE_OSX_ARCHITECTURES=$arch -DCMAKE_BUILD_TYPE=Release ..
make -j `sysctl -n hw.ncpu`
cd ..
cp -r build/bin/. pkg/osx/build/Release/raptorlauncher.app/Contents/MacOS
cp -R /Library/Frameworks/SDL2.framework pkg/osx/build/Release/raptorlauncher.app/Contents/MacOS
printf '#!/bin/sh\ncd "${0%%/*}"\n./raptor' >> pkg/osx/build/Release/raptorlauncher.app/Contents/MacOS/raptor.sh
printf '#!/bin/sh\ncd "${0%%/*}"\n./raptorsetup' >> pkg/osx/build/Release/raptorlauncher.app/Contents/MacOS/raptorsetup.sh
chmod 755 pkg/osx/build/Release/raptorlauncher.app/Contents/MacOS/raptor.sh
chmod 755 pkg/osx/build/Release/raptorlauncher.app/Contents/MacOS/raptorsetup.sh
install_name_tool -add_rpath @executable_path pkg/osx/build/Release/raptorlauncher.app/Contents/MacOS/raptor
install_name_tool -add_rpath @executable_path pkg/osx/build/Release/raptorlauncher.app/Contents/MacOS/raptorsetup
mv pkg/osx/build/Release/raptorlauncher.app pkg/osx/build/Release/Raptor.app

echo "Check Raptor.app contains all files"
if [ -f pkg/osx/build/Release/Raptor.app/Contents/MacOS/raptorlauncher ]; then
    echo -e "raptorlauncher \033[0;32mPASS\033[0m"
else
    echo -e "raptorlauncher \033[0;31mFAILED\033[0m"
    exit 1
fi
if [ -f pkg/osx/build/Release/Raptor.app/Contents/MacOS/raptor ]; then
    echo -e "raptor \033[0;32mPASS\033[0m"
else
    echo -e "raptor \033[0;31mFAILED\033[0m"
    exit 1
fi
if [ -f pkg/osx/build/Release/Raptor.app/Contents/MacOS/raptorsetup ]; then
    echo -e "raptorsetup \033[0;32mPASS\033[0m"
else
    echo -e "raptorsetup \033[0;31mFAILED\033[0m"
    exit 1
fi
if [ -f pkg/osx/build/Release/Raptor.app/Contents/MacOS/raptor.sh ]; then
    echo -e "raptor.sh \033[0;32mPASS\033[0m"
else
    echo -e "raptor.sh \033[0;31mFAILED\033[0m"
    exit 1
fi
if [ -f pkg/osx/build/Release/Raptor.app/Contents/MacOS/raptorsetup.sh ]; then
    echo -e "raptorsetup.sh \033[0;32mPASS\033[0m"
else
    echo -e "raptorsetup.sh \033[0;31mFAILED\033[0m"
    exit 1
fi
if [ -d pkg/osx/build/Release/Raptor.app/Contents/MacOS/SDL2.framework ]; then
    echo -e "SDL2.framework \033[0;32mPASS\033[0m"
else
    echo -e "SDL2.framework \033[0;31mFAILED\033[0m"
    exit 1
fi

read -p "Bundle assets y/n: " assets
if [ "$assets" == "y" ] || [ "$assets" == "yes" ] || [ "$assets" == "Y" ] || [ "$assets" == "YES" ]; then
    read -p "Path to assets folder: " assetspath
    if [ "${assetspath: -1}" != "/" ]; then
        assetspath="${assetspath}/"
    fi
    cp -r "${assetspath}"*.GLB pkg/osx/build/Release/Raptor.app/Contents/MacOS
else
   echo "Continue without bundle assets"
fi

rm -rf pkg/osx/build/Release/raptorlauncher.app.dSYM

./pkg/osx/create-dmg/create-dmg \
  --volname "$volumename" \
  --volicon "rsrc/raptor.icns" \
  --background "rsrc/raptorback.png" \
  --window-pos 200 120 \
  --window-size 800 400 \
  --icon-size 100 \
  --icon "Raptor.app" 200 190 \
  --hide-extension "Raptor.app" \
  --app-drop-link 600 185 \
  --add-file "LICENSETSF" "include/TinySoundFont/LICENSE" 730 290 \
  --add-file "LICENSE" "LICENSE" 730 90 \
  $filename \
  "pkg/osx/build/Release/"
