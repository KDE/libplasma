/*
 *    SPDX-FileCopyrightText: 2026 Nicolas Fella <nicolas.fella@gmx.de>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "tooltipareatest.h"

#include <QDebug>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QTest>

QTEST_MAIN(ToolTipAreaTest)

ToolTipAreaTest::ToolTipAreaTest()
{
}

void ToolTipAreaTest::testToolTipArea()
{
    QQmlEngine engine;
    QQmlComponent component(&engine, "org.kde.plasma.core", "ToolTipArea");
    QObject *toolTipArea = component.create();

    QVERIFY(toolTipArea);

    toolTipArea->setProperty("mainText", "Hello");

    QMetaObject::invokeMethod(toolTipArea, "showToolTip");

    QMetaObject::invokeMethod(toolTipArea, "hideToolTip");

    QMetaObject::invokeMethod(toolTipArea, "showToolTip");

    QMetaObject::invokeMethod(toolTipArea, "hideImmediately");

    delete toolTipArea;
}
