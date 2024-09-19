# Galaxy
## 快速开始
```
mkdir build
cd build
conan install .. --build missing -s build_type=Release
cmake .. -DCMAKE_BUILD_TYPE=Release --preset conan-release
cd Release
make
./galaxy
```

## 安装conan
```
conan profile detect
```

## 在Macos中开发调试
### 安装扩展
```
"twxs.cmake",
"ms-vscode.cmake-tools",
"josetr.cmake-language-support-vscode",
"ms-vscode.cpptools"
```
### 生成构建配置
```
mkdir build
cd build
conan install .. --build missing -s build_type=Debug
cmake .. -DCMAKE_BUILD_TYPE=Debug --preset conan-debug
```
### 初始化VScode
```
Command + Shift + P

Cmake:Configure
```
### Debugger
```
可以通过launcher.json进行debug
// 注意：build type 为debug才可以 debug
```

## 在ubuntu中交叉编译
```
conan profile detect --name rc-build-root // conan2.x

vi rc-build-root


[settings]
os=Linux
arch=armv8
compiler=gcc
compiler.version=9
compiler.libcxx=libstdc++11

[conf]
tools.build:compiler_executables={'c': 'aarch64-rockchip-linux-gnu-gcc', 'cpp': 'aarch64-rockchip-linux-gnu-g++'}

```
### ubuntu conan
直接安装的conan是3.10.2版本比较旧
建议用pip进行安装

### 安装conan依赖
```
mkdir build
cd build
conan install .. -pr=rc-build-root --build missing -s build_type=Release
cmake .. -DCMAKE_BUILD_TYPE=Release --preset conan-release
cd Release
make
```

## 代码规范

https://cvte.kdocs.cn/l/cjSxTVp1QeLl

### 工具

Clang-format
https://www.cnblogs.com/simpleGao/p/17022517.html
https://juejin.cn/post/7071871587674243102