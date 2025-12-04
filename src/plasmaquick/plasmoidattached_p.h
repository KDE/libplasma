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
// Class used exclusively to generate the Plasmoid.* attached properties, which are Applet instances
/*!
 * \qmltype Plasmoid
 * \nativetype Plasma::Applet
 * \inqmlmodule org.kde.plasma.plasmoid
 * \brief Common attached properties for plasmoids.
 */
/*!
 * \qmlattachedproperty int Plasmoid::id
 * The plasmoid's id is unique in the whole Plasma session and will never change across restarts.
 */
/*!
 * \qmlattachedproperty string Plasmoid::title
 * User friendly title for the plasmoid, the localized plasmoid name by default.
 */
/*!
 * \qmlattachedproperty string Plasmoid::icon
 * Icon to represent the Plasmoid.
 */
/*!
 * \qmlattachedproperty Plasma.Types.FormFactor Plasmoid::formFactor
 * The current form factor the plasmoid is being displayed in.
 */
/*!
 * \qmlattachedproperty Plasma.Types.Location Plasmoid::location
 * The location of the scene which is displaying this plasmoid.
 */
/*!
 * \qmlattachedproperty Plasma.Types.ItemStatus Plasmoid::status
 * Allows to instruct the shell if this plasmoid is requesting attention, accepting input, or if it is in an idle, inactive state.
 */
/*!
 * \qmlattachedproperty Plasma.Types.ImmutabilityType Plasmoid::immutability
 * Sets the immutability of the Corona.
 * Tells the plasmoid whether it should allow for any modification by the user.
 */
/*!
 * \qmlattachedproperty bool Plasmoid::immutable
 * Whether the Corona is immutable.
 *
 * The plasmoid implementation should avoid allowing "dangerous" modifications from the user when in an immutable mode.
 *
 * This is true when immutability is not Mutable.
 */
/*!
 * \qmlattachedproperty Plasma.Types.ContainmentDisplayHints Plasmoid::containmentDisplayHints
 * Display hints that come from the containment that suggest how the plasmoid should look and behave.
 * TODO: only in containment?
 */
/*!
 * \qmlattachedproperty bool Plasmoid::busy
 * True if the plasmoid should show a busy status, for instance while doing
 * some network operation.
 */
/*!
 * \qmlattachedproperty bool Plasmoid::userConfiguring
 * True when the user is configuring, for instance when the configuration dialog is open.
 */
/*!
 * \qmlattachedproperty Plasma.Types.BackgroundHints Plasmoid::backgroundHints
 * How the plasmoid wants its background to be drawn.
 *
 * The containment may ignore this hint with userBackgroundHints.
 */
/*!
 * \qmlattachedproperty Plasma.Types.BackgroundHints Plasmoid::userBackgroundHints
 * Allows the containment or user to override the set backgroundHints if they exist.
 */
/*!
 * \qmlattachedproperty Plasma.Types.BackgroundHints Plasmoid::effectiveBackgroundHints
 * Returns the effective background hints.
 *
 * The userBackgroundHints will be preferred if set, otherwise returns the plasmoid's backgroundHints.
 */
/*!
 * \qmlattachedproperty KConfigPropertyMap Plasmoid::configuration
 * Configuration for the plasmoid.
 *
 * Can be used to read and write settings like any JavaScript Object.
 */
/*!
 * \qmlattachedproperty Shortcut Plasmoid::globalShortcut
 * The global shortcut to activate the plasmoid.
 *
 * This is typically only used by the default configuration module.
 */
/*!
 * \qmlattachedproperty bool Plasmoid::configurationRequired
 * If true, the plasmoid requires manual configuration from the user.
 */
/*!
 * \qmlattachedproperty bool Plasmoid::hasConfigurationInterface
 * True if this plasmoid will provide a UI for its configuration.
 */
/*!
 * \qmlattachedproperty Plasma.Applet.ConstraintHints Plasmoid::constraintHints
 * Hints how the plasmoid should be constrained,
 * such as asking to fill all the available space ignoring margins.
 */
/*!
 * \qmlattachedproperty KPluginMetaData Plasmoid::metaData
 * The metadata of the plasmoid.
 */
/*!
 * \qmlattachedproperty Containment Plasmoid::containment
 * The Containment managing this plasmoid.
 */
/*!
 * \qmlattachedproperty list<Action> Plasmoid::contextualActions
 * Actions to be added in the plasmoid's context menu.
 *
 * Use PlasmaCore.Action to instantiate QActions declaratively.
 */
/*!
 * \qmlattachedproperty bool Plasmoid::isContainment
 * Whether this plasmoid is a Containment and is acting as one, such as a desktop or a panel.
 */
/*!
 * \qmlattachedproperty string Plasmoid::pluginName
 * Plugin name for the plasmoid.
 */
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

// Class used exclusively to generate the Plasmoid.* attached properties, which are Applet instances
/*!
 * \qmltype Containment
 * \inqmlmodule org.kde.plasma.plasmoid
 * \brief The base class for plugins that provide backgrounds and applet grouping containers.
 *
 * Containment objects provide the means to group applets into functional sets.
 * They also provide the following:
 *
 * \list
 * \li creation of focusing event
 * \li drawing of the background image (which can be interactive)
 * \li form factors (e.g. panel, desktop, full screen, etc)
 * \li applet layout management
 * \endlist
 *
 * Since Containment is itself a Plasmoid (native C++ class Plasma::Applet), all the techniques used
 * for writing the visual presentation of Plasmoids is applicable to Containments.
 * Containments are differentiated from Plasmoids by being marked with the ServiceType
 * of Plasma/Containment. Plugins registered with both the Applet and the Containment
 * ServiceTypes can be loaded for use in either situation.
 */

/*!
 * \qmlattachedproperty list<Plasmoid> Containment::applets
 * List of plasmoids in this containment.
 */
/*!
 * \qmlattachedproperty Plasma::Corona Containment::corona
 * The corona for this contaiment.
 */
/*!
 * \qmlattachedproperty Containment.Type Containment::containmentType
 * Type of this containment.
 */
/*!
 * \qmlattachedproperty string Containment::activity
 * Activity UID of this containment.
 */
/*!
 * \qmlattachedproperty string Containment::activityName
 * Activity name of this containment.
 */
/*!
 * \qmlattachedproperty Plasma.Types.ContainmentDisplayHints Containment::containmentDisplayHints
 */
/*!
 * \qmlattachedproperty string Containment::wallpaperPlugin
 */
/*!
 * \qmlattachedproperty QObject Containment::wallpaperGraphicsObject
 */
/*!
 * \qmlattachedproperty bool Containment::isUiReady
 */
/*!
 * \qmlattachedproperty int Containment::screen
 * The screen number this containment is serving as the desktop for, or -1 if none
 */
/*!
 * \qmlattachedproperty rect Containment::availableScreenRect
 * Screen area used by the Containment by itself (free of enveloping panels).
 *
 * The coordinates are relative to the containment (that is, x:0 and y:0 match the Containment's upper left corner),
 * and independent from the screen position.
 *
 * For more precise available geometry use availableScreenRegion().
 */
/*!
 * \qmlattachedproperty list<rect> Containment::availableScreenRegion
 * A list of rectangles matching the available region of this screen, panels excluded.
 */
/*!
 * \qmlattachedproperty rect Containment::screenGeometry
 * Provides access to the geometry of the screen the plasmoid is in.
 *
 * Can be useful to figure out what's the absolute position of the plasmoid.
 */
/*!
 * \qmlattachedproperty enumeration Containment::Type
 * The type of the Containment.
 *
 * This value is specified in the "X-Plasma-ContainmentType" JSON-metadata value of containments.
 *
 * \value NoContainment
 * \value Desktop
 *        Main containments that will own a screen in a mutually exclusive fashion.
 * \value Panel
 *        A desktop panel. Multiple can be present per screen.
 * \value Custom
 *        A containment that is neither a desktop nor a panel but something application specific.
 * \value CustomPanel
 *        A customized desktop panel.
 *        "CustomPanel" in metadata.
 * \value CustomEmbedded
 *        A customized containment embedded into another applet.
 */
/*!
 * \qmlmethod void Containment::addApplet(Plasma::Applet applet, rect geometryHint = rect)
 * \brief Adds an existing \a applet (plasmoid) to this Containment with the given \a geometryHint.
 *
 * The hint determines the preferred location and size for the newly created plasmoid. The hint will not be respected if it's not possible to position it in the preferred location.
 */
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
