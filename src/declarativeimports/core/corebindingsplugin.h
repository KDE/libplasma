/*
    SPDX-FileCopyrightText: 2009 Alan Alpert <alan.alpert@nokia.com>
    SPDX-FileCopyrightText: 2010 Ménard Alexis <menard@kde.org>
    SPDX-FileCopyrightText: 2010 Marco Martin <mart@kde.org>
    SPDX-FileCopyrightText: 2013 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef COREBINDINGSPLUGIN_H
#define COREBINDINGSPLUGIN_H

#include <QQmlEngine>
#include <QQmlExtensionPlugin>
#include <QQmlPropertyMap>

#include <Plasma/Plasma>

#include "action.h"
#include "applet.h"
#include "appletpopup.h"
#include "containment.h"
#include "corona.h"
#include "dialog.h"

struct TypesForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Types)
    QML_UNCREATABLE("")
    QML_FOREIGN(Plasma::Types)
};

struct PropertyMapForeign {
    Q_GADGET
    QML_ANONYMOUS
    QML_FOREIGN(QQmlPropertyMap)
};

struct PlasmaWindowForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Window)
    QML_FOREIGN(PlasmaQuick::PlasmaWindow)
};

struct AppletPopupForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(AppletPopup)
    QML_FOREIGN(PlasmaQuick::AppletPopup)
};

struct PopupPlasmaWindowForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(PopupPlasmaWindow)
    QML_FOREIGN(PlasmaQuick::PopupPlasmaWindow)
};

struct DialogForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Dialog)
    QML_FOREIGN(PlasmaQuick::Dialog)
};

struct ContainmentForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Containment)
    QML_UNCREATABLE("")
    QML_FOREIGN(Plasma::Containment)
};

struct CoronaForeign {
    Q_GADGET
    QML_ANONYMOUS
    QML_FOREIGN(Plasma::Corona)
};

struct AppletForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Applet)
    QML_UNCREATABLE("")
    QML_FOREIGN(Plasma::Applet)
};

struct ActionForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Action)
    QML_FOREIGN(QAction)
    QML_EXTENDED(ActionExtension)
};

class CoreBindingsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void initializeEngine(QQmlEngine *engine, const char *uri) override;
    void registerTypes(const char *uri) override;
};

#endif
