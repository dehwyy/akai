#include <iostream>

#define DEBUG true

class Debug {
    public:
        template <typename... Args>
        static void Print(Args... args) {
            if (DEBUG) {
                (std::cout << ... << args) << std::endl;
            }
        }
};
