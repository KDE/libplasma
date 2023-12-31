/*
 *   Copyright 2011 by Marco Martin <mart@kde.org>

 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef PLASMACOMPONENTSPLUGIN_H
#define PLASMACOMPONENTSPLUGIN_H

#include <QQmlExtensionPlugin>
#include <QSet>

class QQmlEngine;
class QQuickItem;
class PlasmaComponentsPlugin;

class EngineBookKeeping : public QObject
{
    Q_OBJECT

public:
    EngineBookKeeping();
    static EngineBookKeeping *self();

    void insertEngine(QQmlEngine *engine);
    QQmlEngine *engine() const;

private Q_SLOTS:
    void engineDestroyed(QObject *deleted);

private:
    QSet <QQmlEngine*> m_engines;
};

class PlasmaComponentsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void initializeEngine(QQmlEngine *engine, const char *uri);
    void registerTypes(const char *uri);
};

#endif
