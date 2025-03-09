#pragma once
#include <unordered_map>
#include "SFML/Graphics.hpp"
#include "components.hpp"
#include "deps/imgui-sfml/imgui-sfml.hpp"
#include "lib.hpp"
#include "state/state.hpp"

namespace ui {

    class UI {
        private:
            static void RenderVariableConfig(const std::string& variable, double& value) {
                static std::unordered_map<std::string, std::pair<double, double>> ranges;
                if (ranges.find(variable) == ranges.end()) {
                    ranges[variable] = std::make_pair(0, 10);
                }

                const std::string label = "Variable: " + variable + " = " + std::to_string(value);
                const std::string lowerBoundaryLabel = "Lower boundary: " + variable;
                const std::string upperBoundaryLabel = "Upper boundary: " + variable;

                // ImGui::PushID(label.c_str());
                ImGui::InputDouble(lowerBoundaryLabel.c_str(), &ranges[variable].first, 0.01f, 1.0);
                ImGui::InputDouble(upperBoundaryLabel.c_str(), &ranges[variable].second, 0.01f, 1.0f);

                float t = value;
                ImGui::SliderFloat(
                    variable.c_str(), &t, ranges[variable].first, ranges[variable].second);

                value = t;
                // Log::Print(ranges[variable].first, " ", ranges[variable].second, " ", value);

                // ImGui::PopID();
            }
            static void RenderSettings(Rc<sf::RenderWindow> window, state::State& state) {
                ImGui::Begin("Settings");

                if (ImGui::TreeNode("Function")) {
                    Input("Function", state.getFunctionInput());

                    for (auto& [key, value] : state.getFunctionVariables()) {
                        RenderVariableConfig(key, value);
                    }

                    // Speed, time, etc. adjustments

                    ImGui::TreePop();
                }

                ImGui::End();
            }

            static void RenderFunction(Rc<sf::RenderWindow> window, state::State& state) {
                sf::VertexArray graph(sf::LineStrip);

                state.updateFunctionIfNecessary();

                if (state.getFunction().isNone()) {
                    Log::Error("Function is not defined");
                    return;
                }

                auto f = state.getFunction().unwrapRef();

                auto [w, h] = window->getSize();

                for (double x = (double)w / -2; x < (double)w / 2; x += (double)w / 10000) {
                    state.getFunctionVariables().put("x", x);

                    auto [y, err] = f->GetValue(state.getFunctionVariables()).get();
                    if (err) {
                        Log::Error(*err);
                        continue;
                    }

                    // Log::Print("x = ", x, ", y = ", y);

                    float pixelX = w / 2 + x * 10;
                    float pixelY = h / 2 - y * 10;

                    graph.append(sf::Vertex(sf::Vector2f(pixelX, pixelY), sf::Color::Magenta));
                }

                window->draw(graph);
            }

        public:
            static void Render(Rc<sf::RenderWindow> window, state::State& state);
    };
}  // namespace ui
