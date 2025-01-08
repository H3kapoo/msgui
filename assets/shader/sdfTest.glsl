/// vertex ///
#version 330

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTex;

uniform mat4 uModelMat;
uniform mat4 uProjMat;

out vec2 fTex;

void main()
{
    fTex = vTex;
    gl_Position = uProjMat * uModelMat * vec4(vPos, 1.0f);
}

/// frag ///
#version 330

uniform vec4 uColor = vec4(0.3, 0.1, 0.4, 1.0);
uniform vec2 uResolution;// = vec2(700, 400);

in vec2 fTex;

float roundedBoxSDF(vec2 uv, vec2 size, vec4 radii)
{
    // radii: x TL y TR z BR w BL
    vec2 absPos = abs(uv) - size;
    float radius = uv.x > 0 ? (uv.y > 0 ? radii.z : radii.y) : (uv.y > 0 ? radii.w : radii.x);
    return length(max(absPos + radius, 0.0)) - radius;
}

void main()
{
    vec2 p = fTex;
    float aspect = uResolution.x / uResolution.y;
    p.x *= aspect;
    p -= vec2(0.5*aspect, 0.5);
    // p.x *= aspect;
    // p.x *= uResolution.x;// * aspect;
    // p.y *= uResolution.y;

    // vec2 center = vec2(0.5*aspect, 0.5);
    vec2 center = vec2(0);

    vec2 boxSize = vec2(1*aspect, 1.0);
    vec4 cornerRadii = vec4(0.1);

    float dist = roundedBoxSDF(center - p, boxSize / 2.0, cornerRadii);
    float sdfValue = step(0.001, dist); // Smooth edge
    if (sdfValue >= 1)
    {
        discard;
    }
    gl_FragColor = vec4(vec3(sdfValue), 1.0);
    // gl_FragColor = vec4(p.x, p.y, 0.0, 1.0);
}