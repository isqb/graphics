Computer Graphics, Assignment 3, Part 1
=======================================


Build instructions for Linux
============================

Dependencies
------------
gcc/g++, CMake 2.6 (or higher), make, freeglut, OpenGL, GLEW

These dependencies can be installed via the package manager of your
Linux distribution.

When CMake tries to locate freeglut it might complain if two
additional libraries, Xi and Xmu, are not installed. On Ubuntu 12.04
you can install these two libraries by typing

 sudo apt-get install libxmu-dev libxi-dev

If GLEW triggers linking errors, install the binutils-gold package.

Environment variables
---------------------
The environment variable ASSIGNMENT3_ROOT must be set pointing to the
extracted assignment3 directory. Example:

  export ASSIGNMENT3_ROOT=$HOME/assignment3

Building and running the program for Part 1
-------------------------------------------
To build the program, open a terminal, navigate to
$ASSIGNMENT3_ROOT/part1/build, and type

  mkdir linux
  cd linux
  cmake ../
  make

To run the program, navigate to the resulting executable (part1), and
type

  ./part1

Alternatively, run the attached script build.sh (which will perform
all these steps for you):

  ./build.sh

Tip: You don't have to run CMake every time you change something in
the source files. Just use the generated makefile (or the
build.sh script) to rebuild the program.


Build instructions for Windows
==============================

Dependencies
------------
Visual Studio 20XX, CMake GUI 2.6 (or higher)

Environment variables
---------------------
Define a user environment variable ASSIGNMENT3_ROOT that points to the
extracted assignment1 directory.

Building and running the program
--------------------------------
1. Before you start CMake or do anything else, set the
   ASSIGNMENT3_ROOT variable as described above.
2. Start CMake GUI.
3. Put the search path YOUR_PATH_TO_ASSIGNMENT3\part1\build in the
   "Where is the source code:" text box.
4. Create a subdirectory YOUR_PATH_TO_ASSIGNMENT3\part1\build\vc, and
   put the search path to this directory in the "Where to build the
   binaries:" text box.
5. Press the "Configure" button and choose the compiler
   (Visual Studio 20XX).
6. Press the "Configure" button again.
7. Press the "Generate" button to generate Visual Studio project
   files.
8. Close CMake GUI.
9. Navigate to the YOUR_PATH_TO_ASSIGNMENT3\part1\build\vc directory
   and open the part1.sln file in Visual Studio.
10. Right-click on the Part1 project and choose Set as StartUp Project
11. Press F5 to compile and run the program. The resulting executable
   (part1.exe) will be placed in
   YOUR_PATH_TO_ASSIGNMENT3\part1\build\vc\Debug

Note 1: Just to clarify, YOUR_PATH_TO_ASSIGNMENT3 should be replaced
with a real path that points to the extracted assignment3 folder.
CMake cannot expand environment variables in the text boxes, so you
need to specify the path explicitly.

Note 2: Every time you change the ASSIGNMENT3_ROOT variable you have
to close all open command prompts and restart CMake (since CMake will
not reload the values of environment variables automatically). In rare
cases, it might be necessary to reboot the system to update the
environment variable.

Note 3: CMake seems to have problem handling paths that contains
non-ASCII (international) characters. Assuming that you don't want to
change your language settings, a simple fix is to move the assignment3
folder to a folder that only has ASCII symbols in its path.

Note 4: You need to copy freeglut.dll and glew32.dll to the generated
vc solution folder (or the vc/Debug or vc/Release subfolders) before
running the program.

Note 5: If the GLSL shader files (*.vert and *.frag) are not visible
in the solution file tree, open them via the file manager (or drag and
drop them into the Visual Studio window).

Note 6: In step 5, select "Visual Studio 20XX", not "Visual Studio
20XX Win64". The included versions of freeglut.dll and glew32.dll are
32-bit DLLs, so we need to use a 32-bit compiler.


/Johan
