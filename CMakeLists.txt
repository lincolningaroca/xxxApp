cmake_minimum_required(VERSION 3.5)

project(
  xxxapp
  VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra -Wconversion")

# set the application icon
set(APP_ICON "${CMAKE_CURRENT_SOURCE_DIR}/appIcon.rc")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Network)

set(PROJECT_SOURCES
  main.cpp
  widget.cpp
  widget.hpp
  widget.ui
  dlgnewcategory.cpp
  dlgnewcategory.hpp
  dlgnewcategory.ui
  logindialog.hpp
  logindialog.cpp
  logindialog.ui
  util/helper.cpp
  util/helper.hpp
  helperdatabase/helperdb.cpp
  helperdatabase/helperdb.hpp
  ${APP_ICON})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  find_package(Qt6 REQUIRED COMPONENTS Core)

  qt_add_executable(
    xxxapp
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    rsc.qrc
    resetpassworddialog.hpp
    resetpassworddialog.cpp
    resetpassworddialog.ui
    publicurldialog.hpp
    publicurldialog.cpp
    publicurldialog.ui
    categorydialog.hpp
    categorydialog.cpp
    categorydialog.ui
    swtablemodel.hpp
    swtablemodel.cpp
    swlabel.hpp
    swlabel.cpp
    acercadedialog.hpp
    acercadedialog.cpp
    acercadedialog.ui
    util/excelexporter.hpp util/excelexporter.cpp
    util/systemthemewatcher.hpp util/systemthemewatcher.cpp
    # qaesencryption.cpp qaesencryption.h
  )
add_custom_command(
  TARGET xxxapp
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory
  "$<TARGET_FILE_DIR:xxxapp>/tools/sqlite-tools-win-x64-3450100"
  COMMAND
  ${CMAKE_COMMAND} -E copy_directory
  "${CMAKE_CURRENT_SOURCE_DIR}/tools/sqlite-tools-win-x64-3450100"
  "$<TARGET_FILE_DIR:xxxapp>/tools/sqlite-tools-win-x64-3450100"
  COMMENT "Copiando herramientas SQLite al directorio de salida")

# Define target properties for Android with Qt 6 as: set_property(TARGET
# xxxapp APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
# ${CMAKE_CURRENT_SOURCE_DIR}/android) For more information, see
# https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
  if(ANDROID)
    add_library(xxxapp SHARED ${PROJECT_SOURCES})
    # Define properties for Android with Qt 5 after find_package() calls as:
    # set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
  else()
    add_executable(xxxapp ${PROJECT_SOURCES})
  endif()
endif()

target_include_directories(${PROJECT_NAME}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include/QXlsx/QXlsxQt6)


target_link_libraries(
  xxxapp
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/libcrypto.lib
  Qt${QT_VERSION_MAJOR}::Network
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/libQXlsxQt6.a)
target_link_libraries(xxxapp PRIVATE Qt6::Core)

set_target_properties(
  xxxapp
  PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
  MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
  MACOSX_BUNDLE_SHORT_VERSION_STRING
  ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
  MACOSX_BUNDLE TRUE
  WIN32_EXECUTABLE TRUE)

install(
  TARGETS xxxapp
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_executable(xxxapp)
endif()

# message("Nombre del sistema: ${CMAKE_HOST_SYSTEM_NAME}")
