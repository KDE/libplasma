/*
    SPDX-FileCopyrightText: 2013 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef WALLPAPERITEM_H
#define WALLPAPERITEM_H

#include <QQmlEngine>
#include <QQuickItem>

#include <KPackage/Package>

#include "plasmaquick_export.h"

class KConfigLoader;
class KConfigPropertyMap;

class ContainmentItem;

namespace Plasma
{
class Containment;
}

namespace PlasmaQuick
{
class SharedQmlEngine;
}

/*!
 * \qmltype WallpaperItem
 * \inqmlmodule org.kde.plasma.plasmoid
 * \inherits Item
 *
 * \brief This class is exposed to wallpapers as the WallpaperItem root qml item.
 */
class PLASMAQUICK_EXPORT WallpaperItem : public QQuickItem
{
    Q_OBJECT

    /*!
     * \qmlproperty string WallpaperItem::pluginName
     */
    Q_PROPERTY(QString pluginName READ pluginName WRITE setPluginName NOTIFY pluginNameChanged)

    /*!
     * \qmlproperty KConfigPropertyMap WallpaperItem::configuration
     */
    Q_PROPERTY(KConfigPropertyMap *configuration READ configuration WRITE setConfiguration NOTIFY configurationChanged)

    /*!
     * \qmlproperty list<QAction> WallpaperItem::contextualActions
     *
     * Actions to be added in the desktop context menu. To instantiate QActions in a declarative way,
     * PlasmaCore.Action {} can be used
     */
    Q_PROPERTY(QQmlListProperty<QAction> contextualActions READ qmlContextualActions NOTIFY contextualActionsChanged)

    /*!
     * \qmlproperty bool WallpaperItem::loading
     */
    Q_PROPERTY(bool loading MEMBER m_loading NOTIFY isLoadingChanged)

    /*!
     * \qmlproperty color WallpaperItem::accentColor
     *
     * The accent color manually set by the wallpaper plugin.
     * By default it's transparent, which means either the dominant color is used
     * when "Accent Color From Wallpaper" is enabled, or the theme color is used.
     *
     * \since 6.0
     */
    Q_PROPERTY(QColor accentColor READ accentColor WRITE setAccentColor NOTIFY accentColorChanged RESET resetAccentColor)

public:
    explicit WallpaperItem(QQuickItem *parent = nullptr);
    ~WallpaperItem() override;

    void classBegin() override;
    void componentComplete() override;

    /*
     * Returns a list of all known wallpapers that can accept the given mimetype
     * mimetype the mimetype to search for
     * formFactor the format of the wallpaper being search for (e.g. desktop)
     * Returns list of wallpapers
     */
    static QList<KPluginMetaData> listWallpaperMetadataForMimetype(const QString &mimetype, const QString &formFactor = QString());

    /*
     * Instantiate the WallpaperItem for a given containment, using the proper plugin
     */
    static WallpaperItem *loadWallpaper(ContainmentItem *ContainmentItem);

    KPackage::Package kPackage() const;

    QString pluginName() const;
    void setPluginName(const QString &pluginName);

    KConfigPropertyMap *configuration() const;
    void setConfiguration(KConfigPropertyMap *config);

    void requestOpenUrl(const QUrl &url);

    QList<QAction *> contextualActions() const;

    QML_LIST_PROPERTY_ASSIGN_BEHAVIOR_REPLACE
    QQmlListProperty<QAction> qmlContextualActions();

    bool supportsMimetype(const QString &mimetype) const;

    bool isLoading() const;

    QColor accentColor() const;
    void setAccentColor(const QColor &newColor);
    void resetAccentColor();

Q_SIGNALS:
    void isLoadingChanged();
    void contextualActionsChanged(const QList<QAction *> &actions);
    void accentColorChanged();
    void configurationChanged();
    void pluginNameChanged();

    /*!
     *
     */
    void openUrlRequested(const QUrl &url);

private:
    static void contextualActions_append(QQmlListProperty<QAction> *prop, QAction *action);
    static qsizetype contextualActions_count(QQmlListProperty<QAction> *prop);
    static QAction *contextualActions_at(QQmlListProperty<QAction> *prop, qsizetype idx);
    static void contextualActions_clear(QQmlListProperty<QAction> *prop);
    static void contextualActions_replace(QQmlListProperty<QAction> *prop, qsizetype idx, QAction *action);
    static void contextualActions_removeLast(QQmlListProperty<QAction> *prop);

    QString m_wallpaperPlugin;
    Plasma::Containment *m_containment = nullptr;
    KPackage::Package m_pkg;
    KConfigPropertyMap *m_configuration = nullptr;
    QList<QAction *> m_contextualActions;
    bool m_loading = false;
    std::optional<QColor> m_accentColor;
};

#endif
