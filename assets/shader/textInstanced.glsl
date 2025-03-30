/// vertex ///
#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTex;
layout (location = 2) in vec3 vCol;

uniform mat4[100] uModelMatv;
uniform mat4 uProjMat;

out vec2 fTex;
flat out int instanceId;

void main()
{
    fTex = vTex;
    instanceId = gl_InstanceID;
    gl_Position = uProjMat * uModelMatv[gl_InstanceID] * vec4(vPos.xyz, 1.0);
}

/// frag ///
#version 330 core

uniform sampler2DArray uTextureArray;
uniform int[100] uCharIdxv;
uniform vec4 uColor;

in vec2 fTex;
flat in int instanceId;

void main()
{
    int zSliceIndex = uCharIdxv[instanceId];
    float t = texture(uTextureArray, vec3(fTex, zSliceIndex)).r;

    gl_FragColor = vec4(uColor.xyz, t);
}