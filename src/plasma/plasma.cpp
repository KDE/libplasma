/*
    SPDX-FileCopyrightText: 2005 Aaron Seigo <aseigo@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <plasma/plasma.h>

#include <QAction>
#include <QApplication>
#include <QLibraryInfo>
#include <QMenu>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QVersionNumber>

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
    // TODO: drop the version check once the minimum required Qt version is 6.12
    const char *toolTipProperty = QLibraryInfo::version() >= QVersionNumber(6, 12) ? "_q_shared_QQuickToolTip" : "_q_QQuickToolTip";

    if (engine->property(toolTipProperty).isNull()) {
        QQmlComponent tooltipComponent(engine, "org.kde.plasma.components", "ToolTip");
        if (QObject *tooltip = tooltipComponent.create()) {
            tooltip->setParent(engine);
            engine->setProperty(toolTipProperty, QVariant::fromValue(tooltip));
        }
    }
}

} // Plasma namespace

#include "moc_plasma.cpp"
