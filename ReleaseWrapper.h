//
// Created by Kamil Rojewski on 15.07.2021.
//

#ifndef OPENRGB_AMBIENT_RELEASEWRAPPER_H
#define OPENRGB_AMBIENT_RELEASEWRAPPER_H

#include <memory>

template <typename T>
std::shared_ptr<T> releasing(T* z)
{
    return std::shared_ptr<T>(z, [](T *z) { z->Release(); });
}

#endif //OPENRGB_AMBIENT_RELEASEWRAPPER_H
