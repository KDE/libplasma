// SPDX-FileCopyrightText: 2026 Noah Davis <noahadvs@gmail.comm>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QQmlEngine>
#include <QQmlExtensionPlugin>

class PlasmaComponentsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)
public:
    PlasmaComponentsPlugin(QObject *parent = nullptr);
    void registerTypes(const char *uri) override;
    void unregisterTypes() override;

private:
    static constexpr auto s_plasmaComponentsUri = "org.kde.plasma.components";
    QByteArray m_importUri;
};
