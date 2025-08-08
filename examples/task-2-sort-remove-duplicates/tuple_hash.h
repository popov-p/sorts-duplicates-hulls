#pragma once

#include <cstdint>
#include <tuple>
#include <cstring>
#include <functional>

inline uint32_t float_to_uint32(float f) {
    uint32_t i;
    static_assert(sizeof(float) == sizeof(uint32_t), "float и uint32_t должны быть одного размера");
    std::memcpy(&i, &f, sizeof(float));
    return i;
}

struct TupleHash {
    std::size_t operator()(const std::tuple<float, float, float>& t) const noexcept {
        auto hash_combine = [](size_t seed, size_t value) {
            return seed ^ (value + 0x9e3779b9 + (seed << 6) + (seed >> 2));
        };

        std::size_t h1 = std::hash<uint32_t>{}(float_to_uint32(std::get<0>(t)));
        std::size_t h2 = std::hash<uint32_t>{}(float_to_uint32(std::get<1>(t)));
        std::size_t h3 = std::hash<uint32_t>{}(float_to_uint32(std::get<2>(t)));

        std::size_t seed = 0;
        seed = hash_combine(seed, h1);
        seed = hash_combine(seed, h2);
        seed = hash_combine(seed, h3);

        return seed;
    }
};
