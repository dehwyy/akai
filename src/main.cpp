#include <SFML/Graphics.hpp>
#include <map>
#include <optional>
#include <regex>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include "app/function/parser.hpp"
#include "deps/imgui-sfml/imgui-sfml.hpp"
#include "deps/imgui-sfml/imgui.h"

sf::Color graphColor = sf::Color::Green;
sf::Color backgroundColor = sf::Color::Black;

struct Graph {
        std::string expression;
        std::vector<sf::Vertex> vertices;
        sf::Color color;
        std::set<std::string> variables;
};

struct VariableState {
        float value = 1.0f;
        bool animated = false;
        float min = 0.0f;
        float max = 2.0f;
        float period = 2.0f;  // seconds
};

float xMin = -10.f;
float xMax = 10.f;
float yMin = -10.f;
float yMax = 10.f;

const float panStep = 1.f;
const float zoomFactor = 1.2f;

std::vector<Graph> graphs;
std::unordered_map<std::string, VariableState> globalVariables;

float currentTime = 0.0f;

// Извлекаем переменные из выражения с помощью regex
std::set<std::string> extractVariables(const std::string& expr) {
    std::set<std::string> vars;
    std::regex re(R"(\b([a-zA-Z_][a-zA-Z0-9_]*)\b)");
    std::sregex_iterator it(expr.begin(), expr.end(), re);
    std::sregex_iterator end;

    while (it != end) {
        std::string var = (*it)[1].str();
        if (var != "sin" && var != "cos" && var != "tan" && var != "log" && var != "exp" &&
            var != "sqrt" && var != "abs" && var != "x") {
            vars.insert(var);
        }
        ++it;
    }
    return vars;
}

std::optional<std::string> updateGraphPoints(Graph& graph, unsigned width, unsigned height) {
    graph.vertices.clear();

    const unsigned samples = width;
    for (unsigned i = 0; i < samples; ++i) {
        float x = xMin + (xMax - xMin) * i / (samples - 1);
        std::map<std::string, float> vars = {{"x", x}};
        for (const auto& var : graph.variables) {
            vars[var] = globalVariables[var].value;
        }
        auto res = parse_and_eval(graph.expression, vars);
        if (!res.ok())
            return res.unwrap_err();

        float y = res.unwrap();
        float normX = (x - xMin) / (xMax - xMin) * width;
        float normY = height - (y - yMin) / (yMax - yMin) * height;

        if (normY < 0 || normY > height)
            continue;

        graph.vertices.emplace_back(sf::Vector2f(normX, normY), graph.color);
    }

    return std::nullopt;
}

void drawGraphs(sf::RenderWindow& window) {
    for (const auto& graph : graphs) {
        if (!graph.vertices.empty()) {
            window.draw(graph.vertices.data(), graph.vertices.size(), sf::LinesStrip);
        }
    }
}

void drawGrid(sf::RenderWindow& window, unsigned width, unsigned height) {
    sf::VertexArray lines(sf::Lines);

    if (yMin < 0 && yMax > 0) {
        float yZero = height - (-yMin) / (yMax - yMin) * height;
        lines.append(sf::Vertex(sf::Vector2f(0, yZero), sf::Color(100, 100, 100)));
        lines.append(sf::Vertex(sf::Vector2f(width, yZero), sf::Color(100, 100, 100)));
    }
    if (xMin < 0 && xMax > 0) {
        float xZero = (-xMin) / (xMax - xMin) * width;
        lines.append(sf::Vertex(sf::Vector2f(xZero, 0), sf::Color(100, 100, 100)));
        lines.append(sf::Vertex(sf::Vector2f(xZero, height), sf::Color(100, 100, 100)));
    }

    window.draw(lines);
}

void handleControls(unsigned width, unsigned height) {
    ImGui::Begin("Graph Settings");

    static char expr_buf[256] = "";
    ImGui::InputText("Add expression f(x)", expr_buf, sizeof(expr_buf));
    ImGui::SameLine();
    if (ImGui::Button("Add graph")) {
        if (strlen(expr_buf) > 0) {
            std::map<std::string, float> vars = {{"x", 0.f}};
            auto res = parse_and_eval(expr_buf, vars);
            if (res.ok()) {
                Graph g;
                g.expression = expr_buf;
                g.color = graphColor;
                g.variables = extractVariables(expr_buf);
                for (const auto& var : g.variables) {
                    globalVariables.try_emplace(var, VariableState{});
                }
                graphs.push_back(g);
                expr_buf[0] = 0;
            } else {
                ImGui::OpenPopup("Error");
            }
        }
    }

    if (ImGui::BeginPopup("Error")) {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Invalid expression!");
        ImGui::EndPopup();
    }

    ImGui::Separator();

    if (graphs.empty()) {
        ImGui::Text("No graphs. Add an expression.");
    } else {
        for (size_t i = 0; i < graphs.size(); ++i) {
            ImGui::PushID((int)i);

            ImGui::Text("Graph %d:", (int)i + 1);
            ImGui::SameLine();
            if (ImGui::Button("Delete")) {
                graphs.erase(graphs.begin() + i);
                ImGui::PopID();
                break;
            }

            static char buf[256];
            strncpy(buf, graphs[i].expression.c_str(), sizeof(buf));
            buf[sizeof(buf) - 1] = 0;

            if (ImGui::InputText("Expression", buf, sizeof(buf), ImGuiInputTextFlags_EnterReturnsTrue)) {
                graphs[i].expression = std::string(buf);
                graphs[i].variables = extractVariables(buf);
                for (const auto& var : graphs[i].variables) {
                    globalVariables.try_emplace(var, VariableState{});
                }
                updateGraphPoints(graphs[i], width, height);
            }

            float col[3] = {
                graphs[i].color.r / 255.f, graphs[i].color.g / 255.f, graphs[i].color.b / 255.f};
            if (ImGui::ColorEdit3("Graph Color", col)) {
                graphs[i].color = sf::Color(static_cast<sf::Uint8>(col[0] * 255),
                                            static_cast<sf::Uint8>(col[1] * 255),
                                            static_cast<sf::Uint8>(col[2] * 255));
                updateGraphPoints(graphs[i], width, height);
            }

            ImGui::Separator();
            ImGui::PopID();
        }
    }

    ImGui::Text("Background color:");
    float bg_col[3] = {backgroundColor.r / 255.f, backgroundColor.g / 255.f, backgroundColor.b / 255.f};
    if (ImGui::ColorEdit3("##bgcolor", bg_col)) {
        backgroundColor = sf::Color(static_cast<sf::Uint8>(bg_col[0] * 255),
                                    static_cast<sf::Uint8>(bg_col[1] * 255),
                                    static_cast<sf::Uint8>(bg_col[2] * 255));
    }

    ImGui::Separator();

    if (ImGui::Button("Zoom In")) {
        float dx = (xMax - xMin) * (1 - 1 / zoomFactor) / 2;
        float dy = (yMax - yMin) * (1 - 1 / zoomFactor) / 2;
        xMin += dx;
        xMax -= dx;
        yMin += dy;
        yMax -= dy;
    }
    ImGui::SameLine();
    if (ImGui::Button("Zoom Out")) {
        float dx = (xMax - xMin) * (zoomFactor - 1) / 2;
        float dy = (yMax - yMin) * (zoomFactor - 1) / 2;
        xMin -= dx;
        xMax += dx;
        yMin -= dy;
        yMax += dy;
    }

    ImGui::Separator();

    if (ImGui::Button("Left")) {
        xMin -= panStep;
        xMax -= panStep;
    }
    ImGui::SameLine();
    if (ImGui::Button("Right")) {
        xMin += panStep;
        xMax += panStep;
    }
    ImGui::SameLine();
    if (ImGui::Button("Up")) {
        yMin += panStep;
        yMax += panStep;
    }
    ImGui::SameLine();
    if (ImGui::Button("Down")) {
        yMin -= panStep;
        yMax -= panStep;
    }

    ImGui::Separator();
    ImGui::Text("X: %.2f .. %.2f", xMin, xMax);
    ImGui::Text("Y: %.2f .. %.2f", yMin, yMax);
    ImGui::Separator();

    for (auto& [name, var] : globalVariables) {
        ImGui::PushID(name.c_str());
        ImGui::Text("Variable: %s", name.c_str());
        if (!var.animated) {
            ImGui::SliderFloat("Value", &var.value, -10.f, 10.f);
        }
        ImGui::Checkbox("Animate", &var.animated);
        if (var.animated) {
            ImGui::SliderFloat("Min", &var.min, -10.f, var.max);
            ImGui::SliderFloat("Max", &var.max, var.min, 10.f);
            ImGui::SliderFloat("Period (s)", &var.period, 0.1f, 10.f);
        }
        ImGui::Separator();
        ImGui::PopID();
    }

    ImGui::End();
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Akai");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    sf::Clock deltaClock;
    graphs.push_back({"sin(x)", {}, graphColor});
    updateGraphPoints(graphs.back(), window.getSize().x, window.getSize().y);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::Resized) {
                sf::View view = window.getView();
                view.setSize(event.size.width, event.size.height);
                window.setView(view);
            }
        }

        float dt = deltaClock.restart().asSeconds();
        currentTime += dt;

        for (auto& [_, var] : globalVariables) {
            if (var.animated && var.period > 0.01f) {
                float t = fmod(currentTime, var.period) / var.period;
                var.value =
                    var.min + (var.max - var.min) * (0.5f * (1 + sin(2 * 3.14159f * t - 3.14159f / 2)));
            }
        }

        ImGui::SFML::Update(window, sf::seconds(dt));

        for (auto& graph : graphs) {
            updateGraphPoints(graph, window.getSize().x, window.getSize().y);
        }

        handleControls(window.getSize().x, window.getSize().y);

        window.clear(backgroundColor);
        drawGrid(window, window.getSize().x, window.getSize().y);
        drawGraphs(window);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
