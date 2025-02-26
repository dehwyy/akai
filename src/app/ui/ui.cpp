#include "ui.hpp"
#include "../../deps/imgui-sfml/imgui.h"

using namespace ui;

void UI::Render() {
    ImGui::Begin("Settings");
    static float k = 1;
    static float speed = 1;

    if (ImGui::TreeNode("Function")) {
        ImGui::SliderFloat("K", &k, 0.01, 2.0);
        ImGui::SliderFloat("Speed", &speed, 0.1, 10.0);

        ImGui::TreePop();
    }

    ImGui::End();
}
