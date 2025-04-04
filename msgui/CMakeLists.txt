cmake_minimum_required(VERSION 3.22)

project(msgui)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    #Linux-specific code or configuration here
    message(STATUS "Configuring for Linux")
    set(CXX_STANDARD 23)

    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -ggdb")
    # set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
    # set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../out_debug)
    
    set(ROOT_OF_VENDOR "/home/hekapoo/Downloads")

    # Include paths
    set(FT_LIBRARY_INCLUDE_DIRECTORY   ${ROOT_OF_VENDOR}/freetype-2.13.1/include/)
    set(GLEW_LIBRARY_INCLUDE_DIRECTORY ${ROOT_OF_VENDOR}/glew-2.2.0/include)
    set(GLFW_LIBRARY_INCLUDE_DIRECTORY ${ROOT_OF_VENDOR}/glfw-3.4/include)

    # Path to lib itself
    set(FT_LIBRARY_PATH_DIRECTORY   ${ROOT_OF_VENDOR}/freetype-2.13.1/build)
    set(GLEW_LIBRARY_PATH_DIRECTORY ${ROOT_OF_VENDOR}/glew-2.2.0/lib)
    set(GLFW_LIBRARY_PATH_DIRECTORY ${ROOT_OF_VENDOR}/glfw-3.4/bld/src)

    # "Link" includes
    include_directories (${FT_LIBRARY_INCLUDE_DIRECTORY})
    include_directories (${GLEW_LIBRARY_INCLUDE_DIRECTORY})
    include_directories (${GLFW_LIBRARY_INCLUDE_DIRECTORY})

    add_library(${PROJECT_NAME} SHARED
        Application.cpp
        BELoadingQueue.cpp
        Debug.cpp
        Input.cpp
        layoutEngine/SimpleLayoutEngine.cpp
        layoutEngine/text/SimpleTextLayoutEngine.cpp
        renderer/text/TextBufferStore.cpp
        renderer/text/TextRenderer.cpp
        Logger.cpp
        Mesh.cpp
        MeshLoader.cpp
        loaders/FontLoader.cpp
        node/AbstractNode.cpp
        node/Box.cpp
        node/BoxDivider.cpp
        node/Button.cpp
        node/Dropdown.cpp
        node/Image.cpp
        node/TreeView.cpp
        node/FloatingBox.cpp
        node/RecycleList.cpp
        node/Slider.cpp
        node/TextLabel.cpp
        node/utils/BoxDividerSep.cpp
        node/utils/LayoutData.cpp
        node/utils/ScrollBar.cpp
        node/utils/ScrollBarKnob.cpp
        node/utils/SliderKnob.cpp
        node/WindowFrame.cpp
        Shader.cpp
        ShaderLoader.cpp
        Texture.cpp
        TextureLoader.cpp
        Transform.cpp
        Window.cpp
    )

    # Compile features
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

    # Link libs
    target_link_directories(${PROJECT_NAME} PRIVATE
        ${FT_LIBRARY_PATH_DIRECTORY}
        # ${GLEW_LIBRARY_PATH_DIRECTORY}
        # ${GLFW_LIBRARY_PATH_DIRECTORY}
    )

    # Needed for absolute include paths
    target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../)

    # Link libraries
    target_link_libraries(${PROJECT_NAME}
         glfw GL GLEW X11 freetype z bz2 png brotlicommon brotlidec
    )

# If the operating system is not recognized
else()
    message(FATAL_ERROR "Unsupported operating system: ${CMAKE_SYSTEM_NAME}")
endif()