/*
    SPDX-FileCopyrightText: 2009 Alan Alpert <alan.alpert@nokia.com>
    SPDX-FileCopyrightText: 2010 Ménard Alexis <menard@kde.org>
    SPDX-FileCopyrightText: 2010 Marco Martin <mart@kde.org>
    SPDX-FileCopyrightText: 2013 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef COREBINDINGSPLUGIN_H
#define COREBINDINGSPLUGIN_H

#include <QQmlEngine>
#include <QQmlExtensionPlugin>
#include <QQmlPropertyMap>

#include <Plasma/Plasma>
#include <qqmlintegration.h>

#include "action.h"
#include "applet.h"
#include "appletpopup.h"
#include "containment.h"
#include "corona.h"
#include "dialog.h"

struct TypesForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Types)
    QML_UNCREATABLE("")
    QML_FOREIGN(Plasma::Types)
};

struct PropertyMapForeign {
    Q_GADGET
    QML_ANONYMOUS
    QML_FOREIGN(QQmlPropertyMap)
};

/*!
 * \qmltype Window
 * \inqmlmodule org.kde.plasma.core
 * \nativetype PlasmaQuick::PlasmaWindow
 *
 * \brief Creates a QQuickWindow themed in a Plasma style with background.
 */

/*!
 * \qmlproperty Item Window::mainItem
 *
 * The main QML item that will be displayed in the Dialog
 */

/*!
 * \qmlproperty BackgroundHints Window::backgroundHints
 *
 * Defines the background used for the window
 */

/*!
 * \qmlproperty real Window::topPadding
 */

/*!
 * \qmlproperty real Window::bottomPadding
 */

/*!
 * \qmlproperty real Window::leftPadding
 */

/*!
 * \qmlproperty real Window::rightPadding
 */

struct PlasmaWindowForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Window)
    QML_FOREIGN(PlasmaQuick::PlasmaWindow)
};

/*!
 * \qmltype AppletPopup
 * \inqmlmodule org.kde.plasma.core
 * \inherits PopupPlasmaWindow
 * \nativetype PlasmaQuick::AppletPopup
 *
 * \brief Shows a popup for an applet either in the panel or on the desktop.
 *
 * In addition to the new API this class is resizable and can forward any input events received
 * on the margin to the main item
 *
 * Size hints are transferred from the mainItem's size hints.
 *
 */

/*!
 * \qmlproperty Item AppletPopup::appletInterface
 *
 * This property holds a pointer to the AppletInterface used by
 */

/*!
 * \qmlproperty bool AppletPopup::hideOnWindowDeactivate
 *
 * Whether the dialog should be hidden when the dialog loses focus.
 *
 * The default value is \c false.
 */

struct AppletPopupForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(AppletPopup)
    QML_FOREIGN(PlasmaQuick::AppletPopup)
};

/*!
 * \qmltype PopupPlasmaWindow
 * \inqmlmodule org.kde.plasma.core
 * \nativetype PlasmaQuick::PopupPlasmaWindow
 *
 * \brief Styled Plasma window that can be positioned
 * relative to an existing Item on another window.
 *
 * When shown the popup is placed correctly.
 *
 * On Wayland this is currently an XdgTopLevel with the PlasmaShellSurface used on top.
 * Do not rely on that implementation detail.
 */

/*!
 * \qmlproperty Item PopupPlasmaWindow::visualParent
 * The anchor item to place the popup relative to.
 */

/*!
 * \qmlproperty Qt.Edge PopupPlasmaWindow::popupDirection
 * Defines the default direction to place the popup relative to the visualParent.
 */

/*!
 * \qmlproperty Qt.Edge PopupPlasmaWindow::effectivePopupDirection
 * Defines the direction the popup was placed relative to the visualParent.
 * This property is read-only and is updated when the popup is shown.
 * The value whilst the popup is hidden is undefined.
 */

/*!
 * \qmlproperty bool PopupPlasmaWindow::floating
 * Defines whether the popup can appear (float) over the parent window. The default is false.
 */

/*!
 * \qmlproperty bool PopupPlasmaWindow::animated
 * Defines whether the popup is animated on show and close. The default is false.
 */

/*!
 * \qmlproperty RemoveBorders PopupPlasmaWindow::removeBorderStrategy
 * Defines which borders should be enabled/disabled when the popup is shown. The default is to show all borders
 */

/*!
 * \qmlproperty int PopupPlasmaWindow::margin
 * If set provides a gap between the parent window and all screen edges
 */

struct PopupPlasmaWindowForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(PopupPlasmaWindow)
    QML_FOREIGN(PlasmaQuick::PopupPlasmaWindow)
};

/*!
 * \qmltype Dialog
 * \inqmlmodule org.kde.plasma.core
 * \nativetype PlasmaQuick::Dialog
 * \inherits QtQuick.Window
 *
 * \brief Creates a Plasma themed top level window that can contain any QML component.
 *
 * It can be automatically positioned relative to a visual parent
 * The dialog will resize to the size of the main item
 *
 * \qml
 *  import QtQuick 2.0
 *  import org.kde.plasma.core as PlasmaCore
 *  Item {
 *     PlasmaCore.Dialog {
 *          visible: true
 *          mainItem: Item {
 *              width: 500
 *              height: 500
 *
 *              Text {
 *                  anchors.centerIn: parent
 *                  color: "red"
 *                  text: "text"
 *              }
 *          }
 *      }
 * }
 * \endqml
 */

// TODO QML documentation for dialog

struct DialogForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Dialog)
    QML_FOREIGN(PlasmaQuick::Dialog)
};

struct ContainmentForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Containment)
    QML_UNCREATABLE("")
    QML_FOREIGN(Plasma::Containment)
};

struct CoronaForeign {
    Q_GADGET
    QML_ANONYMOUS
    QML_FOREIGN(Plasma::Corona)
};

struct AppletForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Applet)
    QML_UNCREATABLE("")
    QML_FOREIGN(Plasma::Applet)
};

struct ActionForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Action)
    QML_FOREIGN(QAction)
    QML_EXTENDED(ActionExtension)
};

struct MenuForeign {
    Q_GADGET
    QML_ANONYMOUS
    QML_FOREIGN(QMenu)
};

class CoreBindingsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void initializeEngine(QQmlEngine *engine, const char *uri) override;
    void registerTypes(const char *uri) override;
};

#endif
