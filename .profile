[settings]
os=Linux
arch=armv8
compiler=gcc
compiler.version=10
compiler.libcxx=libstdc++11
compiler.cppstd=17

[conf]
tools.build:compiler_executables={'c': 'aarch64-none-linux-gnu-gcc', 'cpp': 'aarch64-none-linux-gnu-g++'}