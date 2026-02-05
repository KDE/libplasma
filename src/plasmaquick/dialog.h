/*
    SPDX-FileCopyrightText: 2011 Marco Martin <mart@kde.org>
    SPDX-FileCopyrightText: 2013 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef DIALOG_PROXY_P
#define DIALOG_PROXY_P

#include <QPoint>
#include <QQmlParserStatus>
#include <QQuickItem>
#include <QQuickWindow>
#include <QScopedPointer>

#include <Plasma/Plasma>
#include <Plasma/Theme>

#include <netwm_def.h>

#include <plasmaquick/plasmaquick_export.h>

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

class QQuickItem;
class QScreen;

namespace PlasmaQuick
{
class DialogPrivate;

/*!
 * \class PlasmaQuick::Dialog
 * \inheaderfile PlasmaQuick::Dialog
 * \inmodule PlasmaQuick
 *
 * \brief Creates a Plasma themed top level window that can contain any QML component.
 *
 * It can be automatically positioned relative to a visual parent
 * The dialog will resize to the size of the main item
 *
 */
class PLASMAQUICK_EXPORT Dialog : public QQuickWindow, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    /*!
     * \property PlasmaQuick::Dialog::mainItem
     * The main QML item that will be displayed in the Dialog
     */
    Q_PROPERTY(QQuickItem *mainItem READ mainItem WRITE setMainItem NOTIFY mainItemChanged)

    /*!
     * \property PlasmaQuick::Dialog::visualParent
     * The main QML item that will be displayed in the Dialog
     */
    Q_PROPERTY(QQuickItem *visualParent READ visualParent WRITE setVisualParent NOTIFY visualParentChanged)

    /*!
     * \property PlasmaQuick::Dialog::margins
     * Margins of the dialog around the mainItem.
     * \sa DialogMargins
     */
    Q_PROPERTY(QObject *margins READ margins CONSTANT)

    /*!
     * \property PlasmaQuick::Dialog::inset
     * Margins where the dialog background actually starts, excluiding things like shadows or borders
     * \sa DialogMargins
     * \since 5.77
     */
    Q_PROPERTY(QObject *inset READ inset CONSTANT)

    /*!
     * \property PlasmaQuick::Dialog::location
     * Plasma Location of the dialog window.
     *
     * Useful if this dialog is a popup for a panel.
     */
    Q_PROPERTY(Plasma::Types::Location location READ location WRITE setLocation NOTIFY locationChanged)

    /*!
     * \property PlasmaQuick::Dialog::type
     * Type of the window
     */
    Q_PROPERTY(WindowType type READ type WRITE setType NOTIFY typeChanged)

    /*!
     * \property PlasmaQuick::Dialog::hideOnWindowDeactivate
     * Whether the dialog should be hidden when the dialog loses focus.
     *
     * The default value is \c false.
     **/
    Q_PROPERTY(bool hideOnWindowDeactivate READ hideOnWindowDeactivate WRITE setHideOnWindowDeactivate NOTIFY hideOnWindowDeactivateChanged)

    /*!
     * \property PlasmaQuick::Dialog::outputOnly
     * Whether the dialog is output only. Default value is \c false. If it is \c true
     * the dialog does not accept input and all pointer events are not accepted, thus the dialog
     * is click through.
     *
     * This property is currently only supported on the X11 platform. On any other platform the
     * property has no effect.
     **/
    Q_PROPERTY(bool outputOnly READ isOutputOnly WRITE setOutputOnly NOTIFY outputOnlyChanged)

    /*!
     * \property PlasmaQuick::Dialog::flags
     * This property holds the window flags of the window.
     * The window flags control the window's appearance in the windowing system,
     * whether it's a dialog, popup, or a regular window, and whether it should
     * have a title bar, etc.
     * Regardless to what the user sets, the flags will always have the
     * FramelessWindowHint flag set
     */
    Q_PROPERTY(Qt::WindowFlags flags READ flags WRITE setFramelessFlags NOTIFY flagsChanged)

    /*!
     * \property PlasmaQuick::Dialog::backgroundHints
     * This property holds how (and if at all) the dialog should draw its own background
     * or if it is complete responsibility of the content item to render a background.
     * Note that in case of NoBackground it loses kwin side shadows and blur
     */
    Q_PROPERTY(BackgroundHints backgroundHints READ backgroundHints WRITE setBackgroundHints NOTIFY backgroundHintsChanged)

    /*!
     * \property PlasmaQuick::Dialog::visible
     */
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChangedProxy)

    /*!
     * \property PlasmaQuick::Dialog::floating
     * This property holds by how much the applet should be floating even if the location
     * is set to a certain screen side; if this value is positive, the dialog will draw
     * all four sides and maintain the required distance from the screen borders.
     */
    Q_PROPERTY(int floating READ floating WRITE setFloating NOTIFY floatingChanged)

    // TODO: plasmoidItem?
    /*!
     * \property PlasmaQuick::Dialog::appletInterface
     * This property holds a pointer to the AppletInterface used by an applet. It is
     * null when the dialog is not used for an applet.
     */
    Q_PROPERTY(QQuickItem *appletInterface READ appletInterface WRITE setAppletInterface NOTIFY appletInterfaceChanged)

    Q_CLASSINFO("DefaultProperty", "mainItem")

public:
    enum WindowType {
        Normal = NET::Normal,
        Dock = NET::Dock,
        DialogWindow = NET::Dialog,
        PopupMenu = NET::PopupMenu,
        Tooltip = NET::Tooltip,
        Notification = NET::Notification,
        OnScreenDisplay = NET::OnScreenDisplay,
        CriticalNotification = NET::CriticalNotification,
        AppletPopup = NET::AppletPopup,
    };
    Q_ENUM(WindowType)

    enum BackgroundHints {
        NoBackground = 0, /*!< Not drawing a background under the applet, the dialog has its own implementation */
        StandardBackground = 1, /*!< The standard background from the theme is drawn */
        SolidBackground = 2, /*!< The solid version of the background is preferred */
    };
    Q_ENUM(BackgroundHints)

    explicit Dialog(QQuickItem *parent = nullptr);
    ~Dialog() override;

    // PROPERTIES ACCESSORS
    QQuickItem *mainItem() const;
    void setMainItem(QQuickItem *mainItem);

    QQuickItem *visualParent() const;
    void setVisualParent(QQuickItem *visualParent);

    Plasma::Types::Location location() const;
    void setLocation(Plasma::Types::Location location);

    QObject *margins() const;
    QObject *inset() const;

    void setFramelessFlags(Qt::WindowFlags flags);

    void setType(WindowType type);
    WindowType type() const;

    bool hideOnWindowDeactivate() const;
    void setHideOnWindowDeactivate(bool hide);

    void setOutputOnly(bool outputOnly);
    bool isOutputOnly() const;

    BackgroundHints backgroundHints() const;
    void setBackgroundHints(BackgroundHints hints);

    bool isVisible() const;
    void setVisible(bool visible);

    int floating() const;
    void setFloating(int floating);

    QQuickItem *appletInterface() const;
    void setAppletInterface(QQuickItem *appletInterface);

    /*!
     * Returns the suggested screen position for the popup.
     *
     * \a item The item the popup has to be positioned relatively to. If null, the popup will be positioned in the center of the window.
     *
     * \a size The size that the popup will have, which influences the final position.
     */
    virtual QPoint popupPosition(QQuickItem *item, const QSize &size);

Q_SIGNALS:
    void mainItemChanged();
    void locationChanged();
    void visualParentChanged();
    void typeChanged();
    void hideOnWindowDeactivateChanged();
    void outputOnlyChanged();
    void flagsChanged();
    void floatingChanged();
    void backgroundHintsChanged();
    void visibleChangedProxy(); // redeclaration of QQuickWindow::visibleChanged
    void appletInterfaceChanged();
    /*!
     * Emitted when the hideOnWindowDeactivate property is \c true and this dialog lost focus to a
     * window that is neither a parent dialog to nor a child dialog of this dialog.
     */
    void windowDeactivated();

protected:
    /*!
     * Sets the dialog position.
     *
     * Subclasses may change it. ToolTipDialog adjusts the position in an animated way.
     */
    virtual void adjustGeometry(const QRect &geom);

    // Reimplementations
    void classBegin() override;
    void componentComplete() override;
    void resizeEvent(QResizeEvent *re) override;
    void focusInEvent(QFocusEvent *ev) override;
    void focusOutEvent(QFocusEvent *ev) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void moveEvent(QMoveEvent *) override;
    bool event(QEvent *event) override;

private:
    friend class DialogPrivate;
    const QScopedPointer<DialogPrivate> d;

    Q_PRIVATE_SLOT(d, void updateTheme())
    Q_PRIVATE_SLOT(d, void updateVisibility(bool visible))

    Q_PRIVATE_SLOT(d, void updateMinimumWidth())
    Q_PRIVATE_SLOT(d, void updateMinimumHeight())
    Q_PRIVATE_SLOT(d, void updateMaximumWidth())
    Q_PRIVATE_SLOT(d, void updateMaximumHeight())
    Q_PRIVATE_SLOT(d, void updateLayoutParameters())

    Q_PRIVATE_SLOT(d, void slotMainItemSizeChanged())
};

}

#endif
