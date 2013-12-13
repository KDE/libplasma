/*
 *   Copyright 2007 by Aaron Seigo <aseigo@kde.org>
 *   Copyright 2008 by Ménard Alexis <darktears31@gmail.com>
 *   Copyright 2009 Chani Armitage <chani@kde.org>
 *   Copyright 2012 Marco Martin <notmart@kde.org>
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

#include "containment.h"
#include "private/containment_p.h"

#include "config-plasma.h"

#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QFile>
#include <QContextMenuEvent>
#include <QMimeData>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <qtemporaryfile.h>
#include <qmimedatabase.h>

#include <QDebug>
#include <kauthorized.h>
#include <klocalizedstring.h>
#include <kservicetypetrader.h>

#if !PLASMA_NO_KIO
#include "kio/jobclasses.h" // for KIO::JobFlags
#include "kio/job.h"
#include "kio/scheduler.h"
#endif

#include "containmentactions.h"
#include "corona.h"
#include "pluginloader.h"

#include "private/applet_p.h"

#include "plasma/plasma.h"

namespace Plasma
{

Containment::Containment(QObject *parent,
                         const QString &serviceId,
                         uint containmentId)
    : Applet(parent, serviceId, containmentId),
      d(new ContainmentPrivate(this))
{
    // WARNING: do not access config() OR globalConfig() in this method!
    //          that requires a scene, which is not available at this point
    setContainmentType(Types::CustomContainment);
    setHasConfigurationInterface(true);
}

Containment::Containment(QObject *parent, const QVariantList &args)
    : Applet(parent, args),
      d(new ContainmentPrivate(this))
{
    // WARNING: do not access config() OR globalConfig() in this method!
    //          that requires a scene, which is not available at this point
    setHasConfigurationInterface(true);
}

Containment::Containment(const QString &packagePath, uint appletId)
    : Applet(packagePath, appletId),
      d(new ContainmentPrivate(this))
{
    // WARNING: do not access config() OR globalConfig() in this method!
    //          that requires a scene, which is not available at this point
    setHasConfigurationInterface(true);
}

Containment::~Containment()
{
    delete d;
}

void Containment::init()
{
    Applet::init();

    if (d->type == Types::NoContainmentType) {
        //setContainmentType(Plasma::Types::DesktopContainment);
        //Try to determine the containment type. It must be done as soon as possible
        QString type = pluginInfo().property("X-Plasma-ContainmentType").toString();

        if (type == "Panel") {
            setContainmentType(Plasma::Types::PanelContainment);
        } else if (type == "Custom") {
            setContainmentType(Plasma::Types::CustomContainment);
        } else if (type == "CustomPanel") {
            setContainmentType(Plasma::Types::CustomPanelContainment);
        //default to desktop
        } else {
            setContainmentType(Plasma::Types::DesktopContainment);
        }
    }

    //connect actions
    ContainmentPrivate::addDefaultActions(actions(), this);
    bool unlocked = immutability() == Types::Mutable;

    //fix the text of the actions that need title()
    //btw, do we really want to use title() when it's a desktopcontainment?
    QAction *closeApplet = actions()->action("remove");
    if (closeApplet) {
        closeApplet->setText(i18nc("%1 is the name of the applet", "Remove this %1", title()));
    }

    QAction *configAction = actions()->action("configure");
    if (configAction) {
        configAction->setText(i18nc("%1 is the name of the applet", "%1 Settings", title()));
    }

    QAction *appletBrowserAction = actions()->action("add widgets");
    if (appletBrowserAction) {
        appletBrowserAction->setVisible(unlocked);
        appletBrowserAction->setEnabled(unlocked);
        connect(appletBrowserAction, SIGNAL(triggered()), this, SLOT(triggerShowAddWidgets()));
    }

    if (immutability() != Types::SystemImmutable && corona()) {
        QAction *lockDesktopAction = corona()->actions()->action("lock widgets");
        //keep a pointer so nobody notices it moved to corona
        if (lockDesktopAction) {
            actions()->addAction("lock widgets", lockDesktopAction);
        }
    }
}

// helper function for sorting the list of applets
bool appletConfigLessThan(const KConfigGroup &c1, const KConfigGroup &c2)
{
    int i1 = c1.readEntry("id", 0);
    int i2 = c2.readEntry("id", 0);

    return (i1 < i2);
}

void Containment::restore(KConfigGroup &group)
{
    /*
#ifndef NDEBUG
    // qDebug() << "!!!!!!!!!!!!initConstraints" << group.name() << d->type;
    // qDebug() << "    location:" << group.readEntry("location", (int)d->location);
    // qDebug() << "    geom:" << group.readEntry("geometry", geometry());
    // qDebug() << "    formfactor:" << group.readEntry("formfactor", (int)d->formFactor);
    // qDebug() << "    screen:" << group.readEntry("screen", d->screen);
#endif
*/
    setLocation((Plasma::Types::Location)group.readEntry("location", (int)d->location));
    setFormFactor((Plasma::Types::FormFactor)group.readEntry("formfactor", (int)d->formFactor));
    d->lastScreen = group.readEntry("lastScreen", d->lastScreen);

    setWallpaper(group.readEntry("wallpaperplugin", ContainmentPrivate::defaultWallpaper));
    
    d->activityId = group.readEntry("activityId", QString());

    flushPendingConstraintsEvents();
    restoreContents(group);
    setImmutability((Types::ImmutabilityType)group.readEntry("immutability", (int)Types::Mutable));

    KConfigGroup cfg = KConfigGroup(corona()->config(), "ActionPlugins");
    cfg = KConfigGroup(&cfg, QString::number(containmentType()));


    //qDebug() << cfg.keyList();
    if (cfg.exists()) {
        foreach (const QString &key, cfg.keyList()) {
            //qDebug() << "loading" << key;
            setContainmentActions(key, cfg.readEntry(key, QString()));
        }
    } else { //shell defaults
        KConfigGroup defaultActionsCfg;
        if (d->type == Plasma::Types::PanelContainment) {
            defaultActionsCfg = KConfigGroup(KSharedConfig::openConfig(corona()->package().filePath("defaults")), "Panel");
        //Plasma::Types::DesktopContainment
        } else {
            defaultActionsCfg = KConfigGroup(KSharedConfig::openConfig(corona()->package().filePath("defaults")), "Desktop");
        }
        defaultActionsCfg = KConfigGroup(&defaultActionsCfg, "ContainmentActions");

        foreach (const QString &key, defaultActionsCfg.keyList()) {
            setContainmentActions(key, defaultActionsCfg.readEntry(key, QString()));
        }
    }

    /*
#ifndef NDEBUG
    // qDebug() << "Containment" << id() <<
#endif
                "screen" << screen() <<
                "geometry is" << geometry() <<
                "config entries" << group.entryMap();
    */
}

void Containment::save(KConfigGroup &g) const
{
    if (Applet::d->transient) {
        return;
    }

    KConfigGroup group = g;
    if (!group.isValid()) {
        group = config();
    }

    // locking is saved in Applet::save
    Applet::save(group);

//     group.writeEntry("screen", d->screen);
    group.writeEntry("lastScreen", d->lastScreen);
    group.writeEntry("formfactor", (int)d->formFactor);
    group.writeEntry("location", (int)d->location);
    group.writeEntry("activityId", d->activityId);

    group.writeEntry("wallpaperplugin", d->wallpaper);

    saveContents(group);
}

void Containment::saveContents(KConfigGroup &group) const
{
    KConfigGroup applets(&group, "Applets");
    foreach (const Applet *applet, d->applets) {
        KConfigGroup appletConfig(&applets, QString::number(applet->id()));
        applet->save(appletConfig);
    }
}

void Containment::restoreContents(KConfigGroup &group)
{
    KConfigGroup applets(&group, "Applets");

    // Sort the applet configs in order of geometry to ensure that applets
    // are added from left to right or top to bottom for a panel containment
    QList<KConfigGroup> appletConfigs;
    foreach (const QString &appletGroup, applets.groupList()) {
        //qDebug() << "reading from applet group" << appletGroup;
        KConfigGroup appletConfig(&applets, appletGroup);
        appletConfigs.append(appletConfig);
    }
    qStableSort(appletConfigs.begin(), appletConfigs.end(), appletConfigLessThan);

    QMutableListIterator<KConfigGroup> it(appletConfigs);
    while (it.hasNext()) {
        KConfigGroup &appletConfig = it.next();
        int appId = appletConfig.name().toUInt();
        QString plugin = appletConfig.readEntry("plugin", QString());

        if (plugin.isEmpty()) {
            continue;
        }

        d->createApplet(plugin, QVariantList(), appId);
    }
}

Plasma::Types::ContainmentType Containment::containmentType() const
{
    return d->type;
}

void Containment::setContainmentType(Plasma::Types::ContainmentType type)
{
    if (d->type == type) {
        return;
    }

    d->type = type;
    emit containmentTypeChanged();
}

Corona *Containment::corona() const
{
    return qobject_cast<Corona*>(parent());
}

void Containment::setFormFactor(Types::FormFactor formFactor)
{
    if (d->formFactor == formFactor) {
        return;
    }

    //qDebug() << "switching FF to " << formFactor;
    d->formFactor = formFactor;

    updateConstraints(Plasma::Types::FormFactorConstraint);

    KConfigGroup c = config();
    c.writeEntry("formfactor", (int)formFactor);
    emit configNeedsSaving();
    emit formFactorChanged(formFactor);
}

void Containment::setLocation(Types::Location location)
{
    if (d->location == location) {
        return;
    }

    d->location = location;

    foreach (Applet *applet, d->applets) {
        applet->updateConstraints(Plasma::Types::LocationConstraint);
    }

    updateConstraints(Plasma::Types::LocationConstraint);

    KConfigGroup c = config();
    c.writeEntry("location", (int)location);
    emit configNeedsSaving();
    emit locationChanged(location);
}

Applet *Containment::createApplet(const QString &name, const QVariantList &args)
{
    return d->createApplet(name, args);
}

void Containment::addApplet(Applet *applet)
{
    if (immutability() != Types::Mutable) {
        return;
    }

    if (!applet) {
#ifndef NDEBUG
        // qDebug() << "adding null applet!?!";
#endif
        return;
    }

    if (d->applets.contains(applet)) {
#ifndef NDEBUG
        // qDebug() << "already have this applet!";
#endif
    }

    Containment *currentContainment = applet->containment();

    if (currentContainment && currentContainment != this) {
        emit currentContainment->appletRemoved(applet);

        disconnect(applet, 0, currentContainment, 0);
        KConfigGroup oldConfig = applet->config();
        currentContainment->d->applets.removeAll(applet);
        applet->setParent(this);

        // now move the old config to the new location
        //FIXME: this doesn't seem to get the actual main config group containing plugin=, etc
        KConfigGroup c = config().group("Applets").group(QString::number(applet->id()));
        oldConfig.reparent(&c);
        applet->d->resetConfigurationObject();

        disconnect(applet, SIGNAL(activate()), currentContainment, SIGNAL(activate()));
    } else {
        applet->setParent(this);
    }

    d->applets << applet;

    if (!applet->d->uiReady) {
        d->loadingApplets << applet;
        if (static_cast<Applet *>(this)->d->uiReady) {
            static_cast<Applet *>(this)->d->uiReady = false;
            emit uiReadyChanged(false);
        }
    }

    connect(applet, SIGNAL(configNeedsSaving()), this, SIGNAL(configNeedsSaving()));
    connect(applet, SIGNAL(appletDeleted(Plasma::Applet*)), this, SLOT(appletDeleted(Plasma::Applet*)));
    connect(applet, SIGNAL(statusChanged(Plasma::Types::ItemStatus)), this, SLOT(checkStatus(Plasma::Types::ItemStatus)));
    connect(applet, SIGNAL(activate()), this, SIGNAL(activate()));

    if (!currentContainment) {
        const bool isNew = applet->d->mainConfigGroup()->entryMap().isEmpty();

        if (!isNew) {
            applet->restore(*applet->d->mainConfigGroup());
        }

        applet->init();

        if (isNew) {
            applet->save(*applet->d->mainConfigGroup());
            emit configNeedsSaving();
        }
        //FIXME: an on-appear animation would be nice to have again
    }

    applet->updateConstraints(Plasma::Types::AllConstraints);
    applet->flushPendingConstraintsEvents();

    emit appletAdded(applet);

    if (!currentContainment) {
        applet->updateConstraints(Plasma::Types::StartupCompletedConstraint);
        applet->flushPendingConstraintsEvents();
    }

    applet->d->scheduleModificationNotification();
}

QList<Applet *> Containment::applets() const
{
    return d->applets;
}

int Containment::screen() const
{
    return corona()->screenForContainment(this);
}

int Containment::lastScreen() const
{
    return d->lastScreen;
}

void Containment::setDrawWallpaper(bool drawWallpaper)
{
    if (d->drawWallpaper == drawWallpaper) {
        return;
    }

    d->drawWallpaper = drawWallpaper;
    emit drawWallpaperChanged();
}

bool Containment::drawWallpaper()
{
    return d->drawWallpaper;
}

void Containment::setWallpaper(const QString &pluginName)
{
    if (pluginName != d->wallpaper) {
        d->wallpaper = pluginName;

        KConfigGroup cfg = config();
        cfg.writeEntry("wallpaperplugin", d->wallpaper);
        emit configNeedsSaving();
        emit wallpaperChanged();
    }
}

QString Containment::wallpaper() const
{
    return d->wallpaper;
}

void Containment::setContainmentActions(const QString &trigger, const QString &pluginName)
{
    KConfigGroup cfg = d->containmentActionsConfig();
    ContainmentActions *plugin = 0;

    if (containmentActions().contains(trigger)) {
        plugin = containmentActions().value(trigger);
        if (plugin->pluginInfo().pluginName() != pluginName) {
            containmentActions().remove(trigger);
            delete plugin;
            plugin = 0;
        }
    }

    if (pluginName.isEmpty()) {
        cfg.deleteEntry(trigger);
    } else if (plugin) {
        //it already existed, just reload config
        plugin->setContainment(this); //to be safe
        //FIXME make a truly unique config group
        KConfigGroup pluginConfig = KConfigGroup(&cfg, trigger);
        plugin->restore(pluginConfig);

    } else {
        plugin = PluginLoader::self()->loadContainmentActions(this, pluginName);

        if (plugin) {
            cfg.writeEntry(trigger, pluginName);
            containmentActions().insert(trigger, plugin);
            plugin->setContainment(this);
            KConfigGroup pluginConfig = KConfigGroup(&cfg, trigger);
            plugin->restore(pluginConfig);
        } else {
            //bad plugin... gets removed. is this a feature or a bug?
            cfg.deleteEntry(trigger);
        }
    }

    emit configNeedsSaving();
}

QHash<QString, ContainmentActions*> &Containment::containmentActions()
{
    return d->localActionPlugins;
}

bool Containment::isUiReady() const
{
    return static_cast<const Applet *>(this)->d->uiReady;
}

void Containment::setActivity(const QString &activityId)
{
    if (activityId.isEmpty() || d->activityId == activityId) {
        return;
    }

    d->activityId = activityId;
    KConfigGroup c = config();
    c.writeEntry("activityId", activityId);

    emit configNeedsSaving();
    emit activityChanged(activityId);
}

QString Containment::activity() const
{
    return d->activityId;
}

void Containment::reactToScreenChange()
{
    int newScreen = screen();

    KConfigGroup c = config();
    if (newScreen >= 0) {
        d->lastScreen = newScreen;
        c.writeEntry("lastScreen", d->lastScreen);
    }
    emit configNeedsSaving();
    emit screenChanged(newScreen);

    if (newScreen >= 0) {
        emit activate();
    }
}

} // Plasma namespace



#include "moc_containment.cpp"
