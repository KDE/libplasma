/***************************************************************************
 *   Copyright 2013 Sebastian Kügler <sebas@kde.org>                       *
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

#ifndef TOOLTIPDIALOG_H
#define TOOLTIPDIALOG_H

#include "dialog.h"

#include <QTimer>
#include <QQuickWindow>
#include <QWeakPointer>
#include <QtCore/QVariant>

class QQuickItem;
class QGraphicsWidget;
class QPropertyAnimation;

namespace KDeclarative
{
    class QmlObject;
}

/**
 * QML wrapper for kdelibs Plasma::ToolTipDialog
 *
 * Exposed as `ToolTipDialog` in QML.
 */
class ToolTipDialog : public DialogProxy
{
    Q_OBJECT

public:
    ToolTipDialog(QQuickItem *parent = 0);
    ~ToolTipDialog();

    QQuickItem *loadDefaultItem();

    Plasma::Types::Direction direction() const;
    void setDirection(Plasma::Types::Direction loc);

    /**
     * animate the position change if visible
     */
    virtual void adjustGeometry(const QRect &geom);

    static ToolTipDialog* instance();

    void dismiss();
    void keepalive();

    bool interactive();
    void setInteractive(bool interactive);

protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
    void resizeEvent(QResizeEvent *re);
    bool event(QEvent *e);

private Q_SLOTS:
    void valueChanged(const QVariant &value);

private:
    KDeclarative::QmlObject *m_qmlObject;
    QTimer *m_showTimer;
    QPropertyAnimation *m_animation;
    int m_hideTimeout;
    Plasma::Types::Direction m_direction;
    bool m_interactive;
};

#endif
