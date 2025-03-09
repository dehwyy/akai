#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "config.hpp"
#include "lib.hpp"

namespace app {
    class App {
        private:
            Rc<sf::RenderWindow> window;

            void loadIcon(const std::string& filepath);

        public:
            App(const config::Config& config);
            ~App();

            void start();
    };
}  // namespace app
