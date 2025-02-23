#include "deps/imgui-sfml/imgui-sfml.hpp"
#include "deps/imgui-sfml/imgui.h"

#include <cmath>
#include <iostream>
#include "SFML/Audio.hpp"
#include "SFML/Graphics.hpp"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

const int WIDTH = 640;         // Ширина окна
const int HEIGHT = 480;        // Высота окна
const float SCALE_X = 50.0f;   // Масштаб по X (увеличение)
const float SCALE_Y = 100.0f;  // Масштаб по Y (увеличение)

const double EPSILON = 0.1;

double f(double x) {
    double radian = std::fmod(x, 2. * 3.14);

    // std::cout << x << " " << radian << std::endl;
    const double breakpoin = radian > 3.14 ? 3. * 3.14 / 2. : 3.14 / 2.;
    if (radian > breakpoin - EPSILON && radian <= breakpoin + EPSILON) {
        const double borderX = breakpoin - ((breakpoin - radian) > 0 ? -EPSILON : EPSILON);
        return f(borderX);
    }

    return std::fabs(std::cos(radian));
}

void drawGraph(sf::RenderWindow* window, double k) {
    // std::cout << "k = " << k << std::endl;
    sf::VertexArray graph(sf::LineStrip);

    for (float x = -8.0 * std::fabs(1 / k); x <= 8.0 * std::fabs(1 / k);
         x += 0.1) {            // Диапазон X от -8 до 8
        float y = std::sin(x);  // Функция

        // Преобразование координат в пиксели SFML
        float pixelX = WIDTH / 2 + x * SCALE_X * k;
        float pixelY = HEIGHT / 2 - y * SCALE_Y;

        graph.append(sf::Vertex(sf::Vector2f(pixelX, pixelY), sf::Color::Magenta));
    }

    window->draw(graph);
}

void imgui() {
    sf::RenderWindow window(sf::VideoMode(640, 480), "Akai", sf::Style::Default);

    sf::Image appIcon;
    appIcon.loadFromFile("joker1.jpg");
    auto [iconWidth, iconHeight] = appIcon.getSize();

    window.setIcon(iconWidth, iconHeight, appIcon.getPixelsPtr());
    window.setFramerateLimit(60);

    if (!ImGui::SFML::Init(window)) {
        exit(1);
    }

    ImGui::Spectrum::StyleColorsSpectrum();

    sf::Clock deltaClock;
    unsigned counter = 0;
    while (window.isOpen()) {
        counter++;
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        ImGui::SFML::Update(window, deltaClock.restart());
        window.clear(sf::Color::Transparent);

        ImGui::Begin("Settings");
        static float k = 1;
        if (ImGui::TreeNode("Function")) {
            ImGui::SliderFloat("K", &k, 0.01, 2.0);

            ImGui::TreePop();
        }
        // Colored button
        for (int i = 0; i < 7; i++) {
            if (i > 0) {
                ImGui::SameLine();
            }
            ImGui::PushID(i);
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
            ImGui::Button("Click");
            ImGui::PopStyleColor(3);
            ImGui::PopID();
        }

        drawGraph(&window, k * f(1));

        ImGui::End();
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}

int main(int, char**) {
    imgui();
    // auto window = new sf::RenderWindow(sf::VideoMode(WIDTH, HEIGHT), "Akai");

    // ImGui::SFML::Init(*window);

    // // Load a sprite to display
    // sf::Texture texture;
    // if (!texture.loadFromFile("joker1.jpg")) {
    //     delete window;
    //     return 1;
    // }

    // // window.setSize({x, y});

    // sf::Sprite sprite(texture);

    // sf::Vector2u windowSize = window->getSize();
    // sf::Vector2u textureSize = texture.getSize();

    // // Вычисляем масштаб для растягивания на весь экран
    // float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    // float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    // sprite.setScale(scaleX, scaleY);

    // // Create a graphical text to display
    // sf::Font font;
    // if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
    //     return EXIT_FAILURE;
    // }
    // sf::Text text(L"Однозубый джокер", font, 50);
    // text.setPosition(100, 100);

    // // Load a music to play
    // // sf::Music music;
    // // if (!music.openFromFile("music.mp3")) {
    // //     return EXIT_FAILURE;
    // // }

    // // Play the music
    // // music.play();

    // // Start the game loop
    // unsigned counter = 0;
    // while (window->isOpen()) {
    //     counter++;
    //     // Process events
    //     sf::Event event;
    //     while (window->pollEvent(event)) {
    //         // Close window: exit
    //         if (event.type == sf::Event::Closed) {
    //             window->close();
    //         }

    //         if (event.type == sf::Event::KeyPressed) {
    //             std::cout << "Key pressed: " << event.key.scancode << std::endl;
    //         }
    //     }

    //     window->clear(sf::Color::Transparent);
    //     // Draw the sprite
    //     // window->draw(sprite);

    //     // Draw the string
    //     // window->draw(text);

    //     // std::cout << "Tick: " << counter << std::endl;
    //     // std::cout << "fx = " << f((double)counter / 10000.0) << std::endl;
    //     drawGraph(window, f(double(counter) / 10000.0));  // Рисуем график

    //     // Update the window
    //     window->display();
    // }

    // return 0;
}
