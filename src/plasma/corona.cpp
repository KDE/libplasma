/*
 *   Copyright 2007 Matt Broadstone <mbroadst@gmail.com>
 *   Copyright 2007-2011 Aaron Seigo <aseigo@kde.org>
 *   Copyright 2007 Riccardo Iaconelli <riccardo@kde.org>
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

#include "corona.h"
#include "private/corona_p.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QMimeData>
#include <QPainter>
#include <QTimer>

#include <cmath>

#include <QDebug>
#include <kiconloader.h>
#include <klocalizedstring.h>
#include <kwindowsystem.h>

#include "containment.h"
#include "pluginloader.h"
#include "private/applet_p.h"
#include "private/containment_p.h"

using namespace Plasma;

namespace Plasma
{

Corona::Corona(QObject *parent)
    : QObject(parent),
      d(new CoronaPrivate(this))
{
#ifndef NDEBUG
    // qDebug() << "!!{} STARTUP TIME" << QTime().msecsTo(QTime::currentTime()) << "Corona ctor start";
#endif
    d->init();
    //setViewport(new QGLWidget(QGLFormat(QGL::StencilBuffer | QGL::AlphaChannel)));
}

Corona::~Corona()
{
    KConfigGroup trans(KSharedConfig::openConfig(), "PlasmaTransientsConfig");
    trans.deleteGroup();

    delete d;
}

Plasma::Package Corona::package() const
{
    return d->package;
}

void Corona::setPackage(const Plasma::Package &package)
{
    d->package = package;
    emit packageChanged(package);
}

void Corona::saveLayout(const QString &configName) const
{
    KSharedConfigPtr c;

    if (configName.isEmpty() || configName == d->configName) {
        c = config();
    } else {
        c = KSharedConfig::openConfig(configName, KConfig::SimpleConfig);
    }

    d->saveLayout(c);
}

void Corona::exportLayout(KConfigGroup &config, QList<Containment*> containments)
{
    foreach (const QString &group, config.groupList()) {
        KConfigGroup cg(&config, group);
        cg.deleteGroup();
    }

    //temporarily unlock so that removal works
    Types::ImmutabilityType oldImm = immutability();
    d->immutability = Types::Mutable;

    KConfigGroup dest(&config, "Containments");
    KConfigGroup dummy;
    foreach (Plasma::Containment *c, containments) {
        c->save(dummy);
        c->config().reparent(&dest);

        //ensure the containment is unlocked
        //this is done directly because we have to bypass any Types::SystemImmutable checks
        c->Applet::d->immutability = Types::Mutable;
        foreach (Applet *a, c->applets()) {
            a->d->immutability = Types::Mutable;
        }

        c->destroy();
    }

    //restore immutability
    d->immutability = oldImm;

    config.sync();
}

void Corona::requestConfigSync()
{
    // constant controlling how long between requesting a configuration sync
    // and one happening should occur. currently 10 seconds
    static const int CONFIG_SYNC_TIMEOUT = 10000;

    // TODO: should we check into our immutability before doing this?

    //NOTE: this is a pretty simplistic model: we simply save no more than CONFIG_SYNC_TIMEOUT
    //      after the first time this is called. not much of a heuristic for save points, but
    //      it should at least compress these activities a bit and provide a way for applet
    //      authors to ween themselves from the sync() disease. A more interesting/dynamic
    //      algorithm for determining when to actually sync() to disk might be better, though.
    if (!d->configSyncTimer->isActive()) {
        d->configSyncTimer->start(CONFIG_SYNC_TIMEOUT);
    }
}

void Corona::requireConfigSync()
{
    d->syncConfig();
}

void Corona::loadLayout(const QString &configName)
{
    if (!configName.isEmpty() && configName != d->configName) {
        // if we have a new config name passed in, then use that as the config file for this Corona
        d->config = 0;
        d->configName = configName;
    }

    KConfigGroup conf(config(), QString());
    d->importLayout(conf, false);
}

QList<Plasma::Containment *> Corona::importLayout(const KConfigGroup &conf)
{
    return d->importLayout(conf, true);
}

Containment *Corona::containmentForScreen(int screen) const
{
    foreach (Containment *containment, d->containments) {
        if (containment->screen() == screen &&
            (containment->containmentType() == Plasma::Types::DesktopContainment ||
             containment->containmentType() == Plasma::Types::CustomContainment)) {
            return containment;
        }
    }

    return 0;
}

QList<Containment*> Corona::containments() const
{
    return d->containments;
}

KSharedConfigPtr Corona::config() const
{
    if (!d->config) {
        d->config = KSharedConfig::openConfig(d->configName, KConfig::SimpleConfig);
    }

    return d->config;
}

Containment *Corona::createContainment(const QString &name, const QVariantList &args)
{
    if (d->immutability == Types::Mutable) {
        return d->addContainment(name, args, 0);
    }

    return 0;
}

int Corona::numScreens() const
{
    return 1;
}

QRect Corona::screenGeometry(int id) const
{
    return qApp->desktop()->screenGeometry(id);
}

QRegion Corona::availableScreenRegion(int id) const
{
    return QRegion(screenGeometry(id));
}

void Corona::loadDefaultLayout()
{
    //Default implementation does nothing
}

Types::ImmutabilityType Corona::immutability() const
{
    return d->immutability;
}

void Corona::setImmutability(const Types::ImmutabilityType immutable)
{
    if (d->immutability == immutable || d->immutability == Types::SystemImmutable) {
        return;
    }

#ifndef NDEBUG
    // qDebug() << "setting immutability to" << immutable;
#endif
    d->immutability = immutable;
    d->updateContainmentImmutability();
    //tell non-containments that might care (like plasmaapp or a custom corona)
    emit immutabilityChanged(immutable);

    //update our actions
    QAction *action = d->actions.action("lock widgets");
    if (action) {
        if (d->immutability == Types::SystemImmutable) {
            action->setEnabled(false);
            action->setVisible(false);
        } else {
            bool unlocked = d->immutability == Types::Mutable;
            action->setText(unlocked ? i18n("Lock Widgets") : i18n("Unlock Widgets"));
            action->setIcon(QIcon::fromTheme(unlocked ? "object-locked" : "object-unlocked"));
            action->setEnabled(true);
            action->setVisible(true);
        }
    }

    if (d->immutability != Types::SystemImmutable) {
        KConfigGroup cg(config(), "General");

        // we call the dptr member directly for locked since isImmutable()
        // also checks kiosk and parent containers
        cg.writeEntry("immutability", (int)d->immutability);
        requestConfigSync();
    }
}

QList<Plasma::Types::Location> Corona::freeEdges(int screen) const
{
    QList<Plasma::Types::Location> freeEdges;
    freeEdges << Plasma::Types::TopEdge << Plasma::Types::BottomEdge
              << Plasma::Types::LeftEdge << Plasma::Types::RightEdge;

    foreach (Containment *containment, containments()) {
        if (containment->screen() == screen &&
            freeEdges.contains(containment->location())) {
            freeEdges.removeAll(containment->location());
        }
    }

    return freeEdges;
}

KActionCollection *Corona::actions() const
{
    return &d->actions;
}

CoronaPrivate::CoronaPrivate(Corona *corona)
    : q(corona),
      immutability(Types::Mutable),
      config(0),
      configSyncTimer(new QTimer(corona)),
      actions(corona)
{
    //TODO: make Package path configurable
    KConfigGroup config(KSharedConfig::openConfig(), "General");

    if (QCoreApplication::instance()) {
        configName = QCoreApplication::instance()->applicationName() + "-appletsrc";
    } else {
        configName = "plasma-appletsrc";
    }
}

CoronaPrivate::~CoronaPrivate()
{
    qDeleteAll(containments);
}

void CoronaPrivate::init()
{
    desktopDefaultsConfig = KConfigGroup(KSharedConfig::openConfig(package.filePath("defaults")), "Desktop");

    configSyncTimer->setSingleShot(true);
    QObject::connect(configSyncTimer, SIGNAL(timeout()), q, SLOT(syncConfig()));

    //some common actions
    actions.setConfigGroup("Shortcuts");

    QAction *lockAction = actions.add<QAction>("lock widgets");
    QObject::connect(lockAction, SIGNAL(triggered(bool)), q, SLOT(toggleImmutability()));
    lockAction->setText(i18n("Lock Widgets"));
    lockAction->setAutoRepeat(true);
    lockAction->setIcon(QIcon::fromTheme("object-locked"));
    lockAction->setData(Plasma::Types::ControlAction);
    lockAction->setShortcut(QKeySequence("alt+d, l"));
    lockAction->setShortcutContext(Qt::ApplicationShortcut);

    //fake containment/applet actions
    KActionCollection *containmentActions = AppletPrivate::defaultActions(q); //containment has to start with applet stuff
    ContainmentPrivate::addDefaultActions(containmentActions); //now it's really containment
}

void CoronaPrivate::toggleImmutability()
{
    if (immutability == Types::Mutable) {
        q->setImmutability(Types::UserImmutable);
    } else {
        q->setImmutability(Types::Mutable);
    }
}

void CoronaPrivate::saveLayout(KSharedConfigPtr cg) const
{
    KConfigGroup containmentsGroup(cg, "Containments");
    foreach (const Containment *containment, containments) {
        QString cid = QString::number(containment->id());
        KConfigGroup containmentConfig(&containmentsGroup, cid);
        containment->save(containmentConfig);
    }
}

void CoronaPrivate::updateContainmentImmutability()
{
    foreach (Containment *c, containments) {
        // we need to tell each containment that immutability has been altered
        c->updateConstraints(Types::ImmutableConstraint);
    }
}

void CoronaPrivate::containmentDestroyed(QObject *obj)
{
        // we do a static_cast here since it really isn't an Containment by this
        // point anymore since we are in the qobject dtor. we don't actually
        // try and do anything with it, we just need the value of the pointer
        // so this unsafe looking code is actually just fine.
        Containment* containment = static_cast<Plasma::Containment*>(obj);
        int index = containments.indexOf(containment);

        if (index > -1) {
            containments.removeAt(index);
            q->requestConfigSync();
        }
    }

void CoronaPrivate::syncConfig()
{
    q->config()->sync();
    emit q->configSynced();
}

Containment *CoronaPrivate::addContainment(const QString &name, const QVariantList &args, uint id)
{
    QString pluginName = name;
    Containment *containment = 0;
    Applet *applet = 0;

    // qDebug() << "Loading" << name << args << id;

    if (pluginName.isEmpty() || pluginName == "default") {
        // default to the desktop containment
        pluginName = desktopDefaultsConfig.readEntry("Containment", "org.kde.desktopcontainment");
    }

    bool loadingNull = pluginName == "null";
    if (!loadingNull) {
        applet = PluginLoader::self()->loadApplet(pluginName, id, args);
        containment = dynamic_cast<Containment*>(applet);
        if (containment) {
            containment->setParent(q);
        }
    }

    if (!containment) {
        if (!loadingNull) {
#ifndef NDEBUG
            // qDebug() << "loading of containment" << name << "failed.";
#endif
        }

        // in case we got a non-Containment from Applet::loadApplet or
        // a null containment was requested
        if (applet) {
            // the applet probably doesn't know what's hit it, so let's pretend it can be
            // initialized to make assumptions in the applet's dtor safer
            applet->init();
            delete applet;
        }
        applet = containment = new Containment(q, 0, id);

        if (loadingNull) {
            containment->setDrawWallpaper(false);
        }

        // we want to provide something and don't care about the failure to launch
        containment->setFormFactor(Plasma::Types::Planar);
    }

    // if this is a new containment, we need to ensure that there are no stale
    // configuration data around
    if (id == 0) {
        KConfigGroup conf(q->config(), "Containments");
        conf = KConfigGroup(&conf, QString::number(containment->id()));
        conf.deleteGroup();
    }

    containments.append(containment);

    QObject::connect(containment, SIGNAL(destroyed(QObject*)),
            q, SLOT(containmentDestroyed(QObject*)));
    QObject::connect(containment, SIGNAL(configNeedsSaving()),
            q, SLOT(requestConfigSync()));
    QObject::connect(containment, SIGNAL(screenChanged(int,int,Plasma::Containment*)),
            q, SIGNAL(screenOwnerChanged(int,int,Plasma::Containment*)));

    containment->init();
    KConfigGroup cg = containment->config();
    containment->restore(cg);
    containment->updateConstraints(Plasma::Types::StartupCompletedConstraint);
    containment->save(cg);
    q->requestConfigSync();
    containment->flushPendingConstraintsEvents();
    emit q->containmentAdded(containment);

    return containment;
}

QList<Plasma::Containment *> CoronaPrivate::importLayout(const KConfigGroup &conf, bool mergeConfig)
{
    if (!conf.isValid()) {
        return QList<Containment *>();
    }

    QList<Plasma::Containment *> newContainments;
    QSet<uint> containmentsIds;

    foreach (Containment *containment, containments) {
        containmentsIds.insert(containment->id());
    }

    KConfigGroup containmentsGroup(&conf, "Containments");

    foreach (const QString &group, containmentsGroup.groupList()) {
        KConfigGroup containmentConfig(&containmentsGroup, group);

        if (containmentConfig.entryMap().isEmpty()) {
            continue;
        }

        uint cid = group.toUInt();
        if (containmentsIds.contains(cid)) {
            cid = ++AppletPrivate::s_maxAppletId;
        } else if (cid > AppletPrivate::s_maxAppletId) {
            AppletPrivate::s_maxAppletId = cid;
        }

        if (mergeConfig) {
            KConfigGroup realConf(q->config(), "Containments");
            realConf = KConfigGroup(&realConf, QString::number(cid));
            // in case something was there before us
            realConf.deleteGroup();
            containmentConfig.copyTo(&realConf);
        }

        //qDebug() << "got a containment in the config, trying to make a" << containmentConfig.readEntry("plugin", QString()) << "from" << group;
#ifndef NDEBUG
        // qDebug() << "!!{} STARTUP TIME" << QTime().msecsTo(QTime::currentTime()) << "Adding Containment" << containmentConfig.readEntry("plugin", QString());
#endif
        Containment *c = addContainment(containmentConfig.readEntry("plugin", QString()), QVariantList(), cid);
        if (!c) {
            continue;
        }

        newContainments.append(c);
        containmentsIds.insert(c->id());

#ifndef NDEBUG
//         qDebug() << "!!{} STARTUP TIME" << QTime().msecsTo(QTime::currentTime()) << "Restored Containment" << c->pluginName();
#endif
    }

    return newContainments;
}

} // namespace Plasma



#include "moc_corona.cpp"
