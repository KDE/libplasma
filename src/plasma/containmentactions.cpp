/*
 *   Copyright (c) 2009 Chani Armitage <chani@kde.org>
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

#include "containmentactions.h"
#include "containment.h"

#include "private/packages_p.h"
#include "private/containmentactions_p.h"
#include "private/containment_p.h"

#include <QMetaEnum>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include <kdebug.h>
#include <klocalizedstring.h>
#include <kservicetypetrader.h>

#include "version.h"

namespace Plasma
{

ContainmentActions::ContainmentActions(QObject * parentObject)
    : d(new ContainmentActionsPrivate(KService::serviceByStorageId(QString()), this))
{
    setParent(parentObject);
}

ContainmentActions::ContainmentActions(QObject *parentObject, const QVariantList &args)
    : d(new ContainmentActionsPrivate(KService::serviceByStorageId(args.count() > 0 ?
                             args[0].toString() : QString()), this))
{
    // now remove first item since those are managed by Wallpaper and subclasses shouldn't
    // need to worry about them. yes, it violates the constness of this var, but it lets us add
    // or remove items later while applets can just pretend that their args always start at 0
    QVariantList &mutableArgs = const_cast<QVariantList &>(args);
    if (!mutableArgs.isEmpty()) {
        mutableArgs.removeFirst();
    }

    setParent(parentObject);
}

ContainmentActions::~ContainmentActions()
{
    delete d;
}

KPluginInfo ContainmentActions::pluginInfo() const
{
    return d->containmentActionsDescription;
}

Containment *ContainmentActions::containment()
{
    if (d->containment) {
        return d->containment;
    }
    return qobject_cast<Containment*>(parent());
}

KConfigGroup ContainmentActions::config() const
{
    KConfigGroup cfg;
    if (!d->containment) {
        return cfg;
    }

    cfg = KConfigGroup(d->containment->corona()->config(), "ActionPlugins");
    return KConfigGroup(&cfg, QString::number(d->containment->containmentType()));
}

void ContainmentActions::restore(const KConfigGroup &config)
{
    Q_UNUSED(config);
}

void ContainmentActions::save(KConfigGroup &config)
{
    Q_UNUSED(config);
}

QWidget *ContainmentActions::createConfigurationInterface(QWidget *parent)
{
    Q_UNUSED(parent);
    return 0;
}

void ContainmentActions::performNextAction()
{
    //do nothing by default, implement in subclasses
}

void ContainmentActions::performPreviousAction()
{
    //do nothing by default, implement in subclasses
}

QList<QAction*> ContainmentActions::contextualActions()
{
    return QList<QAction*>();
}

QString ContainmentActions::eventToString(QEvent *event)
{
    QString trigger;
    Qt::KeyboardModifiers modifiers;

    switch (event->type()) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
        {
            QMouseEvent *e = static_cast<QMouseEvent*>(event);
            int m = QObject::staticQtMetaObject.indexOfEnumerator("MouseButtons");
            QMetaEnum mouse = QObject::staticQtMetaObject.enumerator(m);
            trigger += mouse.valueToKey(e->button());
            modifiers = e->modifiers();
            break;
        }
        case QEvent::Wheel:
        {
            QWheelEvent *e = static_cast<QWheelEvent*>(event);
            int o = QObject::staticQtMetaObject.indexOfEnumerator("Orientations");
            QMetaEnum orient = QObject::staticQtMetaObject.enumerator(o);
            trigger = "wheel:";
            trigger += orient.valueToKey(e->orientation());
            modifiers = e->modifiers();
            break;
        }
        case QEvent::ContextMenu:
        {
            int m = QObject::staticQtMetaObject.indexOfEnumerator("MouseButtons");
            QMetaEnum mouse = QObject::staticQtMetaObject.enumerator(m);
            trigger = mouse.valueToKey(Qt::RightButton);
            modifiers = Qt::NoModifier;
            break;
        }
        default:
            return QString();
    }

    int k = QObject::staticQtMetaObject.indexOfEnumerator("KeyboardModifiers");
    QMetaEnum kbd = QObject::staticQtMetaObject.enumerator(k);
    trigger += ';';
    trigger += kbd.valueToKeys(modifiers);

    return trigger;
}

void ContainmentActions::setContainment(Containment *newContainment)
{
    d->containment = newContainment;
}

} // Plasma namespace


#include "moc_containmentactions.cpp"
