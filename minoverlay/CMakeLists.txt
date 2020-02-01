set(TARGET_NAME minoverlay)
set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/../OpenVROverlay_imgui)
add_executable(${TARGET_NAME} WIN32
    main.cpp
    ${SRC_DIR}/imgui-master/imgui.cpp
    ${SRC_DIR}/imgui-master/imgui_draw.cpp
    ${SRC_DIR}/imgui-master/imgui_widgets.cpp
    ${SRC_DIR}/imgui-master/examples/imgui_impl_dx11.cpp
    ${SRC_DIR}/imgui-master/examples/imgui_impl_win32.cpp
    )
target_include_directories(${TARGET_NAME} PRIVATE
    ${SRC_DIR}/openvr/headers
    ${SRC_DIR}/imgui-master
    )
target_compile_definitions(${TARGET_NAME} PRIVATE
    _UNICODE
    UNICODE
    )
target_link_directories(${TARGET_NAME} PRIVATE
    ${SRC_DIR}/openvr/lib/win64
    )
target_link_libraries(${TARGET_NAME} PRIVATE
    openvr_api
    d3d11
    )