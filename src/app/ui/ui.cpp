#include "ui.hpp"
#include <SFML/Graphics.hpp>
// #include <cmath>
// #include <iostream>
// #include <string>
// #include "app/function/parser.hpp"
// #include "deps/imgui-sfml/imgui.h"
#include "lib.hpp"

using namespace ui;

void UI::Render(Rc<sf::RenderWindow> window, state::State& state) {
    UI::RenderSettings(window, state);
    UI::RenderFunction(window, state);

    // ImGui::Begin("Settings");
    // static float z = 1;
    // static float speed = 1;
    // static char functionInput[256];
    // static variables::VariablesContainer vars;
    // static function::F func;

    // if (ImGui::TreeNode("Function")) {
    //     if (ImGui::InputText("Input", functionInput, 256)) {
    //         state.SetFunctionInput(functionInput);
    //     }
    //     ImGui::SliderFloat("z", &z, 0.01, 3.0);
    //     ImGui::SliderFloat("Speed", &speed, 0.1, 10.0);

    //     ImGui::TreePop();
    // }

    // sf::VertexArray graph(sf::LineStrip);
    // try {
    //     if (state.ShouldRebuildFunction()) {
    //         Log::Info("Function recompilation...");
    //         state.UpdatePreviousRenderFunctionInput();
    //         func = parser::TokensSequence(state.GetFunctionInput()).AsFunction();
    //     }
    //     vars.put("z", z);

    //     if (!func) {
    //         return;
    //     }

    //     for (int x = -WIDTH / 2; x <= WIDTH / 2; ++x) {
    //         vars.put("x", (double)x);

    //         auto [y, e] = func->GetValue(vars).get();
    //         if (e) {
    //             // Log::Error("Error: ", *e, " X = ", x);
    //             continue;
    //         }

    //         float pixelX = WIDTH / 2 + x;
    //         float pixelY = HEIGHT / 2 - y;

    //         // Преобразование координат в пиксели
    //         // float pixelX = WIDTH / 2 + x * SCALE_X * z;
    //         // float pixelY = HEIGHT / 2 - y * SCALE_Y;

    //         graph.append(sf::Vertex(sf::Vector2f(pixelX, pixelY), sf::Color::Magenta));
    //     }

    //     window.draw(graph);
    // } catch (std::exception& e) {
    //     // Log::Error("Error: ", e.what());
    // }
    // ImGui::End();
}
