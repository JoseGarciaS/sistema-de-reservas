#include "tableWindow.h"
#include "imgui.h"
#include "mongoDBHandler.h"
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>

#include <iostream>
#include <string>

using namespace bsoncxx::builder;

namespace tableWindow
{
    // Private variables
    namespace
    {

        mongoDBHandler *dbHandler = mongoDBHandler::getInstance();

        static int selectedTable = -1;
        static vector<bsoncxx::document::view> tables;

        static int tableNumber = -1;
        static int maxCapacity = 1;
        static char description[256] = "";
        static bool active = true;

        static bool submitted = false;

        enum TableSteps
        {
            ViewTables,
            TableInfoInput,
            Completed,
        };

        enum DataMode
        {
            None,
            Insert,
            Edit
        };

        static int step = TableSteps::ViewTables;
        static int mode = DataMode::None;

    }

    // Utility functions
    namespace
    {
        void resetValues()
        {
            selectedTable = -1;
            tableNumber = -1;
            maxCapacity = 1;
            std::fill(std::begin(description), std::end(description), '\0');
            active = true;
            submitted = false;
            tables.clear();
            mode = DataMode::None;
        }
    }

    // Render functions
    namespace
    {

        void RenderTableInformationInput()
        {

            ImGui::Text("Table Information");
            ImGui::Separator();

            ImGui::Text("Maximum capacity");
            ImGui::SliderInt("##maxCapacity", &maxCapacity, 1, 10);

            ImGui::Text("Table Description");
            ImGui::InputText("##description", description, IM_ARRAYSIZE(description));

            if (mode == DataMode::Edit)
            {
                ImGui::Text("Active");
                ImGui::Checkbox("##Active", &active);
            }

            if (ImGui::Button("Submit"))
            {
                step = TableSteps::Completed;
            }

            ImGui::SameLine();

            if (ImGui::Button("Back"))
            {
                step = TableSteps::ViewTables;
            }
        }

        void RenderCompleted()
        {
            static string status = "";

            if (!submitted)
            {

                if (mode == DataMode::Insert)
                {
                    try
                    {

                        auto countersDoc = dbHandler->findDocument("counters", stream::document{} << stream::finalize);
                        auto tableCounter = countersDoc["table_counter"].get_int32().value;

                        auto log = stream::document{} << "id" << to_string(tableCounter)
                                                      << "action" << "Insert"
                                                      << "action_object" << "Reservation"
                                                      << "timestamp" << bsoncxx::types::b_date{chrono::system_clock::now()} << stream::finalize;

                        dbHandler->createDocument("logs", log);

                        auto document = stream::document{} << "table_number" << tableCounter
                                                           << "max_capacity" << maxCapacity
                                                           << "description" << description
                                                           << "active" << true << stream::finalize;

                        dbHandler->createDocument("tables", document);

                        auto update = stream::document{} << "$set" << stream::open_document
                                                         << "table_counter" << tableCounter + 1
                                                         << stream::close_document << stream::finalize;

                        dbHandler->updateDocument("counters", stream::document{} << "table_counter" << tableCounter << stream::finalize, update);

                        status = "Table submitted successfully!";
                    }
                    catch (const std::exception &e)
                    {
                        std::cerr << e.what() << '\n';
                        status = "An error has occurred!";
                    }
                }
                else if (mode == DataMode::Edit)
                {

                    try
                    {

                        try
                        {

                            auto filter = stream::document{} << "table_number" << tableNumber << stream::finalize;

                            auto log = stream::document{} << "id" << to_string(tableNumber)
                                                          << "action" << "Insert"
                                                          << "action_object" << "Reservation"
                                                          << "timestamp" << bsoncxx::types::b_date{chrono::system_clock::now()} << stream::finalize;

                            dbHandler->createDocument("logs", log);

                            auto update = stream::document{} << "$set" << stream::open_document
                                                             << "max_capacity" << maxCapacity
                                                             << "description" << description
                                                             << "active" << active
                                                             << stream::close_document << stream::finalize;

                            dbHandler->updateDocument("tables", filter, update);
                            status = "Table updated successfully!";
                        }
                        catch (const std::exception &e)
                        {
                            status = "An error has occurred!";
                            std::cerr << e.what() << '\n';
                        }
                    }
                    catch (const std::invalid_argument &e)
                    {
                        std::cerr << e.what() << '\n';
                        status = "An error has occurred!";
                    }
                }
                submitted = true;
            }

            ImGui::Text(status.c_str());

            if (ImGui::Button("Finish"))
            {
                resetValues();
                status = "";
                step = TableSteps::ViewTables;
            }
        }

        void RenderViewTables()
        {
            ImGui::Text("View Tables");
            ImGui::Separator();

            if (tables.empty())
            {
                auto filter = stream::document{} << stream::finalize;
                tables = dbHandler->findDocuments("tables", filter);
            }

            if (ImGui::Button("Update"))
            {
                auto filter = stream::document{} << stream::finalize;
                tables = dbHandler->findDocuments("tables", filter);
            }

            ImGui::SameLine();

            if (ImGui::Button("Add"))
            {
                mode = DataMode::Insert;
                step = TableSteps::TableInfoInput;
            }

            if (selectedTable != -1)
            {
                ImGui::SameLine();
                if (ImGui::Button("Edit"))
                {
                    mode = DataMode::Edit;

                    auto doc = tables[selectedTable];

                    tableNumber = doc["table_number"].get_int32().value;
                    maxCapacity = doc["max_capacity"].get_int32().value;
                    active = doc["active"].get_bool().value;

                    strncpy_s(description, doc["description"].get_string().value.data(), sizeof(description) - 1);
                    description[sizeof(description) - 1] = '\0';

                    step = TableSteps::TableInfoInput;
                }
            }

            if (ImGui::BeginTable("Tables", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg, {0, 0}))
            {

                ImGui::TableSetupColumn("Table Number");
                ImGui::TableSetupColumn("Maximum capacity");
                ImGui::TableSetupColumn("Description");
                ImGui::TableSetupColumn("Active");

                ImGui::TableHeadersRow();

                int row = 0;

                for (const auto &doc : tables)
                {
                    auto tableNumber = doc["table_number"].get_int32().value;
                    auto maxCapacity = doc["max_capacity"].get_int32().value;
                    auto description = doc["description"].get_string().value.data();
                    auto active = doc["active"].get_bool().value;

                    char label[32];
                    sprintf_s(label, "%d", tableNumber);

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Selectable(label, selectedTable == row, ImGuiSelectableFlags_SpanAllColumns))
                    {
                        selectedTable = row;
                    }

                    ImGui::TableNextColumn();
                    ImGui::Text("%d", maxCapacity);
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", description);
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", active ? "Yes" : "No");

                    row++;
                }

                ImGui::EndTable();
            }
        }
    }

    void Render()
    {
        switch (step)
        {
        case TableSteps::ViewTables:
            RenderViewTables();
            break;

        case TableSteps::TableInfoInput:
            RenderTableInformationInput();
            break;

        case TableSteps::Completed:
            RenderCompleted();
            break;
        default:
            break;
        }
    }
}
