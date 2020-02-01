set(TARGET_NAME OpenVROverlay_imgui)
add_executable(${TARGET_NAME} WIN32
    OpenVROverlay_imgui.cpp
    imgui-master/imgui.cpp
    imgui-master/imgui_draw.cpp
    imgui-master/imgui_widgets.cpp
    imgui-master/examples/imgui_impl_dx11.cpp
    imgui-master/examples/imgui_impl_win32.cpp
    )
target_include_directories(${TARGET_NAME} PRIVATE
    openvr/headers
    imgui-master
    )
target_compile_definitions(${TARGET_NAME} PRIVATE
    _UNICODE
    UNICODE
    )
target_link_directories(${TARGET_NAME} PRIVATE
    openvr/lib/win64
    )
target_link_libraries(${TARGET_NAME} PRIVATE
    openvr_api
    d3d11
    )