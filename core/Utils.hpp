#pragma once

#include <string>

#include <glm/glm.hpp>

namespace msgui
{
class Utils
{
public:
    static inline glm::vec4 hexToVec4(const std::string& hexColor)
    {
        // A bit restrictive but good enough for now 
        if ((hexColor.size() != 7 && hexColor.size() != 9) || hexColor[0] != '#')
        {
            fprintf(stderr, "Invalid hex color format!\n");
            return {0.0f, 0.0f, 0.0f, 1.0f};
        }

        uint32_t r, g, b, a;
        sscanf(hexColor.c_str(), "#%02x%02x%02x%02x", &r, &g, &b, &a);

        // Normalize the color values to the range [0, 1]
        glm::vec4 normalizedColor;
        normalizedColor.r = static_cast<float>(r) / 255.0f;
        normalizedColor.g = static_cast<float>(g) / 255.0f;
        normalizedColor.b = static_cast<float>(b) / 255.0f;
        normalizedColor.a = static_cast<float>(a) / 255.0f;

        return normalizedColor;
    }

    static inline float remap(float value, const float startA, const float endA,
        const float startB, const float endB)
    {
        /*
            [a,b]
            [c,d]
            x from [a,b]
            t1 = (x-a)/(b-a)
            y = (1-t1)*c + t1*d
        */
        if (value > endA) { return endB; }
        if (value < startA) { return startB; }

        const float t = (value - startA) / (endA - startA);
        return (1.0f - t) * startB + t * endB;
    }
};
} // namespace msgui