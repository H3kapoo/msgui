cmake_minimum_required(VERSION 3.22)

project(core)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    #Linux-specific code or configuration here
    message(STATUS "Configuring for Linux")
    set(CXX_STANDARD 23)

    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -ggdb -g")
    # set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
    # set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../out_debug)
    
    set(ROOT_OF_VENDOR "/home/hekapoo/Downloads")

    # Include paths
    set(FT_LIBRARY_INCLUDE_DIRECTORY   ${ROOT_OF_VENDOR}/freeType2/freetype-2.13.1/include/)
    set(GLEW_LIBRARY_INCLUDE_DIRECTORY ${ROOT_OF_VENDOR}/glew-2.1.0/include)
    set(GLFW_LIBRARY_INCLUDE_DIRECTORY ${ROOT_OF_VENDOR}/glfw-3.3.8/include)

    # Path to lib itself
    set(FT_LIBRARY_PATH_DIRECTORY   ${ROOT_OF_VENDOR}/freeType2/freetype-2.13.1/objs/.libs)
    set(GLEW_LIBRARY_PATH_DIRECTORY ${ROOT_OF_VENDOR}/glew-2.1.0/lib)
    set(GLFW_LIBRARY_PATH_DIRECTORY ${ROOT_OF_VENDOR}/glfw-3.3.8/src)

    # "Link" includes
    include_directories (${FT_LIBRARY_INCLUDE_DIRECTORY})
    include_directories (${GLEW_LIBRARY_INCLUDE_DIRECTORY})
    include_directories (${GLFW_LIBRARY_INCLUDE_DIRECTORY})

    add_library(${PROJECT_NAME} SHARED
        Window.cpp
        Input.cpp
        Mesh.cpp
        Logger.cpp
        Debug.cpp
        Shader.cpp
        ShaderLoader.cpp
        MeshLoader.cpp
        Transform.cpp
        TextureLoader.cpp
        Texture.cpp
        Listeners.cpp
        node/AbstractNode.cpp
        node/Button.cpp
        node/WindowFrame.cpp
        node/Box.cpp
        node/Slider.cpp
        node/utils/SliderKnob.cpp
        node/utils/ScrollBar.cpp
        node/utils/ScrollBarKnob.cpp
        layoutEngine/SimpleLayoutEngine.cpp
        Application.cpp
    )

    # Compile features
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

    # Link libs
    target_link_directories(${PROJECT_NAME} PRIVATE
        ${FT_LIBRARY_PATH_DIRECTORY}
        ${GLEW_LIBRARY_PATH_DIRECTORY}
        ${GLFW_LIBRARY_PATH_DIRECTORY}
    )

    # Needed for absolute include paths
    target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../)

    # Link libraries
    target_link_libraries(${PROJECT_NAME} glfw GL GLEW X11)

# If the operating system is not recognized
else()
    message(FATAL_ERROR "Unsupported operating system: ${CMAKE_SYSTEM_NAME}")
endif()