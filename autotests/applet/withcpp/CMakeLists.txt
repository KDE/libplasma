plasma_add_applet(org.kde.plasma.testapplet2 QML_SOURCES main.qml CPP_SOURCES main.cpp)

target_link_libraries(org.kde.plasma.testapplet2 PRIVATE Plasma::Plasma)
