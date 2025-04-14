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

public:
    // HACK: QML_ATTACHED doesn't make the enums from Plasma::Applet available, so copy those here

    enum Constraint {
        NoConstraint = 0, /**< No constraint; never passed in to Applet::constraintsEvent on its own */
        FormFactorConstraint = 1, /**< The FormFactor for an object */
        LocationConstraint = 2, /**< The Location of an object */
        ScreenConstraint = 4, /**< Which screen an object is on */
        ImmutableConstraint = 8, /**< the immutability (locked) nature of the applet changed  */
        StartupCompletedConstraint = 16, /**< application startup has completed */
        UiReadyConstraint = 32,
        /**< The ui has been completely loaded */ // (FIXME: merged with StartupCompletedConstraint?)
        AllConstraints = FormFactorConstraint | LocationConstraint | ScreenConstraint | ImmutableConstraint,
    };
    Q_ENUM(Constraint)
    Q_DECLARE_FLAGS(Constraints, Constraint)

    /**
     * This enumeration lists the various hints that an applet can pass to its
     * constraint regarding the way that it is represented
     */
    enum ConstraintHint {
        NoHint = 0,
        CanFillArea = 1,
        /**< The CompactRepresentation can fill the area and ignore constraint margins*/ // (TODO: KF6 CanFillArea -> CompactRepresentationFillArea)
        MarginAreasSeparator = CanFillArea | 2, /**< The applet acts as a separator between the standard and slim panel margin areas*/
    };
    Q_DECLARE_FLAGS(ConstraintHints, ConstraintHint)
    Q_FLAG(ConstraintHints)

    /** TODO: When the migration to the new action api is done, remove this enum
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

public:
    // HACK: QML_ATTACHED doesn't make the enums from Plasma::Containment available, so copy those here

    enum Type {
        NoContainment = -1, /**< @internal */
        Desktop = 0, /**< A desktop containment */
        Panel, /**< A desktop panel */
        Custom = 127, /**< A containment that is neither a desktop nor a panel but something application specific */
        CustomPanel = 128, /**< A customized desktop panel. "CustomPanel" in metadata */
        CustomEmbedded = 129, /**< A customized containment embedded in another applet */
    };
    Q_ENUM(Type)

    ContainmentAttached(QObject *parent = nullptr);
    ~ContainmentAttached() override;

    ////NEEDED BY QML TO CREATE ATTACHED PROPERTIES
    static Plasma::Containment *qmlAttachedProperties(QObject *object);
};

}

#endif
