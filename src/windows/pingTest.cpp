#include "pingTest.h"
#include "imgui.h"
#include "mongoDBHandler.h"

using namespace std;

namespace pingTest
{
    mongoDBHandler *dbHandler = mongoDBHandler::getInstance();
    void Render()
    {
        static bool pingSuccess = false;
        static bool pingAttempted = false;

        ImGui::Begin("template");

        if (ImGui::Button("Ping Database"))
        {
            pingAttempted = true;
            pingSuccess = dbHandler->ping();
        }

        if (pingAttempted)
        {
            ImGui::Text(pingSuccess ? "Success" : "Failure");
        }
        else
        {
            ImGui::Text("Not pinged");
        }

        ImGui::End();
    }
}