/*
    SPDX-FileCopyrightText: 2012 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plasmaextracomponentsplugin.h"

#include "appbackgroundprovider_p.h"
#include "fallbackcomponent.h"
#include "menu.h"

#include <QQmlEngine>
#include <QtQml>

// #include <KSharedConfig>
// #include <KConfigGroup>

void PlasmaExtraComponentsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_ASSERT(uri == QByteArray("org.kde.plasma.extras"));
    engine->addImageProvider(QStringLiteral("appbackgrounds"), new AppBackgroundProvider);
}

void PlasmaExtraComponentsPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QByteArray("org.kde.plasma.extras"));
    qRegisterMetaType<MenuExtension*>();
    qRegisterMetaType<MenuItem*>();
    qRegisterMetaType<ForeignMenu>();
    qmlRegisterTypesAndRevisions<ForeignMenu>("org.kde.plasma.extras", 2);
    qmlRegisterTypesAndRevisions<MenuItem>("org.kde.plasma.extras", 2);
    qmlRegisterType<FallbackComponent>(uri, 2, 0, "FallbackComponent");
}
