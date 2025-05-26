#include <imgui.h>
#include <SFML/Graphics.hpp>
#include <deps/imgui-sfml/imgui-sfml.hpp>
#include <map>  // добавил для std::map
#include <optional>
#include <string>
#include <vector>
#include "parser.hpp"

// Цвета и базовые настройки
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

// Функция для вычисления и обновления точек графика
// Возвращает optional с ошибкой в случае ошибки парсинга
std::optional<std::string> updateGraphPoints(Graph& graph, unsigned width, unsigned height) {
    graph.vertices.clear();

    const unsigned samples = width;  // кол-во точек равно ширине окна
    for (unsigned i = 0; i < samples; ++i) {
        float x = xMin + (xMax - xMin) * i / (samples - 1);
        // Теперь передаем переменные в виде map
        std::map<std::string, float> vars = {{"x", x}};
        auto res = parse_and_eval(graph.expression, vars);
        if (!res.ok()) {
            return res.unwrap_err();
        }
        float y = res.unwrap();

        // Нормализация координат в экранные
        float normX = (x - xMin) / (xMax - xMin) * width;
        float normY = height - (y - yMin) / (yMax - yMin) * height;

        // Фильтруем точки вне окна по Y (можно добавить и по X, если нужно)
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

    // Ось X
    if (yMin < 0 && yMax > 0) {
        float yZero = height - (-yMin) / (yMax - yMin) * height;
        lines.append(sf::Vertex(sf::Vector2f(0, yZero), sf::Color(100, 100, 100)));
        lines.append(sf::Vertex(sf::Vector2f(width, yZero), sf::Color(100, 100, 100)));
    }
    // Ось Y
    if (xMin < 0 && xMax > 0) {
        float xZero = (-xMin) / (xMax - xMin) * width;
        lines.append(sf::Vertex(sf::Vector2f(xZero, 0), sf::Color(100, 100, 100)));
        lines.append(sf::Vertex(sf::Vector2f(xZero, height), sf::Color(100, 100, 100)));
    }

    window.draw(lines);
}

void handleControls(unsigned width, unsigned height) {
    ImGui::Begin("Настройки графика");

    // Буфер для ввода нового выражения
    static char expr_buf[256] = "";
    ImGui::InputText("Добавить выражение f(x)", expr_buf, sizeof(expr_buf));

    ImGui::SameLine();
    if (ImGui::Button("Добавить график")) {
        if (strlen(expr_buf) > 0) {
            // Проверка выражения на x=0
            std::map<std::string, float> vars = {{"x", 0.f}};
            auto res = parse_and_eval(expr_buf, vars);
            if (res.ok()) {
                graphs.push_back({expr_buf, {}, graphColor});
                expr_buf[0] = 0;  // очистить буфер
            } else {
                ImGui::OpenPopup("Ошибка при добавлении");
            }
        }
    }
    if (ImGui::BeginPopup("Ошибка при добавлении")) {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Некорректное выражение!");
        ImGui::Text("Проверьте синтаксис.");
        ImGui::EndPopup();
    }

    ImGui::Separator();

    if (graphs.empty()) {
        ImGui::Text("Графики отсутствуют. Добавьте выражение.");
    } else {
        for (size_t i = 0; i < graphs.size(); ++i) {
            ImGui::PushID((int)i);

            ImGui::Text("График %d:", (int)i + 1);
            ImGui::SameLine();

            if (ImGui::Button("Удалить")) {
                graphs.erase(graphs.begin() + i);
                ImGui::PopID();
                break;
            }

            // Временный буфер для редактирования выражения
            static char buf[256];
            strncpy(buf, graphs[i].expression.c_str(), sizeof(buf));
            buf[sizeof(buf) - 1] = 0;

            if (ImGui::InputText("Выражение", buf, sizeof(buf), ImGuiInputTextFlags_EnterReturnsTrue)) {
                graphs[i].expression = std::string(buf);
                auto err = updateGraphPoints(graphs[i], width, height);
                if (err) {
                    // Можно обработать ошибку, например, показать в отдельном popup
                }
            }

            // Цвет графика
            float col[3] = {
                graphs[i].color.r / 255.f, graphs[i].color.g / 255.f, graphs[i].color.b / 255.f};
            if (ImGui::ColorEdit3("Цвет графика", col)) {
                graphs[i].color = sf::Color(static_cast<sf::Uint8>(col[0] * 255),
                                            static_cast<sf::Uint8>(col[1] * 255),
                                            static_cast<sf::Uint8>(col[2] * 255));
                updateGraphPoints(graphs[i], width, height);
            }

            ImGui::Separator();
            ImGui::PopID();
        }
    }

    ImGui::Text("Цвет фона:");
    float bg_col[3] = {backgroundColor.r / 255.f, backgroundColor.g / 255.f, backgroundColor.b / 255.f};
    if (ImGui::ColorEdit3("##bgcolor", bg_col)) {
        backgroundColor = sf::Color(static_cast<sf::Uint8>(bg_col[0] * 255),
                                    static_cast<sf::Uint8>(bg_col[1] * 255),
                                    static_cast<sf::Uint8>(bg_col[2] * 255));
    }

    ImGui::Separator();

    // Зум
    if (ImGui::Button("Приблизить")) {
        float dx = (xMax - xMin) * (1 - 1 / zoomFactor) / 2;
        float dy = (yMax - yMin) * (1 - 1 / zoomFactor) / 2;
        xMin += dx;
        xMax -= dx;
        yMin += dy;
        yMax -= dy;
    }
    ImGui::SameLine();
    if (ImGui::Button("Отдалить")) {
        float dx = (xMax - xMin) * (zoomFactor - 1) / 2;
        float dy = (yMax - yMin) * (zoomFactor - 1) / 2;
        xMin -= dx;
        xMax += dx;
        yMin -= dy;
        yMax += dy;
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Масштабирование графика");
    }

    ImGui::Separator();

    // Панорамирование X
    if (ImGui::Button("Влево")) {
        xMin -= panStep;
        xMax -= panStep;
    }
    ImGui::SameLine();
    if (ImGui::Button("Вправо")) {
        xMin += panStep;
        xMax += panStep;
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Сдвиг графика по оси X");
    }

    // Панорамирование Y
    ImGui::SameLine();
    if (ImGui::Button("Вверх")) {
        yMin += panStep;
        yMax += panStep;
    }
    ImGui::SameLine();
    if (ImGui::Button("Вниз")) {
        yMin -= panStep;
        yMax -= panStep;
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Сдвиг графика по оси Y");
    }

    ImGui::Separator();

    ImGui::Text("X: %.2f .. %.2f", xMin, xMax);
    ImGui::Text("Y: %.2f .. %.2f", yMin, yMax);

    ImGui::End();
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Графики функций");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    sf::Clock deltaClock;

    // Начальный график для примера
    graphs.push_back({"sin(x)", {}, graphColor});
    updateGraphPoints(graphs.back(), window.getSize().x, window.getSize().y);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::Resized) {
                // Обновляем вид при изменении окна
                sf::View view = window.getView();
                view.setSize(event.size.width, event.size.height);
                window.setView(view);
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        // Обновляем точки для каждого графика
        for (auto& graph : graphs) {
            auto err = updateGraphPoints(graph, window.getSize().x, window.getSize().y);
            if (err) {
                // Можно выводить ошибку в отдельном окошке или логировать
                // Пока просто игнорируем
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
