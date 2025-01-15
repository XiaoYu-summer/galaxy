cmake_minimum_required(VERSION 3.10)
project(libmdns VERSION 1.0.0 LANGUAGES CXX C)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 POCO 库
find_package(Poco REQUIRED Foundation)

# 收集源文件和头文件
file(GLOB_RECURSE MDNS_PUB_HEADERS  "include/*.h" "*.h")
file(GLOB_RECURSE MDNS_SOURCES "src/*.cpp" "src/*.c" "*.c")
file(GLOB_RECURSE MDNS_PRI_HEADERS "src/*.h")

if(DEPLOY_ANDROID_LIB)
    add_library(mh_mdns STATIC ${MDNS_SOURCES} ${MDNS_PUB_HEADERS} ${MDNS_PRI_HEADERS})
    get_target_property(TARGET_OUTPUT_PATH mh_mdns LIBRARY_OUTPUT_DIRECTORY)
    set(TARGET_INCLUDE_PATH "${TARGET_OUTPUT_PATH}")
    file(COPY "include" DESTINATION ${TARGET_INCLUDE_PATH})
else()
    add_library(mh_mdns ${MDNS_SOURCES} ${MDNS_PUB_HEADERS} ${MDNS_PRI_HEADERS})
endif()

# 设置库的包含目录
target_include_directories(mh_mdns
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/mdns>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 链接 POCO 库
target_link_libraries(mh_mdns 
    PRIVATE 
        Poco::Foundation
)

# 导出目标
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/libmdns-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# 安装规则
install(TARGETS mh_mdns
    EXPORT mh_mdns-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

install(EXPORT mh_mdns-targets
    FILE libmdns-targets.cmake
    NAMESPACE libmdns::
    DESTINATION lib/cmake/libmdns
)

# 生成并安装配置文件
configure_file(cmake/libmdns-config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/libmdns-config.cmake"
    @ONLY
)
