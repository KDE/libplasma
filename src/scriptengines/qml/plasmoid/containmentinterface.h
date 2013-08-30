/*
 *   Copyright 2008-2013 Aaron Seigo <aseigo@kde.org>
 *   Copyright 2010-2013 Marco Martin <mart@kde.org>
 *
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

#ifndef CONTAINMENTINTERFACE_H
#define CONTAINMENTINTERFACE_H

#include <QMenu>

#include <Plasma/Containment>

#include "appletinterface.h"

class QmlObject;
class WallpaperInterface;

namespace KIO {
    class Job;
}

class ContainmentInterface : public AppletInterface
{
    Q_OBJECT
    Q_PROPERTY(QList <QObject *> applets READ applets NOTIFY appletsChanged)
    Q_PROPERTY(bool drawWallpaper READ drawWallpaper WRITE setDrawWallpaper)
    Q_PROPERTY(Plasma::Types::ContainmentType containmentType READ containmentType WRITE setContainmentType)
    Q_PROPERTY(int screen READ screen NOTIFY screenChanged)
    Q_PROPERTY(QString activity READ activity NOTIFY activityChanged)

public:
    ContainmentInterface(DeclarativeAppletScript *parent);
//Not for QML
    inline Plasma::Containment *containment() const { return static_cast<Plasma::Containment *>(m_appletScriptEngine->applet()->containment()); }

    inline WallpaperInterface *wallpaperInterface() const { return m_wallpaperInterface;}

//For QML use
    QList<QObject *> applets();

    void setDrawWallpaper(bool drawWallpaper);
    bool drawWallpaper();
    Plasma::Types::ContainmentType containmentType() const;
    void setContainmentType(Plasma::Types::ContainmentType type);
    int screen() const;

    QString activity() const;

    Q_INVOKABLE void lockWidgets(bool locked);
    Q_INVOKABLE QRectF screenGeometry(int id) const;
    Q_INVOKABLE QVariantList availableScreenRegion(int id) const;
    Q_INVOKABLE void processMimeData(QMimeData *data, int x, int y);

protected:
    void init();
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    void addAppletActions(QMenu &desktopMenu, Plasma::Applet *applet, QEvent *event);
    void addContainmentActions(QMenu &desktopMenu, QEvent *event);

Q_SIGNALS:
    void appletAdded(QObject *applet, int x, int y);
    void appletRemoved(QObject *applet);
    void screenChanged();
    void activityChanged();
    void availableScreenRegionChanged();
    void appletsChanged();
    ///void immutableChanged();

protected Q_SLOTS:
    void appletAddedForward(Plasma::Applet *applet);
    void appletRemovedForward(Plasma::Applet *applet);
    void loadWallpaper();
    void dropJobResult(KJob *job);
    void mimeTypeRetrieved(KIO::Job *job, const QString &mimetype);

private:
    void clearDataForMimeJob(KIO::Job *job);
    void addApplet(const QString &plugin, const QVariantList &args, const QPoint &pos);

    WallpaperInterface *m_wallpaperInterface;
    QList<QObject *> m_appletInterfaces;
    QHash<KJob*, QPoint> m_dropPoints;
    QHash<KJob*, QMenu*> m_dropMenus;
};

#endif
