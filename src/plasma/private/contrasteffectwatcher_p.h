/*
    SPDX-FileCopyrightText: 2011 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ContrastEffectWatcher_P_H
#define ContrastEffectWatcher_P_H

#include <QGuiApplication>
#include <QObject>

#include <QAbstractNativeEventFilter>

#include <xcb/xcb.h>

#include <config-plasma.h>

namespace Plasma
{

class ContrastEffectWatcher : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    explicit ContrastEffectWatcher(QObject *parent = nullptr);
    bool isEffectActive() const;

protected:
#if HAVE_X11
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *) override;
#endif

Q_SIGNALS:
    void effectChanged(bool on);

private:
    void init();
    bool fetchEffectActive() const;
#if HAVE_X11
    xcb_atom_t m_property;
    QNativeInterface::QX11Application *m_x11Interface = nullptr;
#endif
    bool m_isWayland = false;
    bool m_effectActive = false;
};

} // namespace Plasma

#endif
