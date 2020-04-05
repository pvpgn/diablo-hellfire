# Diablo: Hellfire
The source code to Diablo: Hellfire, the expansion to Diablo by Synergistic Software

https://archive.org/details/hellfire_source

http://www.oldpcgaming.net/diablo-hellfire-review/

![](https://i.imgur.com/9NdmmD6.png)


## How to compile

1. Install Visual C++ 6.0

    https://archive.org/details/MicrosoftVisualC6.0StandardEdition

3. Download the repository, switch to `\src` and open the workspace file `DIABLO.DSW` with VC++ 6.0

    There are two projects in the workspace: `Diablo files` (hellfire.exe) and `ui files` (hellfrui.dll)
   
    

4. From the main menu `Build > Rebuild` will create `\Windebug\hellfire.exe` and `\Windebug\hellfrui.dll`

	  Run `Z.BAT` will create `\WinRel\Diablo.exe` using a makefile `DIABLO.MAK` (probably it's for a production)
  


## How to debug

1. To be able to run the game (`\Windebug\hellfire.exe`) you have to download and place [HELLFIRE.MPQ](http://cdn.pvpgn.pro/diablo1/hellfire/HELLFIRE.MPQ) and [DIABDAT.MPQ](http://cdn.pvpgn.pro/diablo1/DIABDAT.MPQ) (from original Diablo: Retail) into `\Windebug` directory


2. In VC++ project open `Project > Settings`, switch tab `Debug` and set option `Working directory` to full path to your `\Windebug` directory.


3. `Build > Start Debug > Step Into` will set break point on entrypoint function `WinMain(..)` in `DIABLO.cpp`.

<img src="https://i.imgur.com/PocFOvk.png" width="49%" /> <img src="https://i.imgur.com/ukf4Nmz.png" width="49%" />

