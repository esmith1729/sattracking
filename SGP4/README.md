SGP4 library
============

License
-------

    Copyright 2017 Daniel Warner

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

Building
========

   Using Mingw32-make and cmake, in a MinGW64 Terminal, run the following *from this SGP4 directory*: 

   `mkdir build`
   
   `cd build` 
   
   `cmake -G "MinGW Makefiles" ..`
   
   `mingw32-make`

This should build all of the C++ binaries into the `build` directory, and allow for use of the SGP4 headers into Sattrack's code
