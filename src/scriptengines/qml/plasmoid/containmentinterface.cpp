/*
 *   Copyright 2008 Chani Armitage <chani@kde.org>
 *   Copyright 2008, 2009 Aaron Seigo <aseigo@kde.org>
 *   Copyright 2010 Marco Martin <mart@kde.org>
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

#include "containmentinterface.h"
#include "wallpaperinterface.h"
#include <kdeclarative/qmlobject.h>

#include <QClipboard>
#include <QQmlExpression>
#include <QQmlProperty>
#include <QMimeData>

#include <kactioncollection.h>
#include <kauthorized.h>
#include <QDebug>
#include <klocalizedstring.h>
#include <kurlmimedata.h>
#include <QMimeDatabase>

#ifndef PLASMA_NO_KIO
#include "kio/jobclasses.h" // for KIO::JobFlags
#include "kio/job.h"
#include "kio/scheduler.h"
#endif

#include <plasma.h>
#include <Plasma/ContainmentActions>
#include <Plasma/Corona>
#include <Plasma/Package>
#include <Plasma/PluginLoader>

#include <kactivities/info.h>

#include "kdeclarative/configpropertymap.h"
#include <packageurlinterceptor.h>

ContainmentInterface::ContainmentInterface(DeclarativeAppletScript *parent, const QVariantList &args)
    : AppletInterface(parent, args),
      m_wallpaperInterface(0),
      m_activityInfo(0)
{
    m_containment = static_cast<Plasma::Containment *>(appletScript()->applet()->containment());

    setAcceptedMouseButtons(Qt::AllButtons);

    connect(m_containment.data(), &Plasma::Containment::appletRemoved,
            this, &ContainmentInterface::appletRemovedForward);
    connect(m_containment.data(), &Plasma::Containment::appletAdded,
            this, &ContainmentInterface::appletAddedForward);

    if (!m_appletInterfaces.isEmpty()) {
        emit appletsChanged();
    }

    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
            [=]() {
                if (!m_containment) {
                    return;
                }
                disconnect(m_containment.data(), &Plasma::Containment::appletRemoved,
                           this, &ContainmentInterface::appletRemovedForward);
            });
}

void ContainmentInterface::init()
{
    if (qmlObject()->rootObject()) {
        return;
    }

    m_activityInfo = new KActivities::Info(m_containment->activity(), this);
    connect(m_activityInfo, &KActivities::Info::nameChanged,
            this, &ContainmentInterface::activityNameChanged);
    emit activityNameChanged();

    AppletInterface::init();

    //Create the ToolBox
    if (m_containment) {
        KConfigGroup defaults;
        if (m_containment->containmentType() == Plasma::Types::DesktopContainment) {
            defaults = KConfigGroup(KSharedConfig::openConfig(m_containment->corona()->package().filePath("defaults")), "Desktop");
        } else if (m_containment->containmentType() == Plasma::Types::PanelContainment) {
            defaults = KConfigGroup(KSharedConfig::openConfig(m_containment->corona()->package().filePath("defaults")), "Panel");
        }

        Plasma::Package pkg = Plasma::PluginLoader::self()->loadPackage("Plasma/Generic");

        if (defaults.isValid()) {
            pkg.setPath(defaults.readEntry("ToolBox", "org.kde.desktoptoolbox"));
        } else {
            pkg.setPath("org.kde.desktoptoolbox");
        }

        PlasmaQuick::PackageUrlInterceptor *interceptor = dynamic_cast<PlasmaQuick::PackageUrlInterceptor *>(qmlObject()->engine()->urlInterceptor());
        if (interceptor) {
            interceptor->addAllowedPath(pkg.path());
        }

        if (pkg.isValid()) {
            QObject *containmentGraphicObject = qmlObject()->rootObject();

            QVariantHash toolboxProperties;
            toolboxProperties["parent"] = QVariant::fromValue(this);
            QObject *toolBoxObject = qmlObject()->createObjectFromSource(QUrl::fromLocalFile(pkg.filePath("mainscript")), 0, toolboxProperties);
            if (toolBoxObject && containmentGraphicObject) {
                containmentGraphicObject->setProperty("toolBox", QVariant::fromValue(toolBoxObject));
            }
        } else {
            qWarning() << "Could not load toolbox package." << pkg.path();
        }
    }

    //set parent, both as object hierarchically and visually
    //do this only for containments, applets will do it in compactrepresentationcheck
    if (qmlObject()->rootObject()) {
        qmlObject()->rootObject()->setProperty("parent", QVariant::fromValue(this));

        //set anchors
        QQmlExpression expr(qmlObject()->engine()->rootContext(), qmlObject()->rootObject(), "parent");
        QQmlProperty prop(qmlObject()->rootObject(), "anchors.fill");
        prop.write(expr.evaluate());
    }

    if (!m_containment->wallpaper().isEmpty()) {
        loadWallpaper();
    }

    connect(m_containment.data(), &Plasma::Containment::activityChanged,
            this, &ContainmentInterface::activityChanged);
    connect(m_containment.data(), &Plasma::Containment::activityChanged,
            [ = ]() {
                delete m_activityInfo;
                m_activityInfo = new KActivities::Info(m_containment->activity(), this);
                connect(m_activityInfo, &KActivities::Info::nameChanged,
                        this, &ContainmentInterface::activityNameChanged);
                emit activityNameChanged();
            });
    connect(m_containment.data(), &Plasma::Containment::wallpaperChanged,
            this, &ContainmentInterface::loadWallpaper);
    connect(m_containment.data(), &Plasma::Containment::containmentTypeChanged,
            this, &ContainmentInterface::containmentTypeChanged);

    if (m_containment->corona()) {
        connect(m_containment->corona(), &Plasma::Corona::availableScreenRegionChanged,
                this, &ContainmentInterface::availableScreenRegionChanged);
        connect(m_containment->corona(), &Plasma::Corona::availableScreenRectChanged,
                this, &ContainmentInterface::availableScreenRectChanged);
    }
}

QList <QObject *> ContainmentInterface::applets()
{
    return m_appletInterfaces;
}

Plasma::Types::ContainmentType ContainmentInterface::containmentType() const
{
    return appletScript()->containmentType();
}

void ContainmentInterface::setContainmentType(Plasma::Types::ContainmentType type)
{
    appletScript()->setContainmentType(type);
}

QVariantList ContainmentInterface::availableScreenRegion() const
{
    QRegion reg = QRect(0, 0, width(), height());
    int screenId = screen();
    if (screenId > -1 && m_containment->corona()) {
        reg = m_containment->corona()->availableScreenRegion(screenId);
    }

    QVariantList regVal;
    foreach (QRect rect, reg.rects()) {
        //make it relative
        QRect geometry = m_containment->corona()->screenGeometry(screenId);
        rect.moveTo(rect.topLeft() - geometry.topLeft());
        regVal << QVariant::fromValue(QRectF(rect));
    }
    return regVal;
}

QRect ContainmentInterface::availableScreenRect() const
{
    QRect rect(0, 0, width(), height());

    int screenId = screen();

    if (screenId > -1 && m_containment->corona()) {
        rect = m_containment->corona()->availableScreenRect(screenId);
        //make it relative
        QRect geometry = m_containment->corona()->screenGeometry(screenId);
        rect.moveTo(rect.topLeft() - geometry.topLeft());
    }

    return rect;
}

Plasma::Applet *ContainmentInterface::createApplet(const QString &plugin, const QVariantList &args, const QPoint &pos)
{
    //HACK
    //This is necessary to delay the appletAdded signal (of containmentInterface) AFTER the applet graphics object has been created
    blockSignals(true);
    Plasma::Applet *applet = m_containment->createApplet(plugin, args);

    if (applet) {
        QObject *appletGraphicObject = applet->property("_plasma_graphicObject").value<QObject *>();

        blockSignals(false);

        emit appletAdded(appletGraphicObject, pos.x(), pos.y());
        emit appletsChanged();
    } else {
        blockSignals(false);
    }
    return applet;
}

void ContainmentInterface::setAppletArgs(Plasma::Applet *applet, const QString &mimetype, const QString &data)
{
    if (!applet) {
        return;
    }

    AppletInterface *appletInterface = applet->property("_plasma_graphicObject").value<AppletInterface *>();
    if (appletInterface) {
        emit appletInterface->externalData(mimetype, data);
    }
}

QObject *ContainmentInterface::containmentAt(int x, int y)
{
    foreach (Plasma::Containment *c, m_containment->corona()->containments()) {
        ContainmentInterface *contInterface = c->property("_plasma_graphicObject").value<ContainmentInterface *>();

        if (contInterface && contInterface->isVisible()) {
            QWindow *w = contInterface->window();
            if (w && w->geometry().contains(QPoint(window()->x(), window()->y()) + QPoint(x, y))) {
                return contInterface;
            }
        }
    }
    return 0;
}

void ContainmentInterface::addApplet(AppletInterface *applet, int x, int y)
{
    if (!applet || applet->applet()->containment() == m_containment) {
        return;
    }

    blockSignals(true);
    m_containment->addApplet(applet->applet());
    blockSignals(false);
    emit appletAdded(applet, x, y);
}

QPointF ContainmentInterface::mapFromApplet(AppletInterface *applet, int x, int y)
{
    if (!applet->window() || !window()) {
        return QPointF();
    }

    //x,y in absolute screen coordinates of current view
    QPointF pos = applet->mapToScene(QPointF(x, y));
    pos = QPointF(pos + applet->window()->geometry().topLeft());
    //return the coordinate in the relative view's coords
    return pos - window()->geometry().topLeft();
}

QPointF ContainmentInterface::mapToApplet(AppletInterface *applet, int x, int y)
{
    if (!applet->window() || !window()) {
        return QPointF();
    }

    //x,y in absolute screen coordinates of current view
    QPointF pos(x, y);
    pos = QPointF(pos + window()->geometry().topLeft());
    //the coordinate in the relative view's coords
    pos = pos - applet->window()->geometry().topLeft();
    //make it relative to applet coords
    return pos - applet->mapToScene(QPointF(0, 0));
}

void ContainmentInterface::processMimeData(QMimeData *mimeData, int x, int y)
{
    if (!mimeData) {
        return;
    }

    //const QMimeData *mimeData = data;

    qDebug() << "Arrived mimeData" << mimeData->urls() << mimeData->formats() << "at" << x << ", " << y;

    if (mimeData->hasFormat("text/x-plasmoidservicename")) {
        QString data = mimeData->data("text/x-plasmoidservicename");
        const QStringList appletNames = data.split('\n', QString::SkipEmptyParts);
        foreach (const QString &appletName, appletNames) {
            qDebug() << "adding" << appletName;

            metaObject()->invokeMethod(this, "createApplet", Qt::QueuedConnection, Q_ARG(QString, appletName), Q_ARG(QVariantList, QVariantList()), Q_ARG(QPoint, QPoint(x, y)));
        }
    } else if (mimeData->hasUrls()) {
        //TODO: collect the mimetypes of available script engines and offer
        //      to create widgets out of the matching URLs, if any
        const QList<QUrl> urls = KUrlMimeData::urlsFromMimeData(mimeData);
        foreach (const QUrl &url, urls) {

#ifndef PLASMA_NO_KIO
            QMimeDatabase db;
            const QMimeType &mime = db.mimeTypeForUrl(url);
            QString mimeName = mime.name();
            QVariantList args;
            args << url.url();
            qDebug() << "can decode" << mimeName << args;

            // It may be a directory or a file, let's stat
            KIO::JobFlags flags = KIO::HideProgressInfo;
            KIO::MimetypeJob *job = KIO::mimetype(url, flags);
            m_dropPoints[job] = QPoint(x, y);

            QObject::connect(job, SIGNAL(result(KJob*)), this, SLOT(dropJobResult(KJob*)));
            QObject::connect(job, SIGNAL(mimetype(KIO::Job*,QString)),
                             this, SLOT(mimeTypeRetrieved(KIO::Job*,QString)));

            QMenu *choices = new QMenu("Content dropped");
            choices->addAction(QIcon::fromTheme("process-working"), i18n("Fetching file type..."));
            choices->popup(window() ? window()->mapToGlobal(QPoint(x, y)) : QPoint(x, y));

            m_dropMenus[job] = choices;
#endif
        }

    } else {
        QStringList formats = mimeData->formats();
        QHash<QString, KPluginInfo> seenPlugins;
        QHash<QString, QString> pluginFormats;

        foreach (const QString &format, formats) {
            KPluginInfo::List plugins = Plasma::PluginLoader::self()->listAppletInfoForMimeType(format);

            foreach (const KPluginInfo &plugin, plugins) {
                if (seenPlugins.contains(plugin.pluginName())) {
                    continue;
                }

                seenPlugins.insert(plugin.pluginName(), plugin);
                pluginFormats.insert(plugin.pluginName(), format);
            }
        }
        //qDebug() << "Mimetype ..." << formats << seenPlugins.keys() << pluginFormats.values();

        QString selectedPlugin;

        if (seenPlugins.isEmpty()) {
            // do nothing
        } else if (seenPlugins.count() == 1) {
            selectedPlugin = seenPlugins.constBegin().key();
        } else {
            QMenu choices;
            QHash<QAction *, QString> actionsToPlugins;
            foreach (const KPluginInfo &info, seenPlugins) {
                QAction *action;
                if (!info.icon().isEmpty()) {
                    action = choices.addAction(QIcon::fromTheme(info.icon()), info.name());
                } else {
                    action = choices.addAction(info.name());
                }

                actionsToPlugins.insert(action, info.pluginName());
            }

            QAction *choice = choices.exec(window() ? window()->mapToGlobal(QPoint(x, y)) : QPoint(x, y));
            if (choice) {
                selectedPlugin = actionsToPlugins[choice];
            }
        }

        if (!selectedPlugin.isEmpty()) {

            Plasma::Applet *applet = createApplet(selectedPlugin, QVariantList(), QPoint(x, y));
            setAppletArgs(applet, pluginFormats[selectedPlugin], mimeData->data(pluginFormats[selectedPlugin]));

        }
    }
}

void ContainmentInterface::clearDataForMimeJob(KIO::Job *job)
{
#ifndef PLASMA_NO_KIO
    QObject::disconnect(job, 0, this, 0);
    m_dropPoints.remove(job);
    QMenu *choices = m_dropMenus.take(job);
    delete choices;
    job->kill();
#endif // PLASMA_NO_KIO
}

void ContainmentInterface::dropJobResult(KJob *job)
{
#ifndef PLASMA_NO_KIO
    KIO::TransferJob *tjob = dynamic_cast<KIO::TransferJob *>(job);
    if (!tjob) {
        qDebug() << "job is not a KIO::TransferJob, won't handle the drop...";
        clearDataForMimeJob(tjob);
        return;
    }
    if (job->error()) {
        qDebug() << "ERROR" << tjob->error() << ' ' << tjob->errorString();
    }
    // We call mimetypeRetrieved since there might be other mechanisms
    // for finding suitable applets. Cleanup happens there as well.
    mimeTypeRetrieved(qobject_cast<KIO::Job *>(job), QString());
#endif // PLASMA_NO_KIO
}

void ContainmentInterface::mimeTypeRetrieved(KIO::Job *job, const QString &mimetype)
{
#ifndef PLASMA_NO_KIO
    qDebug() << "Mimetype Job returns." << mimetype;
    KIO::TransferJob *tjob = dynamic_cast<KIO::TransferJob *>(job);
    if (!tjob) {
        qDebug() << "job should be a TransferJob, but isn't";
        clearDataForMimeJob(job);
        return;
    }
    KPluginInfo::List appletList = Plasma::PluginLoader::self()->listAppletInfoForUrl(tjob->url());
    if (mimetype.isEmpty() && !appletList.count()) {
        clearDataForMimeJob(job);
        qDebug() << "No applets found matching the url (" << tjob->url() << ") or the mimetype (" << mimetype << ")";
        return;
    } else {

        QPoint posi; // will be overwritten with the event's position
        if (m_dropPoints.keys().contains(tjob)) {
            posi = m_dropPoints[tjob];
            qDebug() << "Received a suitable dropEvent at" << posi;
        } else {
            qDebug() << "Bailing out. Cannot find associated dropEvent related to the TransferJob";
            clearDataForMimeJob(job);
            return;
        }

        QMenu *choices = m_dropMenus.value(tjob);
        if (!choices) {
            qDebug() << "Bailing out. No QMenu found for this job.";
            clearDataForMimeJob(job);
            return;
        }

        qDebug() << "Creating menu for:" << mimetype  << posi;

        appletList << Plasma::PluginLoader::self()->listAppletInfoForMimeType(mimetype);
        KPluginInfo::List wallpaperList;

        if (m_wallpaperInterface && m_wallpaperInterface->supportsMimetype(mimetype)) {
            wallpaperList << m_wallpaperInterface->package().metadata();
        } else {
            wallpaperList = WallpaperInterface::listWallpaperInfoForMimetype(mimetype);
        }

        if (!appletList.isEmpty() || !wallpaperList.isEmpty()) {
            choices->clear();
            QHash<QAction *, QString> actionsToApplets;
            choices->addSection(i18n("Widgets"));
            foreach (const KPluginInfo &info, appletList) {
                qDebug() << info.name();
                QAction *action;
                if (!info.icon().isEmpty()) {
                    action = choices->addAction(QIcon::fromTheme(info.icon()), info.name());
                } else {
                    action = choices->addAction(info.name());
                }

                actionsToApplets.insert(action, info.pluginName());
                qDebug() << info.pluginName();
            }
            actionsToApplets.insert(choices->addAction(i18n("Icon")), "org.kde.plasma.icon");

            QHash<QAction *, QString> actionsToWallpapers;
            if (!wallpaperList.isEmpty())  {
                choices->addSection(i18n("Wallpaper"));

                QMap<QString, KPluginInfo> sorted;
                foreach (const KPluginInfo &info, appletList) {
                    sorted.insert(info.name(), info);
                }

                foreach (const KPluginInfo &info, wallpaperList) {
                    QAction *action;
                    if (!info.icon().isEmpty()) {
                        action = choices->addAction(QIcon::fromTheme(info.icon()), info.name());
                    } else {
                        action = choices->addAction(info.name());
                    }

                    actionsToWallpapers.insert(action, info.pluginName());
                }
            }

            QAction *choice = choices->exec();
            if (choice) {
                // Put the job on hold so it can be recycled to fetch the actual content,
                // which is to be expected when something's dropped onto the desktop and
                // an applet is to be created with this URL
                if (!mimetype.isEmpty() && !tjob->error()) {
                    tjob->putOnHold();
                    KIO::Scheduler::publishSlaveOnHold();
                }
                QString plugin = actionsToApplets.value(choice);
                if (plugin.isEmpty()) {
                    //set wallpapery stuff
                    plugin = actionsToWallpapers.value(choice);
                    if (m_wallpaperInterface && tjob->url().isValid()) {
                        m_wallpaperInterface->setUrl(tjob->url());
                    }
                } else {
                    Plasma::Applet *applet = createApplet(actionsToApplets[choice], QVariantList(), posi);
                    setAppletArgs(applet, mimetype, tjob->url().toString());
                }

                clearDataForMimeJob(job);
                return;
            }
        } else {
            // we can at least create an icon as a link to the URL
            Plasma::Applet *applet = createApplet("org.kde.plasma.icon", QVariantList(), posi);
            setAppletArgs(applet, mimetype, tjob->url().toString());
        }
    }

    clearDataForMimeJob(job);
#endif // PLASMA_NO_KIO
}

void ContainmentInterface::appletAddedForward(Plasma::Applet *applet)
{
    if (!applet) {
        return;
    }

    QObject *appletGraphicObject = applet->property("_plasma_graphicObject").value<QObject *>();
    QObject *contGraphicObject = m_containment->property("_plasma_graphicObject").value<QObject *>();

//     qDebug() << "Applet added on containment:" << m_containment->title() << contGraphicObject
//              << "Applet: " << applet << applet->title() << appletGraphicObject;

    if (!appletGraphicObject) {
        appletGraphicObject = new AppletInterface(applet, QVariantList(), this);
        applet->setProperty("_plasma_graphicObject", QVariant::fromValue(appletGraphicObject));
        static_cast<AppletInterface *>(appletGraphicObject)->init();
    }

    if (contGraphicObject) {
        appletGraphicObject->setProperty("visible", false);
        appletGraphicObject->setProperty("parent", QVariant::fromValue(contGraphicObject));
    }

    m_appletInterfaces << appletGraphicObject;
    emit appletAdded(appletGraphicObject, -1, -1);
    emit appletsChanged();
}

void ContainmentInterface::appletRemovedForward(Plasma::Applet *applet)
{
    QObject *appletGraphicObject = applet->property("_plasma_graphicObject").value<QObject *>();
    m_appletInterfaces.removeAll(appletGraphicObject);
    emit appletRemoved(appletGraphicObject);
    emit appletsChanged();
}

void ContainmentInterface::loadWallpaper()
{
    if (m_containment->containmentType() != Plasma::Types::DesktopContainment &&
            m_containment->containmentType() != Plasma::Types::CustomContainment) {
        return;
    }

    if (!m_containment->wallpaper().isEmpty()) {
        delete m_wallpaperInterface;

        m_wallpaperInterface = new WallpaperInterface(this);
        m_wallpaperInterface->setZ(-1000);
        //Qml seems happier if the parent gets set in this way
        m_wallpaperInterface->setProperty("parent", QVariant::fromValue(this));

        //set anchors
        QQmlExpression expr(qmlObject()->engine()->rootContext(), m_wallpaperInterface, "parent");
        QQmlProperty prop(m_wallpaperInterface, "anchors.fill");
        prop.write(expr.evaluate());

        m_containment->setProperty("wallpaperGraphicsObject", QVariant::fromValue(m_wallpaperInterface));
    } else {
        if (m_wallpaperInterface) {
            m_wallpaperInterface->deleteLater();
            m_wallpaperInterface = 0;
        }
    }
}

QString ContainmentInterface::activity() const
{
    return m_containment->activity();
}

QString ContainmentInterface::activityName() const
{
    if (!m_activityInfo) {
        return QString();
    }
    return m_activityInfo->name();
}

QList<QObject *> ContainmentInterface::actions() const
{
    //FIXME: giving directly a QList<QAction*> crashes

    //use a multimap to sort by action type
    QMultiMap<int, QObject *> actions;
    foreach (QAction *a, m_containment->actions()->actions()) {
        actions.insert(a->data().toInt(), a);
    }
    foreach (QAction *a, m_containment->corona()->actions()->actions()) {
        if (a->objectName() == QStringLiteral("lock widgets")) {
            //It is up to the Containment to decide if the user is allowed or not
            //to lock/unluck the widgets, so corona should not add one when there is none
            //(user is not allow) and it shouldn't add another one when there is already
            //one
            continue;
        }
        actions.insert(a->data().toInt(), a);
    }
    return actions.values();
}

//PROTECTED--------------------

void ContainmentInterface::mousePressEvent(QMouseEvent *event)
{
    event->setAccepted(m_containment->containmentActions().contains(Plasma::ContainmentActions::eventToString(event)));
}

void ContainmentInterface::mouseReleaseEvent(QMouseEvent *event)
{
    //even if the menu is executed synchronously, other events may be processed
    //by the qml incubator when plasma is loading, so we need to guard there
    if (m_contextMenu) {
        m_contextMenu.data()->close();
        return;
    }

    const QString trigger = Plasma::ContainmentActions::eventToString(event);
    Plasma::ContainmentActions *plugin = m_containment->containmentActions().value(trigger);

    if (!plugin || plugin->contextualActions().isEmpty()) {
        event->setAccepted(false);
        return;
    }

    //the plugin can be a single action or a context menu
    //Don't have an action list? execute as single action
    //and set the event position as action data
    if (plugin->contextualActions().length() == 1) {
        QAction *action = plugin->contextualActions().first();
        action->setData(event->pos());
        action->trigger();
        event->accept();
        return;
    }

    //FIXME: very inefficient appletAt() implementation
    Plasma::Applet *applet = 0;
    foreach (QObject *appletObject, m_appletInterfaces) {
        if (AppletInterface *ai = qobject_cast<AppletInterface *>(appletObject)) {
            if (ai->contains(ai->mapFromItem(this, event->posF()))) {
                applet = ai->applet();
                break;
            } else {
                ai = 0;
            }
        }
    }
    //qDebug() << "Invoking menu for applet" << applet;

    QMenu desktopMenu;

    m_contextMenu = &desktopMenu;

    if (applet) {
        addAppletActions(desktopMenu, applet, event);
    } else {
        addContainmentActions(desktopMenu, event);
    }

    //this is a workaround where Qt now creates the menu widget
    //in .exec before oxygen can polish it and set the following attribute
    desktopMenu.setAttribute(Qt::WA_TranslucentBackground);
    //end workaround

    desktopMenu.exec(event->globalPos());
    event->accept();
}

void ContainmentInterface::wheelEvent(QWheelEvent *event)
{
    const QString trigger = Plasma::ContainmentActions::eventToString(event);
    Plasma::ContainmentActions *plugin = m_containment->containmentActions().value(trigger);

    if (plugin) {
        if (event->delta() < 0) {
            plugin->performNextAction();
        } else {
            plugin->performPreviousAction();
        }
    } else {
        event->setAccepted(false);
    }
}

void ContainmentInterface::addAppletActions(QMenu &desktopMenu, Plasma::Applet *applet, QEvent *event)
{
    foreach (QAction *action, applet->contextualActions()) {
        if (action) {
            desktopMenu.addAction(action);
        }
    }

    if (!applet->failedToLaunch()) {
        QAction *configureApplet = applet->actions()->action("configure");
        if (configureApplet && configureApplet->isEnabled()) {
            desktopMenu.addAction(configureApplet);
        }

        QAction *runAssociatedApplication = applet->actions()->action("run associated application");
        if (runAssociatedApplication && runAssociatedApplication->isEnabled()) {
            desktopMenu.addAction(runAssociatedApplication);
        }
    }

    QMenu *containmentMenu = new QMenu(i18nc("%1 is the name of the containment", "%1 Options", m_containment->title()), &desktopMenu);
    addContainmentActions(*containmentMenu, event);

    if (!containmentMenu->isEmpty()) {
        int enabled = 0;
        //count number of real actions
        QListIterator<QAction *> actionsIt(containmentMenu->actions());
        while (enabled < 3 && actionsIt.hasNext()) {
            QAction *action = actionsIt.next();
            if (action->isVisible() && !action->isSeparator()) {
                ++enabled;
            }
        }

        if (enabled) {
            //if there is only one, don't create a submenu
            if (enabled < 2) {
                foreach (QAction *action, containmentMenu->actions()) {
                    if (action->isVisible() && !action->isSeparator()) {
                        desktopMenu.addAction(action);
                    }
                }
            } else {
                desktopMenu.addMenu(containmentMenu);
            }
        }
    }

    if (m_containment->immutability() == Plasma::Types::Mutable &&
        (m_containment->containmentType() != Plasma::Types::PanelContainment || m_containment->isUserConfiguring())) {
        QAction *closeApplet = applet->actions()->action("remove");
        //qDebug() << "checking for removal" << closeApplet;
        if (closeApplet) {
            if (!desktopMenu.isEmpty()) {
                desktopMenu.addSeparator();
            }

            //qDebug() << "adding close action" << closeApplet->isEnabled() << closeApplet->isVisible();
            desktopMenu.addAction(closeApplet);
        }
    }
}

void ContainmentInterface::addContainmentActions(QMenu &desktopMenu, QEvent *event)
{
    if (m_containment->corona()->immutability() != Plasma::Types::Mutable &&
            !KAuthorized::authorizeKAction("plasma/containment_actions")) {
        //qDebug() << "immutability";
        return;
    }

    //this is what ContainmentPrivate::prepareContainmentActions was
    const QString trigger = Plasma::ContainmentActions::eventToString(event);
    Plasma::ContainmentActions *plugin = m_containment->containmentActions().value(trigger);

    if (!plugin) {
        return;
    }

    if (plugin->containment() != m_containment) {
        plugin->setContainment(m_containment);

        // now configure it
        KConfigGroup cfg(m_containment->corona()->config(), "ActionPlugins");
        cfg = KConfigGroup(&cfg, QString::number(m_containment->containmentType()));
        KConfigGroup pluginConfig = KConfigGroup(&cfg, trigger);
        plugin->restore(pluginConfig);
    }

    QList<QAction *> actions = plugin->contextualActions();

    if (actions.isEmpty()) {
        //it probably didn't bother implementing the function. give the user a chance to set
        //a better plugin.  note that if the user sets no-plugin this won't happen...
        if ((m_containment->containmentType() != Plasma::Types::PanelContainment &&
                m_containment->containmentType() != Plasma::Types::CustomPanelContainment) &&
                m_containment->actions()->action("configure")) {
            desktopMenu.addAction(m_containment->actions()->action("configure"));
        }
    } else {
        desktopMenu.addActions(actions);
    }

    return;
}

#include "moc_containmentinterface.cpp"
