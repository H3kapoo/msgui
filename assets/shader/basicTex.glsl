/// vertex ///
#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTex;
layout (location = 2) in vec3 vCol;

uniform mat4 uModelMat;
uniform mat4 uProjMat;

out vec2 fragUV;

void main()
{
    fragUV = vTex;
    gl_Position = uProjMat * uModelMat * vec4(vPos.xyz, 1.0);
}

/// frag ///
#version 330 core

uniform sampler2D uTexture;
uniform vec4 uColor = vec4(1.0f);

in vec2 fragUV;

void main()
{
    vec4 color = texture(uTexture, fragUV) * uColor;
    gl_FragColor = color;
}