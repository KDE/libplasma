/*
 *  SPDX-FileCopyrightText: 2026 Nicolas Fella <nicolas.fella@gmx.de>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "plasmaquick.h"
#include "plasma.h"

namespace PlasmaQuick
{
static std::weak_ptr<QQmlEngine> s_engine;

std::shared_ptr<QQmlEngine> globalEngine()
{
    if (auto locked = s_engine.lock()) {
        return locked;
    }
    auto createdEngine = std::make_shared<QQmlEngine>();

    Plasma::setupPlasmaStyle(createdEngine.get());

    s_engine = createdEngine;
    return createdEngine;
}
};
