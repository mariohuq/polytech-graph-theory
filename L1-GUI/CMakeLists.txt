project(lab1gui)

find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(lab1gui main.cpp)
target_link_libraries(lab1gui
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        )
