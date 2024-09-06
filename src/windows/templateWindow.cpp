#include "templateWindow.h"
#include "imgui.h"

namespace templateWindow
{
    void Render()
    {
        ImGui::Begin("template");
        ImGui::Button("Button");
        static float value = 0.0f;
        ImGui::DragFloat("value", &value);
        ImGui::End();
    }
}