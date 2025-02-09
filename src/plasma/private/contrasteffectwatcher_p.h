/*
    SPDX-FileCopyrightText: 2011 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ContrastEffectWatcher_P_H
#define ContrastEffectWatcher_P_H

#include <QGuiApplication>
#include <QObject>

#include <QAbstractNativeEventFilter>

#include <config-plasma.h>

#if HAVE_X11
    #include <xcb/xcb.h>
#endif

namespace Plasma
{
class ContrastManager;

class ContrastEffectWatcher : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    explicit ContrastEffectWatcher(QObject *parent = nullptr);
    ~ContrastEffectWatcher() override;
    bool isEffectActive() const;

protected:
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *) override;

Q_SIGNALS:
    void effectChanged(bool on);

private:
    void init();
    bool fetchEffectActive() const;
#if HAVE_X11
    xcb_atom_t m_property;
    QNativeInterface::QX11Application *m_x11Interface = nullptr;
#endif
    std::unique_ptr<ContrastManager> m_contrastManager;
    bool m_effectActive = false;
};

} // namespace Plasma

#endif
