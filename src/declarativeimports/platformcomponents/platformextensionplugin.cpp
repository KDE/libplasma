/*
 *   Copyright (C) 2013 Ivan Cukic <ivan.cukic(at)kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2,
 *   or (at your option) any later version, as published by the Free
 *   Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <QtQml>
#include <QtQml/QQmlExtensionPlugin>
#include <QDebug>

#include "application.h"

class PlatformComponentsPlugin: public QQmlExtensionPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.plasma.platformcomponents")

public:
    PlatformComponentsPlugin(QObject * parent = Q_NULLPTR)
        : QQmlExtensionPlugin(parent)
    {
        qDebug() << "instantiated plugin object";
    }

    void registerTypes(const char * uri) Q_DECL_OVERRIDE
    {
        qDebug() << "plugin loaded, registering types " << uri;

        Q_ASSERT(QLatin1String(uri) == QLatin1String("org.kde.plasma.platformcomponents"));

        qmlRegisterType<Application> (uri, 1, 0, "Application");
    }

};

#include "platformextensionplugin.moc"

