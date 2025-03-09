#pragma once

namespace utils {
    template <typename T, typename U>
    inline T To(U value) {
        return static_cast<T>(value);
    }
}  // namespace utils
