/*
    SPDX-FileCopyrightText: 2010 Ryan Rix <ry@n.rix.si>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "pluginloader.h"

#include <QDebug>
#include <QRegularExpression>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KPackage/PackageLoader>
#include <KPluginFactory>
#include <KRuntimePlatform>
#include <KSharedConfig>

#include "applet.h"
#include "containment.h"
#include "containmentactions.h"
#include "private/applet_p.h"

using namespace Qt::Literals;

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

    // name can be either an actual applet name or an absolute path, in the
    // latter case, ensure we only use the name part of the path.
    const QString pluginName = name.section(QLatin1Char('/'), -1);

    /*
     * Cases:
     * - Pure KPackage
     * - KPackage + C++
     * - KPackage + C++ (with X-Plasma-RootPath)
     * - C++ with embedded QML
     */

    KPluginMetaData plugin(u"plasma/applets/" + pluginName, KPluginMetaData::AllowEmptyMetaData);
    const KPackage::Package package = KPackage::PackageLoader::self()->loadPackage(u"Plasma/Applet"_s, name);

    // If the applet is using another applet package, search for the plugin of the other applet
    const QString parentPlugin = package.metadata().value(u"X-Plasma-RootPath");
    if (!parentPlugin.isEmpty()) {
        plugin = KPluginMetaData(u"plasma/applets/" + parentPlugin, KPluginMetaData::AllowEmptyMetaData);
    }

    // pure KPackage
    if (package.isValid() && !plugin.isValid()) {
        QVariantList allArgs;
        allArgs << QVariant::fromValue(package) << appletId << args;

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

    // KPackage + C++
    if (package.isValid()) {
        QVariantList allArgs;
        allArgs << QVariant::fromValue(package) << appletId << args;

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

    // C++ with embedded QML
    if (plugin.isValid()) {
        return KPluginFactory::instantiatePlugin<Plasma::Applet>(plugin, nullptr, {{}, appletId}).plugin;
    }

    // Add fake extension to parse completeBaseName() as pluginId
    // without having to construct a fake JSON metadata object.
    // This would help with better error messages which would
    // at least show the missing applet's ID.
    const auto fakeFileName = name + u'.';
    // metadata = KPluginMetaData(QJsonObject(), fakeFileName);
    return new Applet(nullptr, KPluginMetaData(QJsonObject(), fakeFileName), {{}, appletId});
}

ContainmentActions *PluginLoader::loadContainmentActions(Containment *parent, const QString &name, const QVariantList &args)
{
    Q_UNUSED(parent)
    Q_UNUSED(args)
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

    const auto kpackages = KPackage::PackageLoader::self()->findPackages(QStringLiteral("Plasma/Applet"), QString(), filter);

    const auto plugins = KPluginMetaData::findPlugins(QStringLiteral("plasma/applets/"), {}, KPluginMetaData::AllowEmptyMetaData);

    QList<KPluginMetaData> extraPlugins;

    for (const KPluginMetaData &plugin : plugins) {
        auto it = std::find_if(kpackages.cbegin(), kpackages.cend(), [plugin](const KPluginMetaData &md) {
            return md.pluginId() == plugin.pluginId();
        });

        if (it != kpackages.cend()) {
            continue;
        }

        extraPlugins << plugin;
    }

    return kpackages + extraPlugins;
}

QList<KPluginMetaData> PluginLoader::listAppletMetaDataForMimeType(const QString &mimeType)
{
    auto filter = [&mimeType](const KPluginMetaData &md) -> bool {
        return md.value(u"X-Plasma-DropMimeTypes", QStringList()).contains(mimeType);
    };
    return KPackage::PackageLoader::self()->findPackages(QStringLiteral("Plasma/Applet"), QString(), filter);
}

QList<KPluginMetaData> PluginLoader::listAppletMetaDataForUrl(const QUrl &url)
{
    auto filter = [](const KPluginMetaData &md) -> bool {
        return !md.value(u"X-Plasma-DropUrlPatterns", QStringList()).isEmpty();
    };
    const QList<KPluginMetaData> allApplets = KPackage::PackageLoader::self()->findPackages(QStringLiteral("Plasma/Applet"), QString(), filter);

    QList<KPluginMetaData> filtered;
    for (const KPluginMetaData &md : allApplets) {
        const QStringList urlPatterns = md.value(u"X-Plasma-DropUrlPatterns", QStringList());
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
        return md.value(u"X-Plasma-ContainmentType") == type;
    };

    return listContainmentsMetaData(filter);
}

QList<KPluginMetaData> PluginLoader::listContainmentActionsMetaData(const QString &parentApp)
{
    auto filter = [&parentApp](const KPluginMetaData &md) -> bool {
        return md.value(u"X-KDE-ParentApp") == parentApp;
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
