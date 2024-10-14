# Galaxy
jenkins job: http://mlci.gz.cvte.cn/job/1602/job/sfu_build/job/AoIP/job/aoip_build/

## 前置依赖
- conan2.x
```
conan profile detect
```
- CMake3.x

## 在Macos中使用VSCode开发调试
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

## 在Windows中使用Visual Studio开发
```
conan install . --output-folder=build --build=missing --settings=build_type=Release
conan install . --output-folder=build --build=missing --settings=build_type=Debug

// 生成解决方案 请自行根据vs版本调整
cmake -G "Visual Studio 17 2022" -A x64 -B build -DCMAKE_POLICY_DEFAULT_CMP0091=NEW
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

### 构建
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