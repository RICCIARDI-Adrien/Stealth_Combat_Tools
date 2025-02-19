# Stealth Combat Tools

Contain some tools to mod Stealth Combat - Ultimate War video game.

* IDP archive : allows to extract IDP files, like `SCom.idp`, which contain the game resources.
* Map files : allows to extract some resources from a game map (the `SCom.idp` archive must be first extracted to get access to the `app/maps` directory containing the maps).

# Usage

This guide is designed for Windows 10 and Windows 11 with a x86_64 (64-bit) processor.

## Extracting the game resource

1. Install Stealth Combat on your computer. Let's assume that you installed the game to the default directory `C:\Program Files (x86)\Deck13\Stealth Combat - Ultimate War`.
1. Copy the game directory `C:\Program Files (x86)\Deck13\Stealth Combat - Ultimate War` and all its content to your desktop, this will allow to modify the game files without being annoyed by the Windows protections on the `Program Files (x86)` directory.
1. Build the Stealth Combat Tools command-line program using Visual Studio Community 2022 or higher. You can also download a prebuilt executable named `Stealth_Combat_Tools.exe` [here](https://github.com/RICCIARDI-Adrien/Stealth_Combat_Tools/releases) (select the latest one).
1. Using the Windows Explorer, go to the directory where the `Stealth_Combat_Tools.exe` has been built or downloaded.
1. Hold the Shift key then right-click with the mouse. In the opened context menu, select "Open PowerShell window here". The Windows command-line interface window will open.
1. Assuming that you have made a copy of the game on your desktop, copy the following command into the Windows command-line interface and press Enter to execute it :
   1. `.\Stealth_Combat_Tools.exe -idp-extract "$env:USERPROFILE\Desktop\Stealth Combat - Ultimate War\SCom.idp" "$env:USERPROFILE\Desktop\Stealth Combat - Ultimate War"`
   1. After a while, the tool will finish to extract all game resources and will display `All files were successfully created.`.

## Moding the game

* **Important** : to make the Stealth Combat game on your Windows desktop use the extracted game resource from the newly created `App` directory, you have to rename the `SCom.idp` file (for instance you can rename it to `SCom.idp.bak`). Otherwise, the `SCom.idp` resource will be used.
* You can mod the `App` directory content, then start the game by running the `SCom.exe` executable.
* If you broke the game when modifying it, just delete the `App` directory and extract it again to have a fresh copy.

# Technical information

## IDP file structure

* Offset 0 to 3 : "IDPK" : IDP file signature
* Offset 4 to 7 : 0x64 : IDP file version
* Offset 8 to 11 : 0x1101 : tags count
* Offset 12 to <value determined when all tag names are parsed> : tags
* Offset <value determined when all tag names are parsed> + 1 to file end : tags data

### Tag structure

* 4 bytes : tag name size. Tag name is an ASCIIZ string, terminating zero is included in name size. For example, "app\rules_weapon.txt" tag name has a size of 0x15 bytes.
* tag name size bytes : tag name ASCIIZ string.
* 4 bytes : tag data offset from data section start.
* 4 bytes : tag data size in bytes.
* 8 bytes : unknown, maybe flags.

## MovieCam

Each map contains a disabled entity called "Movie Camera". This entity, when enabled, is part of your team and can be controlled as a classic flying unit. It allows to fly everywhere on the map (it's a developers feature).  
Make sure flying units automatic stabilizations are disabled in game controls or you won't be able to use the camera.

To enable Movie Camera, add the line `moviecam		activate;` at the end of the `units` section of a script file (script files are located in `App\scripts`).  
For instance, first EMA mission script file (`App\scripts\ema1.txt`) would look like below with Movie Camera enabled  :
```
/*__________________________________________________________________________*/
/* units */

scenery			activate;
buildings		activate;
outer_fence		activate;
inner_fence		activate;
inner_wall		activate;
enemyposition1		activate;
village_zaun		activate;
actiontgt		activate;
htscenery		activate;
rocks			activate;
boxes			activate;
towers			activate;
gate			activate;	
patroltank		activate;
patroltank2		activate;	
patroltank3		activate;
patroltank4		activate;
patroltank5		activate;
patroltank6		activate;
gbuildings		activate;
vegetation_north	activate;
nav_alpha		activate;
blockade		activate;
mowag			activate;
moviecam		activate;

/*__________________________________________________________________________*/
/* actions */
...
```
