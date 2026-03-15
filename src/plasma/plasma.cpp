/*
    SPDX-FileCopyrightText: 2005 Aaron Seigo <aseigo@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <plasma/plasma.h>

#include <QAction>
#include <QApplication>
#include <QMenu>
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
}

} // Plasma namespace

#include "moc_plasma.cpp"
