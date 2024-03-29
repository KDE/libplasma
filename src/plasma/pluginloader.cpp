/*
    SPDX-FileCopyrightText: 2010 Ryan Rix <ry@n.rix.si>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "pluginloader.h"

#include <QDebug>
#include <QGuiApplication>
#include <QPluginLoader>
#include <QPointer>
#include <QRegularExpression>
#include <QStandardPaths>

#include <KLazyLocalizedString>
#include <KPackage/PackageLoader>
#include <KRuntimePlatform>

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

    if (appletId == 0) {
        appletId = ++AppletPrivate::s_maxAppletId;
    }

    /*
     * Cases:
     * - Pure KPackage
     * - KPackage + C++
     * - KPackage + C++ (with X-Plasma-RootPath)
     * - C++ with embedded QML
     *
     */

    KPluginMetaData plugin(QStringLiteral("plasma/applets/") + name, KPluginMetaData::AllowEmptyMetaData);
    const KPackage::Package package = KPackage::PackageLoader::self()->loadPackage(QStringLiteral("Plasma/Applet"), name);

    // If the applet is using another applet package, search for the plugin of the other applet
    const QString parentPlugin = package.metadata().value(QStringLiteral("X-Plasma-RootPath"));
    if (!parentPlugin.isEmpty()) {
        plugin = KPluginMetaData(QStringLiteral("plasma/applets/") + parentPlugin, KPluginMetaData::AllowEmptyMetaData);
    }

    if (package.isValid() && !plugin.isValid()) {
        qWarning() << "pure kpackage" << package.metadata().pluginId();

        const QVariantList allArgs{QVariant::fromValue(package), appletId, args};

        Applet *applet = nullptr;

        if (isContainmentMetaData(package.metadata())) {
            applet = new Containment(nullptr, package.metadata(), allArgs);
        } else {
            applet = new Applet(nullptr, package.metadata(), allArgs);
        }

        const QString localePath = package.filePath("translations");
        if (!localePath.isEmpty()) {
            KLocalizedString::addDomainLocaleDir(QByteArray("plasma_applet_") + name.toLatin1(), localePath);
        }

        return applet;
    }

    Q_ASSERT(plugin.isValid());

    if (package.isValid()) {
        qWarning() << "kpackage + c++" << package.metadata().pluginId();

        QVariantList allArgs = QVariantList{QVariant::fromValue(package), appletId} << args;

        KPluginFactory *factory = KPluginFactory::loadFactory(plugin).plugin;

        if (plugin.rawData().isEmpty()) {
            // Plugin has empty metadata, use metadata from KPackage
            factory->setMetaData(package.metadata());
        } else {
            // Plugin has its own metadata
            factory->setMetaData(plugin);
        }
        return factory->create<Plasma::Applet>(nullptr, allArgs);
    }

    qWarning() << "pure plugin" << plugin.pluginId();

    return KPluginFactory::instantiatePlugin<Plasma::Applet>(plugin, nullptr, {{}, appletId}).plugin;
}

ContainmentActions *PluginLoader::loadContainmentActions(Containment *parent, const QString &name, const QVariantList &args)
{
    if (name.isEmpty()) {
        return nullptr;
    }

    KPluginMetaData plugin(QStringLiteral("plasma/containmentactions/") + name, KPluginMetaData::AllowEmptyMetaData);

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

    auto kpackages = KPackage::PackageLoader::self()->findPackages(QStringLiteral("Plasma/Applet"), QString(), filter);

    auto plugins = KPluginMetaData::findPlugins(QStringLiteral("plasma/applets/"), {}, KPluginMetaData::AllowEmptyMetaData);

    QList<KPluginMetaData> extraPlugins;

    for (auto plugin : plugins) {
        qWarning() << "check" << plugin;
        auto it = std::find_if(kpackages.cbegin(), kpackages.cend(), [plugin](const KPluginMetaData &md) {
            return md.pluginId() == plugin.pluginId();
        });

        if (it != kpackages.cend()) {
            continue;
        }

        qWarning() << "found" << plugin.pluginId();

        extraPlugins << plugin;
    }

    return kpackages + extraPlugins;
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
        plugins = KPluginMetaData::findPlugins(QStringLiteral("plasma/containmentactions"));
    } else {
        plugins = KPluginMetaData::findPlugins(QStringLiteral("plasma/containmentactions"), filter);
    }

    return plugins;
}

} // Plasma Namespace
