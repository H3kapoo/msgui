/// vertex ///
#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTex;
layout (location = 2) in vec3 vCol;

uniform mat4 uModelMat;
uniform mat4 uProjMat;

out vec2 fTex;

void main()
{
    fTex = vTex;
    gl_Position = uProjMat * uModelMat * vec4(vPos.xyz, 1.0);
}

/// frag ///
#version 330 core

uniform sampler2DArray uTextureArray;
uniform int uCharIndex;
uniform vec4 uColor;

in vec2 fTex;

void main()
{
    float t = texture(uTextureArray, vec3(fTex, uCharIndex)).r;
    // if (t < 0.5f) { discard; }
    // float aaf = fwidth(t);
    // float alpha = smoothstep(0.5 - aaf, 0.5 + aaf, t);
    float feather = 0.04;
    t = smoothstep(0.5f - feather, 0.5f + feather, t);
    // gl_FragColor = vec4(uColor.xyz, alpha);
    gl_FragColor = vec4(uColor.xyz, t);
    // gl_FragColor = vec4(t, t, t, alpha);

    // gl_FragColor = vec4(1.0f, 0.0f, 0.0f, t);
    // gl_FragColor = vec4(t, t, t, 1.0f);
    // gl_FragColor = uColor;// * vec4(t, t, 1.0, 1.0f);
}