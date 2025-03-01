#include "app.hpp"
#include "state/state.hpp"
#include "ui/ui.hpp"

#include <string>
#include "../deps/imgui-sfml/imgui-sfml.hpp"
#include "../deps/imgui-sfml/imgui.h"

using namespace app;
using namespace ui;

namespace {
    const int WIDTH = 640;   // Ширина окна
    const int HEIGHT = 480;  // Высота окна
    const std::string APP_NAME = "Akai";
    const std::string ICON_FILEPATH = "joker1.jpg";
}  // namespace

App::App() {
    window = new sf::RenderWindow(sf::VideoMode(WIDTH, HEIGHT), APP_NAME);
}

App::~App() {
    delete window;
    ImGui::SFML::Shutdown();
}

void App::loadIcon() {
    sf::Image appIcon;
    appIcon.loadFromFile(ICON_FILEPATH);
    auto [iconWidth, iconHeight] = appIcon.getSize();
    window->setIcon(iconWidth, iconHeight, appIcon.getPixelsPtr());
}

void App::initImGUI() {
    if (!ImGui::SFML::Init(*window)) {
        delete window;
        exit(1);
    }

    ImGui::Spectrum::StyleColorsSpectrum();
}

void App::Init() {
    window->setFramerateLimit(60);

    this->initImGUI();
    this->loadIcon();
}

void App::Start() {
    sf::Clock deltaClock;
    state::State state;
    // unsigned counter = 0;
    while (window->isOpen()) {
        // counter++;
        sf::Event event;
        while (window->pollEvent(event)) {
            ImGui::SFML::ProcessEvent(*window, event);
            if (event.type == sf::Event::Closed) {
                window->close();
            }
        }
        ImGui::SFML::Update(*window, deltaClock.restart());

        window->clear(sf::Color::Transparent);

        // UI
        UI::Render(*window, state);

        ImGui::SFML::Render(*window);
        window->display();
    }
}
