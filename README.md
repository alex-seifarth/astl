# ASTL - C++ Utility Library

## Description
Several header and binary libraries with useful C++ utilities like event delegation,  memory pools, event-loop for 
asynchronous communication, message boxes, etc.

## Installation
### Requirements
- Linux or BSD based OS 
- CMAKE version >= 3.14 (see https://cmake.org/, (C) Kitware, Inc.)
- C++ compiler with support for C++ 2017 Standard (tests on gcc 7.3 and gcc 9.2)
- if ASTL_GTESTS = ON
    -  Google GTest version >= 1.8 (see https://github.com/google/googletest, (C) Google Inc.)
- if documentation shall be build:
    - Doxygen version >= 1.8
    - Graphviz version >= 2.40
    
## Build, Installation
- clone the source code from the repository
  ```bash 
    git clone https://github.com/alex-seifarth/astl.git astl
  ```
- create a build directory and configure cmake in it
  ```bash
    mkdir astl-build
    cd astl-build
    cmake -DASTL_GTESTS=ON -DCMAKE_INSTALL_PREFIX=<path to install dir> ../astl
  ```
  The ASTL_GTESTS flag controls whether gtests are build or not, default is OFF. 
  CMAKE_INSTALL_PREFIX can be used to define where the build will install the header and library files.
- now build and install
  ```bash 
  make 
  [sudo] make install
  ```
- running the test case when ASTL_GTESTS was set to ON with
  ```bash
  make test
  ```  
  or 
  ```bash
  ctest
  ```
  
  
  
  
