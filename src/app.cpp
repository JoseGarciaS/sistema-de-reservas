#include "app.h"
#include "imgui.h"
#include "reservationWindow.h"
#include "tableWindow.h"
#include "logsWindow.h"

namespace app
{

    // Variables
    namespace
    {
        enum selectedWindow
        {
            Reservations,
            Tables,
            Logs,
            UnitTesting,
            None
        };

        selectedWindow currentWindow = None;

    }
    // Layout
    namespace
    {
        void RenderSidebar()
        {
            ImVec2 windowSize = ImGui::GetIO().DisplaySize;
            ImVec2 size = ImVec2(windowSize.x * 0.20f, windowSize.y * 0.90f);
            if (ImGui::BeginChild("Sidebar", size, ImGuiChildFlags_Border, ImGuiWindowFlags_NoSavedSettings))
            {

                ImGui::Text("Sidebar");
                ImGui::Separator();

                if (ImGui::Selectable("Reservations", currentWindow == Reservations))
                {
                    currentWindow = Reservations;
                }

                if (ImGui::Selectable("Tables", currentWindow == Tables))
                {
                    currentWindow = Tables;
                }

                if (ImGui::Selectable("Logs", currentWindow == Logs))
                {
                    currentWindow = Logs;
                }

                if (ImGui::Selectable("", currentWindow == UnitTesting))
                {
                    currentWindow = UnitTesting;
                }

                ImGui::EndChild();
            }
        }

        void RenderContent()
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
            ImVec2 windowSize = ImGui::GetIO().DisplaySize;
            ImVec2 size = ImVec2(windowSize.x * 0.785f, windowSize.y * 0.90f);
            ImGui::SameLine();
            if (ImGui::BeginChild("Content", size, ImGuiChildFlags_Border, ImGuiWindowFlags_NoSavedSettings))
            {
                switch (currentWindow)
                {
                case Reservations:
                    reservationWindow::Render();
                    break;
                case Tables:
                    ImGui::Text("Tables");
                    tableWindow::Render();
                    break;
                case Logs:
                    ImGui::Text("Logs");
                    logsWindow::Render();
                    break;
                case UnitTesting:
                    ImGui::Text("Unit Testing");
                    break;
                default:
                    break;
                }

                ImGui::EndChild();
            }
        }

        void RenderActionButtons()
        {
            ImVec2 windowSize = ImGui::GetIO().DisplaySize;
            ImVec2 size = ImVec2(windowSize.x * 0.99f, windowSize.y * 0.075f);
        }

    }

    void Render()
    {

        // ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImVec2 windowSize = ImGui::GetIO().DisplaySize;
        ImGui::SetNextWindowSize(windowSize);
        ImGui::SetNextWindowPos(ImVec2(0, 0));

        if (ImGui::Begin("Main Window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
        {

            RenderSidebar();
            RenderContent();
            RenderActionButtons();

            ImGui::End();
        }

        // ImGui::PopStyleColor();
    }
}
