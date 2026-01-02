/*
    SPDX-FileCopyrightText: 2023 David Edmundson <davidedmundson@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "plasmawindow.h"

#include <QQuickItem>
#include <plasmaquick/plasmaquick_export.h>

namespace PlasmaQuick
{
class PopupPlasmaWindowPrivate;

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

/*!
 * \class PlasmaQuick::PopupPlasmaWindow
 * \inheaderfile PlasmaQuick/PopupPlasmaWindow
 * \inmodule PlasmaQuick
 *
 * \brief Styled Plasma window that can be positioned
 * relative to an existing Item on another window.
 *
 * When shown the popup is placed correctly.
 *
 * On Wayland this is currently an XdgTopLevel with the PlasmaShellSurface used on top.
 * Do not rely on that implementation detail
 */
class PLASMAQUICK_EXPORT PopupPlasmaWindow : public PlasmaWindow
{
    Q_OBJECT

    /*!
     * \property PlasmaQuick::PopupPlasmaWindow::visualParent
     * The anchor item to place the popup relative to.
     */
    Q_PROPERTY(QQuickItem *visualParent READ visualParent WRITE setVisualParent NOTIFY visualParentChanged)

    /*!
     * \property PlasmaQuick::PopupPlasmaWindow::popupDirection
     * Defines the default direction to place the popup relative to the visualParent.
     */
    Q_PROPERTY(Qt::Edge popupDirection READ popupDirection WRITE setPopupDirection NOTIFY popupDirectionChanged)

    /*!
     * \property PlasmaQuick::PopupPlasmaWindow::effectivePopupDirection
     * Defines the direction the popup was placed relative to the visualParent.
     * This property is read-only and is updated when the popup is shown.
     * The value whilst the popup is hidden is undefined.
     */
    Q_PROPERTY(Qt::Edge effectivePopupDirection READ effectivePopupDirection NOTIFY effectivePopupDirectionChanged)

    /*!
     * \property PlasmaQuick::PopupPlasmaWindow::floating
     * Defines whether the popup can appear (float) over the parent window. The default is false.
     */
    Q_PROPERTY(bool floating READ floating WRITE setFloating NOTIFY floatingChanged)

    /*!
     * \property PlasmaQuick::PopupPlasmaWindow::animated
     * Defines whether the popup is animated on show and close. The default is false.
     */
    Q_PROPERTY(bool animated READ animated WRITE setAnimated NOTIFY animatedChanged)

    /*!
     * \property PlasmaQuick::PopupPlasmaWindow::removeBorderStrategy
     * Defines which borders should be enabled/disabled when the popup is shown. The default is to show all borders
     */
    Q_PROPERTY(RemoveBorders removeBorderStrategy READ removeBorderStrategy WRITE setRemoveBorderStrategy NOTIFY removeBorderStrategyChanged)

    /*!
     * \property PlasmaQuick::PopupPlasmaWindow::margin
     * If set provides a gap between the parent window and all screen edges
     */
    Q_PROPERTY(int margin READ margin WRITE setMargin NOTIFY marginChanged)

public:
    /*!
     * \value Never
     * \value AtScreenEdges
     * \value AtPanelEdges
     */
    /*!
     * \qmlproperty enumeration PopupPlasmaWindow::RemoveBorder
     * \value Never
     * \value AtScreenEdges
     * \value AtPanelEdges
     */
    enum RemoveBorder {
        Never = 0x0,
        AtScreenEdges = 0x1,
        AtPanelEdges = 0x2
    };
    Q_DECLARE_FLAGS(RemoveBorders, RemoveBorder)
    Q_FLAG(RemoveBorders);

    /*!
     * Constructor
     */
    PopupPlasmaWindow(const QString &svgPrefix = QStringLiteral("dialogs/background"));
    ~PopupPlasmaWindow() override;
    QQuickItem *visualParent() const;
    void setVisualParent(QQuickItem *parent);

    Qt::Edge popupDirection() const;
    void setPopupDirection(Qt::Edge popupDirection);

    Qt::Edge effectivePopupDirection() const;

    bool floating() const;
    void setFloating(bool floating);

    bool animated() const;
    void setAnimated(bool animated);

    RemoveBorders removeBorderStrategy() const;
    void setRemoveBorderStrategy(RemoveBorders borders);

    int margin() const;
    void setMargin(int margin);

    bool event(QEvent *event) override;

    // Popups that have positive margin won't be directly touching
    // any screen edge or panel, so they will not have disabled borders.
    // However, knowing which borders the margin leans on is still
    // useful, as an example, to correctly set the resize handle edges.
    Qt::Edges nearbyBorders() const;

Q_SIGNALS:
    void visualParentChanged();
    void popupDirectionChanged();
    void effectivePopupDirectionChanged();
    void floatingChanged();
    void animatedChanged();
    void removeBorderStrategyChanged();
    void marginChanged();
    void nearbyBordersChanged();

protected Q_SLOTS:
    void queuePositionUpdate();

private:
    Q_PRIVATE_SLOT(d, void updateVisualParentWindow())

    friend class PopupPlasmaWindowPrivate;
    const std::unique_ptr<PopupPlasmaWindowPrivate> d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(PopupPlasmaWindow::RemoveBorders)
}
