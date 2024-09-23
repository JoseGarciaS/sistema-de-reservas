#include "logsWindow.h"
#include "imgui.h"
#include "mongoDBHandler.h"
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>

#include <iostream>
#include <string>

using namespace bsoncxx::builder;

namespace logsWindow
{
    // Private variables
    namespace
    {

        mongoDBHandler *dbHandler = mongoDBHandler::getInstance();
        static vector<bsoncxx::document::view> logs;

    }

    // Utility functions
    namespace
    {
        void resetValues()
        {
            logs.clear();
        }

        string b_dateToString(const bsoncxx::types::b_date &bsonDate)
        {
            time_t time = chrono::system_clock::to_time_t(bsonDate);
            tm tm;
            localtime_s(&tm, &time);
            char buffer[80];
            strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
            return string(buffer);
        }
    }

    // Render functions
    namespace
    {

        void RenderViewLogs()
        {
            ImGui::Text("View Logs");
            ImGui::Separator();

            if (logs.empty())
            {
                auto filter = stream::document{} << stream::finalize;
                logs = dbHandler->findDocuments("logs", filter);
            }

            if (ImGui::Button("Update"))
            {
                auto filter = stream::document{} << stream::finalize;
                logs = dbHandler->findDocuments("logs", filter);
            }

            if (ImGui::BeginTable("Tables", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg, {0, 0}))
            {

                ImGui::TableSetupColumn("ID");
                ImGui::TableSetupColumn("Action");
                ImGui::TableSetupColumn("Action object");
                ImGui::TableSetupColumn("Timestamp");
                ImGui::TableHeadersRow();

                int row = 0;

                for (const auto &doc : logs)
                {
                    auto id = doc["id"].get_string().value.data();
                    auto action = doc["action"].get_string().value.data();
                    auto actionObject = doc["action_object"].get_string().value.data();
                    auto timestamp = doc["timestamp"].get_date();

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", id);
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", action);
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", actionObject);
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", b_dateToString(timestamp));

                    row++;
                }

                ImGui::EndTable();
            }
        }
    }

    void Render()
    {
        RenderViewLogs();
    }
}
