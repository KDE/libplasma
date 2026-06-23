# SPDX-FileCopyrightText: %{CURRENT_YEAR} %{AUTHOR} <%{EMAIL}>
# SPDX-License-Identifier: BSD-2-Clause

add_definitions(-DTRANSLATION_DOMAIN=\"plasma_wallpaper_org.kde.plasma.%{APPNAMELC}\")

ecm_add_qml_module(%{APPNAMELC}plugin URI org.kde.plasma.private.%{APPNAMELC} SOURCES helloworld.cpp GENERATE_PLUGIN_SOURCE)

target_link_libraries(%{APPNAMELC}plugin PRIVATE
    KF6::I18n
    Qt6::Gui
    Qt6::Qml
)

ecm_finalize_qml_module(%{APPNAMELC}plugin)
