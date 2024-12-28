/// vertex ///
#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTex;
layout (location = 2) in vec3 vCol;

uniform mat4 uModelMat;
uniform mat4 uProjMat;

out vec2 fragUV;
out vec3 fragCol;

void main()
{
    fragUV = vTex;
    fragCol = vCol;
    gl_Position = uProjMat * uModelMat * vec4(vPos.xyz, 1.0);
}

/// frag ///
#version 330 core

uniform sampler2D uTexture;
uniform vec4 uColor;

in vec2 fragUV;
in vec3 fragCol;

void main()
{
    vec4 color = texture(uTexture, fragUV);
    // gl_FragColor = uColor;
    gl_FragColor = color;
    // gl_FragColor = vec4(color, 1.0);
}