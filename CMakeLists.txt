cmake_minimum_required(VERSION 3.10)
project(galaxy)

# 设置编译器
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
endif()

# 判断是否是 Windows 环境
if(WIN32)
    # 设置 CMake policy CMP0091 为 NEW
    if(POLICY CMP0091)
        cmake_policy(SET CMP0091 NEW)
    endif()
endif()

# 根据不同的操作系统环境包含相应的 Conan 工具链文件
if(APPLE)
    set(CONAN_TOOLCHAIN_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/generators/conan_toolchain.cmake")
elseif(WIN32)
    set(CONAN_TOOLCHAIN_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build/build/generators/conan_toolchain.cmake")
elseif(UNIX)
    set(CONAN_TOOLCHAIN_PATH "${CMAKE_BINARY_DIR}/generators/conan_toolchain.cmake")
endif()

# 包含 Conan 工具链文件
include(${CONAN_TOOLCHAIN_PATH})

# 定义资源目录
set(ASSETS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
set(ASSETS_DESTINATION_DIR "${CMAKE_CURRENT_BINARY_DIR}/assets")
set(CONFIG_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/config")
set(CONFIG_DESTINATION_DIR "${CMAKE_CURRENT_BINARY_DIR}/config")

# 创建自定义命令来复制资源
add_custom_command(
    OUTPUT ${ASSETS_DESTINATION_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${ASSETS_SOURCE_DIR}
    ${ASSETS_DESTINATION_DIR}
    DEPENDS ${ASSETS_SOURCE_DIR}
    COMMENT "Copying assets directory"
)

# 创建自定义命令来复制配置文件
add_custom_command(
    OUTPUT ${CONFIG_DESTINATION_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CONFIG_SOURCE_DIR}
    ${CONFIG_DESTINATION_DIR}
    DEPENDS ${CONFIG_SOURCE_DIR}
    COMMENT "Copying config directory"
)

# 创建自定义目标
add_custom_target(copy_assets ALL DEPENDS ${ASSETS_DESTINATION_DIR})
add_custom_target(copy_config ALL DEPENDS ${CONFIG_DESTINATION_DIR})

# 匹配 src 和 include 目录中的所有源文件和头文件
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

# 添加 include 目录到头文件搜索路径
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs
)

# 添加子目录
add_subdirectory(libs/libaoip)
add_subdirectory(libs/mdns)

# 添加可执行文件
add_executable(galaxy main.cpp ${SOURCES} ${HEADERS})

add_dependencies(galaxy copy_assets)
add_dependencies(galaxy copy_config)

# 首先找到所有需要的包
find_package(Boost REQUIRED thread system filesystem)
find_package(Crow REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)
find_package(LibArchive REQUIRED)
find_package(Poco REQUIRED COMPONENTS Foundation Util)

# 链接所有依赖库
target_link_libraries(galaxy 
    PRIVATE 
        Boost::thread
        Boost::system
        Boost::filesystem
        Crow::Crow
        OpenSSL::SSL 
        OpenSSL::Crypto
        nlohmann_json::nlohmann_json
        spdlog::spdlog
        LibArchive::LibArchive
        $<BUILD_INTERFACE:jr_aoip>
        $<BUILD_INTERFACE:mh_mdns>
        Poco::Foundation
)

if(APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-dead_strip")
endif()

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
