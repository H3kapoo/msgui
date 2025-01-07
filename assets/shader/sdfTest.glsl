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
uniform vec2 uResolution = vec2(700, 400);

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
    vec2 tex = fTex;
    float aspect = uResolution.x / uResolution.y; 
    vec2 correctedTex = vec2(tex.x * aspect, tex.y);

    vec2 p = correctedTex - vec2(0.5 * aspect, 0.5);

    vec2 boxSize = vec2(0.5*aspect, 0.5);
    vec4 cornerRadii = vec4(0.1);

    float dist = roundedBoxSDF(p, boxSize, cornerRadii);
    float sdfValue = step(0.01, dist); // Smooth edge
    // float sdfValue = dist;
    // if (sdfValue >= 1.0)
    // {
    //     discard;
    // }
    if (sdfValue > 0.5)
    {
        gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
    else
    {
        gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    }
    // gl_FragColor = vec4(vec3(sdfValue), 1.0);
}