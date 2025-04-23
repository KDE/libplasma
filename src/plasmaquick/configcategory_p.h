/*
    SPDX-FileCopyrightText: 2013 Marco Martin <mart@kde.org>
    SPDX-FileCopyrightText: 2015 Eike Hein <hein@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CONFIGCATEGORY_P_H
#define CONFIGCATEGORY_P_H

#include <QObject>

#include "plasmaquick_export.h"

//
//  W A R N I N G
//  -------------
//
// This file is not part of the public Plasma API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

namespace PlasmaQuick
{
// This class represents a single row item of the ConfigModel model in a QML friendly manner.
// the properties contains all the data needed to represent an icon in the sidebar of a configuration dialog, of applets or containments
class PLASMAQUICK_EXPORT ConfigCategory : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QString pluginName READ pluginName WRITE setPluginName NOTIFY pluginNameChanged)
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(QString configUiModule READ configUiModule WRITE setConfigUiModule NOTIFY configUiModuleChanged)
    Q_PROPERTY(QString configUiComponent READ configUiComponent WRITE setConfigUiComponent NOTIFY configUiComponentChanged)

public:
    ConfigCategory(QObject *parent = nullptr);
    ~ConfigCategory() override;

    QString name() const;
    void setName(const QString &name);

    QString icon() const;
    void setIcon(const QString &icon);

    QString source() const;
    void setSource(const QString &source);

    QString pluginName() const;
    void setPluginName(const QString &pluginName);

    bool visible() const;
    void setVisible(bool visible);

    QString configUiModule() const;
    void setConfigUiModule(const QString &configUiModule);

    QString configUiComponent() const;
    void setConfigUiComponent(const QString &configUiComponent);

Q_SIGNALS:
    void nameChanged();
    void iconChanged();
    void sourceChanged();
    void pluginNameChanged();
    void visibleChanged();
    void configUiModuleChanged();
    void configUiComponentChanged();

private:
    QString m_name;
    QString m_icon;
    QString m_source;
    QString m_pluginName;
    bool m_visible;
    QString m_configUiModule;
    QString m_configUiComponent;
};

}

#endif // multiple inclusion guard
