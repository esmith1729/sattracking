# Sattracking

Satellite tracking algorithm which returns a look angle direction for an antenna to point toward, given:
- GPS location of antenna
- TLE of satellite
- Datetime of user

## How to use
Go to SGP4 folder's README to understand how to build that library first. You will need MSYS2, MinGW64 Terminal, Cmake, 
and MinGW32-make if using Windows. After installing all of those properly, and *after following the build instructions in 
SGP4's README*, run the following in a Terminal with g++:

`g++ -std=c++17 -I/SGP4/libsgp4 sattrack.cpp SGP4/build/libsgp4/libsgp4.a -o sattrack `

You can then run the program by running 

`.\sattrack.exe`
