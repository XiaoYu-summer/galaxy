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

# 定义包含 Conan 工具链文件的函数
function(include_conan_toolchain toolchain_path)
    if(EXISTS ${toolchain_path})
        include(${toolchain_path})
    else()
        message(FATAL_ERROR "Conan toolchain file not found: ${toolchain_path}")
    endif()
endfunction()

# 根据不同的操作系统环境包含相应的 Conan 工具链文件
if(APPLE)
    set(CONAN_TOOLCHAIN_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/generators/conan_toolchain.cmake")
elseif(WIN32)
    set(CONAN_TOOLCHAIN_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build/generators/conan_toolchain.cmake")
elseif(UNIX)
    set(CONAN_TOOLCHAIN_PATH "${CMAKE_BINARY_DIR}/generators/conan_toolchain.cmake")
endif()

# 包含 Conan 工具链文件
include_conan_toolchain(${CONAN_TOOLCHAIN_PATH})

# 定义资源目录
set(ASSETS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
set(ASSETS_DESTINATION_DIR "${CMAKE_CURRENT_BINARY_DIR}/assets")

# 创建自定义命令来复制资源
add_custom_command(
    OUTPUT ${ASSETS_DESTINATION_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${ASSETS_SOURCE_DIR}
    ${ASSETS_DESTINATION_DIR}
    DEPENDS ${ASSETS_SOURCE_DIR}
    COMMENT "Copying assets directory"
)

# 创建自定义目标
add_custom_target(copy_assets ALL DEPENDS ${ASSETS_DESTINATION_DIR})

# 匹配 src 和 include 目录中的所有源文件和头文件
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 添加 include 目录到头文件搜索路径
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 添加可执行文件
add_executable(galaxy main.cpp ${SOURCES})

add_dependencies(galaxy copy_assets)

find_package(Crow REQUIRED)

# 链接 Crow 库和其他依赖
target_link_libraries(galaxy PUBLIC Crow::Crow)

find_package(OpenSSL REQUIRED)
target_link_libraries(galaxy PUBLIC openssl::openssl)

find_package(nlohmann_json REQUIRED)
target_link_libraries(galaxy PUBLIC nlohmann_json::nlohmann_json)

find_package(spdlog REQUIRED)
target_link_libraries(galaxy PUBLIC spdlog::spdlog)

find_package(Boost REQUIRED)
target_link_libraries(galaxy PUBLIC boost::boost)

find_package(LibArchive REQUIRED)
target_link_libraries(galaxy PUBLIC LibArchive::LibArchive)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 14) # 交叉编译的gcc是6.4版本 所以采用14
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
