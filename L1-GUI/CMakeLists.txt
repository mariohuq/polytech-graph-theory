project(lab1gui)

find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        Sql
        REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(lab1gui
    L9.cpp
    Editor.h
    Editor.cpp
    CheckableComboBox.cpp
    ComboHeaderView.h
    ComboHeaderView.cpp
    AmountDelegate.h
    CheckableComboBox.h
    AmountDelegate.cpp
    edge.h
    edge.cpp
    GraphScene.h
    GraphScene.cpp
    node.h
    node.cpp
        Editor.ui
    resources.qrc
    L9.ico
    L9.rc
        Lab1.cpp Lab1.h Lab1.ui ResizeFilter.cpp ResizeFilter.h)
target_link_libraries(lab1gui
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Sql
        graphsLib
        )
