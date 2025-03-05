#include "ui.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include "app/function/parser.hpp"
#include "deps/imgui-sfml/imgui.h"

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
    static float z = 1;
    static float speed = 1;
    static char functionInput[256];

    static variables::VariablesContainer vars;

    if (ImGui::TreeNode("Function")) {
        if (ImGui::InputText("Input", functionInput, 256)) {
            state.SetFunctionInput(functionInput);
        }
        ImGui::SliderFloat("z", &z, 0.01, 3.0);
        ImGui::SliderFloat("Speed", &speed, 0.1, 10.0);

        ImGui::TreePop();
    }

    std::cout << "Function: " << state.GetFunctionInput() << std::endl;

    // std::cout << "k = " << k << std::endl;
    sf::VertexArray graph(sf::LineStrip);
    try {
        auto f = parser::TokensSequence(state.GetFunctionInput()).AsFunction().get();
        vars.put("z", z);

        for (int x = -WIDTH / 2; x <= WIDTH / 2; ++x) {
            vars.put("x", (double)x);

            std::cout << f << std::endl;
            auto [y, e] = f->GetValue(vars).unwrapRef();
            std::cout << "Y = " << y << std::endl;

            float pixelX = WIDTH / 2 + x;
            float pixelY = HEIGHT / 2 - y;

            // Преобразование координат в пиксели
            // float pixelX = WIDTH / 2 + x * SCALE_X * z;
            // float pixelY = HEIGHT / 2 - y * SCALE_Y;

            graph.append(sf::Vertex(sf::Vector2f(pixelX, pixelY), sf::Color::Magenta));
        }

        window.draw(graph);
    } catch (std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
    ImGui::End();
}
