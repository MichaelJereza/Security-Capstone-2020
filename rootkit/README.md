# Rootkit

This rootkit demonstrates Windows XP privilege escalation. It kills the explorer.exe process and reopens it a minute later at the system privilege level.

## Compiling on Linux

To cross-compile C++ for 32-bit windows a program called "Mingw" is used. 
Here is the command containing required flags

`i686-w64-mingw32-g++ -static-libgcc -static-libstdc++ -static rootkit.cpp -o rootkit.exe`
