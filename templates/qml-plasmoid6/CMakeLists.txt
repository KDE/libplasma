# SPDX-FileCopyrightText: %{CURRENT_YEAR} %{AUTHOR} <%{EMAIL}>
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.16)

project(plasma-%{APPNAMELC})
 
find_package(ECM 1.4.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

find_package(Plasma REQUIRED)
find_package(KF6 REQUIRED COMPONENTS Config)

# TODO: adapt "org.kde.plasma" here & elsewhere if needed (see README)
plasma_add_applet(package org.kde.plasma.%{APPNAMELC}
    QML_SOURCES
        qml/main.qml
    RESOURCES
        qml/images/pairs.svgz
    GENERATE_APPLET_CLASS
)
