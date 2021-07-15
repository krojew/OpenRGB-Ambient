//
// Created by Kamil Rojewski on 15.07.2021.
//

#ifndef OPENRGB_AMBIENT_LIMITER_H
#define OPENRGB_AMBIENT_LIMITER_H

#include <chrono>

class Limiter final
{
public:
    explicit Limiter(float hz);

    void sleep();

private:
    std::chrono::high_resolution_clock::time_point workStart{std::chrono::high_resolution_clock::now()};
    std::chrono::microseconds periodUs;
};

#endif //OPENRGB_AMBIENT_LIMITER_H
