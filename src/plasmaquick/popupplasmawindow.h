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

/**
 * @brief The PopupPlasmaWindow class is a styled Plasma window that can be positioned
 * relative to an existing Item on another window. When shown the popup is placed correctly.
 *
 * On Wayland this is currently an XdgTopLevel with the PlasmaShellSurface used on top.
 * Do not rely on that implementation detail
 */
class PLASMAQUICK_EXPORT PopupPlasmaWindow : public PlasmaWindow
{
    Q_OBJECT

    /**
     * The anchor item to place the popup relative to.
     */
    Q_PROPERTY(QQuickItem *visualParent READ visualParent WRITE setVisualParent NOTIFY visualParentChanged)

    /**
     * Defines the default direction to place the popup relative to the visualParent.
     */
    Q_PROPERTY(Qt::Edge popupDirection READ popupDirection WRITE setPopupDirection NOTIFY popupDirectionChanged)

    /**
     * Defines the direction the popup was placed relative to the visualParent.
     * This property is read-only and is updated when the popup is shown.
     * The value whilst the popup is hidden is undefined.
     */
    Q_PROPERTY(Qt::Edge effectivePopupDirection READ effectivePopupDirection NOTIFY effectivePopupDirectionChanged)

    /**
     * Defines whether the popup can appaer (float) over the parent window. The default is false.
     */
    Q_PROPERTY(bool floating READ floating WRITE setFloating NOTIFY floatingChanged)

    /**
     * Defines whether the popup is animated on show and close. The default is false.
     */
    Q_PROPERTY(bool animated READ animated WRITE setAnimated NOTIFY animatedChanged)

    /**
     * Defines which borders should be enabled/disabled when the popup is shown. The default is to show all borders
     */
    Q_PROPERTY(RemoveBorders removeBorderStrategy READ removeBorderStrategy WRITE setRemoveBorderStrategy NOTIFY removeBorderStrategyChanged)

    /**
     * If set provides a gap between the parent window and all screen edges
     */
    Q_PROPERTY(int margin READ margin WRITE setMargin NOTIFY marginChanged)

public:
    enum RemoveBorder { Never = 0x0, AtScreenEdges = 0x1, AtPanelEdges = 0x2 };
    Q_DECLARE_FLAGS(RemoveBorders, RemoveBorder)
    Q_ENUM(RemoveBorder);

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

Q_SIGNALS:
    void visualParentChanged();
    void popupDirectionChanged();
    void effectivePopupDirectionChanged();
    void floatingChanged();
    void animatedChanged();
    void removeBorderStrategyChanged();
    void marginChanged();

protected Q_SLOTS:
    void queuePositionUpdate();

private:
    Q_PRIVATE_SLOT(d, void updateVisualParentWindow())

    friend class PopupPlasmaWindowPrivate;
    const std::unique_ptr<PopupPlasmaWindowPrivate> d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(PopupPlasmaWindow::RemoveBorders)
}
