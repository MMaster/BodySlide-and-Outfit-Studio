BodySlide-and-Outfit-Studio
===========================

BodySlide and Outfit Studio, a tool to convert, create, and customize outfits and bodies for The Elder Scrolls and Fallout.

Forked from main repo to add NOA Group Assigner functionality.

**NOA Group Assigner**

  Easy way to assign outfit from all outfits with same output to one of three groups (nude, outfit, armor). 

  Shows list of all outfits that would be built with batch build with the ability to choose one outfit for each output file and option to automatically assign that outfit to one of three groups.

  Supports preview of outfits. User can also pick preset for each of the groups to be used in the preview.
  
  Save & load groups to/from single group xml file, preserving items that were not touched. The file name decides the names of groups e.g. Beauty.xml will have groups "Beauty (nude)", "Beauty (outfit)" & "Beauty (armor)". 
  
**Why would I need this?**

  I usually have 3 groups of outfits: no support for breasts (nude), push-up effect (outfit) and one where outfit flattens the breasts (armor). I make separate preset for each group and batch build each preset separately with groups filter. Assigning outfits to the groups when you have many outfits that many times conflict was huge pain. Using the existing Group Manager was absolutely not an option for this task (I found out after 10 hours of trying, then I wrote python script for it and finally I've just made this.


**Created by and/or with the help of:**
* Caliente
* ousnius
* jonwd7 for NIF and general help
* degenerated1123 for help with shaders
* NifTools team
* MMaster (NOA Group Assigner)

**Libraries used:**
* OpenGL
* OpenGL Image (GLI)
* Simple OpenGL Image Library 2 (SOIL2)
* TinyXML-2
* FSEngine (BSA/BA2 library)
* Autodesk FBX SDK
* half - IEEE 754-based half-precision floating point library
* Miniball
* LZ4(F)
* wxWidgets

https://github.com/ousnius/BodySlide-and-Outfit-Studio/wiki
