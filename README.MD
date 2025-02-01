# Msgui

Pronounced "miss gui" is a lightweight C++ GUI library build using OpenGL.

- [Msgui](#msgui)
    - [Features](#features)
        - [Currently available UI elements include:](#currently-available-ui-elements-include)
    - [Exmples](#exmples)
    - [In Project usage](#in-project-usage)
    - [Build from source](#build-from-source)
    - [Notes \& Disclaimers](#notes--disclaimers)

### Features
Pretty simple to use. No deep OOP hierarchies. No abstraction madness. Straight to the point. Very few external dependencies to build.
Almost no dependencies to use the library dynamically.

##### Currently available UI elements include:
| UI Element         | Details                                      |
| --------     | -------------------------------------------- |
| Box          | Box container holding elements horizontally/vertically/in a grid manner |
| BoxDivider   | Box containers that can be resized by mouse click & drag |
| Button       | Simple text holding button |
| RecycleList  | List of Button nodes that recycle when they go offscren. Efficient and suitable for big data lists |
| Slider       | Mouse draggable slider for selecting values |
| OtherNodes   | TBA                                          |


### Exmples
There's a directory with examples you can explore. You need to go in the ```examples``` directory and run:
```bash
    chmod +x ./build.sh
```
and then:
```bash
    ./build.sh <exampleName>
```
For a list of available examples, leave the .sh argument empty.

### In Project usage
All you need to do to use the library is link against it: ```msguilib.so```

Obviously basic stuff like OpenGL needs to be installed on your system.

### Build from source
⚠️ **Note** : Project was compiled using GCC C++23. If you want to add modifications be sure to have the correct version.

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
### Notes & Disclaimers
 - UI Element or UI widget or nodes are used interchangeably and mean the same thing in this project.