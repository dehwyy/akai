#include "ui.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include "../../deps/imgui-sfml/imgui.h"
#include "../function/parser.hpp"

using namespace ui;

namespace {
    const int WIDTH = 640;        // Ширина окна
    const int HEIGHT = 480;       // Высота окна
    const float SCALE_X = 5.0f;   // Масштаб по X (увеличение)
    const float SCALE_Y = 10.0f;  // Масштаб по Y (увеличение)

    // const double EPSILON = 0.1;
}  // namespace

void UI::Render(sf::RenderWindow& window, state::State& state) {
    ImGui::Begin("Settings");
    static float k = 1;
    static float speed = 1;
    static char functionInput[256];
    static variable::Variables vars;

    if (ImGui::TreeNode("Function")) {
        if (ImGui::InputText("Input", functionInput, 256)) {
            state.SetFunctionInput(functionInput);
        }
        ImGui::SliderFloat("x", &k, 0.01, 3.0);
        ImGui::SliderFloat("Speed", &speed, 0.1, 10.0);

        ImGui::TreePop();
    }

    std::cout << "Function: " << state.GetFunctionInput() << std::endl;

    // std::cout << "k = " << k << std::endl;
    sf::VertexArray graph(sf::LineStrip);

    auto f = parser::Parser::Parse(state.GetFunctionInput());
    vars.SetVariable("x", k);

    if (f.has_value()) {
        for (float x = -8.0; x <= 8.0; x += 0.1) {
            float y = f->get()->GetValue(vars);
            // float y = std::sin(x);  // Функция

            // Преобразование координат в пиксели
            float pixelX = WIDTH / 2 + x * SCALE_X * k;
            float pixelY = HEIGHT / 2 - y * SCALE_Y;

            graph.append(sf::Vertex(sf::Vector2f(pixelX, pixelY), sf::Color::Magenta));
        }

        window.draw(graph);
    }

    ImGui::End();
}
