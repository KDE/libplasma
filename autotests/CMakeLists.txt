find_package(Qt6Test ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE)
set_package_properties(Qt6Test PROPERTIES PURPOSE "Required for tests")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
remove_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_STRICT_ITERATORS -DQT_NO_CAST_FROM_BYTEARRAY -DQT_NO_KEYWORDS)

include(ECMMarkAsTest)
include(ECMAddTests)

find_package(KF6CoreAddons REQUIRED)
find_package(Qt6 COMPONENTS QuickTest Widgets REQUIRED)

MACRO(PLASMA_UNIT_TESTS)
       FOREACH(_testname ${ARGN})
               set(libs Qt6::Qml Qt6::Test Plasma::Plasma Plasma::PlasmaQuick
                        KF6::Archive KF6::CoreAddons KF6::ConfigGui KF6::I18n KF6::IconThemes)
               if(QT_QTOPENGL_FOUND)
                   list(APPEND libs Qt6::OpenGL)
               endif()
               ecm_add_test(${_testname}.cpp
                            LINK_LIBRARIES ${libs}
                            NAME_PREFIX "plasma-")
               target_include_directories(${_testname} PRIVATE "$<BUILD_INTERFACE:$<TARGET_PROPERTY:PlasmaQuick,INCLUDE_DIRECTORIES>>;")
       ENDFOREACH(_testname)
ENDMACRO(PLASMA_UNIT_TESTS)

PLASMA_UNIT_TESTS(
    dialogqmltest
    dialogstatetest
    pluginloadertest
    themetest
    sharedqmlenginetest
)

kcoreaddons_add_plugin(dummycontainmentaction SOURCES dummycontainmentaction.cpp INSTALL_NAMESPACE "plasma/containmentactions" STATIC)
target_link_libraries(dummycontainmentaction Plasma::Plasma)
kcoreaddons_target_static_plugins(pluginloadertest NAMESPACE "plasma/containmentactions")

if(HAVE_X11)
    ecm_add_test(
        dialognativetest.cpp
        TEST_NAME dialognativetest
        LINK_LIBRARIES Qt6::Gui Qt6::Test Qt6::Qml Qt6::Quick KF6::WindowSystem Plasma::Plasma Plasma::PlasmaQuick
    )
endif()

ecm_add_test(
    coronatest.cpp
    coronatestresources.qrc
    TEST_NAME coronatest
    LINK_LIBRARIES Qt6::Gui Qt6::Qml Qt6::Widgets Qt6::Test Plasma::Plasma KF6::CoreAddons
)

ecm_add_test(
    quickviewsharedenginetest.cpp
    LINK_LIBRARIES Qt6::Test Plasma::PlasmaQuick
)


#Add a test that i18n is not used directly in any import.
# It should /always/ be i18nd
find_program(SH bash)
if(SH)
    add_test(i18ndcheck ${SH} ${CMAKE_CURRENT_SOURCE_DIR}/i18ndcheck.sh ${CMAKE_SOURCE_DIR}/src/declarativeimports)
endif()

add_subdirectory(applet)
add_subdirectory(declarativetests)
