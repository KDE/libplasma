cmake_minimum_required(VERSION 3.16)

project(plasma-%{APPNAMELC})

find_package(ECM 6.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

find_package(KF6 REQUIRED COMPONENTS
    I18n
)

find_package(Qt6 CONFIG REQUIRED COMPONENTS
    Qml
    Gui
    Core
)

find_package(Plasma CONFIG REQUIRED)

# wallpaper applet
# TODO: adapt "org.kde.plasma" here & elsewhere if needed (see README)
plasma_install_package(package org.kde.plasma.%{APPNAMELC} wallpapers wallpaper)

# qml extension plugin
add_subdirectory(plugin)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
