cmake_minimum_required(VERSION 3.22)
project(ZKIC_project LANGUAGES CXX)

# ============================================================
# C++ 标准
# ============================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================
# 自动发现根目录下的所有源文件
# CONFIGURE_DEPENDS 会在每次 build 时自动检测文件增减变化，
# 无需手动 CMake: Configure。只扫描根目录，排除子目录。
# ============================================================
file(GLOB SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# ============================================================
# FFTW 库
# ============================================================
set(FFTW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/fftw")
set(FFTW_INCLUDE_DIR "${FFTW_DIR}")
set(FFTW_LIBRARY "${FFTW_DIR}/libfftw3-3.lib")

# ============================================================
# ZKIC 第三方库
# ============================================================
set(ZKIC_BASE "${CMAKE_CURRENT_SOURCE_DIR}/ZKIC_lib")

set(ZKIC_MATH_INCLUDE "${ZKIC_BASE}/ZKIC_mathworld/ZKIC_mathworld2p5/include")
set(ZKIC_MATH_LIB_DIR "${ZKIC_BASE}/ZKIC_mathworld/ZKIC_mathworld2p5/x64/debug/lib")
set(ZKIC_MATH_LIB "${ZKIC_MATH_LIB_DIR}/ZKIC_math_world2p5d.lib")

set(ZKIC_GV_INCLUDE "${ZKIC_BASE}/GlobalValueForZD/include")
set(ZKIC_GV_LIB_DIR "${ZKIC_BASE}/GlobalValueForZD/x64/debug/lib")
set(ZKIC_GV_LIB "${ZKIC_GV_LIB_DIR}/signals_globalvalue_world1p1d.lib")

# ============================================================
# 头文件路径
# ============================================================
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FFTW_INCLUDE_DIR}
    ${ZKIC_MATH_INCLUDE}
    ${ZKIC_GV_INCLUDE}
)

# ============================================================
# 可执行文件
# ============================================================
add_executable(${PROJECT_NAME} ${SOURCES})

# ============================================================
# 链接库
# ============================================================
target_link_directories(${PROJECT_NAME} PRIVATE
    "${FFTW_DIR}"
    "${ZKIC_MATH_LIB_DIR}"
    "${ZKIC_GV_LIB_DIR}"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    "${FFTW_LIBRARY}"
    "${ZKIC_MATH_LIB}"
    "${ZKIC_GV_LIB}"
)

# ============================================================
# 复制 DLL 到输出目录
# ============================================================
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${FFTW_DIR}/libfftw3-3.dll"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${ZKIC_MATH_LIB_DIR}/../bin/ZKIC_math_world2p5d.dll"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${ZKIC_GV_LIB_DIR}/../bin/signals_globalvalue_world1p1d.dll"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)

# ============================================================
# MSVC 特定设置
# ============================================================
if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:_DEBUG>
        _CONSOLE
    )
    target_compile_options(${PROJECT_NAME} PRIVATE /W3 /utf-8)
    # 设置子系统为 Console
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:CONSOLE"
    )
endif()
