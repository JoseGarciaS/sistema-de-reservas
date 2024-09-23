#include "reservationWindow.h"
#include "imgui.h"
#include "ImGuiDatePicker.hpp"
#include "mongoDBHandler.h"
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <sstream>
#include "app.h"
#include <iomanip>

using namespace bsoncxx::builder;

namespace reservationWindow
{
    // Private variables
    namespace
    {

        mongoDBHandler *dbHandler = mongoDBHandler::getInstance();
        const char *hours[] = {"10 AM", "11 AM", "12 PM", "1 PM", "2 PM", "3 PM", "4 PM", "5 PM", "6 PM", "7 PM", "8 PM", "9 PM", "10 PM"};

        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
        tm selectedDate = *std::localtime(&currentTime);
        // gmtime_s(&t, &currentTime);

        static int selectedStartHour = 0;
        static int selectedEndHour = 0;
        static int partySize = 1;
        static int selectedRow = -1;
        static int selectedReservation = -1;
        static vector<bsoncxx::document::view> tables;
        static vector<bsoncxx::document::view> reservations;

        enum ReservationSteps
        {
            ActionSelect,
            TableReservation,
            PersonalInfoInput,
            Completed,
            ViewReservations
        };

        enum DataMode
        {
            None,
            Create,
            Edit
        };

        static int step = ReservationSteps::ActionSelect;
        static int mode = DataMode::None;

        static char firstName[128] = "";
        static char lastName[128] = "";
        static char phoneNumber[128] = "";
        static char emailAddress[128] = "";
        static string reservationCode = "";

        static bool reservationSubmitted = false;

    }

    // Utility functions
    namespace
    {
        string generateReservationCode()
        {
            srand(static_cast<unsigned int>(time(0)));
            std::string code = "";
            for (int i = 0; i < 6; i++)
            {
                int asciiValue = rand() % 26 + 65;
                code += static_cast<char>(asciiValue);
            }
            return code;
        }

        bsoncxx::types::b_date generateTimeStamp(tm &date, const string &timeStr)
        {

            istringstream ss(timeStr);
            string hourPart, periodPart;
            ss >> hourPart >> periodPart;

            int hour = stoi(hourPart);
            if (periodPart == "PM" && hour != 12)
            {
                hour += 12;
            }
            else if (periodPart == "AM" && hour == 12)
            {
                hour = 0;
            }

            tm time = {};
            time.tm_hour = hour;
            time.tm_min = 0;
            time.tm_sec = 0;

            tm reservationTime = {};
            reservationTime.tm_year = date.tm_year;
            reservationTime.tm_mon = date.tm_mon;
            reservationTime.tm_mday = date.tm_mday;
            reservationTime.tm_hour = time.tm_hour;
            reservationTime.tm_min = time.tm_min;
            reservationTime.tm_sec = time.tm_sec;

            time_t timestamp = mktime(&reservationTime);

            bsoncxx::types::b_date bsonDate{chrono::system_clock::from_time_t(timestamp)};

            return bsonDate;
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

        void resetFormFields()
        {
            reservationSubmitted = false;
            selectedStartHour = 0;
            selectedEndHour = 0;
            partySize = 1;
            selectedRow = -1;
            std::fill(std::begin(firstName), std::end(firstName), '\0');
            std::fill(std::begin(lastName), std::end(lastName), '\0');
            std::fill(std::begin(phoneNumber), std::end(phoneNumber), '\0');
            std::fill(std::begin(emailAddress), std::end(emailAddress), '\0');

            selectedDate = *std::localtime(&currentTime);
            reservationCode = "";
            selectedReservation = -1;
            reservations.clear();
            tables.clear();

            mode = DataMode::None;
        }
    }

    // Render functions
    namespace
    {
        void RenderActionSelect()
        {
            ImVec2 parentSize = ImGui::GetContentRegionAvail();
            float width = parentSize.x * 0.25f;
            float height = parentSize.y;
            ImVec2 childPos = ImVec2((parentSize.x - width) / 2.0f, (parentSize.y - height) / 2.0f);

            ImGui::Text("Select an action");
            ImGui::Separator();

            ImGui::SetCursorPos(childPos);
            ImVec2 buttonSize = ImVec2(width * 0.8f, 50.0f);
            ImVec2 buttonPos = ImVec2((width - buttonSize.x) / 2.0f, height * 0.3f);
            ImGui::SetCursorPos(buttonPos);

            if (ImGui::Button("Make reservation", buttonSize))
            {
                step = ReservationSteps::TableReservation;
                mode = DataMode::Create;
            }

            buttonPos.y += buttonSize.y + 20.0f;
            ImGui::SetCursorPos(buttonPos);

            if (ImGui::Button("View reservations", buttonSize))
            {
                step = ReservationSteps::ViewReservations;
                mode = DataMode::None;
            }
        }

        void RenderTableReservation()
        {
            ImGui::Text("Reservation details");
            ImGui::Separator();
            if (ImGui::BeginTable("table2", 2, ImGuiTableFlags_None, {0, 0}))
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Reservation Date");
                ImGui::DatePicker("##date", selectedDate, false, -1.00f);

                ImGui::Text("Start Time");
                ImGui::Combo("##start", &selectedStartHour, hours, IM_ARRAYSIZE(hours));

                ImGui::Text("End Time");
                ImGui::Combo("##end", &selectedEndHour, hours, IM_ARRAYSIZE(hours));

                ImGui::Text("Party Size");

                ImGui::SliderInt("##partySize", &partySize, 1, 10);

                if (ImGui::Button("Back"))
                {
                    step = ReservationSteps::ActionSelect;
                }

                ImGui::SameLine();

                if (ImGui::Button("Check availability"))
                {
                    auto filter = stream::document{} << stream::finalize;
                    tables = dbHandler->findDocuments("tables", filter);
                }

                if (selectedRow != -1)
                {
                    ImGui::SameLine();
                    if (ImGui::Button("Next"))
                    {
                        step = ReservationSteps::PersonalInfoInput;
                    }
                }

                ImGui::TableNextColumn();
                ImGui::Text("Tables");

                if (ImGui::BeginTable("Available tables", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg, {0, 0}))
                {

                    ImGui::TableSetupColumn("Table number");
                    ImGui::TableSetupColumn("Maximum capacity");
                    ImGui::TableSetupColumn("Description");
                    ImGui::TableHeadersRow();

                    int row = 0;
                    for (const auto &doc : tables)
                    {
                        auto num = doc["table_number"].get_int32().value;
                        auto cap = doc["max_capacity"].get_int32().value;
                        auto desc = doc["description"].get_string().value.data();
                        char label[32];
                        sprintf_s(label, "%d", num);

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        if (ImGui::Selectable(label, selectedRow == row, ImGuiSelectableFlags_SpanAllColumns))
                        {
                            selectedRow = row;
                        }

                        ImGui::TableNextColumn();
                        ImGui::Text("%d", cap);
                        ImGui::TableNextColumn();
                        ImGui::Text("%s", desc);

                        row++;
                    }

                    ImGui::EndTable();
                }

                ImGui::EndTable();
            }
        }

        void RenderPersonalInfoInput()
        {

            ImGui::Text("Personal Information");
            ImGui::Separator();

            ImGui::Text("First Name");
            ImGui::InputText("##First Name", firstName, IM_ARRAYSIZE(firstName));

            ImGui::Text("Last Name");
            ImGui::InputText("##Last Name", lastName, IM_ARRAYSIZE(lastName));

            ImGui::Text("Phone Number");
            ImGui::InputText("##Phone Number", phoneNumber, IM_ARRAYSIZE(phoneNumber));

            ImGui::Text("Email Address");
            ImGui::InputText("##Email Address", emailAddress, IM_ARRAYSIZE(emailAddress));

            if (ImGui::Button("Submit"))
            {
                step = ReservationSteps::Completed;
            }

            ImGui::SameLine();

            if (ImGui::Button("Back"))
            {
                step = ReservationSteps::TableReservation;
            }
        }

        void RenderCompleted()
        {
            static string status = "";

            if (!reservationSubmitted)
            {

                if (mode == DataMode::Create)
                {
                    try
                    {
                        string code = generateReservationCode();
                        auto startTime = generateTimeStamp(selectedDate, hours[selectedStartHour]);
                        auto endTime = generateTimeStamp(selectedDate, hours[selectedEndHour]);
                        auto tableNumber = tables[selectedRow]["table_number"].get_int32().value;

                        auto log = stream::document{} << "id" << code
                                                      << "action" << "Create"
                                                      << "action_object" << "Reservation"
                                                      << "timestamp" << bsoncxx::types::b_date{chrono::system_clock::now()} << stream::finalize;

                        dbHandler->createDocument("logs", log);

                        auto document = stream::document{} << "reservation_code" << code
                                                           << "first_name" << firstName
                                                           << "last_name" << lastName
                                                           << "phone_number" << phoneNumber
                                                           << "email_address" << emailAddress
                                                           << "start_time" << startTime
                                                           << "end_time" << endTime
                                                           << "table_number" << tableNumber
                                                           << "party_size" << partySize
                                                           << stream::finalize;

                        dbHandler->createDocument("reservations", document);

                        status = "Reservation submitted successfully!";
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

                        auto filter = stream::document{} << "reservation_code" << reservationCode << stream::finalize;

                        auto startTime = generateTimeStamp(selectedDate, hours[selectedStartHour]);
                        auto endTime = generateTimeStamp(selectedDate, hours[selectedEndHour]);
                        auto tableNumber = tables[selectedRow]["table_number"].get_int32().value;

                        auto log = stream::document{} << "id" << reservationCode
                                                      << "action" << "Update"
                                                      << "action_object" << "Reservation"
                                                      << "timestamp" << bsoncxx::types::b_date{chrono::system_clock::now()} << stream::finalize;

                        dbHandler->createDocument("logs", log);

                        auto update = stream::document{} << "$set" << stream::open_document
                                                         << "first_name" << firstName
                                                         << "last_name" << lastName
                                                         << "phone_number" << phoneNumber
                                                         << "email_address" << emailAddress
                                                         << "start_time" << startTime
                                                         << "end_time" << endTime
                                                         << "table_number" << tableNumber
                                                         << "party_size" << partySize
                                                         << stream::close_document << stream::finalize;

                        try
                        {
                            dbHandler->updateDocument("reservations", filter, update);
                            status = "Reservation updated successfully!";
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
                reservationSubmitted = true;
            }

            ImGui::Text(status.c_str());

            if (ImGui::Button("Finish"))
            {
                resetFormFields();
                status = "";
                step = ReservationSteps::ActionSelect;
            }
        }

        void RenderViewReservations()
        {
            ImGui::Text("View Reservations");
            ImGui::Separator();

            // if (reservations.empty())
            // {
            //     auto filter = stream::document{} << stream::finalize;
            //     reservations = dbHandler->findDocuments("reservations", filter);
            // }

            if (ImGui::Button("Back"))
            {
                step = ReservationSteps::ActionSelect;
            }
            ImGui::SameLine();

            if (ImGui::Button("Update"))
            {
                reservations.clear();
                auto filter = stream::document{} << stream::finalize;
                reservations = dbHandler->findDocuments("reservations", filter);
            }

            if (selectedReservation != -1)
            {
                ImGui::SameLine();
                if (ImGui::Button("Edit"))
                {
                    mode = DataMode::Edit;

                    auto doc = reservations[selectedReservation];

                    auto startTime = doc["start_time"].get_date();
                    auto endTime = doc["end_time"].get_date();

                    partySize = doc["party_size"].get_int32().value;

                    reservationCode = doc["reservation_code"].get_string().value.data();

                    time_t startTimeT = std::chrono::system_clock::to_time_t(startTime);
                    selectedDate = *std::localtime(&startTimeT);

                    strncpy_s(firstName, doc["first_name"].get_string().value.data(), sizeof(firstName) - 1);
                    firstName[sizeof(firstName) - 1] = '\0';

                    strncpy_s(lastName, doc["last_name"].get_string().value.data(), sizeof(lastName) - 1);
                    lastName[sizeof(lastName) - 1] = '\0';

                    strncpy_s(phoneNumber, doc["phone_number"].get_string().value.data(), sizeof(phoneNumber) - 1);
                    phoneNumber[sizeof(phoneNumber) - 1] = '\0';

                    strncpy_s(emailAddress, doc["email_address"].get_string().value.data(), sizeof(emailAddress) - 1);
                    emailAddress[sizeof(emailAddress) - 1] = '\0';

                    for (int i = 0; i < IM_ARRAYSIZE(hours); i++)
                    {
                        if (hours[i] == b_dateToString(startTime))
                        {
                            selectedStartHour = i;
                        }
                        if (hours[i] == b_dateToString(endTime))
                        {
                            selectedEndHour = i;
                        }
                    }

                    step = ReservationSteps::TableReservation;
                }

                ImGui::SameLine();
                if (ImGui::Button("Delete"))
                {
                    auto code = reservations[selectedReservation]["reservation_code"].get_string().value.data();
                    auto filter = stream::document{} << "reservation_code" << code << stream::finalize;
                    dbHandler->deleteDocument("reservations", filter);
                    selectedReservation = -1;

                    filter = stream::document{} << stream::finalize;
                    reservations = dbHandler->findDocuments("reservations", filter);
                }
            }

            if (ImGui::BeginTable("Reservations", 9, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg, {0, 0}))
            {

                ImGui::TableSetupColumn("Reservation Code");
                ImGui::TableSetupColumn("First Name");
                ImGui::TableSetupColumn("Last Name");
                ImGui::TableSetupColumn("Start Time");
                ImGui::TableSetupColumn("End Time");
                ImGui::TableSetupColumn("Phone Number");
                ImGui::TableSetupColumn("Email Address");
                ImGui::TableSetupColumn("Table Number");
                ImGui::TableSetupColumn("Party Size");
                ImGui::TableHeadersRow();

                int row = 0;

                for (const auto &doc : reservations)
                {
                    auto code = doc["reservation_code"].get_string().value.data();
                    auto firstName = doc["first_name"].get_string().value.data();
                    auto lastName = doc["last_name"].get_string().value.data();

                    auto phoneNumber = doc["phone_number"].get_string().value.data();
                    auto emailAddress = doc["email_address"].get_string().value.data();
                    auto tableNumber = doc["table_number"].get_int32().value;
                    auto partySize = doc["party_size"].get_int32().value;

                    auto startTime = doc["start_time"].get_date();
                    auto endTime = doc["end_time"].get_date();
                    string startStr = b_dateToString(startTime);
                    string endStr = b_dateToString(endTime);

                    char label[32];
                    sprintf_s(label, "%s", code);

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Selectable(label, selectedReservation == row, ImGuiSelectableFlags_SpanAllColumns))
                    {
                        selectedReservation = row;
                    }

                    ImGui::TableNextColumn();
                    ImGui::Text("%s", firstName);
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", lastName);
                    ImGui::TableNextColumn();
                    ImGui::Text(startStr.c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(endStr.c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", phoneNumber);
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", emailAddress);
                    ImGui::TableNextColumn();
                    ImGui::Text("%d", tableNumber);
                    ImGui::TableNextColumn();
                    ImGui::Text("%d", partySize);

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
        case ReservationSteps::ActionSelect:
            RenderActionSelect();
            break;
        case ReservationSteps::TableReservation:
            RenderTableReservation();
            break;
        case ReservationSteps::PersonalInfoInput:
            RenderPersonalInfoInput();
            break;
        case ReservationSteps::Completed:
            RenderCompleted();
            break;
        case ReservationSteps::ViewReservations:
            RenderViewReservations();
            break;
        default:
            break;
        }
    }
}
