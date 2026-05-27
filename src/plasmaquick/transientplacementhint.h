/*
    SPDX-FileCopyrightText: 2023 David Edmundson <davidedmundson@kde.org>
    SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QRect>
#include <QSharedDataPointer>
#include <QtGlobal>
#include <plasmaquick/plasmaquick_export.h>

namespace PlasmaQuick
{
class TransientPlacementHintPrivate;

/*!
 * \class PlasmaQuick::TransientPlacementHint
 * \inheaderfile PlasmaQuick/TransientPlacementHint
 * \inmodule PlasmaQuick
 *
 * \brief Placement rules for transient popups, effectively mirroring the XDGShell positioner but with Qt types and gadget friendly APIs.
 */
class PLASMAQUICK_EXPORT TransientPlacementHint
{
public:
    /*! Constructs an empty placement hint. */
    TransientPlacementHint();
    /*! Destroys the placement hint. */
    ~TransientPlacementHint();
    /*! Copies another placement hint. */
    TransientPlacementHint(const TransientPlacementHint &other);
    /*! Assigns the contents of another placement hint. */
    TransientPlacementHint &operator=(const TransientPlacementHint &other);
    /*! Returns whether the hint is valid or not. */
    bool isValid() const;
    /*! Sets the anchor rectangle within the parent surface. */
    void setParentAnchorArea(const QRect &parentAnchorRect);
    /*! Returns the anchor rectangle within the parent surface. */
    QRect parentAnchorArea() const;
    /*! Sets which corner/edge of the parent anchor area the popup is positioned relative to. */
    void setParentAnchor(Qt::Edges parentAnchor);
    /*! Returns which corner/edge of the parent anchor area the popup is positioned relative to. */
    Qt::Edges parentAnchor() const;
    /*! Sets which corner/edge of the popup is placed at the parent anchor point. */
    void setPopupAnchor(Qt::Edges popupAnchor);
    /*! Returns which corner/edge of the popup is placed at the parent anchor point. */
    Qt::Edges popupAnchor() const;
    /*! Returns whether anchor Rect is padded so that the popup appears outside the window of the transient parent. */
    bool constrainByAnchorWindow() const;
    /*! Sets whether anchor Rect is padded so that the popup appears outside the window of the transient parent. */
    void setConstrainByAnchorWindow(bool constrainByAnchorWindow);
    /*! Returns which axes may slide to keep the popup unconstrained. */
    Qt::Orientations slideConstraintAdjustments() const;
    void setSlideConstraintAdjustments(Qt::Orientations slideConstraintAdjustments);
    /*! Returns which axes may flip anchor and popup gravity to stay unconstrained. */
    Qt::Orientations flipConstraintAdjustments() const;
    /*! Sets which axes may flip anchor and popup gravity to stay unconstrained. */
    void setFlipConstraintAdjustments(Qt::Orientations flipConstraintAdjustments);
    /*! Returns the margin applied around the popup during placement. */
    int margin() const;
    /*! Sets the margin applied around the popup during placement. */
    void setMargin(int margin);

private:
    QSharedDataPointer<TransientPlacementHintPrivate> d;
};
}
