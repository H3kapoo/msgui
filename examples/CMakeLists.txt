cmake_minimum_required(VERSION 3.22)

set(MAIN_FILE CACHE STRING "Main source file to compile")

project(${MAIN_FILE})

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    #Linux-specific code or configuration here
    message(STATUS "Configuring for Linux")
    set(CXX_STANDARD 23)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -ggdb -g")
    # set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")


    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../test_executables)

    add_executable(${PROJECT_NAME}
        ${MAIN_FILE}.cpp
    )

    # Compile features
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

    # Link libs
    target_link_directories(${PROJECT_NAME} PRIVATE
        "../lib_out"
    )

    # Needed for absolute include paths
    target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../)

    # Link libraries
    target_link_libraries(${PROJECT_NAME} core GL GLEW)

# If the operating system is not recognized
else()
    message(FATAL_ERROR "Unsupported operating system: ${CMAKE_SYSTEM_NAME}")
endif()