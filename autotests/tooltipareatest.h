/*
    SPDX-FileCopyrightText: 2026 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include <QObject>

class ToolTipAreaTest : public QObject
{
    Q_OBJECT
public:
    ToolTipAreaTest();

private Q_SLOTS:
    void testToolTipArea();
};
