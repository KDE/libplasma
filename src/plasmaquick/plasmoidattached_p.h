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
 * \brief Common attached properties for applets.
 */
/*!
 * \qmlattachedproperty int Plasmoid::id
 * Applet id: is unique in the whole Plasma session and will never change across restarts
 */
/*!
 * \qmlattachedproperty string Plasmoid::title
 * User friendly title for the plasmoid: it's the localized applet name by default
 */
/*!
 * \qmlattachedproperty string Plasmoid::icon
 * Icon to represent the Plasmoid
 */
/*!
 * \qmlattachedproperty Plasma.Types.FormFactor Plasmoid::formFactor
 * The current form factor the applet is being displayed in.
 */
/*!
 * \qmlattachedproperty Plasma.Types.Location Plasmoid::location
 * The location of the scene which is displaying applet.
 */
/*!
 * \qmlattachedproperty Plasma.Types.ItemStatus Plasmoid::status
 * Status of the plasmoid: useful to instruct the shell if this plasmoid is requesting attention, if is accepting input, or if is in an idle, inactive state
 */
/*!
 * \qmlattachedproperty Plasma.Types.ImmutabilityType Plasmoid::immutability
 * The immutability of the Corona.
 * Tells the applet whether it should allow for any modification by the user.
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
 * Display hints that come from the containment that suggest the applet how to look and behave.
 * TODO: only in containment?
 */
/*!
 * \qmlattachedproperty bool Plasmoid::busy
 * True if the applet should show a busy status, for instance doing
 * some network operation
 */
/*!
 * \qmlattachedproperty bool Plasmoid::userConfiguring
 * True when the user is configuring, for instance when the configuration dialog is open.
 */
/*!
 * \qmlattachedproperty Plasma.Types.BackgroundHints Plasmoid::backgroundHints
 * How the applet wants its background to be drawn. The containment may chose to ignore this hint.
 */
/*!
 * \qmlattachedproperty Plasma.Types.BackgroundHints Plasmoid::userBackgroundHints
 * The containment (and/or the user) may decide to use another kind of background instead (if supported by the applet)
 */
/*!
 * \qmlattachedproperty Plasma.Types.BackgroundHints Plasmoid::effectiveBackgroundHints
 * The effective background hints the applet has, internally decided how to mix with userBackgroundHints
 */
/*!
 * \qmlattachedproperty KConfigPropertyMap Plasmoid::configuration
 * A KConfigPropertyMap instance that represents the configuration
 * which is usable from QML to read and write settings like any JavaScript Object
 */
/*!
 * \qmlattachedproperty Shortcut Plasmoid::globalShortcut
 * The global shortcut to activate the plasmoid.
 *
 * This is typically only used by the default configuration module.
 */
/*!
 * \qmlattachedproperty bool Plasmoid::configurationRequired
 * If true, the applet requires manual configuration from the user.
 */
/*!
 * \qmlattachedproperty bool Plasmoid::hasConfigurationInterface
 * True if this applet will provide a UI for its configuration
 */
/*!
 * \qmlattachedproperty Plasma.Applet.ConstraintHints Plasmoid::constraintHints
 * The hints that the applet gives to its constraint,
 * such as asking to fill all the available space ignoring margins.
 */
/*!
 * \qmlattachedproperty KPluginMetaData Plasmoid::metaData
 * The metadata of the applet.
 */
/*!
 * \qmlattachedproperty Containment Plasmoid::containment
 * The Containment managing this applet
 */
/*!
 * \qmlattachedproperty list<Action> Plasmoid::contextualActions
 * Actions to be added in the plasmoid context menu. To instantiate QActions in a declarative way,
 * PlasmaCore.Action {} can be used
 */
/*!
 * \qmlattachedproperty bool Plasmoid::isContainment
 * True if this applet is a Containment and is acting as one, such as a desktop or a panel
 */
/*!
 * \qmlattachedproperty string Plasmoid::pluginName
 * Plugin name for the applet
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
 * Since containment is actually just a Plasma::Applet, all the techniques used
 * for writing the visual presentation of Applets is applicable to Containtments.
 * Containments are differentiated from Applets by being marked with the ServiceType
 * of Plasma/Containment. Plugins registered with both the Applet and the Containment
 * ServiceTypes can be loaded for us in either situation.
 *
 * See techbase.kde.org for a tutorial on writing Containments using this class.
 */

/*!
 * \qmlattachedproperty list<Applet> Containment::applets
 * List of applets this containment has: the containments
 */
/*!
 * \qmlattachedproperty Plasma::Corona Containment::corona
 * The corona for this contaiment
 */
/*!
 * \qmlattachedproperty Containment.Type Containment::containmentType
 * Type of this containment
 */
/*!
 * \qmlattachedproperty string Containment::activity
 * Activity UID of this containment
 */
/*!
 * \qmlattachedproperty string Containment::activityName
 * Activity name of this containment
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
 * screen area free of panels: the coordinates are relative to the containment,
 * it's independent from the screen position
 * For more precise available geometry use availableScreenRegion()
 */
/*!
 * \qmlattachedproperty list<rect> Containment::availableRelativeScreenRegion
 * The available region of this screen, panels excluded. It's a list of rectangles
 */
/*!
 * \qmlattachedproperty rect Containment::screenGeometry
 * Provides access to the geometry of the applet is in.
 * Can be useful to figure out what's the absolute position of the applet.
 */
/*!
 * \qmlattachedproperty enumeration Containment::Type
 * This enumeration describes the type of the Containment.
 * DesktopContainments represent main containments that will own a screen in a mutually exclusive fashion,
 * while PanelContainments are accessories which can be present multiple per screen.
 *
 * This value is specified in the "X-Plasma-ContainmentType" JSON-metadata value of containments.
 *
 * \value NoContainment
 * \value Desktop A desktop containment
 * \value Panel A desktop panel
 * \value Custom A containment that is neither a desktop nor a panel but something application specific
 * \value CustomPanel A customized desktop panel. "CustomPanel" in metadata
 * \value CustomEmbedded A customized containment embedded in another applet
 *
 */
/*!
 * \qmlmethod void Containment::addApplet(Plasma::Applet applet, rect geometryHint = rect)
 * \brief Add an existing applet to this Containment
 *
 * \a applet the applet that should be added
 *
 * \a geometryHint an hint to pass to the GUI on the location
 *           and size we prefer for the newly created applet;
 *           the gui might choose whether to respect or not this hint
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
