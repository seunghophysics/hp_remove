hp_remove
=========

This is a tool for removing most of the hot pixels in a digital camera image in ROOT format.  
Conversion of any raw camera image to ROOT is required before applying this program.



Compatibility
=============


Compiles and works well in macOS 10.13.4 with Xcode 9.3 and ROOT 6.10/08.  
Compiles and works well in Ubuntu 17.10 with GCC 7.2 and ROOT 6.12/04.



Quick Start
===========

```
> make hp_remove
```
```
> ./hp_remove [input root file] [output root file] [threshold]
```

Example Usage:
```
> ./hp_remove ../BPM_ROOT/r0033_0005.root test_out.root 250
```

Note: Choose threshold larger than the maximum value(bin content) in the signal.

To use a macro, edit and run hp_macro.sh.


Structure
=========

* **lib_hp.cc**

	A library of essential functions usable in both hot pixel removal and general analysis.

* **lib_hp.h**

	Header of lib_hp.cc. Include this library to use the library in your own program.		 

* **main_hp.cc**

	This is the main program. It reads an input root file and produces an output with hot pixels removed. 

* **Makefile**

	Contains Makefile information.

* **hp_map.root**

	 Contains a combined map of hot pixel candidates in the camera.

* **dark.card**

	In case hp_map.root does not exist, you can build your own hot pixel map.   
	Put the addresses of ROOT files with dark images in this card file.

* **hp_macro.sh**

	This is a macro for the main program (hp_remove) in case you need to apply it to multiple ROOT files in a directory.

For more details, check out individual files. 



Hot Pixel Moderation Sequences
===============================

There are three moderation steps applied in the main program.

* **Mod1**

	Averages out any pixel brighter than a pre-set threshold. Each hot pixel value is replaced with the average of its "good" neighbors(surrounding non-hot pixels with values lower than the thresold). The lower the threshold, the more bright pixels are removed. But you might not want to set it lower than your signal, because Mod1 might even average out the good pixels in your signal. 

* **Mod2**
	
	Averages out any pixel which appears as hot in the given hot pixel candidate map "and" appears to be larger than any of its good neighbors(surrounding pixels which do not appear as hot in the given map). Notice that the definition of a good neighbor is slightly different than in Mod2. Mod2 can be applied after Mod1, to remove hot pixels with values lower than the pre-set threshold. 

Note: Mod2 cannot remove "clustered" hot pixels, because in that case there can be no "good neighbor" around the pixel.

* **Mod3**

	Averages out any pixel which is some coefficient times brighter than the average of its surrounding neighbors. The coefficient is set to 1.5 by default. It averages out most of the left-out hot pixels after applying Mod1 and Mod2.

The current version of the program cannot deal with hot pixel clusters with values lower than the pre-set threshold. Most of them can be avoided by setting a proper regional cut of the data, but they may cause a serious trouble in case some of them are located in the signal region.



Contact
===========================

Author : Seungho Han (SNU)

E-mail : v3pro2004@snu.ac.kr

Last edited : April 20, 2018


