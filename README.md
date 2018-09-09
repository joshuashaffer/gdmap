# gdmap
Fork of GDMap-0.8.1.

- Added context menu for Path Copy and Delete.
- Added context menu for open, open-dir, open-term


```
Build:
mkdir build
cd build
cmake ..
make
```

(Used CMake so I could use CLion. Also the pkg-config macros make this a very simple build. CMake is generally garbage tho the way a lot of ppl use it. God I wasted like a week getting Trilinos to work. If you want to see a bad example of cmake look there.)

Required libraries (by pkg-config):
	- cairo 
	- pango 
	- glib-2.0 
	- gtk+-2.0 
	- libxml-2.0
