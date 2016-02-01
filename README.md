Libccnx-portal
=======
A CCNx base API

## Quick Start ##
```
$ git clone git@github.com:PARC/Libccnx-portal.git Libccnx-portal
$ mkdir Libccnx-portal.build
$ cd Libccnx-portal.build
$ cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} ../Libccnx-portal
$ make
$ make test
$ make install
```

## Introduction ##

The CCNx Portal API is a simple API to communicate via Interests and Content Objects. It connects to a transport stack (like [Transport RTA](https://github.com/PARC/Libccnx-transport-rta)).  

## Using Libccnx-portal ##

### Distillery ###

Libccnx-portal is part of [CCNx Distillery](https://github.com/PARC/CCNx_Distillery). You may want to get the software via that distribution if you want to work on CCNx.

### Platforms ###

Libccnx-portal has been tested in:

- Ubuntu 14.04 (x86_64)
- MacOSX 10.10
- MacOSX 10.11

Other platforms and architectures may work.

### Dependencies ###

Build dependencies:

- c99 ( clang / gcc )
- CMake 3.4

Basic dependencies:

- OpenSSL
- pthreads
- Libevent
- [LongBow](https://github.com/PARC/LongBow)
- [Libparc](https://github.com/PARC/Libparc)
- [Libccnx-common](https://github.com/PARC/Libccnx-common)
- [Libccnx-transport-rta](https://github.com/PARC/Libccnx-transport-rta)


Documentation dependencies:

- Doxygen


### Getting Started ###

Libccnx-portal is built using cmake. You will need to have CMake 3.4 installed in order to build it.

```
Download Libccnx-portal
$ git clone git@github.com:PARC/Libccnx-portal.git Libccnx-portal

Create build directory
$ mkdir Libccnx-portal.build
$ cd Libccnx-portal.build

Prepare the build, give an install directory
$ cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} ../Libccnx-portal

Compile the software
$ make

Run unit tests
$ make test

Install the software
$ make install
```

This will create the `include` and `lib` directories containing the necessary files to compile with LongBow in the `${INSTALL_DIR}`.



### Using Libccnx-portal ###

Libccnx-portal is a set of functions and data structures for C. You can use it in your code by including the right header files and linking to the Libccnx_api_portal library.

```
CCNX_HOME=<directory-where-Libccnx-portal-is-installed>

 -I${CCNX_HOME}/include -L${CCNX_HOME}/lib -lccnx_api_portal
```

### Contact ###

- [Libccnx-portal GitHub](https://github.com/PARC/Libccnx-portal)
- [CCNx Website](http://www.ccnx.org/)
- [CCNx Mailing List](https://www.ccnx.org/mailman/listinfo/ccnx/)


### License ###

This software is distributed under the following license:

```
Copyright (c) 2013, 2014, 2015, 2016, Xerox Corporation (Xerox)and Palo Alto
Research Center (PARC)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
* Patent rights are not granted under this agreement. Patent rights are
  available under FRAND terms.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL XEROX or PARC BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```
