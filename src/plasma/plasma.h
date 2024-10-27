/*
    SPDX-FileCopyrightText: 2005 Aaron Seigo <aseigo@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PLASMA_DEFS_H
#define PLASMA_DEFS_H

#include <QObject>

#include <plasma/plasma_export.h>

class QAction;

namespace Plasma
{
/*!
 * \class Plasma::Types
 * \inheaderfile Plasma/Plasma
 * \inmodule Plasma
 *
 * \brief Enums and constants used in Plasma.
 */
class PLASMA_EXPORT Types : public QObject
{
    Q_OBJECT

public:
    ~Types() override;

    /*!
     * The FormFactor enumeration describes how a Plasma::Applet should arrange
     * itself. The value is derived from the container managing the Applet
     * (e.g. in Plasma, a Corona on the desktop or on a panel).
     *
     * \value Planar The applet lives in a plane and has two degrees of freedom to grow. Optimize for desktop, laptop or tablet usage: a high resolution screen
     *1-3 feet distant from the viewer.
     * \value MediaCenter As with Planar, the applet lives in a plane but the interface should be optimized for medium-to-high resolution screens that are 5-15
     *feet distant from the viewer. Sometimes referred to as a "ten foot interface".
     * \value Horizontal The applet is constrained vertically, but can expand horizontally.
     * \value Vertical The applet is constrained horizontally, but can expand vertically.
     * \value Application The Applet lives in a plane and should be optimized to look as a full application, for the desktop or the particular device.
     **/
    enum FormFactor {
        Planar = 0,
        MediaCenter,
        Horizontal,
        Vertical,
        Application,
    };
    Q_ENUM(FormFactor)

    /*!
     * Display hints that come from the containment that suggest the applet how to look and behave.
     * \since 5.77
     *
     * \value NoContainmentDisplayHint
     * \value ContainmentDrawsPlasmoidHeading The containment will draw an titlebar-looking header for the applets, so the applets shouldn't attempt to paint a
     * similar thing
     * \value ContainmentForcesSquarePlasmoids The containment will force every plasmoid to be constrained in a square icon (An example is the System Tray)
     * \value ContainmentPrefersOpaqueBackground The containment prefers the style of the applets to be opaque, where applicable
     * \value ContainmentPrefersFloatingApplets The containment prefers applet's dialogs to be floating, where applicable
     */
    enum ContainmentDisplayHint {
        NoContainmentDisplayHint = 0,
        ContainmentDrawsPlasmoidHeading = 1,
        ContainmentForcesSquarePlasmoids = 2,
        ContainmentPrefersOpaqueBackground = 4,
        ContainmentPrefersFloatingApplets = 8
    };
    Q_ENUM(ContainmentDisplayHint)
    Q_DECLARE_FLAGS(ContainmentDisplayHints, ContainmentDisplayHint)
    Q_FLAG(ContainmentDisplayHints)

    /*!
     * The Location enumeration describes where on screen an element, such as an
     * Applet or its managing container, is positioned on the screen.
     *
     * \value Floating Free floating. Neither geometry or z-ordering is described precisely by this value.
     * \value Desktop On the planar desktop layer, extending across the full screen from edge to edge.
     * \value FullScreen Full screen
     * \value TopEdge Along the top of the screen
     * \value BottomEdge Along the bottom of the screen
     * \value LeftEdge Along the left side of the screen
     * \value RightEdge Along the right side of the screen
     **/
    enum Location {
        Floating = 0,
        Desktop,
        FullScreen,
        TopEdge,
        BottomEdge,
        LeftEdge,
        RightEdge,
    };
    Q_ENUM(Location)

    /*!
     * Defines the immutability of items like applets, corona and containments
     * they can be free to modify, locked down by the user or locked down by the
     * system (e.g. kiosk setups).
     *
     * \value Mutable The item can be modified in any way
     * \value UserImmutable The user has requested a lock down, and can undo the lock down at any time
     * \value SystemImmutable The item is locked down by the system, the user can't unlock it
     */
    enum ImmutabilityType {
        Mutable = 1,
        UserImmutable = 2,
        SystemImmutable = 4,
    };
    Q_ENUM(ImmutabilityType)

    /*!
     * Status of an applet
     * \since 4.3
     *
     * \value UnknownStatus The status is unknown
     * \value PassiveStatus The item is passive
     * \value ActiveStatus The item is active
     * \value NeedsAttentionStatus The item needs attention
     * \value RequiresAttentionStatus The item needs persistent attention
     * \value AcceptingInputStatus The item is accepting input
     * \value HiddenStatus The item will be hidden totally
     */
    enum ItemStatus {
        UnknownStatus = 0,
        PassiveStatus = 1,
        ActiveStatus = 2,
        NeedsAttentionStatus = 3,
        RequiresAttentionStatus = 4,
        AcceptingInputStatus = 5,
        HiddenStatus = 6,
    };
    Q_ENUM(ItemStatus)

    /*!
     * Description on how draw a background for the applet
     *
     * \value NoBackground Not drawing a background under the applet, the applet has its own implementation
     * \value StandardBackground The standard background from the theme is drawn
     * \value TranslucentBackground An alternate version of the background is drawn, usually more translucent
     * \value ShadowBackground The applet won't have a svg background but a drop shadow of its content done via a shader
     * \value ConfigurableBackground If the hint has this flag, the user is able to configure this background
     * \value DefaultBackground Default settings: both standard background
     */
    enum BackgroundHints {
        NoBackground = 0,
        StandardBackground = 1,
        TranslucentBackground = 2,
        ShadowBackground = 4,
        ConfigurableBackground = 8,
        DefaultBackground = StandardBackground,
    };
    Q_ENUM(BackgroundHints)
    Q_DECLARE_FLAGS(BackgroundFlags, BackgroundHints)
    // TODO KF6: BackgroundHint and BackgroundHints

private:
    Types(QObject *parent = nullptr);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Types::ContainmentDisplayHints)
Q_DECLARE_OPERATORS_FOR_FLAGS(Types::BackgroundFlags)

} // Plasma namespace

#endif // multiple inclusion guard
