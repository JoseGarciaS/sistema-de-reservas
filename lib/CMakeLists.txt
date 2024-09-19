add_library(imgui STATIC 
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_tables.cpp
    imgui/backends/imgui_impl_dx11.cpp
    imgui/backends/imgui_impl_win32.cpp
    imgui/addons/ImGuiDatePicker.cpp
)
target_include_directories(imgui PUBLIC 
    imgui/include
    imgui/backends/include
    imgui/addons/include
)

