#pragma once

#include <string>
#include <random>

#include <glm/glm.hpp>

#include "Logger.hpp"
#include "core/node/AbstractNode.hpp"

namespace msgui
{
class Utils
{
public:
    /**
        Generates a random RGB color.

        @return Random color computed
     */
    static inline glm::vec4 randomRGB()
    {
        return {random01(), random01(), random01(), 1.0f};
    }

    /**
        Generates a random RGBA color.

        @return Random color computed
     */
    static inline glm::vec4 randomRGBA()
    {
        return {random01(), random01(), random01(), random01()};
    }

    /**
        Generates a random value between 0 and 1.

        @return Random value computed
     */
    static inline float random01()
    {
        static std::random_device rd;
        static std::mt19937 generator(rd());

        static std::uniform_real_distribution<> distance(0.0, 1.0);
        return distance(generator);
    }

    /**
        Convert a hex string to a normalized RGBA vector.

        @param hexColor Hex color to convert (#RRGGBBAA)

        @return Normalized color vector
    */
    static inline glm::vec4 hexToVec4(const std::string& hexColor)
    {
        /* A bit restrictive but good enough for now */
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

    /**
        Remamps a value that is normally between A and B to a value between C and D linearly.

        @param value Value to remap
        @param startA Value range start before remap
        @param startB Value range end before remap
        @param endA Value range start after remap
        @param endB Value range end after remap

        @return Remapped value
    */
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
        if (endA - startA < 0.001f) {return startA;}

        const float t = (value - startA) / (endA - startA);
        return (1.0f - t) * startB + t * endB;
    }

    /**
        Fatally exit because of specific reason.

        @param Reason for exit
     */
    static void fatalExit(const std::string& reason)
    {
        Logger("Utils").errorLn("Fatal exit from: %s", reason.c_str());
        exit(2);
    }

    /**
        Cast an AbstractNodePtr as a templated type Type.

        @param node Node to be cast

        @return Type as shared pointer
    */
    template<typename Type>
    static std::shared_ptr<Type> as(AbstractNodePtr node)
    {
        if (!node) { Logger().errorLn("Cast to type of null node!"); }
        return std::static_pointer_cast<Type>(node);
    }

    /**
        Easier variant to create a new node instead of std::make_shared each time.

        @param Type Type of node supplied as template argument
        @param args Type constructor arguments

        @return New shared pointer of specified type
    */
    template<typename Type, typename... Args>
    static std::shared_ptr<Type> make(Args... args)
    {
        return std::make_shared<Type>(std::forward<Args>(args)...);
    }
};
} // namespace msgui