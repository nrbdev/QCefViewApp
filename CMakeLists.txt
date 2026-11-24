cmake_minimum_required(VERSION 3.19)
project(QCefViewApp LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
get_target_property(_qmake_executable Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

if(WIN32)
  find_program(DEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
endif()

qt_standard_project_setup()

qt_add_executable(QCefViewApp
    WIN32
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

add_subdirectory(thirdparty/QCefView)

set_target_properties(QCefView PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/QCefView/$<CONFIG>/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/QCefView/$<CONFIG>/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/QCefView/$<CONFIG>/lib"
)

target_link_libraries(QCefViewApp
    PRIVATE
        Qt::Core
        Qt::Widgets
        QCefView
)

if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD

        COMMAND ${CMAKE_COMMAND} -E copy_directory
        $<TARGET_FILE_DIR:QCefView>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>

        COMMAND ${DEPLOYQT_EXECUTABLE}
        --no-svg
        --no-translations
        --no-compiler-runtime
        $<TARGET_FILE:${PROJECT_NAME}>

        VERBATIM
    )
endif()

install(TARGETS QCefViewApp
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET QCefViewApp
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

install(SCRIPT ${deploy_script})

set_target_properties(${PROJECT_NAME}
  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/output/$<CONFIG>/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/output/$<CONFIG>/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/output/$<CONFIG>/lib"
)