#pragma once
#include <string>
#include <vector>
#include "lib.hpp"

namespace {
    std::string parse(char* str, std::vector<std::string> aliases) {
        const std::string s = str;
        for (const auto& alias : aliases) {
            if (s.find(alias) == 0) {
                if (s.at(alias.length()) == '=') {
                    return s.substr(alias.length() + 1);
                }
            }
        }

        return "";
    }
}  // namespace

namespace config {
    class Config {
        private:
            int windowWidth = 1280;
            int windowHeight = 960;
            std::string windowTitle = "Akai";
            std::string iconFilepath = "assets/icon.png";
            int FPSLimit = 60;

            logger::LogLevel logLevel = logger::LogLevel::Print;

            void setLogLevel(std::string level) {
                if (std::isdigit(level[0])) {
                    setLogLevel(atoi(level.c_str()));
                    return;
                }
                if (level == "print") {
                    logLevel = logger::LogLevel::Print;
                } else if (level == "info") {
                    logLevel = logger::LogLevel::Info;
                } else if (level == "warning") {
                    logLevel = logger::LogLevel::Warning;
                } else if (level == "debug") {
                    logLevel = logger::LogLevel::Debug;
                } else if (level == "trace") {
                    logLevel = logger::LogLevel::Trace;
                }
            }
            void setLogLevel(int level) { logLevel = To<logger::LogLevel>(level); }

        public:
            Config(int argc, char** argv) {
                std::string value = "";
                for (int i = 1; i < argc; i++) {
                    if ((value = parse(argv[i], {"--log", "-l"})) != "") {
                        setLogLevel(value);
                    }
                }
            }

            logger::LogLevel getLogLevel() const { return logLevel; }

            int getWindowWidth() const { return windowWidth; }
            int getWindowHeight() const { return windowHeight; }
            std::string getWindowTitle() const { return windowTitle; }
            std::string getIconFilepath() const { return iconFilepath; }
            int getFPSLimit() const { return FPSLimit; }
    };
}  // namespace config
