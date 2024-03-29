/*
    SPDX-FileCopyrightText: 2010 Ryan Rix <ry@n.rix.si>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "pluginloader.h"

#include <QGuiApplication>
#include <QPluginLoader>
#include <QPointer>
#include <QStandardPaths>

#include <KLazyLocalizedString>
#include <KRuntimePlatform>
#include <QDebug>
#include <QRegularExpression>
#include <kcoreaddons_export.h>
#include <kpackage/packageloader.h>

#include "config-plasma.h"

#include "applet.h"
#include "containment.h"
#include "containmentactions.h"
#include "debug_p.h"
#include "private/applet_p.h"

namespace Plasma
{
inline bool isContainmentMetaData(const KPluginMetaData &md)
{
    return md.rawData().contains(QStringLiteral("X-Plasma-ContainmentType"));
}

class PluginLoaderPrivate
{
public:
    PluginLoaderPrivate()
    {
    }

    static QString s_plasmoidsPluginDir;
    static QString s_containmentActionsPluginDir;
};

QString PluginLoaderPrivate::s_plasmoidsPluginDir = QStringLiteral("plasma/applets");
QString PluginLoaderPrivate::s_containmentActionsPluginDir = QStringLiteral("plasma/containmentactions");

PluginLoader::PluginLoader()
    : d(new PluginLoaderPrivate)
{
}

PluginLoader::~PluginLoader()
{
    delete d;
}

PluginLoader *PluginLoader::self()
{
    static PluginLoader self;
    return &self;
}

Applet *PluginLoader::loadApplet(const QString &name, uint appletId, const QVariantList &args)
{
    if (name.isEmpty()) {
        return nullptr;
    }

    Applet *applet = nullptr;

    if (appletId == 0) {
        appletId = ++AppletPrivate::s_maxAppletId;
    }

    KPluginMetaData plugin(PluginLoaderPrivate::s_plasmoidsPluginDir + QLatin1Char('/') + name, KPluginMetaData::AllowEmptyMetaData);
    const KPackage::Package p = KPackage::PackageLoader::self()->loadPackage(QStringLiteral("Plasma/Applet"), name);

    if (!p.isValid()) {
        qWarning(LOG_PLASMA) << "Applet invalid: Cannot find a package for" << name;
    }

    // If the applet is using another applet package, search for the plugin of the other applet
    if (!plugin.isValid()) {
        const QString parentPlugin = p.metadata().value(QStringLiteral("X-Plasma-RootPath"));
        if (!parentPlugin.isEmpty()) {
            plugin = KPluginMetaData(PluginLoaderPrivate::s_plasmoidsPluginDir + QLatin1Char('/') + parentPlugin, KPluginMetaData::AllowEmptyMetaData);
        }
    }

    if (plugin.isValid()) {
        QVariantList allArgs = QVariantList{QVariant::fromValue(p), appletId} << args;
        if (KPluginFactory *factory = KPluginFactory::loadFactory(plugin).plugin) {
            if (factory->metaData().rawData().isEmpty()) {
                factory->setMetaData(p.metadata());
            }
            applet = factory->create<Plasma::Applet>(nullptr, allArgs);
        }
    }
    if (applet) {
        return applet;
    }

    QVariantList allArgs;
    allArgs << QVariant::fromValue(p) << appletId << args;

    if (isContainmentMetaData(p.metadata())) {
        applet = new Containment(nullptr, p.metadata(), allArgs);
    } else {
        applet = new Applet(nullptr, p.metadata(), allArgs);
    }

    const QString localePath = p.filePath("translations");
    if (!localePath.isEmpty()) {
        KLocalizedString::addDomainLocaleDir(QByteArray("plasma_applet_") + name.toLatin1(), localePath);
    }
    return applet;
}

ContainmentActions *PluginLoader::loadContainmentActions(Containment *parent, const QString &name, const QVariantList &args)
{
    if (name.isEmpty()) {
        return nullptr;
    }

    KPluginMetaData plugin(PluginLoaderPrivate::s_containmentActionsPluginDir + QLatin1Char('/') + name, KPluginMetaData::AllowEmptyMetaData);

    if (plugin.isValid()) {
        if (auto res = KPluginFactory::instantiatePlugin<Plasma::ContainmentActions>(plugin, nullptr, {QVariant::fromValue(plugin)})) {
            return res.plugin;
        }
    }

    return nullptr;
}

QList<KPluginMetaData> PluginLoader::listAppletMetaData(const QString &category)
{
    auto platforms = KRuntimePlatform::runtimePlatform();
    // For now desktop always lists everything
    if (platforms.contains(QStringLiteral("desktop"))) {
        platforms.clear();
    }

    // FIXME: this assumes we are always use packages.. no pure c++
    std::function<bool(const KPluginMetaData &)> filter;
    if (category.isEmpty()) { // use all but the excluded categories
        KConfigGroup group(KSharedConfig::openConfig(), QStringLiteral("General"));
        QStringList excluded = group.readEntry("ExcludeCategories", QStringList());

        filter = [excluded, platforms](const KPluginMetaData &md) -> bool {
            if (!platforms.isEmpty() && !md.formFactors().isEmpty()) {
                bool found = false;
                for (const auto &plat : platforms) {
                    if (md.formFactors().contains(plat)) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    return false;
                }
            }

            return !excluded.contains(md.category());
        };
    } else { // specific category (this could be an excluded one - is that bad?)

        filter = [category, platforms](const KPluginMetaData &md) -> bool {
            if (!platforms.isEmpty() && !md.formFactors().isEmpty()) {
                bool found = false;
                for (const auto &plat : platforms) {
                    if (md.formFactors().contains(plat)) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    return false;
                }
            }

            if (category == QLatin1String("Miscellaneous")) {
                return md.category() == category || md.category().isEmpty();
            } else {
                return md.category() == category;
            }
        };
    }

    return KPackage::PackageLoader::self()->findPackages(QStringLiteral("Plasma/Applet"), QString(), filter);
}

QList<KPluginMetaData> PluginLoader::listAppletMetaDataForMimeType(const QString &mimeType)
{
    auto filter = [&mimeType](const KPluginMetaData &md) -> bool {
        return md.value(QStringLiteral("X-Plasma-DropMimeTypes"), QStringList()).contains(mimeType);
    };
    return KPackage::PackageLoader::self()->findPackages(QStringLiteral("Plasma/Applet"), QString(), filter);
}

QList<KPluginMetaData> PluginLoader::listAppletMetaDataForUrl(const QUrl &url)
{
    auto filter = [](const KPluginMetaData &md) -> bool {
        return !md.value(QStringLiteral("X-Plasma-DropUrlPatterns"), QStringList()).isEmpty();
    };
    const QList<KPluginMetaData> allApplets = KPackage::PackageLoader::self()->findPackages(QStringLiteral("Plasma/Applet"), QString(), filter);

    QList<KPluginMetaData> filtered;
    for (const KPluginMetaData &md : allApplets) {
        const QStringList urlPatterns = md.value(QStringLiteral("X-Plasma-DropUrlPatterns"), QStringList());
        for (const QString &glob : urlPatterns) {
            QRegularExpression rx(QRegularExpression::anchoredPattern(QRegularExpression::wildcardToRegularExpression(glob)));
            if (rx.match(url.toString()).hasMatch()) {
                filtered << md;
            }
        }
    }

    return filtered;
}

QList<KPluginMetaData> PluginLoader::listContainmentsMetaData(std::function<bool(const KPluginMetaData &)> filter)
{
    auto ownFilter = [filter](const KPluginMetaData &md) -> bool {
        return isContainmentMetaData(md) && filter(md);
    };

    return KPackage::PackageLoader::self()->findPackages(QStringLiteral("Plasma/Applet"), QString(), ownFilter);
}

QList<KPluginMetaData> PluginLoader::listContainmentsMetaDataOfType(const QString &type)
{
    auto filter = [type](const KPluginMetaData &md) -> bool {
        return md.value(QStringLiteral("X-Plasma-ContainmentType")) == type;
    };

    return listContainmentsMetaData(filter);
}

QList<KPluginMetaData> PluginLoader::listContainmentActionsMetaData(const QString &parentApp)
{
    auto filter = [&parentApp](const KPluginMetaData &md) -> bool {
        return md.value(QStringLiteral("X-KDE-ParentApp")) == parentApp;
    };

    QList<KPluginMetaData> plugins;
    if (parentApp.isEmpty()) {
        plugins = KPluginMetaData::findPlugins(PluginLoaderPrivate::s_containmentActionsPluginDir);
    } else {
        plugins = KPluginMetaData::findPlugins(PluginLoaderPrivate::s_containmentActionsPluginDir, filter);
    }

    return plugins;
}

} // Plasma Namespace
