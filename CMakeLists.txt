cmake_minimum_required(VERSION 3.16)

set(PROJECT_VERSION "6.7.80")
project(libplasma VERSION ${PROJECT_VERSION})

set(QT_MIN_VERSION "6.10.0")
set(PROJECT_DEP_VERSION "6.6.90")
set(KF6_MIN_VERSION "6.26.0")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} )

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEGitCommitHooks)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEClangFormat)

include(ECMGenerateExportHeader)
include(ECMGenerateHeaders)
include(CMakePackageConfigHelpers)
include(ECMSetupVersion)
include(ECMQtDeclareLoggingCategory)
include(KDEPackageAppTemplates)
include(ECMGenerateQmlTypes)
include(ECMMarkNonGuiExecutable)
include(ECMDeprecationSettings)
include(ECMQmlModule)
include(ECMGenerateQDoc)
include(ECMFindQmlModule)

option(ENABLE_ACTIVITIES "Enable support for Plasma Activities in containments" ON)

ecm_setup_version(PROJECT
                  VARIABLE_PREFIX PLASMA
                  VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/plasma_version.h"
                  PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/PlasmaConfigVersion.cmake"
                  SOVERSION 7)

################# now find all used packages #################

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Quick Gui Qml Svg QuickControls2 DBus)

find_package(Qt6GuiPrivate ${QT_MIN_VERSION} REQUIRED NO_MODULE)

find_package(KF6 ${KF6_MIN_VERSION} REQUIRED
    COMPONENTS
        Config
        CoreAddons
        GlobalAccel
        GuiAddons
        I18n
        IconThemes
        KIO
        WindowSystem
        Notifications
        Package
        KirigamiPlatform
        Svg
        ColorScheme
)

# Wayland features
find_package(PlasmaWaylandProtocols 1.10.0 REQUIRED)
find_package(Qt6WaylandClient ${QT_MIN_VERSION} REQUIRED CONFIG)
find_package(Wayland 1.9 REQUIRED Client)
find_package(WaylandProtocols 1.46 REQUIRED)

if(ENABLE_ACTIVITIES)
    find_package(PlasmaActivities ${PROJECT_DEP_VERSION} REQUIRED)
endif()

ecm_find_qmlmodule(Qt5Compat.GraphicalEffects REQUIRED)

ecm_find_qmlmodule(org.kde.config REQUIRED)
ecm_find_qmlmodule(org.kde.kirigami REQUIRED)
ecm_find_qmlmodule(org.kde.ksvg REQUIRED)

#########################################################################

ecm_set_disabled_deprecation_versions(
    QT 6.11.0
    KF 6.27.0
)

#add_definitions(-Wno-deprecated)

include(PlasmaMacros.cmake)

#########################################################################


option(BUILD_EXAMPLES "Build and install Plasma examples." OFF)
option(BUILD_COVERAGE "Build Plasma Frameworks with gcov support" OFF)

if (BUILD_EXAMPLES)
    find_package(KF6DBusAddons ${KF6_MIN_VERSION})
endif()

if(BUILD_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov")
endif()

# make plasma_version.h available
include_directories(${CMAKE_CURRENT_BINARY_DIR})

################# list the subdirectories #################
add_definitions(-DTRANSLATION_DOMAIN=\"libplasma6\")
add_definitions(
    -DQT_NO_CONTEXTLESS_CONNECT
)
ki18n_install(po)
add_subdirectory(src)

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (BUILD_TESTING)
    add_subdirectory(autotests)
    add_subdirectory(tests)
endif()
add_subdirectory(templates)

################ create PlasmaConfig.cmake and install it ###########################

# create a Config.cmake and a ConfigVersion.cmake file and install them

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/Plasma")

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/PlasmaConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/PlasmaConfig.cmake"
  INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
  PATH_VARS  CMAKE_INSTALL_PREFIX
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/PlasmaConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/PlasmaConfigVersion.cmake"
  "${CMAKE_CURRENT_SOURCE_DIR}/PlasmaMacros.cmake"
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
  COMPONENT Devel
)

install(EXPORT PlasmaTargets
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        FILE PlasmaTargets.cmake
        NAMESPACE Plasma::
        COMPONENT Devel)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/plasma_version.h
        DESTINATION ${KDE_INSTALL_INCLUDEDIR}/Plasma COMPONENT Devel )

include(ECMFeatureSummary)
ecm_feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

# add clang-format target for all our source files
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
