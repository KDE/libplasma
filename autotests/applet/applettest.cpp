/*
 *  SPDX-FileCopyrightText: 2025 Nicolas Fella <nicolas.fella@gmx.de>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <QDirIterator>
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

    void copyDirectory(const QString &srcDir, const QString &dstDir)
    {
        QDir targetDir(dstDir);
        targetDir.mkpath(".");
        QDirIterator it(srcDir, QDir::Filters(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Name), QDirIterator::Subdirectories);
        while (it.hasNext()) {
            it.next();
            QString path = it.filePath();
            QString relDestPath = path.last(it.filePath().length() - srcDir.length() - 1);
            if (it.fileInfo().isDir()) {
                QVERIFY(targetDir.mkpath(relDestPath));
            } else {
                QVERIFY(QFile::copy(path, dstDir + '/' + relDestPath));
            }
        }
    }

    void initTestCase()
    {
        QStandardPaths::setTestModeEnabled(true);

        const QString appletDir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/plasma/plasmoids/org.kde.plasma.testkpackage";
        QDir(appletDir).mkpath(".");
        QDir(appletDir).removeRecursively();

        copyDirectory(QFINDTESTDATA("kpackage/package"), appletDir);
    }

    void testLoad_data()
    {
        QTest::addColumn<QString>("id");

        QTest::addRow("simple") << u"org.kde.plasma.testapplet"_s;
        QTest::addRow("withcpp") << u"org.kde.plasma.testapplet2"_s;
        QTest::addRow("simplecontainment") << u"org.kde.plasma.testcontainment"_s;
        QTest::addRow("kpackage") << u"org.kde.plasma.testkpackage"_s;
    }

    void testLoad()
    {
        QFETCH(QString, id);

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

    void testListContainment()
    {
        auto containments = Plasma::PluginLoader::listContainmentsMetaData();
        QVERIFY(!containments.isEmpty());

        auto testContainment = std::find_if(containments.cbegin(), containments.cend(), [](const KPluginMetaData &md) {
            return md.pluginId() == "org.kde.plasma.testcontainment";
        });

        QVERIFY(testContainment != containments.cend());

        auto notAContainment = std::find_if(containments.cbegin(), containments.cend(), [](const KPluginMetaData &md) {
            return md.pluginId() == "org.kde.plasma.testapplet";
        });

        QVERIFY(notAContainment == containments.cend());
    }
};

QTEST_MAIN(AppletTest);

#include "applettest.moc"
