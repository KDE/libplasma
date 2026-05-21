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
PLASMAQUICK_EXPORT std::shared_ptr<QQmlEngine> globalEngine();
};
