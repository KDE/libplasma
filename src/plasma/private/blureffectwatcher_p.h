/*
    SPDX-FileCopyrightText: 2011 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ContrastEffectWatcher_P_H
#define ContrastEffectWatcher_P_H

#include <QGuiApplication>
#include <QObject>

namespace Plasma
{
class BackgroundEffectManager;

class BlurEffectWatcher : public QObject
{
    Q_OBJECT

public:
    explicit BlurEffectWatcher(QObject *parent = nullptr);
    ~BlurEffectWatcher();
    bool isEffectActive() const;

protected:
Q_SIGNALS:
    void effectChanged(bool on);

private:
    std::unique_ptr<BackgroundEffectManager> m_backgroundEffectManager;
    bool m_effectActive = false;
};

} // namespace Plasma

#endif
