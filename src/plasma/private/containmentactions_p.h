/*
    SPDX-FileCopyrightText: 2009 Chani Armitage <chani@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PLASMA_CONTAINMENTACTIONSPRIVATE_H
#define PLASMA_CONTAINMENTACTIONSPRIVATE_H

#include <KPluginMetaData>

#include "containment.h"

namespace Plasma
{

class ContainmentActionsPrivate
{
public:
    ContainmentActionsPrivate(const KPluginMetaData &metadata, ContainmentActions * /*containmentActions*/)
        : containmentActionsDescription(metadata)
    {
    }

    const KPluginMetaData containmentActionsDescription;
    Containment *containment = nullptr;
};

} // namespace Plasma
#endif // PLASMA_CONTAINMENTACTIONSPRIVATE_H
