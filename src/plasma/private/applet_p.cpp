/*
 *   Copyright 2005 by Aaron Seigo <aseigo@kde.org>
 *   Copyright 2007 by Riccardo Iaconelli <riccardo@kde.org>
 *   Copyright 2008 by Ménard Alexis <darktears31@gmail.com>
 *   Copyright (c) 2009 Chani Armitage <chani@kde.org>
 *   Copyright 2012 by Marco Martin <mart@kde.org>
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

#include "private/applet_p.h"

#include <config-plasma.h>

#include <QFile>
#include <qstandardpaths.h>

#include <QDebug>
#include <QMessageBox>
#include <klocalizedstring.h>
#include <kkeysequencewidget.h>
#include <kglobalaccel.h>
#include <KConfigLoader>

#include "containment.h"
#include "corona.h"
#include "pluginloader.h"
#include "scripting/scriptengine.h"
#include "scripting/appletscript.h"
#include "private/containment_p.h"
#include "timetracker.h"

namespace Plasma
{

AppletPrivate::AppletPrivate(KService::Ptr service, const KPluginInfo *info, int uniqueID, Applet *applet)
    : appletId(uniqueID),
      q(applet),
      immutability(Types::Mutable),
      appletDescription(info ? *info : KPluginInfo(service)),
      icon(appletDescription.isValid() ? appletDescription.icon() : QString()),
      mainConfig(0),
      pendingConstraints(Types::NoConstraint),
      script(0),
      package(0),
      configLoader(0),
      actions(AppletPrivate::defaultActions(applet)),
      activationAction(0),
      itemStatus(Types::UnknownStatus),
      modificationsTimer(0),
      hasConfigurationInterface(false),
      failed(false),
      transient(false),
      needsConfig(false),
      started(false),
      globalShortcutEnabled(false),
      userConfiguring(false)
{
    if (appletId == 0) {
        appletId = ++s_maxAppletId;
    } else if (appletId > s_maxAppletId) {
        s_maxAppletId = appletId;
    }
    QObject::connect(actions->action("configure"), SIGNAL(triggered()),
                     q, SLOT(requestConfiguration()));
#ifndef NDEBUG
    new TimeTracker(q);
#endif
}

AppletPrivate::~AppletPrivate()
{
    if (activationAction && globalShortcutEnabled) {
        //qDebug() << "resetting global action for" << q->title() << activationAction->objectName();
        KGlobalAccel::self()->removeAllShortcuts(activationAction);
    }

    delete script;
    script = 0;
    delete package;
    package = 0;
    delete configLoader;
    configLoader = 0;
    delete mainConfig;
    mainConfig = 0;
    delete modificationsTimer;
}

void AppletPrivate::init(const QString &packagePath, const QVariantList &args)
{
    // WARNING: do not access config() OR globalConfig() in this method!
    //          that requires a Corona, which is not available at this point
    q->setHasConfigurationInterface(true);

    QAction *closeApplet = actions->action("remove");
    if (closeApplet) {
        closeApplet->setText(i18nc("%1 is the name of the applet", "Remove this %1", q->title()));
    }

    QAction *configAction = actions->action("configure");
    if (configAction) {
        configAction->setText(i18nc("%1 is the name of the applet", "%1 Settings", q->title().replace('&', "&&")));
    }

    if (!appletDescription.isValid()) {
#ifndef NDEBUG
        // qDebug() << "Check your constructor! "
        //         << "You probably want to be passing in a Service::Ptr "
        //         << "or a QVariantList with a valid storageid as arg[0].";
#endif
        return;
    }

    QString api = appletDescription.property("X-Plasma-API").toString();

    if (api.isEmpty()) {
        q->setLaunchErrorMessage(i18n("The %2 widget did not define which ScriptEngine to use.", appletDescription.name()));
        return;
    }

    const QString path = packagePath.isEmpty() ? appletDescription.pluginName() : packagePath;
    package = new Package(PluginLoader::self()->loadPackage("Plasma/Applet", api));
    package->setPath(path);

    if (!package->isValid()) {
        delete package;
        package = 0;
        q->setLaunchErrorMessage(i18nc("Package file, name of the widget",
                                       "Could not open the %1 package required for the %2 widget.",
                                       appletDescription.pluginName(), appletDescription.name()));
        return;
    }

    // now we try and set up the script engine.
    // it will be parented to this applet and so will get
    // deleted when the applet does
    script = Plasma::loadScriptEngine(api, q, args);

    if (!script) {
        delete package;
        package = 0;
        q->setLaunchErrorMessage(
            i18nc("API or programming language the widget was written in, name of the widget",
                  "Could not create a %1 ScriptEngine for the %2 widget.",
                  api, appletDescription.name()));
    }
}

void AppletPrivate::cleanUpAndDelete()
{
    // reimplemented in the UI specific library
    if (configLoader) {
        configLoader->setDefaults();
    }

    resetConfigurationObject();

    if (q->isContainment()) {
        // prematurely emit our destruction if we are a Containment,
        // giving Corona a chance to remove this Containment from its collection
        emit q->QObject::destroyed(q);
    }

    q->deleteLater();
}

void AppletPrivate::showConfigurationRequiredMessage(bool show, const QString &reason)
{
    // reimplemented in the UI specific library
    Q_UNUSED(show)
    Q_UNUSED(reason)
}

void AppletPrivate::askDestroy()
{
    if (q->immutability() != Types::Mutable || transient || !started) {
        return; //don't double delete
    }

    if (q->isContainment()) {
        QMessageBox *box = new QMessageBox(QMessageBox::Warning, i18nc("@title:window %1 is the name of the containment", "Remove %1", q->title()), i18nc("%1 is the name of the containment", "Do you really want to remove this %1?", q->title()), QMessageBox::StandardButtons(QMessageBox::Yes | QMessageBox::No));
        box->setWindowFlags((Qt::WindowFlags)(box->windowFlags() | Qt::WA_DeleteOnClose));
        box->open();

        QObject::connect(q, &Applet::immutabilityChanged, [=] () {
            box->close();
        });
        QObject::connect(q, &QObject::destroyed, [=] () {
            box->close();
        });
        QObject::connect(box->button(QMessageBox::Yes), &QAbstractButton::clicked,
        [ = ]() {
            transient = true;
            cleanUpAndDelete();
        });

        return;
    }

    transient = true;
    cleanUpAndDelete();
}

void AppletPrivate::globalShortcutChanged()
{
    if (!activationAction) {
        return;
    }
    KConfigGroup shortcutConfig(mainConfigGroup(), "Shortcuts");
    QString newShortCut = activationAction->shortcut().toString();
    QString oldShortCut = shortcutConfig.readEntry("global", QString());
    if (newShortCut != oldShortCut) {
        shortcutConfig.writeEntry("global", newShortCut);
        scheduleModificationNotification();
    }
    //qDebug() << "after" << shortcut.primary() << d->activationAction->globalShortcut().primary();
}

KActionCollection *AppletPrivate::defaultActions(QObject *parent)
{
    KActionCollection *actions = new KActionCollection(parent);
    actions->setConfigGroup("Shortcuts-Applet");

    QAction *configAction = actions->add<QAction>("configure");
    configAction->setAutoRepeat(false);
    configAction->setText(i18n("Widget Settings"));
    configAction->setIcon(QIcon::fromTheme("configure"));
    configAction->setShortcut(QKeySequence("alt+d, s"));
    configAction->setData(Plasma::Types::ConfigureAction);

    QAction *closeApplet = actions->add<QAction>("remove");
    closeApplet->setAutoRepeat(false);
    closeApplet->setText(i18n("Remove this Widget"));
    closeApplet->setIcon(QIcon::fromTheme("edit-delete"));
    closeApplet->setShortcut(QKeySequence("alt+d, r"));
    closeApplet->setData(Plasma::Types::DestructiveAction);

    QAction *runAssociatedApplication = actions->add<QAction>("run associated application");
    runAssociatedApplication->setAutoRepeat(false);
    runAssociatedApplication->setText(i18n("Run the Associated Application"));
    runAssociatedApplication->setIcon(QIcon::fromTheme("system-run"));
    runAssociatedApplication->setShortcut(QKeySequence("alt+d, t"));
    runAssociatedApplication->setVisible(false);
    runAssociatedApplication->setEnabled(false);
    runAssociatedApplication->setData(Plasma::Types::ControlAction);

    return actions;
}

void AppletPrivate::requestConfiguration()
{
    if (q->containment()) {
        emit q->containment()->configureRequested(q);
    }
}

void AppletPrivate::updateShortcuts()
{
    if (q->isContainment()) {
        //a horrible hack to avoid clobbering corona settings
        //we pull them out, then read, then put them back
        QList<QString> names;
        QList<QAction *> qactions;
        names << "add sibling containment" << "configure shortcuts" << "lock widgets";
        foreach (const QString &name, names) {
            QAction *a = actions->action(name);
            actions->takeAction(a); //FIXME this is stupid, KActionCollection needs a takeAction(QString) method
            qactions << a;
        }

        actions->readSettings();

        for (int i = 0; i < names.size(); ++i) {
            QAction *a = qactions.at(i);
            if (a) {
                actions->addAction(names.at(i), a);
            }
        }
    } else {
        actions->readSettings();
    }
}

void AppletPrivate::propagateConfigChanged()
{
    Containment *c = qobject_cast<Containment *>(q);
    if (c) {
        c->d->configChanged();
    }
    q->configChanged();
}

void AppletPrivate::setUiReady()
{
    //am i the containment?
    Containment *c = qobject_cast<Containment *>(q);
    if (c && c->isContainment()) {
        c->d->setUiReady();
    } else if (Containment* cc = q->containment()) {
        cc->d->appletLoaded(q);
    }
}

// put all setup routines for script here. at this point we can assume that
// package exists and that we have a script engine
void AppletPrivate::setupPackage()
{
    if (!package) {
        return;
    }

#ifndef NDEBUG
    // qDebug() << "setting up script support, package is in" << package->path()
    //         << ", main script is" << package->filePath("mainscript");
#endif

    // FIXME: Replace with ki18n functionality once semantics is clear.
    // const QString translationsPath = package->filePath("translations");
    // if (!translationsPath.isEmpty()) {
    //     KGlobal::dirs()->addResourceDir("locale", translationsPath);
    // }

    if (!package->filePath("mainconfigui").isEmpty()) {
        q->setHasConfigurationInterface(true);
    }
}

QString AppletPrivate::globalName() const
{
    if (!appletDescription.isValid()) {
        return QString();
    }

    return appletDescription.service()->library();
}

void AppletPrivate::scheduleConstraintsUpdate(Plasma::Types::Constraints c)
{
    // Don't start up a timer if we're just starting up
    // flushPendingConstraints will be called by Corona
    if (started && !constraintsTimer.isActive() && !(c & Plasma::Types::StartupCompletedConstraint)) {
        constraintsTimer.start(0, q);
    }

    if (c & Plasma::Types::StartupCompletedConstraint) {
        started = true;
        if (q->isContainment()) {
            qobject_cast<Containment*>(q)->d->setStarted();
        }
    }

    pendingConstraints |= c;
}

void AppletPrivate::scheduleModificationNotification()
{
    // modificationsTimer is not allocated until we get our notice of being started
    if (modificationsTimer) {
        // schedule a save
        modificationsTimer->start(1000, q);
    }
}

KConfigGroup *AppletPrivate::mainConfigGroup()
{
    if (mainConfig) {
        return mainConfig;
    }

    Containment *c = q->containment();
    Plasma::Applet *parentApplet = 0;
    if (c) {
        parentApplet = qobject_cast<Plasma::Applet *>(c->parent());
    }

    if (q->isContainment()) {
        Corona *corona = static_cast<Containment *>(q)->corona();
        KConfigGroup containmentConfig;
        //qDebug() << "got a corona, baby?" << (QObject*)corona << (QObject*)q;

        if (parentApplet) {
            containmentConfig = parentApplet->config();
            containmentConfig = KConfigGroup(&containmentConfig, "Containments");
        } else if (corona) {
            containmentConfig = KConfigGroup(corona->config(), "Containments");
        } else {
            containmentConfig =  KConfigGroup(KSharedConfig::openConfig(), "Containments");
        }

        mainConfig = new KConfigGroup(&containmentConfig, QString::number(appletId));
    } else {
        KConfigGroup appletConfig;

        if (c) {
            // applet directly in a Containment, as usual
            appletConfig = c->config();
            appletConfig = KConfigGroup(&appletConfig, "Applets");
        } else {
            qWarning() << "requesting config for" << q->title() << "without a containment!";
            appletConfig = KConfigGroup(KSharedConfig::openConfig(), "Applets");
        }

        mainConfig = new KConfigGroup(&appletConfig, QString::number(appletId));
    }

    if (configLoader) {
        configLoader->setSharedConfig(KSharedConfig::openConfig(mainConfig->config()->name()));
        configLoader->load();
    }

    return mainConfig;
}

void AppletPrivate::resetConfigurationObject()
{
    // make sure mainConfigGroup exists in all cases
    mainConfigGroup();
    mainConfig->deleteEntry("plugin");
    mainConfig->deleteEntry("formfactor");
    mainConfig->deleteEntry("immutability");
    mainConfig->deleteEntry("location");
    mainConfig->deleteGroup();
    delete mainConfig;
    mainConfig = 0;

    Containment *cont = qobject_cast<Containment *>(q);

    if (cont && cont->corona()) {
        cont->corona()->requireConfigSync();
    } else {
        if (!q->containment()) {
            return;
        }
        Corona *corona = q->containment()->corona();
        if (corona) {
            corona->requireConfigSync();
        }
    }
}

uint AppletPrivate::s_maxAppletId = 0;

} //namespace Plasma
