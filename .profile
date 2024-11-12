[settings]
os=Linux
arch=armv8
compiler=gcc
compiler.version=12
compiler.libcxx=libstdc++11
compiler.cppstd=20

[conf]
tools.build:compiler_executables={'c': 'aarch64-buildroot-linux-gnu-gcc', 'cpp': 'aarch64-buildroot-linux-gnu-g++'}