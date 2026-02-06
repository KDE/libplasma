/*
    SPDX-FileCopyrightText: 2023 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "popupplasmawindow.h"

#include <QQuickItem>

#include <plasmaquick/plasmaquick_export.h>

namespace PlasmaQuick
{

class AppletQuickItem;
class LayoutChangedProxy;

/*!
 * \class PlasmaQuick::AppletPopup
 * \inheaderfile PlasmaQuick/AppletPopup
 * \inmodule PlasmaQuick
 *
 * \brief The AppletPopup class shows a popup for an applet either in the panel or on the desktop.
 *
 * In addition to the new API this class is resizable and can forward any input events received
 * on the margin to the main item
 *
 * Size hints are transferred from the mainItem's size hints.
 */
class PLASMAQUICK_EXPORT AppletPopup : public PopupPlasmaWindow
{
    Q_OBJECT
    /*!
     * \property PlasmaQuick::AppletPopup::appletInterface
     * This property holds a pointer to the AppletInterface used by
     */
    Q_PROPERTY(QQuickItem *appletInterface READ appletInterface WRITE setAppletInterface NOTIFY appletInterfaceChanged)

public:
    AppletPopup();
    ~AppletPopup() override;
    QQuickItem *appletInterface() const;
    void setAppletInterface(QQuickItem *appletInterface);

Q_SIGNALS:
    void appletInterfaceChanged();

protected:
    void hideEvent(QHideEvent *event) override;

private:
    void onMainItemChanged();
    void updateMinSize();
    void updateMaxSize();
    void updateSize();

    QPointer<AppletQuickItem> m_appletInterface;
    QPointer<QScreen> m_oldScreen;
    bool m_sizeExplicitlySetFromConfig = false;
    QScopedPointer<LayoutChangedProxy> m_layoutChangedProxy;
};

}
