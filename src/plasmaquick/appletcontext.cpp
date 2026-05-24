/*
    SPDX-FileCopyrightText: 2023 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "appletcontext_p.h"

#include <Plasma/Applet>

namespace PlasmaQuick
{

AppletContext::AppletContext(QQmlEngine *engine, Plasma::Applet *applet, QObject *parent)
    : QQmlContext(engine, applet)
    , m_applet(applet)
{
    setParent(parent);
}

AppletContext::~AppletContext()
{
}

Plasma::Applet *AppletContext::applet() const
{
    return m_applet;
}
}

#include "moc_appletcontext_p.cpp"
