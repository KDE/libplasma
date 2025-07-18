/*
 *  SPDX-FileCopyrightText: 2025 Nicolas Fella <nicolas.fella@gmx.de>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <QStandardPaths>
#include <QTest>

#include <KPluginMetaData>

#include <Plasma/Applet>
#include <Plasma/PluginLoader>
#include <PlasmaQuick/AppletQuickItem>

using namespace Qt::StringLiterals;

class AppletTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testLoad_data()
    {
        QTest::addColumn<QString>("id");

        QTest::addRow("simple") << u"org.kde.plasma.testapplet"_s;
        QTest::addRow("withcpp") << u"org.kde.plasma.testapplet2"_s;
    }

    void testLoad()
    {
        QFETCH(QString, id);

        QStandardPaths::setTestModeEnabled(true);

        auto applets = Plasma::PluginLoader::self()->listAppletMetaData(QString());

        auto testApplet = std::find_if(applets.cbegin(), applets.cend(), [id](const KPluginMetaData &md) {
            return md.pluginId() == id;
        });

        QVERIFY(testApplet != applets.cend());

        Plasma::Applet *applet = Plasma::PluginLoader::self()->loadApplet(id);
        QVERIFY(applet);

        QCOMPARE(applet->pluginName(), id);

        PlasmaQuick::AppletQuickItem *item = PlasmaQuick::AppletQuickItem::itemForApplet(applet);

        QVERIFY(item);

        QCOMPARE(item->applet(), applet);
    }
};

QTEST_MAIN(AppletTest);

#include "applettest.moc"
