/*
 *  SPDX-FileCopyrightText: 2025 Nicolas Fella <nicolas.fella@gmx.de>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <qqmlregistration.h>

#include "configcategory_p.h"
#include "configmodel.h"

struct ConfigModelForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(ConfigModel)
    QML_FOREIGN(PlasmaQuick::ConfigModel)
};

struct ConfigCategoryForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(ConfigCategory)
    QML_FOREIGN(PlasmaQuick::ConfigCategory)
};
