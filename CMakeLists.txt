cmake_minimum_required(VERSION 3.19)
project(QCefViewApp LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network)
get_target_property(_qmake_executable Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

if(WIN32)
    find_program(DEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}" REQUIRED)
elseif(APPLE)
    find_program(DEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}" REQUIRED)
endif()

find_program(NPM_EXECUTABLE npm REQUIRED)
set(VITE_APP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/app")

qt_standard_project_setup()

qt_add_executable(QCefViewApp
    WIN32
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

add_subdirectory(thirdparty/QCefView)

add_custom_target(vite_app
    COMMAND ${NPM_EXECUTABLE} run build
    WORKING_DIRECTORY ${VITE_APP_DIR}
    BYPRODUCTS "${VITE_APP_DIR}/dist/index.html"
    COMMENT "Building vite app..."
    VERBATIM
)

set_target_properties(QCefView PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/QCefView/$<CONFIG>/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/QCefView/$<CONFIG>/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/QCefView/$<CONFIG>/lib"
)

target_link_libraries(QCefViewApp
    PRIVATE
    Qt::Core
    Qt::Widgets
    Qt::Network
    QCefView
)

target_compile_definitions(QCefViewApp PRIVATE
    $<$<CONFIG:Debug>:APP_DEV>
    VITE_APP_DIR="${VITE_APP_DIR}"
)


add_dependencies(QCefViewApp vite_app)

add_custom_command(TARGET QCefViewApp
    POST_BUILD

    COMMAND ${CMAKE_COMMAND} -E copy_directory
    $<TARGET_FILE_DIR:QCefView>
    $<TARGET_FILE_DIR:${PROJECT_NAME}>

    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${VITE_APP_DIR}/dist"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/app"

    VERBATIM
)

if(WIN32 OR APPLE)
    add_custom_command(TARGET QCefViewApp
        POST_BUILD

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

install(
    DIRECTORY "${VITE_APP_DIR}/dist/"
    DESTINATION "${CMAKE_INSTALL_BINDIR}/app"
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/output/$<CONFIG>/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/output/$<CONFIG>/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/output/$<CONFIG>/lib"
)
