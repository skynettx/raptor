# Raptor
Based on the reverse-engineered codebase from Raptor Call Of The Shadows by nukeykt

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/Nt2HfchiudY/0.jpg)](https://www.youtube.com/channel/UCedmTTlonJK5DvkiMpA_teQ)
## Installing
You need the original assets (GLB files) from Raptor Call Of The Shadows v1.2 shareware or fullversion.
Important only the DOS version v1.2 is compatible!
You have to take care of these files yourself.

### Shareware
1. Copy the following files to Raptor directory:  
   FILE0000.GLB  
   FILE0001.GLB  
2. Copy the `raptor.exe` and `SETUP(GRAVIS).INI or SETUP(ADLIB).INI or SETUP(MIDI).INI` files under Windows or the `raptor` and `SETUP(ADLIB).INI` files under Linux
   from build directory to Raptor directory. Under Windows copy the `SDL.dll` from `include\SDL2-devel-2.0.14-VC\SDL2-2.0.14\lib\x86\` 
   folder or from `include\SDL2-devel-2.0.14-mingw\SDL2-2.0.14\i686-w64-mingw32\bin\` folder (depending on which version you prefer) to Raptor directory.
   Under Linux install lib-sdl2 32 Bit from the packagemanager of your respective distro. 
3. The final folder should look like this:  
   FILE0000.GLB  
   FILE0001.GLB  
   SDL2.dll (only under Windows required)  
   SETUP(GRAVIS).INI or SETUP(ADLIB).INI or SETUP(MIDI).INI (under Windows) or SETUP(ADLIB).INI (under Linux)  
   raptor.exe (under Windows) or raptor (under Linux)

### Fullversion
1. Copy the following files to Raptor directory:  
   FILE0000.GLB  
   FILE0001.GLB  
   FILE0002.GLB  
   FILE0003.GLB  
   FILE0004.GLB  
2. Copy the `raptor.exe` and `SETUP(GRAVIS).INI or SETUP(ADLIB).INI or SETUP(MIDI).INI` files under Windows or the `raptor` and `SETUP(ADLIB).INI` files under Linux
   from build directory to Raptor directory. Under Windows copy the `SDL.dll` from `include\SDL2-devel-2.0.14-VC\SDL2-2.0.14\lib\x86\` 
   folder or from `include\SDL2-devel-2.0.14-mingw\SDL2-2.0.14\i686-w64-mingw32\bin\` folder (depending on which version you prefer) to Raptor directory.
   Under Linux install lib-sdl2 32 Bit from the packagemanager of your respective distro.
3. The final folder should look like this:  
   FILE0000.GLB  
   FILE0001.GLB  
   FILE0002.GLB  
   FILE0003.GLB  
   FILE0004.GLB  
   SDL2.dll (only under Windows required)  
   SETUP(GRAVIS).INI or SETUP(ADLIB).INI or SETUP(MIDI).INI (under Windows) or SETUP(ADLIB).INI (under Linux)  
   raptor.exe (under Windows) or raptor (under Linux)

### Configuration
You can build or download [Raptor Setup](https://github.com/skynettx/raptorsetup.git) to create and edit the
`SETUP.INI`, or edit it manually as follows.
Rename the `SETUP(GRAVIS).INI` or `SETUP(ADLIB).INI` or the `SETUP(MIDI).INI` file to `SETUP.INI` in Raptor directory.
To play with one of the following input devices set `Control=0` under the [Setup] section in the `SETUP.INI` file to:  
`Control=0` Keyboard  
`Control=1` Mouse  
`Control=2` Joystick (Game Controller)  
Haptic (Game Controller rumble support) can be switched off or on under the [Setup] section in the `SETUP.INI` file:  
`Haptic=0`  
`Haptic=1`  
If you want to turn off the menu pointer control via the joystick in mode `Control=2` and enable the new joystick menu control in all 
input modes, set under the [SETUP] section in the `SETUP.INI` file:  
`joy_ipt_MenuNew=0`  
`joy_ipt_MenuNew=1`  

## Build
The project supports the compilers msvc and gcc. Only 32 bit build is supported in current codebase. 
When you have installed git on your system you can clone the repository by type in `git clone https://github.com/skynettx/raptor.git`.

### Windows
You can use the projectfile for Visual Studio 2019 under `msvc\` or the projectfile for CodeBlocks under `gcc\`.
When you have set the PATH variable for mingw32 `set PATH=C:\mingw\bin;%PATH%` (where PATH=path to mingw\bin)
you can use the makefile by simply type in `mingw32-make` in `makefilewin\` folder in the repository. 

### Linux
Please remember to install the required dependencies lib-sdl2 32 Bit. In some distros there is an extra libsdl2-dev 32 Bit package like Debian or Ubuntu.  
You can use the projectfile for CodeBlocks under `gcc\`.
Otherwise you can use the makefile by simply type in `make` in `makefilelinux\` folder in the repository.

## FAQ
1. No audio under Linux:  
Make sure you get all the necessary 32 Bit Alsa and or PulseAudio dependencies from the packagemanager of your distro (remember 32 Bit not 64 Bit). 
2. Where can i change the video settings:  
The video settings can be set in the config file `SETUP.INI`. To toggle fullscreen mode on edit under the [Video] section `fullscreen=0`
to `fullscreen=1`. Or aspect ratio mode off `aspect_ratio_correct=1` to `aspect_ratio_correct=0`. 

## Thanks
Special thanks to nukeykt for his great work on the reconstructed source code.  


 




