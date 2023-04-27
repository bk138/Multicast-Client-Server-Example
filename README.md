[![CI](https://github.com/bk138/Multicast-Client-Server-Example/actions/workflows/ci.yml/badge.svg)](https://github.com/bk138/Multicast-Client-Server-Example/actions/workflows/ci.yml)
[![Help making this possible](https://img.shields.io/badge/liberapay-donate-yellow.png)](https://liberapay.com/bk138/donate)
[![Become a patron](https://img.shields.io/badge/patreon-donate-yellow.svg)](https://www.patreon.com/bk138)


This is a simple educational example of a multicast 
client and server, running under UNIX and Win32.

# Building
To compile, use

    mkdir build
    cd build
    cmake ..
    cmake --build .

## Crosscompiling from Unix to Android

See https://developer.android.com/ndk/guides/cmake.html as a reference, but
basically it boils down to adding `-DANDROID_NDK=<path> -DCMAKE_TOOLCHAIN_FILE=<path> -DANDROID_NATIVE_API_LEVEL=<API level you want>`
to the `cmake ..` step above.
