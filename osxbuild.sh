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
year="`date +%Y`"

if [ -f $filename ]; then
    rm $filename
fi
rm -rf pkg/osx/build
rm -rf pkg/osx/DerivedData

cat > src/rapver.h <<EOL
const char raptorwindowtitle[] = "Raptor ${version}";
const char setuptitle[] = "Raptor Setup ver ${version}                                       (c) skynettx ${year}";
const char startver[] = "Raptor ver ${version} (c) skynettx ${year}";
EOL

cd pkg/osx/
mkdir build
mkdir build/Release
mkdir build/Release/Raptor.app
mkdir build/Release/Raptor.app/Contents
mkdir build/Release/Raptor.app/Contents/MacOS
mkdir build/Release/Raptor.app/Contents/Resources

cat > build/Release/Raptor.app/Contents/Info.plist <<EOL
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
        <key>CFBundleIdentifier</key>
        <string>com.skynettx.raptor</string>
	<key>CFBundleDevelopmentRegion</key>
	<string>English</string>
	<key>CFBundleDisplayName</key>
	<string>Raptor</string>
	<key>CFBundleExecutable</key>
	<string>raptor.sh</string>
	<key>CFBundleGetInfoString</key>
	<string>Raptor ${version}</string>
	<key>CFBundleIconFile</key>
	<string>raptor.icns</string>
	<key>CFBundleInfoDictionaryVersion</key>
	<string>6.0</string>
	<key>CFBundleName</key>
	<string>Raptor</string>
	<key>CFBundlePackageType</key>
	<string>APPL</string>
	<key>CFBundleShortVersionString</key>
	<string>${version}</string>
	<key>CFBundleVersion</key>
	<string>${version}</string>
	<key>NSHighResolutionCapable</key>
	<string>true</string>
	<key>NSPrincipalClass</key>
	<string>NSApplication</string>
	<key>NSHumanReadableCopyright</key>
	<string>Copyright (C) ${year} skynettx</string>
</dict>
</plist>
EOL

cd ../../
mkdir build
cd build
cmake -DCMAKE_OSX_ARCHITECTURES=$arch -DCMAKE_BUILD_TYPE=Release ..
make -j `sysctl -n hw.ncpu`
cd ..
cp -r build/bin/. pkg/osx/build/Release/Raptor.app/Contents/MacOS
cp -R /Library/Frameworks/SDL2.framework pkg/osx/build/Release/Raptor.app/Contents/MacOS
cp -r rsrc/raptor.icns pkg/osx/build/Release/Raptor.app/Contents/Resources
printf '#!/bin/sh\ncd "${0%%/*}"\n./raptor' >> pkg/osx/build/Release/Raptor.app/Contents/MacOS/raptor.sh
chmod 755 pkg/osx/build/Release/Raptor.app/Contents/MacOS/raptor.sh
install_name_tool -add_rpath @executable_path pkg/osx/build/Release/Raptor.app/Contents/MacOS/raptor

echo "Check Raptor.app contains all files"
if [ -f pkg/osx/build/Release/Raptor.app/Contents/MacOS/raptor ]; then
    echo -e "raptor \033[0;32mPASS\033[0m"
else
    echo -e "raptor \033[0;31mFAILED\033[0m"
    exit 1
fi
if [ -f pkg/osx/build/Release/Raptor.app/Contents/MacOS/raptor.sh ]; then
    echo -e "raptor.sh \033[0;32mPASS\033[0m"
else
    echo -e "raptor.sh \033[0;31mFAILED\033[0m"
    exit 1
fi
if [ -d pkg/osx/build/Release/Raptor.app/Contents/MacOS/SDL2.framework ]; then
    echo -e "SDL2.framework \033[0;32mPASS\033[0m"
else
    echo -e "SDL2.framework \033[0;31mFAILED\033[0m"
    exit 1
fi
if [ -f pkg/osx/build/Release/Raptor.app/Contents/Info.plist ]; then
    echo -e "Info.plist \033[0;32mPASS\033[0m"
else
    echo -e "Info.plist \033[0;31mFAILED\033[0m"
    exit 1
fi
if [ -f pkg/osx/build/Release/Raptor.app/Contents/Resources/raptor.icns ]; then
    echo -e "raptor.icns \033[0;32mPASS\033[0m"
else
    echo -e "raptor.icns \033[0;31mFAILED\033[0m"
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
