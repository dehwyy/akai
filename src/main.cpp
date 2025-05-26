#include <SFML/Graphics.hpp>
#include <map>
#include <optional>
#include <string>
#include <vector>
#include "app/function/parser.hpp"
#include "deps/imgui-sfml/imgui-sfml.hpp"
#include "deps/imgui-sfml/imgui.h"

// Colors and base settings
sf::Color graphColor = sf::Color::Green;
sf::Color backgroundColor = sf::Color::Black;

struct Graph {
        std::string expression;
        std::vector<sf::Vertex> vertices;
        sf::Color color;
};

float xMin = -10.f;
float xMax = 10.f;
float yMin = -10.f;
float yMax = 10.f;

const float panStep = 1.f;
const float zoomFactor = 1.2f;

std::vector<Graph> graphs;

// Function to compute and update graph points
// Returns optional with error string in case of parsing error
std::optional<std::string> updateGraphPoints(Graph& graph, unsigned width, unsigned height) {
    graph.vertices.clear();

    const unsigned samples = width;  // number of points equals window width
    for (unsigned i = 0; i < samples; ++i) {
        float x = xMin + (xMax - xMin) * i / (samples - 1);
        std::map<std::string, float> vars = {{"x", x}};
        auto res = parse_and_eval(graph.expression, vars);
        if (!res.ok()) {
            return res.unwrap_err();
        }
        float y = res.unwrap();

        // Normalize to screen coordinates
        float normX = (x - xMin) / (xMax - xMin) * width;
        float normY = height - (y - yMin) / (yMax - yMin) * height;

        // Filter points outside Y bounds
        if (normY < 0 || normY > height) {
            continue;
        }

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

    // X-axis
    if (yMin < 0 && yMax > 0) {
        float yZero = height - (-yMin) / (yMax - yMin) * height;
        lines.append(sf::Vertex(sf::Vector2f(0, yZero), sf::Color(100, 100, 100)));
        lines.append(sf::Vertex(sf::Vector2f(width, yZero), sf::Color(100, 100, 100)));
    }
    // Y-axis
    if (xMin < 0 && xMax > 0) {
        float xZero = (-xMin) / (xMax - xMin) * width;
        lines.append(sf::Vertex(sf::Vector2f(xZero, 0), sf::Color(100, 100, 100)));
        lines.append(sf::Vertex(sf::Vector2f(xZero, height), sf::Color(100, 100, 100)));
    }

    window.draw(lines);
}

void handleControls(unsigned width, unsigned height) {
    ImGui::Begin("Graph Settings");

    // Input buffer for new expression
    static char expr_buf[256] = "";
    ImGui::InputText("Add expression f(x)", expr_buf, sizeof(expr_buf));

    ImGui::SameLine();
    if (ImGui::Button("Add Graph")) {
        if (strlen(expr_buf) > 0) {
            std::map<std::string, float> vars = {{"x", 0.f}};
            auto res = parse_and_eval(expr_buf, vars);
            if (res.ok()) {
                graphs.push_back({expr_buf, {}, graphColor});
                expr_buf[0] = 0;  // clear buffer
            } else {
                ImGui::OpenPopup("Add Error");
            }
        }
    }
    if (ImGui::BeginPopup("Add Error")) {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Invalid expression!");
        ImGui::Text("Check the syntax.");
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

            if (ImGui::Button("Remove")) {
                graphs.erase(graphs.begin() + i);
                ImGui::PopID();
                break;
            }

            // Temporary buffer for expression editing
            static char buf[256];
            strncpy(buf, graphs[i].expression.c_str(), sizeof(buf));
            buf[sizeof(buf) - 1] = 0;

            if (ImGui::InputText("Expression", buf, sizeof(buf), ImGuiInputTextFlags_EnterReturnsTrue)) {
                graphs[i].expression = std::string(buf);
                auto err = updateGraphPoints(graphs[i], width, height);
                if (err) {
                    // Handle error here if needed
                }
            }

            // Graph color
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

    ImGui::Text("Background Color:");
    float bg_col[3] = {backgroundColor.r / 255.f, backgroundColor.g / 255.f, backgroundColor.b / 255.f};
    if (ImGui::ColorEdit3("##bgcolor", bg_col)) {
        backgroundColor = sf::Color(static_cast<sf::Uint8>(bg_col[0] * 255),
                                    static_cast<sf::Uint8>(bg_col[1] * 255),
                                    static_cast<sf::Uint8>(bg_col[2] * 255));
    }

    ImGui::Separator();

    // Zoom
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
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Scale the graph");
    }

    ImGui::Separator();

    // Pan X
    if (ImGui::Button("Left")) {
        xMin -= panStep;
        xMax -= panStep;
    }
    ImGui::SameLine();
    if (ImGui::Button("Right")) {
        xMin += panStep;
        xMax += panStep;
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Shift graph along X axis");
    }

    // Pan Y
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
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Shift graph along Y axis");
    }

    ImGui::Separator();

    ImGui::Text("X: %.2f .. %.2f", xMin, xMax);
    ImGui::Text("Y: %.2f .. %.2f", yMin, yMax);

    ImGui::End();
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Akai");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    sf::Clock deltaClock;

    // Initial graph example
    graphs.push_back({"sin(x)", {}, graphColor});
    updateGraphPoints(graphs.back(), window.getSize().x, window.getSize().y);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::Resized) {
                // Update view on resize
                sf::View view = window.getView();
                view.setSize(event.size.width, event.size.height);
                window.setView(view);
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        // Update each graph
        for (auto& graph : graphs) {
            auto err = updateGraphPoints(graph, window.getSize().x, window.getSize().y);
            if (err) {
                // You can display or log the error here
            }
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
