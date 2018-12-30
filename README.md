# Stealth Combat Tools
Contain some tools to mod Stealth Combat - Ultimate War video game.

* IDP_Manager allows to uncompress IDP files, like SCom.idp, which contain game resources.

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
