/// vertex ///
#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTex;
layout (location = 2) in vec3 vCol;

uniform mat4 uModelMat;
uniform mat4 uProjMat;

void main()
{
    gl_Position = uProjMat * uModelMat * vec4(vPos.xyz, 1.0);
}

/// frag ///
#version 330 core

uniform vec4 uColor = vec4(0.3, 0.1, 0.4, 1.0);

void main()
{
    gl_FragColor = uColor;
}