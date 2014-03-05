/*
    Copyright (C) 2014  Martin Klapetek <mklapetek@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "notificationshelper.h"
#include <kwindowsystem.h>

#include <QScreen>
#include <QGuiApplication>
#include <QDebug>

#include <QQuickWindow>

QRect NotificationsHelper::workAreaForScreen(int screenId)
{
    QRect workArea = KWindowSystem::workArea();

    return qApp->screens().at(screenId)->availableGeometry().intersected(workArea);
}

void NotificationsHelper::setPlasmoidScreen(int screenId)
{
    m_plasmoidScreen = screenId;
}

void NotificationsHelper::positionPopup(QObject *win)
{
    QQuickWindow *popup = qobject_cast<QQuickWindow*>(win);
    bool queued = false;

    if (m_popups.size() == 3) {
        m_queuedPopups << popup;
        queued = true;
    } else {
        m_popups << popup;
    }

    connect(popup, SIGNAL(visibleChanged(bool)),
            this, SLOT(popupClosed(bool)));

    connect(popup, SIGNAL(destroyed(QObject*)),
            this, SLOT(popupDestroyed(QObject*)));

    popup->setX(workAreaForScreen(m_plasmoidScreen).width() - popup->width() - 20);
    popup->setY(workAreaForScreen(m_plasmoidScreen).height() - (m_popups.size() * (popup->height() + 30)));

    if (!queued) {
        popup->setVisible(true);
    }
}

void NotificationsHelper::popupClosed(bool visible)
{
    if (!visible) {
        m_popups.removeOne(qobject_cast<QQuickWindow*>(sender()));
        repositionPopups();
    }

}

void NotificationsHelper::popupDestroyed(QObject *object)
{
    if (object) {
        m_popups.removeOne(qobject_cast<QQuickWindow*>(object));
    }

    repositionPopups();
}

void NotificationsHelper::repositionPopups()
{
    if (m_popups.isEmpty()) {
        return;
    }

    for (int i = 0; i < m_popups.size(); i++) {
        m_popups[i]->setProperty("y", workAreaForScreen(m_plasmoidScreen).height() - ((i + 1) * (m_popups[i]->height() + 30)));
    }

    if (!m_queuedPopups.isEmpty()) {
        positionPopup(m_queuedPopups.takeFirst());
    }
}

#include "notificationshelper.moc"
