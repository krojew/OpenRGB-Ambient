//
// Created by Kamil Rojewski on 15.07.2021.
//

#include <thread>

#include "Limiter.h"

Limiter::Limiter(float hz)
    : periodUs{static_cast<size_t>(1e6 / hz)}
{
}

void Limiter::sleep()
{
    const auto workTime = std::chrono::high_resolution_clock::now() - workStart;
    if (workTime < periodUs)
    {
        const auto delta = periodUs - workTime;
        std::this_thread::sleep_for(delta);
    }

    workStart = std::chrono::high_resolution_clock::now();
}
