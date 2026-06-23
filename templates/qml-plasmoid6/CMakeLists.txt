# SPDX-FileCopyrightText: %{CURRENT_YEAR} %{AUTHOR} <%{EMAIL}>
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.16)

project(plasma-%{APPNAMELC})

set(KF6_MIN_VERSION "6.0.0")
 
find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

find_package(Plasma REQUIRED)
find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS Config)

# TODO: adapt "org.kde.plasma" here & elsewhere if needed (see README)
plasma_add_applet(org.kde.plasma.%{APPNAMELC}
    QML_SOURCES
        qml/main.qml
    RESOURCES
        qml/images/pairs.svgz
    GENERATE_APPLET_CLASS
)
