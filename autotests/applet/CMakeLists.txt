# Don't install test applets
set(PLASMA_SKIP_APPLET_INSTALL ON)

add_subdirectory(simple)
add_subdirectory(withcpp)
add_subdirectory(simplecontainment)

ecm_add_test(applettest.cpp LINK_LIBRARIES Qt::Test Plasma::Plasma Plasma::PlasmaQuick)
