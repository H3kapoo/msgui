#pragma once

#include <memory>
#include <string>
#include <random>

#include <glm/glm.hpp>

#include "Logger.hpp"
#include "msgui/node/AbstractNode.hpp"

namespace msgui
{
class Utils
{
public: // constants
    static constexpr glm::vec4 COLOR_NONE{0.0f, 0.0f, 0.0f, 0.0f};
    static constexpr glm::vec4 COLOR_BLACK{0.0f, 0.0f, 0.0f, 1.0f};
    static constexpr glm::vec4 COLOR_WHITE{1.0f, 1.0f, 1.0f, 1.0f};
    static constexpr glm::vec4 COLOR_RED{1.0f, 0.0f, 0.0f, 1.0f};
    static constexpr glm::vec4 COLOR_GREEN{0.0f, 1.0f, 0.0f, 1.0f};
    static constexpr glm::vec4 COLOR_BLUE{0.0f, 0.0f, 1.0f, 1.0f};
    static constexpr glm::vec4 COLOR_YELLOW{1.0f, 1.0f, 0.0f, 1.0f};
    static constexpr glm::vec4 COLOR_CYAN{0.0f, 1.0f, 1.0f, 1.0f};
    static constexpr glm::vec4 COLOR_MAGENTA{1.0f, 0.0f, 1.0f, 1.0f};

public:
    /**
        Darken a color by a certain percentage.

        @param color Color to be darkened
        @param perc Percentage representing how much to darken (0 <= perc <= 1)

        @return Darkened color capped if exceeding RGB limits
     */
    static inline glm::vec4 darken(const glm::vec4& color, const float perc)
    {
        return {color.r - (color.r * perc), color.g - (color.g * perc), color.b - (color.b * perc), 1.0f};
    }

    /**
        Lighten a color by a certain percentage.

        @param color Color to be lightened
        @param perc Percentage representing how much to lighten (0 <= perc <= 1)

        @return Lightened color capped if exceeding RGB limits
     */
     static inline glm::vec4 lighten(const glm::vec4& color, const float perc)
     {
         return {color.r + (color.r * perc), color.g + (color.g * perc), color.b + (color.b * perc), 1.0f};
     }

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
        Generates a random integer value between [min, max].

        @param min Minimum value to return
        @param max Maximum value to return

        @return Random value computed
     */
     static inline int32_t randomInt(const int32_t min, const int32_t max)
     {
         return min + (random01() * (max - min));
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

        if (hexColor.size() == 7) { a = 1.0f; }

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
        if (!node)
        {
            Logger("Utils").errorLn("Cast to type of null node!");
            return nullptr;
        }
        return std::static_pointer_cast<Type>(node);
    }

    /**
        Easier variant to create a new node instead of std::make_shared each time.

        @param Type Type of node supplied as template argument
        @param args Type constructor arguments

        @return New shared pointer of specified type
    */
    template<typename Type, typename... Args>
    static std::shared_ptr<Type> make(Args&&... args)
    {
        return std::make_shared<Type>(std::forward<Args>(args)...);
    }

    /**
        Easier variant to create a new weak pointer for a node.
        Useful when capturing ourselves inside a callback in order to about cyclic references.

        @param Type Type of node supplied as template argument
        @param args Type constructor arguments

        @return New shared pointer of specified type
    */
    template<typename Type, typename... Args>
    static std::weak_ptr<Type> ref(Args&&... args)
    {
        return std::weak_ptr<Type>(std::forward<Args>(args)...);
    }
};
} // namespace msgui