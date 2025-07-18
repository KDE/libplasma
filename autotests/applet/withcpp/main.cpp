/*
 *  SPDX-FileCopyrightText: 2025 Nicolas Fella <nicolas.fella@gmx.de>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <Plasma/Applet>

#include <KPluginFactory>

class MyApplet : public Plasma::Applet
{
    Q_OBJECT

public:
    MyApplet(QObject *parent, const KPluginMetaData &md, const QVariantList &args)
        : Plasma::Applet(parent, md, args)
    {
    }
};

K_PLUGIN_CLASS_WITH_JSON(MyApplet, "metadata.json")

#include "main.moc"
