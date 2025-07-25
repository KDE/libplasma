/*
    SPDX-FileCopyrightText: 2008-2013 Aaron Seigo <aseigo@kde.org>
    SPDX-FileCopyrightText: 2010-2013 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CONTAINMENTITEM_H
#define CONTAINMENTITEM_H

#include <QMenu>

#include <Plasma/Containment>

#include "plasmoiditem.h"

class AppletQuickItem;
class WallpaperItem;
class DropMenu;
class KJob;

namespace KIO
{
class Job;
class DropJob;
}

/*!
 * \qmltype ContainmentItem
 * \inqmlmodule org.kde.plasma.plasmoid
 * \inherits PlasmoidItem
 *
 * \brief This class is exposed to containments QML as the attached property Plasmoid.
 */
class PLASMAQUICK_EXPORT ContainmentItem : public PlasmoidItem
{
    Q_OBJECT

    /*!
     * \qmlproperty WallpaperItem ContainmentItem::wallpaper
     */
    Q_PROPERTY(WallpaperItem *wallpaper READ wallpaperItem NOTIFY wallpaperItemChanged)

    /*!
     * \qmlproperty bool ContainmentItem::loading
     * True if the UI is still loading, for instance a desktop which doesn't have its wallpaper yet
     */
    Q_PROPERTY(bool loading READ isLoading NOTIFY isLoadingChanged)

public:
    ContainmentItem(QQuickItem *parent = nullptr);

    void classBegin() override;

    // Not for QML
    Plasma::Containment *containment() const
    {
        return m_containment.data();
    }

    inline WallpaperItem *wallpaperItem() const
    {
        return m_wallpaperItem;
    }

    // For QML use
    /*!
     * \qmlmethod AppletQuickItem ContainmentItem::itemFor(var applet)
     * Returns the corresponding PlasmoidItem of one of its applets
     */
    Q_INVOKABLE AppletQuickItem *itemFor(Plasma::Applet *applet) const;

    /*!
     * \qmlmethod void ContainmentItem::processMimeData(QMimeData data, int x, int y, var dropJob)
     * Process the mime data arrived to a particular coordinate, either with a drag and drop or paste with middle mouse button
     */
    Q_INVOKABLE void processMimeData(QMimeData *data, int x, int y, KIO::DropJob *dropJob = nullptr);

    /*!
     * \qmlmethod void ContainmentItem::processMimeData(QtObject data, int x, int y, var dropJob)
     * Process the mime data arrived to a particular coordinate, either with a drag and drop or paste with middle mouse button
     */
    Q_INVOKABLE void processMimeData(QObject *data, int x, int y, KIO::DropJob *dropJob = nullptr);

    /*!
     * \qmlmethod QtObject ContainmentItem::containmentItemAt(int x, int y)
     * Search for a containment at those coordinates.
     * the coordinates are passed as local coordinates of *this* containment
     */
    Q_INVOKABLE QObject *containmentItemAt(int x, int y);

    /*!
     * \qmlmethod QPointF mapFromApplet(var applet, int x, int y)
     * Map coordinates from relative to the given applet to relative to this containment
     */
    Q_INVOKABLE QPointF mapFromApplet(Plasma::Applet *applet, int x, int y);

    /*!
     * \qmlmethod QPointF mapToApplet(var applet, int x, int y)
     *Map coordinates from relative to this containment to relative to the given applet
     */
    Q_INVOKABLE QPointF mapToApplet(Plasma::Applet *applet, int x, int y);

    /*!
     * \qmlmethod QPointF ContainmentItem::adjustToAvailableScreenRegion(int x, int y, int w, int h)
     * Given a geometry, it adjusts it moving it completely inside of the boundaries
     * of availableScreenRegion
     * @return the toLeft point of the rectangle
     */
    Q_INVOKABLE QPointF adjustToAvailableScreenRegion(int x, int y, int w, int h) const;

    /*!
     * \qmlmethod void openContextMenu(QPointF globalPos)
     * Opens the context menu of the Corona
     *
     * @param globalPos menu position in the global coordinate system
     * @since 5.102
     */
    Q_INVOKABLE void openContextMenu(const QPointF &globalPos);

protected:
    void init() override;
    void loadWallpaper();
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    void addAppletActions(QMenu *desktopMenu, Plasma::Applet *applet, QEvent *event);
    void addContainmentActions(QMenu *desktopMenu, QEvent *event);

    bool isLoading() const;
    void itemChange(ItemChange change, const ItemChangeData &value) override;

Q_SIGNALS:
    // Property notifiers
    void appletsChanged();
    void drawWallpaperChanged();
    void actionsChanged();
    void editModeChanged();
    void wallpaperItemChanged();
    void isLoadingChanged();

private Q_SLOTS:
    // Used only internally by a metaObject()->invokeMethod
    Plasma::Applet *createApplet(const QString &plugin, const QVariantList &args, const QRectF &geom);

private:
    void dropJobResult(KJob *job);
    void mimeTypeRetrieved(KIO::Job *job, const QString &mimetype);
    void appletAddedForward(Plasma::Applet *applet, const QRectF &geometryHint);
    void appletRemovedForward(Plasma::Applet *applet);
    void clearDataForMimeJob(KIO::Job *job);
    void setAppletArgs(Plasma::Applet *applet, const QString &mimetype, const QVariant &data);
    void deleteWallpaperItem();

    WallpaperItem *m_wallpaperItem = nullptr;
    QList<QObject *> m_plasmoidItems;
    QPointer<Plasma::Containment> m_containment;
    std::unique_ptr<QMenu> m_contextMenu; // QTBUG-122409: Keep the context menu so menu actions will keep their accessible interfaces valid
    QPointer<DropMenu> m_dropMenu;
    int m_wheelDelta;
    friend class PlasmoidItem;
};

#endif
