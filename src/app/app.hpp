#pragma once

#include <SFML/Graphics.hpp>

namespace app {
    class App {
        private:
            sf::RenderWindow* window;

            void loadIcon();
            void initImGUI();

        public:
            App();
            ~App();

            void Init();
            void Start();
    };
}  // namespace app
