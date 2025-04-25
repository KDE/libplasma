# SPDX-FileCopyrightText: %{CURRENT_YEAR} %{AUTHOR} <%{EMAIL}>
# SPDX-License-Identifier: BSD-2-Clause

# TODO: adapt "org.kde.plasma" here & elsewhere if needed (see README)
add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_org.kde.plasma.%{APPNAMELC}\")

plasma_add_applet(org.kde.plasma.%{APPNAMELC}
    QML_SOURCES
        qml/main.qml
    CPP_SOURCES
        helloworld.cpp
    RESOURCES
        qml/images/pairs.svg
    GENERATE_APPLET_CLASS
)

target_link_libraries(org.kde.plasma.%{APPNAMELC} PRIVATE
    Qt6::Gui
    Plasma::Plasma
    KF6::I18n
)
