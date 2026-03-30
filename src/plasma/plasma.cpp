/*
    SPDX-FileCopyrightText: 2005 Aaron Seigo <aseigo@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <plasma/plasma.h>

#include <QAction>
#include <QApplication>
#include <QMenu>
#include <QQmlComponent>
#include <QQmlEngine>

#include "containment.h"

namespace Plasma
{
Types::Types(QObject *parent)
    : QObject(parent)
{
}

Types::~Types()
{
}

void setupPlasmaStyle(QQmlEngine *engine)
{
    engine->setProperty("_kirigamiTheme", QStringLiteral("KirigamiPlasmaStyle"));

    // Make sure that attached tooltips use the correct style when using plasmacomponents
    // https://qt-project.atlassian.net/browse/QTBUG-144126
    if (engine->property("_q_QQuickToolTip").isNull()) {
        QQmlComponent tooltipComponent(engine, "org.kde.plasma.components", "ToolTip");
        QObject *tooltip = tooltipComponent.create();
        tooltip->setParent(engine);
        engine->setProperty("_q_QQuickToolTip", QVariant::fromValue(tooltip));
    }
}

} // Plasma namespace

#include "moc_plasma.cpp"
