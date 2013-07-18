# Building

Building this library requires autotools (autoconf and automake),
libtool, and libxml2 to be installed. The library was developed using the
following versions:

 * autoconf 2.69
 * automake 1.31.1
 * libtool (GNU) 2.4.2
 * libxml2 2.7.8

To help keep the working directory clean, it is suggested you perform the
actual configuration and build in a subdirectory named build. Here is the
suggested set of commands for building and installing the library from the
top of the source tree:

    autoreconf --install
    mkdir build
    cd build
    ../configure
    make
    make install
