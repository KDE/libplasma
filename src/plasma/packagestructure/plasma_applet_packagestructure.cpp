/*
    SPDX-FileCopyrightText: 2007-2009 Aaron Seigo <aseigo@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "config-plasma.h"
#include "packages_p.h"
#include <KConfigGroup>
#include <KDesktopFile>
#include <KPackage/Package>
#include <KPackage/PackageStructure>
#include <KPackage/packagestructure_compat_p>

class PlasmoidPackage : public Plasma::GenericPackage
{
    Q_OBJECT
public:
    using Plasma::GenericPackage::GenericPackage;

    void initPackage(KPackage::Package *package) override
    {
        GenericPackage::initPackage(package);
        package->setDefaultPackageRoot(QStringLiteral(PLASMA_RELATIVE_DATA_INSTALL_DIR "/plasmoids/"));

        package->addFileDefinition("configmodel", QStringLiteral("config/config.qml"));
        package->addFileDefinition("mainconfigxml", QStringLiteral("config/main.xml"));
        package->setRequired("metadata", true);
        package->addFileDefinition("metadata", QStringLiteral("metadata.desktop"));
        package->setRequired("metadata", true);
    }

    void pathChanged(KPackage::Package *package) override
    {
        GenericPackage::pathChanged(package);
        if (const KPluginMetaData md = package->metadata(); md.isValid()) {
            if (md.rawData().contains(QStringLiteral("X-Plasma-ContainmentType"))) {
                package->addFileDefinition("compactapplet", QStringLiteral("applet/CompactApplet.qml"));
            } else {
                package->removeDefinition("compactapplet");
            }
        }
        // The widge explorer uses it to display old incompatible plasmoids
        KPackagePrivate::convertCompatMetaDataDesktopFile(package);
    }
};

K_PLUGIN_CLASS_WITH_JSON(PlasmoidPackage, "plasma_applet_packagestructure.json")

#include "plasma_applet_packagestructure.moc"
