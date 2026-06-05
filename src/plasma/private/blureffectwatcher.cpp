/*
    SPDX-FileCopyrightText: 2011 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "blureffectwatcher_p.h"

#include <QWaylandClientExtensionTemplate>

#include "qwayland-ext-background-effect-v1.h"

namespace Plasma
{

class BackgroundEffectManager : public QWaylandClientExtensionTemplate<BackgroundEffectManager>, public QtWayland::ext_background_effect_manager_v1
{
    Q_OBJECT
public:
    BackgroundEffectManager()
        : QWaylandClientExtensionTemplate<BackgroundEffectManager>(1)
    {
        initialize();
        connect(this, &BackgroundEffectManager::activeChanged, this, [this]() {
            if (!isActive()) {
                m_supportsBlur = false;
                Q_EMIT capabilitiesChanged();
            }
        });
    }
    ~BackgroundEffectManager()
    {
        if (isActive()) {
            destroy();
        }
    }

    void ext_background_effect_manager_v1_capabilities(uint32_t flags) override
    {
        m_supportsBlur = flags & capability_blur;
        Q_EMIT capabilitiesChanged();
    }

    bool m_supportsBlur = false;

Q_SIGNALS:
    void capabilitiesChanged();
};

BlurEffectWatcher::BlurEffectWatcher(QObject *parent)
    : QObject(parent)
{
    // TODO: New KWindowEffects APIs are needed.
    static bool insideKwin = QGuiApplication::platformName() == QLatin1String("wayland-org.kde.kwin.qpa");
    if (insideKwin) {
        m_effectActive = true;
    } else {
        m_backgroundEffectManager = std::make_unique<BackgroundEffectManager>();
        connect(m_backgroundEffectManager.get(), &BackgroundEffectManager::capabilitiesChanged, this, [this]() {
            m_effectActive = m_backgroundEffectManager->m_supportsBlur;
            Q_EMIT effectChanged(m_effectActive);
        });
        m_effectActive = m_backgroundEffectManager->m_supportsBlur;
    }
}

BlurEffectWatcher::~BlurEffectWatcher()
{
}

bool BlurEffectWatcher::isEffectActive() const
{
    return m_effectActive;
}

} // namespace Plasma

#include "blureffectwatcher.moc"
