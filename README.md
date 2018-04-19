hp_remove
=========

This is a tool for removing most of the hot pixels in a digital camera image in ROOT format. 

Conversion of any raw camera image to ROOT is required before applying this program.

Quick Start
===========

```
> make hp_remove
```
```
> ./hp_remove [input root file] [output root file] [threshold]
```
```
Example usage: > ./hp_remove ../BPM_ROOT/r0033_0005.root test_out.root 250
```

Note: Choose threshold larger than the maximum value(bin content) in the signal.

To use a macro, edit and run hp_macro.sh.

Structure
=========

* **lib_hp.cc**

	Libraries of essential functions usable in both hot pixel removal and general analysis.

* **lib_hp.h**

	Header of lib_hp.cc. Include this library to use the hp_remove library in your own program.		 

* **main_hp.cc**

	This is the main program. It reads input root file and produces an output with hot pixels removed. 

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

Contact
===========================

Author : Seungho Han (SNU)

E-mail : v3pro2004@snu.ac.kr

Last edited : April 19, 2018


