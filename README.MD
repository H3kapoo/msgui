# Msgui

Pronounced "miss gui" is a lightweight C++ GUI library build using OpenGL.

### Features
Pretty simple to use. No deep OOP hierarchies. No abstraction madness. Straight to the point. Very few external dependencies to build.
Almost no dependencies to use the library dynamically.

##### Currently available UI elements include:
| Node         | Details                                      |
| --------     | -------------------------------------------- |
| Box          | Box container holding elements horizontally/vertically/in a grid manner |
| BoxDivider   | Box containers that can be resized by mouse click & drag |
| Button       | Simple text holding button |
| RecycleList  | List of Button nodes that recycle when they go offscren. Efficient and suitable for big data lists |
| Slider       | Mouse draggable slider for selecting values |
| OtherNodes   | TBA                                          |


### In Project usage:
All you need to do to use the library is link against it: ```msguilib.so```

Obviously basic stuff like OpenGL needs to be installed on your system.

### Build from source
⚠️ **Note** : Project was compiled using C++23 but currently nothing uses any functionality above C++17 so you can go ahead and use c++17 to compile if you want.

For Linux at least:
```bash
    chmod +x ./build.sh
    ./build.sh
```
```bash
    Build dependencies: GLFW glew freetype
```

For Windows:
```bash
    Should normally be the same, but with Windows CMD shenenigans.
```
### Notes