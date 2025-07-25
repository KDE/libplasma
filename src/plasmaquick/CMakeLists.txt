# Consumer's include dir which has to be explicitly used to make headers of this lib visible to documented includes
# Results in duplicate of prefix-dir & C++ namespace below, but part of different things, so by design:
# <visibility-layer>/<C++ namespace>/class header files
set(PLASMAQUICK_INSTALL_INCLUDEDIR "${KDE_INSTALL_INCLUDEDIR}/PlasmaQuick")

add_library(PlasmaQuick SHARED)
add_library(Plasma::PlasmaQuick ALIAS PlasmaQuick)

qt_extract_metatypes(PlasmaQuick)

set_target_properties(PlasmaQuick PROPERTIES
    VERSION     ${PLASMA_VERSION}
    SOVERSION   ${PLASMA_SOVERSION}
    EXPORT_NAME PlasmaQuick
)

target_sources(PlasmaQuick PRIVATE
    appletcontext.cpp
    appletquickitem.cpp
    appletpopup.cpp
    debug_p.cpp
    dialog.cpp
    dialogshadows.cpp
    containmentview.cpp
    configmodel.cpp
    configview.cpp
    edgeeventforwarder.cpp
    plasmashellwaylandintegration.cpp
    sharedqmlengine.cpp
    quickviewsharedengine.cpp
    plasmawindow.cpp
    popupplasmawindow.cpp
    transientplacementhint.cpp
    windowresizehandler.cpp
    configcategory_p.cpp
    dialogbackground_p.cpp
    utils.cpp
    plasmoid/plasmoiditem.cpp
    plasmoid/containmentitem.cpp
    plasmoid/dropmenu.cpp
    plasmoid/wallpaperitem.cpp

    appletcontext_p.h
    appletquickitem.h
    appletpopup.h
    debug_p.h
    dialog.h
    dialogshadows_p.h
    containmentview.h
    configmodel.h
    configview.h
    edgeeventforwarder.h
    plasmashellwaylandintegration.h
    sharedqmlengine.h
    quickviewsharedengine.h
    plasmawindow.h
    popupplasmawindow.h
    transientplacementhint_p.h
    windowresizehandler.h
    configcategory_p.h
    dialogbackground_p.h
    utils.h
    plasmoid/plasmoiditem.h
    plasmoid/containmentitem.h
    plasmoid/dropmenu.h
    plasmoid/wallpaperitem.h
)

if (Qt6_VERSION VERSION_GREATER_EQUAL "6.8.0")
    set(private_code_option  "PRIVATE_CODE")
endif()
qt_generate_wayland_protocol_client_sources(PlasmaQuick
    FILES
        "${PLASMA_WAYLAND_PROTOCOLS_DIR}/plasma-shell.xml"
        "${Wayland_DATADIR}/wayland.xml"
    ${private_code_option}
)

ecm_qt_declare_logging_category(PlasmaQuick
    HEADER debug_p.h
    IDENTIFIER LOG_PLASMAQUICK
    CATEGORY_NAME kf.plasma.quick
    OLD_CATEGORY_NAMES org.kde.plasmaquick
    DESCRIPTION "Plasma Quick lib"
    EXPORT PLASMA
)

set_target_properties(PlasmaQuick PROPERTIES
    VERSION     ${PLASMA_VERSION}
    SOVERSION   ${PLASMA_SOVERSION}
    EXPORT_NAME PlasmaQuick
)

target_include_directories(PlasmaQuick
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR};${CMAKE_CURRENT_BINARY_DIR}/..>"
    INTERFACE
        "$<INSTALL_INTERFACE:${PLASMAQUICK_INSTALL_INCLUDEDIR}>"
        "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/Plasma>" # module version header
)

target_link_libraries(PlasmaQuick
    PUBLIC
        Qt6::Gui
        Qt6::Quick
        Qt6::Qml
        Plasma::Plasma
        KF6::WindowSystem
    PRIVATE
        Qt6::Svg
        Qt6::GuiPrivate
        Qt6::WaylandClient
        Wayland::Client
        KF6::ConfigGui
        KF6::ConfigQml
        KF6::I18n
        KF6::IconThemes
        KF6::CoreAddons
        KF6::Package
        KF6::KIOCore
        KF6::KIOWidgets
        KF6::Notifications
        KF6::Svg
        KF6::GuiAddons
        KF6::I18nQml

)

if(HAVE_X11)
    target_link_libraries(PlasmaQuick
        PRIVATE
            ${X11_LIBRARIES}
            XCB::XCB
            Qt6::GuiPrivate
    )
endif()

ecm_add_qml_module(plasmaconfigplugin
    URI "org.kde.plasma.configuration"
    VERSION 2.0
    GENERATE_PLUGIN_SOURCE
    SOURCES configtypes.h
    DEPENDENCIES QtCore
)
target_link_libraries(plasmaconfigplugin PRIVATE Plasma::PlasmaQuick)
ecm_finalize_qml_module(plasmaconfigplugin)

install(TARGETS PlasmaQuick EXPORT PlasmaQuickTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

ecm_generate_export_header(PlasmaQuick
    BASE_NAME PlasmaQuick
    GROUP_BASE_NAME KF
    VERSION ${KF6_MIN_VERSION}
    USE_VERSION_HEADER
    VERSION_BASE_NAME Plasma
    DEPRECATED_BASE_VERSION 0
    EXCLUDE_DEPRECATED_BEFORE_AND_AT ${EXCLUDE_DEPRECATED_BEFORE_AND_AT}
    DEPRECATION_VERSIONS
)

set(plasmaquick_LIB_INCLUDES
    ${CMAKE_CURRENT_BINARY_DIR}/plasmaquick_export.h
)

ecm_generate_headers(PlasmaQuick_CamelCase_HEADERS
    HEADER_NAMES
        AppletQuickItem
        ContainmentView
        ConfigView
        ConfigModel
        Dialog
        PlasmaWindow
        PlasmaShellWaylandIntegration
        PopupPlasmaWindow
        AppletPopup
        SharedQmlEngine
        QuickViewSharedEngine
    REQUIRED_HEADERS plasmaquick_LIB_INCLUDES
    PREFIX PlasmaQuick
)

ecm_generate_qdoc(PlasmaQuick plasmaquick.qdocconf)

add_subdirectory(plasmoid)

install(
    FILES ${plasmaquick_LIB_INCLUDES}
    DESTINATION ${PLASMAQUICK_INSTALL_INCLUDEDIR}/plasmaquick
    COMPONENT Devel
)

install(
    FILES ${PlasmaQuick_CamelCase_HEADERS}
    DESTINATION ${PLASMAQUICK_INSTALL_INCLUDEDIR}/PlasmaQuick
    COMPONENT Devel
)

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/PlasmaQuick")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/PlasmaQuickConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/PlasmaQuickConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
    PATH_VARS  PLASMAQUICK_INSTALL_INCLUDEDIR CMAKE_INSTALL_PREFIX
)

ecm_setup_version(${PLASMA_VERSION}
                  VARIABLE_PREFIX PLASMAQUICK
                  SOVERSION 6
                  PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/PlasmaQuickConfigVersion.cmake" )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/PlasmaQuickConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/PlasmaQuickConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}" COMPONENT Devel
)

install(EXPORT PlasmaQuickTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE PlasmaQuickTargets.cmake NAMESPACE Plasma:: )
