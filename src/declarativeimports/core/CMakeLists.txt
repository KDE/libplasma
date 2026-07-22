ecm_add_qml_module(corebindingsplugin URI "org.kde.plasma.core" VERSION 2.0 DEPENDENCIES QtCore QtQuick org.kde.config)

target_sources(corebindingsplugin PRIVATE
    action.cpp
    corebindingsplugin.cpp
    tooltiparea.cpp
    tooltipdialog.cpp

    action.h
    corebindingsplugin.h
    tooltiparea.h
    tooltipdialog.h
)

ecm_target_qml_sources(corebindingsplugin SOURCES
    private/DefaultToolTip.qml
    private/DialogBackground.qml
)

target_link_libraries(corebindingsplugin PRIVATE
        Qt6::Quick
        Qt6::Qml
        Qt6::Widgets
        KF6::IconThemes
        KF6::I18n
        Qt6::Svg
        KF6::WindowSystem
        KF6::I18nQml
        Plasma::Plasma
        Plasma::PlasmaQuick
        Wayland::Client
)

ecm_finalize_qml_module(corebindingsplugin DESTINATION ${KDE_INSTALL_QMLDIR})

ecm_generate_qdoc(corebindingsplugin plasmacore.qdocconf)
