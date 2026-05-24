/*
 *  SPDX-FileCopyrightText: 2026 Nicolas Fella <nicolas.fella@gmx.de>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include "plasmaquick_export.h"

#include <QQmlEngine>

namespace PlasmaQuick
{
/*!
 * The shared QML engine to be used for applets and other Plasma components.
 *
 * The engine is set up via Plasma::setupPlasmaStyle().
 * \since 6.8
 */
PLASMAQUICK_EXPORT std::shared_ptr<QQmlEngine> globalEngine();
};
