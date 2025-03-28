cmake_minimum_required(VERSION 3.22)

project(gui)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    #Linux-specific code or configuration here
    message(STATUS "Configuring for Linux")
    set(CXX_STANDARD 23)

    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -ggdb -g")
    # set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../debug)

    set(ROOT_OF_VENDOR "/home/hekapoo/Downloads")

    # Include paths
    set(FT_LIBRARY_INCLUDE_DIRECTORY  ${ROOT_OF_VENDOR}/freetype-2.13.1/include/)

    # "Link" includes
    include_directories (${FT_LIBRARY_INCLUDE_DIRECTORY})

    # Path to lib itself
    set(FT_LIBRARY_PATH_DIRECTORY   ${ROOT_OF_VENDOR}/freetype-2.13.1/build)

    add_executable(${PROJECT_NAME}
        main.cpp
    )

    # Compile features
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

    # Link libs
    target_link_directories(${PROJECT_NAME} PUBLIC
        "lib_out"
        ${FT_LIBRARY_PATH_DIRECTORY}
    )

    # Needed for absolute include paths
    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR})

    # Link libraries
    # target_link_libraries(${PROJECT_NAME} glfw GL GLEW X11)
    # target_link_libraries(${PROJECT_NAME} msgui GL GLEW)
    target_link_libraries(${PROJECT_NAME} msgui GL glfw GL GLEW X11 freetype z bz2 png brotlicommon brotlidec)

# If the operating system is not recognized
else()
    message(FATAL_ERROR "Unsupported operating system: ${CMAKE_SYSTEM_NAME}")
endif()