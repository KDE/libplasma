/*
 *  SPDX-FileCopyrightText: 2025 Nicolas Fella <nicolas.fella@gmx.de>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <containmentitem.h>
#include <corona.h>
#include <plasmoiditem.h>
#include <wallpaperitem.h>

#include <qqmlregistration.h>

struct PlasmoidItemForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(PlasmoidItem)
    QML_FOREIGN(PlasmoidItem);
};

struct ContainmentItemForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(ContainmentItem)
    QML_FOREIGN(ContainmentItem);
};

struct WallpaperItemForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(WallpaperItem)
    QML_FOREIGN(WallpaperItem);
};

struct CoronaForeign {
    Q_GADGET
    QML_ANONYMOUS
    QML_FOREIGN(Plasma::Corona)
};
