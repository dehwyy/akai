#include "app.hpp"
#include "state/state.hpp"
#include "ui/ui.hpp"

#include <string>
#include "../deps/imgui-sfml/imgui-sfml.hpp"
#include "../deps/imgui-sfml/imgui.h"

using namespace app;
using namespace ui;

App::App(const config::Config& config) {
    window = NewRc<sf::RenderWindow>(sf::VideoMode(config.getWindowWidth(), config.getWindowHeight()),
                                     config.getWindowTitle());

    window->setFramerateLimit(60);
    this->loadIcon(config.getIconFilepath());
    if (!ImGui::SFML::Init(*window)) {
        exit(1);
    }
    ImGui::Spectrum::StyleColorsSpectrum();
}

App::~App() {
    ImGui::SFML::Shutdown();
}

void App::loadIcon(const std::string& filepath) {
    sf::Image appIcon;
    appIcon.loadFromFile(filepath);
    auto [iconWidth, iconHeight] = appIcon.getSize();
    window->setIcon(iconWidth, iconHeight, appIcon.getPixelsPtr());
}

void App::start() {
    sf::Clock deltaClock;
    static state::State state;
    while (window->isOpen()) {
        sf::Event event;
        while (window->pollEvent(event)) {
            ImGui::SFML::ProcessEvent(*window, event);
            if (event.type == sf::Event::Closed) {
                window->close();
            }
        }
        ImGui::SFML::Update(*window, deltaClock.restart());

        window->clear(sf::Color::Transparent);

        UI::Render(window, state);

        ImGui::SFML::Render(*window);
        window->display();
    }
}
