#pragma once
#include <iostream>

namespace logger {

    enum class LogLevel { Print = 0, Info = 1, Warning = 2, Debug = 3, Trace = 4 };

    class Log {
        private:
        public:
            static logger::LogLevel LOG_LEVEL;
            static void SetLogLevel(LogLevel level) { LOG_LEVEL = level; }

            template <typename... Args>
            static void Error(Args... args) {
                (std::cerr << ... << args) << std::endl;
            }

            template <typename... Args>
            static void Print(Args... args) {
                if (static_cast<int>(LOG_LEVEL) >= 0) {
                    (std::cout << ... << args) << std::endl;
                }
            }

            template <typename... Args>
            static void Info(Args... args) {
                if (static_cast<int>(LOG_LEVEL) >= 1) {
                    (std::cout << ... << args) << std::endl;
                }
            }

            template <typename... Args>
            static void Warn(Args... args) {
                if (static_cast<int>(LOG_LEVEL) >= 2) {
                    (std::cout << ... << args) << std::endl;
                }
            }

            template <typename... Args>
            static void Debug(Args... args) {
                if (static_cast<int>(LOG_LEVEL) >= 3) {
                    (std::cout << ... << args) << std::endl;
                }
            }

            template <typename... Args>
            static void Trace(Args... args) {
                if (static_cast<int>(LOG_LEVEL) >= 4) {
                    (std::cout << ... << args) << std::endl;
                }
            }
    };

}  // namespace logger
