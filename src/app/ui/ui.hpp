#pragma once
#include "../../deps/imgui-sfml/imgui-sfml.hpp"
#include "../state/state.hpp"

namespace ui {

    class UI {
        public:
            static void Render(sf::RenderWindow& window, state::State& state);
    };
}  // namespace ui
