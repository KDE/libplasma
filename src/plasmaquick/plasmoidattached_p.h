/*
    SPDX-FileCopyrightText: 2023 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PLASMOIDATTACHED_P_H
#define PLASMOIDATTACHED_P_H

#include <QAction>
#include <QObject>
#include <QQmlEngine>

#include <Plasma/Applet>
#include <Plasma/Containment>

//
//  W A R N I N G
//  -------------
//
// This file is not part of the public Plasma API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

namespace PlasmaQuick
{
// Class used exclusively to generate the Plasmoid.* attached proeprties, which are Applet instances
class PlasmoidAttached : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Plasmoid)
    QML_UNCREATABLE("")
    QML_ATTACHED(Plasma::Applet)
    QML_EXTENDED_NAMESPACE(Plasma::Applet) // needed to make enums from Plasma::Applet available

public:
    /*! TODO: When the migration to the new action api is done, remove this enum
     */
    enum ActionPriority {
        LowPriorityAction = QAction::LowPriority,
        NormalPriorityAction = QAction::NormalPriority,
        HighPriorityAction = QAction::HighPriority,
    };
    Q_ENUM(ActionPriority)

    PlasmoidAttached(QObject *parent = nullptr);
    ~PlasmoidAttached() override;

    ////NEEDED BY QML TO CREATE ATTACHED PROPERTIES
    static Plasma::Applet *qmlAttachedProperties(QObject *object);
};

// Class used exclusively to generate the Plasmoid.* attached proeprties, which are Applet instances
class ContainmentAttached : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Containment)
    QML_UNCREATABLE("")
    QML_ATTACHED(Plasma::Containment)
    QML_EXTENDED_NAMESPACE(Plasma::Containment) // needed to make enums from Plasma::Containment available

public:
    ContainmentAttached(QObject *parent = nullptr);
    ~ContainmentAttached() override;

    ////NEEDED BY QML TO CREATE ATTACHED PROPERTIES
    static Plasma::Containment *qmlAttachedProperties(QObject *object);
};

}

#endif
