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

void setGlobalEngine(std::shared_ptr<QQmlEngine> engine)
{
    if (s_engine.lock()) {
        qFatal() << "setGlobalEngine must be called before PlasmaQuick::globalEngine is used";
    }

    Plasma::setupPlasmaStyle(engine.get());
    s_engine = engine;
}

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
