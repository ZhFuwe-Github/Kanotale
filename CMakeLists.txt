cmake_minimum_required(VERSION 3.16)

project(kanotale VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Multimedia)

set(PROJECT_SOURCES
        main.cpp
        widget.cpp
        widget.h

)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(kanotale
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        settings.h settings.cpp settings.ui
        playerheartitem.h
        bulletitem.h
        bulletitem.cpp
        playerheartitem.cpp
        linearbulletitem.h
        linearbulletitem.cpp
        homingbulletitem.h
        homingbulletitem.cpp
        warningitem.h
        warningitem.cpp
        fixedbulletitem.h
        fixedbulletitem.cpp
        attackstick.h
        attackstick.cpp


        dogbulletitem.h
        dogbulletitem.cpp







    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET kanotale APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(kanotale SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(kanotale
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(kanotale PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(kanotale PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Multimedia
)
# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.kanotale)
endif()
set_target_properties(kanotale PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS kanotale
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(kanotale)
endif()

set(WINDOWS_ICON_FILE "${CMAKE_CURRENT_SOURCE_DIR}/build/kanotale.ico")
string(REPLACE "/" "\\\\" WINDOWS_ICON_PATH_RC "${WINDOWS_ICON_FILE}")
        set(RC_CONTENT "IDI_ICON1 ICON \"${WINDOWS_ICON_PATH_RC}\"\n")
        set(RC_FILENAME "${CMAKE_CURRENT_BINARY_DIR}/appicon.rc")
        file(WRITE ${RC_FILENAME} "${RC_CONTENT}")
        target_sources(kanotale PRIVATE ${RC_FILENAME})
        message(STATUS "Windows icon set from ${WINDOWS_ICON_FILE}")
