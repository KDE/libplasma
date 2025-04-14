ecm_add_qml_module(plasmoidplugin URI "org.kde.plasma.plasmoid" GENERATE_PLUGIN_SOURCE VERSION 2.0 DEPENDENCIES QtQuick org.kde.plasma.core org.kde.config)

target_sources(plasmoidplugin PRIVATE ../plasmoidattached_p.cpp types.h)

target_link_libraries(plasmoidplugin PRIVATE PlasmaQuick Qt6::Widgets KF6::Package)

ecm_finalize_qml_module(plasmoidplugin)
