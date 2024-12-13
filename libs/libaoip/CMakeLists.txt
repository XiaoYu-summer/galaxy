cmake_minimum_required(VERSION 3.10)
project(libaoip VERSION 1.0.0 LANGUAGES CXX C)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE AOIP_PUB_HEADERS  "include/*.h")
file(GLOB_RECURSE AOIP_SOURCES "src/*.cpp" "src/*.c")
file(GLOB_RECURSE AOIP_PRI_HEADERS "src/*.h")
source_group("Pub Headers" FILES ${AOIP_PUB_HEADERS})

if(DEPLOY_ANDROID_LIB)
    add_library(jr_aoip STATIC ${AOIP_SOURCES} ${AOIP_PUB_HEADERS} ${AOIP_PRI_HEADERS})

    # 获取目标库的输出路径
    get_target_property(TARGET_OUTPUT_PATH jr_aoip LIBRARY_OUTPUT_DIRECTORY)
    # 创建一个新的目标路径，该路径是在原始目标路径的基础上添加了"include"
    set(TARGET_INCLUDE_PATH "${TARGET_OUTPUT_PATH}")
    # 复制所有公共头文件到目标库所在的目录
    file(COPY "include" DESTINATION ${TARGET_INCLUDE_PATH})
else()
    add_library(jr_aoip ${AOIP_SOURCES} ${AOIP_PUB_HEADERS} ${AOIP_PRI_HEADERS})
endif()

# 设置库的包含目录
target_include_directories(jr_aoip
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 导出目标
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/libaoip-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# 安装规则
install(TARGETS jr_aoip
    EXPORT jr_aoip-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

install(EXPORT jr_aoip-targets
    FILE libaoip-targets.cmake
    NAMESPACE libaoip::
    DESTINATION lib/cmake/libaoip
)

# 生成并安装配置文件
configure_file(cmake/libaoip-config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/libaoip-config.cmake"
    @ONLY
)