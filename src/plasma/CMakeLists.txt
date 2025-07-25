add_subdirectory(packagestructure)

# Consumer's include dir which has to be explicitly used to make headers of this lib visible to documented includes
# Results in duplicate of prefix-dir & C++ namespace below, but part of different things, so by design:
# <visibility-layer>/<C++ namespace>/class header files
set(PLASMA_INSTALL_INCLUDEDIR "${KDE_INSTALL_INCLUDEDIR}/Plasma")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-plasma.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-plasma.h)

add_library(Plasma)
add_library(Plasma::Plasma ALIAS Plasma)

qt_extract_metatypes(Plasma)

set_target_properties(Plasma PROPERTIES
    VERSION     ${PLASMA_VERSION}
    SOVERSION   ${PLASMA_SOVERSION}
    EXPORT_NAME Plasma
)

target_sources(Plasma PRIVATE
#global
    plasma.cpp
    pluginloader.cpp

#applets,containments,corona
    applet.cpp
    containment.cpp
    containmentactions.cpp
    corona.cpp
    private/applet_p.cpp
    private/containment_p.cpp

#graphics
    theme.cpp
    private/theme_p.cpp
    private/blureffectwatcher.cpp

    plasma.h
    pluginloader.h

#applets,containments,corona
    applet.h
    containment.h
    containmentactions.h
    corona.h
    private/applet_p.h
    private/containment_p.h

#graphics
    theme.h
    private/theme_p.h
    private/blureffectwatcher_p.h
)

if (Qt6_VERSION VERSION_GREATER_EQUAL "6.8.0")
    set(private_code_option  "PRIVATE_CODE")
endif()
qt6_generate_wayland_protocol_client_sources(Plasma
    ${private_code_option}
    FILES
        ${PLASMA_WAYLAND_PROTOCOLS_DIR}/blur.xml
)

ecm_qt_declare_logging_category(Plasma
    HEADER debug_p.h
    IDENTIFIER LOG_PLASMA
    CATEGORY_NAME kf.plasma.core
    OLD_CATEGORY_NAMES org.kde.plasma
    DESCRIPTION "Plasma Core lib"
    EXPORT PLASMA
)

ecm_generate_export_header(Plasma
    EXPORT_FILE_NAME plasma/plasma_export.h
    BASE_NAME Plasma
    GROUP_BASE_NAME KF
    VERSION ${KF6_MIN_VERSION}
    USE_VERSION_HEADER
    DEPRECATED_BASE_VERSION 0
    EXCLUDE_DEPRECATED_BEFORE_AND_AT ${EXCLUDE_DEPRECATED_BEFORE_AND_AT}
    DEPRECATION_VERSIONS
)

if(HAVE_X11)
    set(PLASMA_EXTRA_LIBS ${PLASMA_EXTRA_LIBS} XCB::XCB)
endif()

target_link_libraries(Plasma
PUBLIC
    Qt6::Gui
    KF6::ConfigCore
    KF6::CoreAddons
    Qt6::Qml
PRIVATE
    Qt6::Quick
    Qt6::WaylandClient
    KF6::ConfigGui
    KF6::ColorScheme
    KF6::ConfigQml
    KF6::Archive
    KF6::I18n
    KF6::WindowSystem #compositingActive
    KF6::GlobalAccel #Applet::setGlobalShortcut
    KF6::Notifications
    KF6::IconThemes
    Plasma::Activities
    KF6::Svg
    ${PLASMA_EXTRA_LIBS}
)

target_link_libraries(Plasma PRIVATE KF6::Package)

set(Plasma_BUILD_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/Plasma
)
target_include_directories(Plasma
    PUBLIC
        "$<BUILD_INTERFACE:${Plasma_BUILD_INCLUDE_DIRS}>"
    INTERFACE
        "$<INSTALL_INTERFACE:${PLASMA_INSTALL_INCLUDEDIR}>"
)

ecm_generate_qdoc(Plasma plasma.qdocconf)

########### install files ###############
ecm_generate_headers(Plasma_CamelCase_HEADERS
    HEADER_NAMES
        Applet
        Containment
        ContainmentActions
        Corona
        PluginLoader
        Theme
        Plasma
    REQUIRED_HEADERS Plasma_HEADERS
    PREFIX Plasma
)

install(FILES
        ${Plasma_HEADERS}
        ${CMAKE_CURRENT_BINARY_DIR}/plasma/plasma_export.h
    DESTINATION ${PLASMA_INSTALL_INCLUDEDIR}/plasma # C++ namespace
    COMPONENT Devel
)

install(
    FILES ${Plasma_CamelCase_HEADERS}
    DESTINATION ${PLASMA_INSTALL_INCLUDEDIR}/Plasma # C++ namespace
    COMPONENT Devel
)

install(TARGETS Plasma EXPORT PlasmaTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

if(BUILD_QCH)
    ecm_add_qch(
        Plasma_QCH
        NAME Plasma
        BASE_NAME Plasma
        VERSION ${PLASMA_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
            ${Plasma_HEADERS}
            Mainpage.dox
        MD_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md"
        LINK_QCHS
            Qt6Gui_QCH
            KF6Service_QCH
            KF6Package_QCH
        INCLUDE_DIRS
            ${Plasma_BUILD_INCLUDE_DIRS}
        BLANK_MACROS
            PLASMA_EXPORT
            PLASMA_DEPRECATED
            PLASMA_DEPRECATED_EXPORT
            "PLASMA_DEPRECATED_VERSION(x, y, t)"
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()
