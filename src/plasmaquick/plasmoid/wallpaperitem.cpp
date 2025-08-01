/*
    SPDX-FileCopyrightText: 2013 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "wallpaperitem.h"
#include "appletcontext_p.h"

#include "containmentitem.h"
#include "sharedqmlengine.h"

#include <KConfigLoader>
#include <KConfigPropertyMap>
#include <KDesktopFile>

#include <QDebug>
#include <QFile>
#include <QLoggingCategory>
#include <QQmlContext>
#include <QQmlExpression>
#include <QQmlProperty>

#include <Plasma/Corona>
#include <Plasma/PluginLoader>
#include <kpackage/packageloader.h>
#include <qabstractitemmodel.h>
#include <qtmetamacros.h>

#include "debug_p.h"

WallpaperItem::WallpaperItem(QQuickItem *parent)
    : QQuickItem(parent)
{
    // resize at the beginning to avoid as much resize events as possible
    if (parent) {
        setSize(QSizeF(parent->width(), parent->height()));
    }
}

WallpaperItem::~WallpaperItem()
{
}

void WallpaperItem::classBegin()
{
    QQuickItem::classBegin();
    PlasmaQuick::AppletContext *ac = qobject_cast<PlasmaQuick::AppletContext *>(QQmlEngine::contextForObject(this)->parentContext());

    // if there is no applet context we are running inside the screen locker
    if (ac) {
        m_containment = ac->applet()->containment();
        m_wallpaperPlugin = m_containment->wallpaperPlugin();

        m_pkg = KPackage::PackageLoader::self()->loadPackage(QStringLiteral("Plasma/Wallpaper"));
        m_pkg.setPath(m_wallpaperPlugin);

        connect(m_containment->corona(), &Plasma::Corona::startupCompleted, this, &WallpaperItem::accentColorChanged);
    }
}

void WallpaperItem::componentComplete()
{
    QQuickItem::componentComplete();

    m_loading = false;
    Q_EMIT isLoadingChanged();
}

QList<KPluginMetaData> WallpaperItem::listWallpaperMetadataForMimetype(const QString &mimetype, const QString &formFactor)
{
    auto filter = [&mimetype, &formFactor](const KPluginMetaData &md) -> bool {
        if (!formFactor.isEmpty() && !md.value(u"X-Plasma-FormFactors").contains(formFactor)) {
            return false;
        }
        return md.value(u"X-Plasma-DropMimeTypes", QStringList()).contains(mimetype);
    };
    return KPackage::PackageLoader::self()->findPackages(QStringLiteral("Plasma/Wallpaper"), QString(), filter);
}

KPackage::Package WallpaperItem::kPackage() const
{
    return m_pkg;
}

QString WallpaperItem::pluginName() const
{
    return m_wallpaperPlugin;
}

void WallpaperItem::setPluginName(const QString &pluginName)
{
    m_wallpaperPlugin = pluginName;
    Q_EMIT pluginNameChanged();
}

KConfigPropertyMap *WallpaperItem::configuration() const
{
    return m_configuration;
}

void WallpaperItem::requestOpenUrl(const QUrl &url)
{
    Q_EMIT openUrlRequested(url);
}

WallpaperItem *WallpaperItem::loadWallpaper(ContainmentItem *containmentItem)
{
    if (containmentItem->containment()->wallpaperPlugin().isEmpty()) {
        return nullptr;
    }
    KPackage::Package pkg = KPackage::PackageLoader::self()->loadPackage(QStringLiteral("Plasma/Wallpaper"));
    pkg.setPath(containmentItem->containment()->wallpaperPlugin());
    if (!pkg.isValid()) {
        qCWarning(LOG_PLASMAQUICK) << "Error loading the wallpaper, no valid package loaded";
        return nullptr;
    }

    PlasmaQuick::SharedQmlEngine *qmlObject = new PlasmaQuick::SharedQmlEngine(containmentItem->containment());
    qmlObject->setInitializationDelayed(true);

    const QString rootPath = pkg.metadata().value(u"X-Plasma-RootPath");
    if (!rootPath.isEmpty()) {
        qmlObject->setTranslationDomain(QLatin1String("plasma_wallpaper_") + rootPath);
    } else {
        qmlObject->setTranslationDomain(QLatin1String("plasma_wallpaper_") + pkg.metadata().pluginId());
    }

    /*
     * The initialization is delayed, so it's fine to setSource first.
     * This also prevents many undefined wallpaper warnings caused by "wallpaper" being set
     * when the old wallpaper plugin still exists.
     */
    qmlObject->setSource(pkg.fileUrl("mainscript"));
    WallpaperItem *wallpaper = qobject_cast<WallpaperItem *>(qmlObject->rootObject());
    if (!wallpaper) {
        if (qmlObject->mainComponent() && qmlObject->mainComponent()->isError()) {
            qCWarning(LOG_PLASMAQUICK) << "Error loading the wallpaper" << qmlObject->mainComponent()->errors();
        } else if (qmlObject->rootObject()) {
            qCWarning(LOG_PLASMAQUICK) << "Root item of wallpaper" << containmentItem->containment()->wallpaperPlugin()
                                       << "not a WallpaperItem instance, instead is" << qmlObject->rootObject();
        }
        qmlObject->completeInitialization();
        delete qmlObject->rootObject();
        return nullptr;
    }

    if (!qEnvironmentVariableIntValue("PLASMA_NO_CONTEXTPROPERTIES")) {
        qmlObject->rootContext()->setContextProperty(QStringLiteral("wallpaper"), wallpaper);
    }

    // FIXME: do we need "mainconfigxml" in wallpaper packagestructures?
    const QString xmlPath = pkg.filePath("config", QStringLiteral("main.xml"));

    KConfigGroup cfg = containmentItem->containment()->config();
    cfg = KConfigGroup(&cfg, QStringLiteral("Wallpaper"));
    cfg = KConfigGroup(&cfg, containmentItem->containment()->wallpaperPlugin());

    KConfigLoader *m_configLoader;
    if (xmlPath.isEmpty()) {
        m_configLoader = new KConfigLoader(cfg, nullptr, wallpaper);
    } else {
        QFile file(xmlPath);
        m_configLoader = new KConfigLoader(cfg, &file, wallpaper);
    }

    auto config = new KConfigPropertyMap(m_configLoader, wallpaper);

    // initialize with our size to avoid as much resize events as possible
    QVariantHash props;
    props[QStringLiteral("parent")] = QVariant::fromValue(containmentItem);
    props[QStringLiteral("width")] = containmentItem->width();
    props[QStringLiteral("height")] = containmentItem->height();
    props[QStringLiteral("configuration")] = QVariant::fromValue(config);
    props[QStringLiteral("pluginName")] = containmentItem->containment()->wallpaperPlugin();
    qmlObject->completeInitialization(props);
    return wallpaper;
}

QList<QAction *> WallpaperItem::contextualActions() const
{
    return m_contextualActions;
}

QQmlListProperty<QAction> WallpaperItem::qmlContextualActions()
{
    return QQmlListProperty<QAction>(this,
                                     nullptr,
                                     WallpaperItem::contextualActions_append,
                                     WallpaperItem::contextualActions_count,
                                     WallpaperItem::contextualActions_at,
                                     WallpaperItem::contextualActions_clear,
                                     WallpaperItem::contextualActions_replace,
                                     WallpaperItem::contextualActions_removeLast);
}

bool WallpaperItem::supportsMimetype(const QString &mimetype) const
{
    return m_pkg.metadata().value(u"X-Plasma-DropMimeTypes", QStringList()).contains(mimetype);
}

bool WallpaperItem::isLoading() const
{
    return m_loading;
}

QColor WallpaperItem::accentColor() const
{
    return m_accentColor.value_or(QColor(Qt::transparent));
}

void WallpaperItem::setAccentColor(const QColor &newColor)
{
    if (m_accentColor.has_value() && m_accentColor == newColor) {
        return;
    }

    m_accentColor = newColor;
    Q_EMIT accentColorChanged();
}

void WallpaperItem::resetAccentColor()
{
    if (!m_accentColor.has_value()) {
        return;
    }

    m_accentColor.reset();
    Q_EMIT accentColorChanged();
}

void WallpaperItem::contextualActions_append(QQmlListProperty<QAction> *prop, QAction *action)
{
    WallpaperItem *w = static_cast<WallpaperItem *>(prop->object);
    w->m_contextualActions.append(action);
    QObject::connect(action, &QObject::destroyed, w, [w, action]() {
        w->m_contextualActions.removeAll(action);
        Q_EMIT w->contextualActionsChanged(w->m_contextualActions);
    });
    Q_EMIT w->contextualActionsChanged(w->m_contextualActions);
};

qsizetype WallpaperItem::contextualActions_count(QQmlListProperty<QAction> *prop)
{
    WallpaperItem *w = static_cast<WallpaperItem *>(prop->object);
    return w->m_contextualActions.count();
}

QAction *WallpaperItem::contextualActions_at(QQmlListProperty<QAction> *prop, qsizetype idx)
{
    WallpaperItem *w = static_cast<WallpaperItem *>(prop->object);
    return w->m_contextualActions.value(idx);
}

void WallpaperItem::contextualActions_clear(QQmlListProperty<QAction> *prop)
{
    WallpaperItem *w = static_cast<WallpaperItem *>(prop->object);
    w->m_contextualActions.clear();
    Q_EMIT w->contextualActionsChanged(w->m_contextualActions);
}

void WallpaperItem::contextualActions_replace(QQmlListProperty<QAction> *prop, qsizetype idx, QAction *action)
{
    WallpaperItem *w = static_cast<WallpaperItem *>(prop->object);
    w->m_contextualActions.replace(idx, action);
    Q_EMIT w->contextualActionsChanged(w->m_contextualActions);
}

void WallpaperItem::contextualActions_removeLast(QQmlListProperty<QAction> *prop)
{
    WallpaperItem *w = static_cast<WallpaperItem *>(prop->object);
    w->m_contextualActions.pop_back();
    Q_EMIT w->contextualActionsChanged(w->m_contextualActions);
}

void WallpaperItem::setConfiguration(KConfigPropertyMap *config)
{
    m_configuration = config;
    Q_EMIT configurationChanged();
}

#include "moc_wallpaperitem.cpp"
