cmake_minimum_required(VERSION 3.16)

project(MyProject VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES
    "${CMAKE_SOURCE_DIR}/*.cpp"
)

list(FILTER SOURCES EXCLUDE REGEX "build/")
list(FILTER SOURCES EXCLUDE REGEX "CMakeFiles/")
list(FILTER SOURCES EXCLUDE REGEX "Oldcode/")

add_executable(MyProject ${SOURCES})

# Include directory (REQUIRED "include" folder)
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/headers
)

# Compiler warnings
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

find_package(Vulkan REQUIRED)

target_link_libraries(MyProject PRIVATE D:/ProjVSC/VulkanGraphics/Libraries/vulkan/vulkan-1.lib)