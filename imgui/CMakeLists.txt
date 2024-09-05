add_library(imgui STATIC 
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_widgets.cpp
    imgui_tables.cpp
    backends/imgui_impl_dx11.cpp
    backends/imgui_impl_win32.cpp
)
target_include_directories(imgui PUBLIC 
    include
    backends/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    imgui
    d3d11
)