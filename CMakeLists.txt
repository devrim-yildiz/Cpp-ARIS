cmake_minimum_required(VERSION 3.5.0)
project(A.R.I.S VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin/)

add_subdirectory(vendors/sfml/)

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/vendors/imgui)
set(IMGUI_SFML_DIR ${CMAKE_SOURCE_DIR}/vendors/imgui-sfml)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_SFML_DIR}/imgui-SFML.cpp
)

file(GLOB_RECURSE ALL_SOURCE_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE ALL_HEADER_FILES "${CMAKE_SOURCE_DIR}/include/*.h")

add_executable(${PROJECT_NAME} 
    ${ALL_SOURCE_FILES} 
    ${ALL_HEADER_FILES}
    ${IMGUI_SOURCES}
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE ${PROJECT_SOURCE_DIR}/include/
    PRIVATE ${PROJECT_SOURCE_DIR}/vendors/sfml/include/
    PRIVATE ${IMGUI_DIR}
    PRIVATE ${IMGUI_DIR}/backends
    PRIVATE ${IMGUI_SFML_DIR}
)

target_link_libraries(${PROJECT_NAME} 
    sfml-graphics 
    sfml-window 
    sfml-system
    sqlite3
    GL
)

file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR}/bin)
