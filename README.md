# Raptor
Based on the reverse-engineered codebase from Raptor Call Of The Shadows by nukeykt

## Media
Click on the thumbnail to watch some videos showing the project in action  

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/Nt2HfchiudY/0.jpg)](https://www.youtube.com/channel/UCedmTTlonJK5DvkiMpA_teQ)
## Information
Original Raptor Call Of The Shadows author Scott Host is working on a new modernized version of the classic called Raptor Remixed. If you are a Raptor fan it would be cool if you would support the project on Kickstarter. For more information visit [www.mking.com](https://www.mking.com)

## Quick start
Release builds are available for Windows, macOS and Android.
To download the latest release build for your platform, click [Download](https://github.com/skynettx/raptor/releases/latest).
Then install the downloaded release build on your system by following the instructions of the installer.
You then need to obtain the assets (GLB files) of the shareware or full version 1.2 or higher yourself and copy them into the installation directory or the external system specific directory.
That's it now Raptor is ready to play.
For those who want to build themselves or who want more configuration information, continue below otherwise you're done here.

## Installing
You need the original assets (GLB files) from Raptor Call Of The Shadows shareware or full version 1.2 or higher.
Important: No older versions before 1.2 are compatible! You have to take care of these files yourself.  
The assets can be loaded from the current working directory (Raptor directory) or from the external system specific directory.
**Please note that the release build version 0.8.0 does not support the external system specific directory and manages all assets, config and save files in the current working directory.**
The external system specific directories are the following:
```
 Windows: Users\Username\AppData\Roaming\Raptor\  
 Linux: ~/.local/share/Raptor/
 macOS: ~/Library/Application Support/Raptor/
 Android: storage/emulated/0/Android/data/com.raptor.skynettx/files/
```
The config file `SETUP.INI` and the save files are also loaded and saved in these folders.
On other systems that are not officially supported, the Raptor directory is used for loading and saving the config file and the save files.  
Copy the `raptor.exe` (Windows) or `raptor` (Linux or macOS) from build directory to Raptor directory. 
Under Windows copy the file `SDL.dll` from `include\SDL2-devel-2.28.2-VC\SDL2-2.28.2\lib\x86\` for 32 bit installation or for 64 bit installation from 
`include\SDL2-devel-2.28.2-VC\SDL2-2.28.2\lib\x64\` to Raptor directory.
Under Linux install lib-sdl2 from the packagemanager of your respective distro. When you use macOS install lib-sdl2 from dmg or from a packagemanager like brew etc. 
On an Android device, the APK can be installed via your preferred file manager.
### Shareware
Copy the following files to Raptor or external system specific directory:  
```
FILE0000.GLB  
FILE0001.GLB  
```
### Full version
Copy the following files to Raptor or external system specific directory:  
```
FILE0000.GLB  
FILE0001.GLB  
FILE0002.GLB  
FILE0003.GLB  
FILE0004.GLB  
```
### Configuration
If no `SETUP.INI` file exists in the target directory, a default file will be created automatically on Windows, Linux, macOS and Android.
Furthermore, Raptor Setup can be used to create or edit the `SETUP.INI` (not available on Android), or edit it manually as follows.
Copy the `SETUP(ADLIB).INI` or the `SETUP(MIDI).INI` (MIDI is currently not supported on Android) file from build directory to external system specific directory (Windows, Linux, macOS and Android) or Raptor directory (only systems that are not officially supported) and rename it to `SETUP.INI`.  
For MIDI support via TinySoundFont, you need a GM-compatible soundfont in SF2 format, e.g., FluidR3GM.sf2.
You can specify the path to the soundfont in the `SETUP.INI` file:  
`SoundFont=SoundFont.sf2`  
To play with one of the following input devices set `Control=0` under the [Setup] section in the `SETUP.INI` file to:  
`Control=0` Keyboard  
`Control=1` Mouse  
`Control=2` Joystick (Game Controller)  
Haptic (Game Controller rumble support) can be switched off or on under the [Setup] section in the `SETUP.INI` file:  
`Haptic=0`  
`Haptic=1`  
If you want to turn off the menu pointer control via the joystick in mode `Control=2` and enable the new joystick menu control in all 
input modes, set under the [Setup] section in the `SETUP.INI` file:  
`joy_ipt_MenuNew=0`  
`joy_ipt_MenuNew=1`  
System MIDI support (Windows Multimedia, Linux ALSA, macOS CoreAudio and macOS CoreMIDI) can be switched off or on in the [Setup] section in the `SETUP.INI` file:  
`sys_midi=0`  
`sys_midi=1`  
For Windows Multimedia under Windows, the MIDI device is set under the [Setup] section in `SETUP.INI` file:  
`winmm_mpu_device=0`  
Client and port for ALSA MIDI under Linux, is set under the [Setup] section in the `SETUP.INI` file:  
`alsa_output_client=128`  
`alsa_output_port=0`  
To use a software synthesizer with ALSA MIDI, install for example timidity and soundfont-fluid from the packagemanager of your distro.  
CoreAudio support on macOS with software synthesizer DLS synth can be switched off or on under the [Setup] section in the `SETUP.INI` file:  
`core_dls_synth=0`  
`core_dls_synth=1`  
If you want to use CoreMIDI under macOS, `core_dls_synth=0` must be switched off in the `SETUP.INI` file.  
You can set the CoreMIDI port under the [Setup] section in the `SETUP.INI` file as follows:  
`core_midi_port=0`  
To display the text mode ending screen after the game quits in fullscreen mode set under the [Video] section in the `SETUP.INI` file:  
`txt_fullscreen=0`  
`txt_fullscreen=1`  

## Build
The project supports the compilers msvc, gcc and clang.  
When you have installed git on your system you can clone the repository by type in `git clone https://github.com/skynettx/raptor.git`.

### Windows
You can use the projectfile for Visual Studio 2022 under `msvc\` or the projectfile for CodeBlocks under `gcc\`.

### Linux
Please remember to install the required dependencies lib-sdl2. In some distros there is an extra libsdl2-dev package like Debian or Ubuntu.  
You can use the projectfile for CodeBlocks under `gcc\`.
Otherwise you can use CMake. To use CMake type in the root of the repository:   
```
mkdir build  
cd build  
cmake ..  
make  
```

### macOS
Install the required dependencies lib-sdl2. To build use CMake, type in the root of the repository:
```
mkdir build  
cd build  
cmake ..  
make  
```

### Android
Make sure CMake is installed in your Android Studio environment.
If this is the case, open the `android\` project folder in Android Studio and build the APK.

## License
Raptor is licensed under the [GPLv2](https://github.com/skynettx/raptor/blob/master/LICENSE) or later.  
Raptor for Android is licensed under the [GPLv3](https://github.com/skynettx/raptor/blob/master/pkg/android/license/LICENSE) to be compatible with Apache 2.0 licensed libraries.

## FAQ
1. No audio under Linux:  
Make sure you get all the necessary ALSA and or PulseAudio dependencies from the packagemanager of your distro. 
2. Where can i change the video settings:  
The video settings can be set in the config file `SETUP.INI`. To toggle fullscreen mode on edit under the [Video] section `fullscreen=0`
to `fullscreen=1`. Or aspect ratio mode off `aspect_ratio_correct=1` to `aspect_ratio_correct=0`. 

## Thanks
Special thanks to [nukeykt](https://github.com/nukeykt) and [wel97459](https://github.com/wel97459) for their great work on the reconstructed source code.
Big thanks to [Scott Host](https://www.mking.com) for his great support.
Also many thanks to [schellingb](https://github.com/schellingb) for the great TinySoundFont library and to all contributors from the
[chocolate-doom project](https://github.com/chocolate-doom) for the awesome libtextscreen.


 




