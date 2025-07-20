/*
    SPDX-FileCopyrightText: 2023 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <plasmaquick_export.h>

#include "sharedqmlengine.h"

#include <QQmlContext>

namespace Plasma
{
class Applet;
}

namespace PlasmaQuick
{

class PLASMAQUICK_EXPORT AppletContext : public QQmlContext
{
    Q_OBJECT
public:
    AppletContext(QQmlEngine *engine, Plasma::Applet *applet, SharedQmlEngine *parent);
    ~AppletContext() override;

    Plasma::Applet *applet() const;
    SharedQmlEngine *sharedQmlEngine() const;

private:
    Plasma::Applet *m_applet;
    SharedQmlEngine *m_sharedEngine;
};

}
