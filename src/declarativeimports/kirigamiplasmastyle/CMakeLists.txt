
add_library(KirigamiPlasmaStyle MODULE ${org.kde.desktop_SRCS})

target_sources(KirigamiPlasmaStyle PRIVATE
    plugin.cpp
    units.cpp
    plasmatheme.cpp
    plugin.h
    units.h
    plasmatheme.h
)

target_link_libraries(KirigamiPlasmaStyle
    PUBLIC
        Qt6::Core
        KF6::KirigamiPlatform
    PRIVATE
        Qt6::Qml
        Qt6::Quick
        KF6::ColorScheme
        KF6::IconThemes
        Plasma::Plasma
)

install(TARGETS KirigamiPlasmaStyle DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf6/kirigami/platform)

install(FILES
    AbstractApplicationHeader.qml
    DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kirigami/styles/Plasma
)
