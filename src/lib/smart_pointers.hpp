#pragma once
#include <memory>

template <typename T>
using Box = std::unique_ptr<T>;

template <typename T, typename... Args>
Box<T> NewBox(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
using Rc = std::shared_ptr<T>;
