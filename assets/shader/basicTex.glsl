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

uniform sampler2D uTexture;
uniform vec4 uColor = vec4(1.0f);
uniform vec4 uBorderColor = vec4(0.0, 0.0, 1.0, 1.0);
uniform vec4 uBorderSize = vec4(0);
uniform vec4 uBorderRadii = vec4(0);
uniform vec2 uResolution;
uniform int uUseTexture = 1;

in vec2 fTex;

float roundedBoxSDF(vec2 uv, vec2 size, vec4 radii)
{
    vec2 absPos = abs(uv) - size;
    float radius = uv.x > 0 ? (uv.y > 0 ? radii.z : radii.y) : (uv.y > 0 ? radii.w : radii.x);
    return length(max(absPos + radius, 0.0)) - radius;
}

void main()
{
    // gl_FragColor = color;

    vec2 p = fTex;
    p.x *= uResolution.x;
    p.y *= uResolution.y;
    p -= vec2(uResolution.x / 2.0, uResolution.y / 2.0);

    vec2 borderSize = vec2(uResolution.x, uResolution.y);
    vec2 contentSize = vec2(uResolution.x, uResolution.y);

    vec2 newCenter = vec2(
        (uBorderSize.z + uBorderSize.w) * 0.5 - uBorderSize.w,
        (uBorderSize.x + uBorderSize.y) * 0.5 - uBorderSize.y);
    contentSize -= vec2(uBorderSize.z + uBorderSize.w, uBorderSize.x + uBorderSize.y);

    vec4 cornerRadii2 = vec4(0); // Unused for now

    float dist1 = roundedBoxSDF(p, borderSize / 2.0, uBorderRadii);
    float dist2 = roundedBoxSDF(newCenter - p, contentSize / 2.0, uBorderRadii / 2.0f);
    float sdfValue = step(0.001, dist1);
    float sdfValue2 = step(0.001, dist2);
    float sdfValue3 = sdfValue2 - sdfValue;

    if (sdfValue >= 1) { discard; }

    vec4 texColor = uUseTexture == 0 ? uColor : texture(uTexture, fTex) * uColor;

    vec4 finalColor = mix(texColor, vec4(0.0), sdfValue2);
    finalColor += mix(vec4(0.0), uBorderColor, sdfValue3);
    gl_FragColor = finalColor;
}