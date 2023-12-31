/***************************************************************************
 *   Copyright 2011 Viranch Mehta <viranch.mehta@gmail.com>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#ifndef QMENU_PROXY_H
#define QMENU_PROXY_H

#include <QObject>
#include <QMenu>
#include <QQmlListProperty>
#include "qmenuitem.h"
#include "enums.h"

class QDeclarativeItem;

class QMenuProxy : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<QMenuItem> content READ content CONSTANT)
    Q_CLASSINFO("DefaultProperty", "content")

    /**
     * the visualParent is used to position the menu. it can be an item on the scene, like a button (that will open the menu on clicked) or another menuitem (in this case this will be a submenu)
     */
    Q_PROPERTY(QObject *visualParent READ visualParent WRITE setVisualParent NOTIFY visualParentChanged())
    Q_PROPERTY(DialogStatus::Status status READ status NOTIFY statusChanged)

public:
    QMenuProxy(QObject *parent = 0);
    ~QMenuProxy();

    QQmlListProperty<QMenuItem> content();
    int actionCount() const;
    QMenuItem *action(int) const;
    DialogStatus::Status status() const;

    QObject *visualParent() const;
    void setVisualParent(QObject *parent);

    Q_INVOKABLE void open(int x = 0, int y = 0);
    //Q_INVOKABLE void open();
    Q_INVOKABLE void close();
    Q_INVOKABLE void clearMenuItems();
    Q_INVOKABLE void addMenuItem(const QString &text);
    Q_INVOKABLE void addMenuItem(QMenuItem *item);

protected:
    bool event(QEvent *event);

Q_SIGNALS:
    void statusChanged();
    void visualParentChanged();
    void triggered(QMenuItem *item);
    void triggeredIndex(int index);

private Q_SLOTS:
    void itemTriggered(QAction *item);

private:
    QList<QMenuItem*> m_items;
    QMenu *m_menu;
    DialogStatus::Status m_status;
    QWeakPointer<QObject> m_visualParent;
};

#endif //QMENU_PROXY_H

