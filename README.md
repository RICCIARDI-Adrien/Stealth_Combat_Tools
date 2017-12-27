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
* 8 bytes : unknown, maybe flags
