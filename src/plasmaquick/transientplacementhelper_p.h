/*
    SPDX-FileCopyrightText: 2023 David Edmundson <davidedmundson@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "transientplacementhint.h"

class QWindow;

namespace PlasmaQuick
{
namespace TransientPlacementHelper
{
/*! Returns the popup rectangle resolved from the placement hint and window constraints. */
QRect popupRect(QWindow *window, const TransientPlacementHint &placement);
}
}
