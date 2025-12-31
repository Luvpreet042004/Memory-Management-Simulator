cmake_minimum_required(VERSION 3.19)
project(memory-simulator-gui LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()


include_directories(
    ../include
)

qt_add_executable(memory-simulator-gui
    WIN32 MACOSX_BUNDLE

    # GUI sources
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui

    ../src/allocator/physical_memory.cpp
    ../src/cache/cache.cpp
)

target_link_libraries(memory-simulator-gui
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

install(TARGETS memory-simulator-gui
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET memory-simulator-gui
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
