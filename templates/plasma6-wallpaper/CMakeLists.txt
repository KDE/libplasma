cmake_minimum_required(VERSION 3.16)

project(plasma-%{APPNAMELC})

find_package(ECM 6.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Plasma REQUIRED)

# TODO: adapt "org.kde.plasma" here & elsewhere if needed (see README)
plasma_install_package(package org.kde.plasma.%{APPNAMELC} wallpapers wallpaper)
