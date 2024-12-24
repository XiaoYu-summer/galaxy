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
cd Debug
make
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
### 配置Visual Studio
```
建议安装LLVM：https://github.com/llvm/llvm-project/releases/tag/llvmorg-19.1.1
采用最新安装的LLVM中clang-format，而非使用Visual Studio自带的(17.x)
最新版本是 19.x

工具->选项->C/C++->代码样式->格式设置
将安装的clang-format路径设置到自定义的clang-format中

Visual Studio 插件建议：Format on Save for 2022
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

### 1. 文件命名
- 使用大驼峰命名法
- 不使用下划线

```cpp
NetworkManager.h
AudioDevice.cpp
StreamProcessor.h
ConfigurationManager.cpp
```

### 2. 类名、函数名和枚举类型名
- 使用大驼峰命名法
- 不使用下划线

```cpp
// 类名示例
class NetworkManager;
class AudioDevice;

// 函数名示例
void ProcessData();
bool ValidateInput();
int CalculateChecksum();

class AudioDevice {
public:
    void ProcessAudio();          // 公有方法使用大驼峰
    bool ValidateFormat();
    void SetConfiguration();
    
private:
    void HandleError();           // 私有方法也使用大驼峰
    bool CheckStatus();
};

// 枚举类型名示例
enum class AudioState;
enum class ConnectionState;
```

### 3. 成员变量和参数
- 使用小驼峰命名法
- 私有成员变量末尾加下划线
- 除了私有成员变量的末尾下划线外，不使用其他下划线

```cpp
class AudioDevice {
private:
    int sampleRate_;              // 私有成员变量末尾加下划线
    bool isActive_;
    std::string deviceName_;
    std::vector<float> audioBuffer_;
    
public:
    // 参数使用小驼峰，不加下划线
    void ProcessAudio(int bufferSize, float* inputData);
    void Configure(const std::string& configName);
};
```

### 4. 常量、枚举值和宏
- 枚举类型名使用大驼峰命名法
- 枚举值使用大写字母，单词间用下划线分隔
- 常量和宏使用大写字母，单词间用下划线分隔

```cpp
const int MAX_BUFFER_SIZE = 1024;
#define NETWORK_MTU_SIZE 1500

enum class AudioState {
    AUDIO_INITIAL,
    AUDIO_PLAYING,
    AUDIO_PAUSED,
    AUDIO_STOPPED
};

enum class ConnectionState {
    CONN_DISCONNECTED,
    CONN_CONNECTING,
    CONN_CONNECTED,
    CONN_ERROR
};
```

### 5. 引用路径
- 使用绝对路径
- 禁止使用相对路径

```cpp
// 正确示例
#include "Source/Core/AudioDevice.h"
#include "Source/Utils/Logger.h"

// 错误示例
#include "../Core/AudioDevice.h"
#include "../../Utils/Logger.h"
```

### 6. 缩进
- 使用4个空格进行缩进
- 不使用制表符(Tab)

```cpp
class AudioProcessor {
public:
    void ProcessBuffer() {
        if (isActive_) {
            for (int i = 0; i < bufferSize_; i++) {
                // 处理逻辑
            }
        }
    }
    
private:
    bool isActive_;
    int bufferSize_;
};
```

### 7. 枚举使用
- 优先使用enum class而不是enum
- 遵循上述命名规范

```cpp
// 推荐使用
enum class DeviceState {
    DEVICE_NOT_INITIALIZED,
    DEVICE_INITIALIZING,
    DEVICE_RUNNING,
    DEVICE_ERROR
};

// 不推荐使用
enum OldDeviceState {
    NOT_INITIALIZED,
    INITIALIZING,
    RUNNING,
    ERROR
};
```

### 完整示例

```cpp
// NetworkManager.h
#include "Source/Core/Common.h"
#include "Source/Utils/Logger.h"

const int MAX_RETRY_COUNT = 3;
const char* DEFAULT_HOST = "LOCALHOST";

enum class ConnectionState {
    CONN_DISCONNECTED,
    CONN_CONNECTING,
    CONN_CONNECTED,
    CONN_ERROR
};

enum class DeviceType {
    DEVICE_AUDIO,
    DEVICE_VIDEO,
    DEVICE_NETWORK
};

class NetworkManager {
public:
    void Initialize();
    bool ConnectToServer();
    void ProcessPacket();
    void HandleConnection(const std::string& serverName, int portNumber);
    
private:
    bool isConnected_;
    int retryCount_;
    std::string serverAddress_;
    ConnectionState currentState_;
    DeviceType deviceType_;
    
    void HandleError();
    bool ValidateConnection() {
        if (currentState_ == ConnectionState::CONN_CONNECTED) {
            // 处理连接
            return true;
        }
        return false;
    }
};

// 全局函数示例
void ProcessGlobalData();
bool ValidateSystemInput();
int CalculateGlobalChecksum();
```

规范要点总结：
1. 文件名使用大驼峰命名法
2. 类名使用大驼峰命名法
3. 函数名使用大驼峰命名法（包括类方法和全局函数）
4. 成员变量和参数使用小驼峰命名法
5. 私有成员变量末尾加下划线
6. 常量和宏使用大写字母加下划线
7. 枚举类型名使用大驼峰命名法
8. 枚举值使用大写字母加下划线
9. 使用绝对路径
10. 使用4空格缩进
11. 优先使用enum class

### 工具

Clang-format
https://www.cnblogs.com/simpleGao/p/17022517.html
https://juejin.cn/post/7071871587674243102