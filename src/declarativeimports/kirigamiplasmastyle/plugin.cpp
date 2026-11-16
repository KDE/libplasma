/*
    SPDX-FileCopyrightText: 2017 Marco Martin <mart@kde.org>
    SPDX-FileCopyrightText: 2021 Arjen Hiemstra <ahiemstra@heimr.nl>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugin.h"
#include "plasmatheme.h"
#include "units.h"
#include <KConfigWatcher>
#include <StyleHints>

using namespace Qt::StringLiterals;

class KirigamiStyleHints : public Kirigami::Platform::StyleHints
{
public:
    KirigamiStyleHints(QObject *parent = nullptr)
        : Kirigami::Platform::StyleHints(parent)
        , m_globalsWatcher(KConfigWatcher::create(KSharedConfig::openConfig()))
    {
        setScrollingModifiesControls(KConfigGroup(KSharedConfig::openConfig(), u"KDE"_s).readEntry(u"ScrollingModifiesControls"_s, false));
        connect(m_globalsWatcher.data(), &KConfigWatcher::configChanged, this, [this](const KConfigGroup &group, const QByteArrayList &names) {
            if (group.name() == "KDE"_L1) {
                if (names.contains("ScrollingModifiesControls"_ba)) {
                    setScrollingModifiesControls(group.readEntry(u"ScrollingModifiesControls"_s, false));
                }
            }
        });
    }

private:
    KConfigWatcher::Ptr m_globalsWatcher;
};

Plugin::Plugin(QObject *parent)
    : Kirigami::Platform::PlatformPluginFactory(parent)
{
    Kirigami::Platform::StyleHints::setMakeStyleHintsFunction([](QObject *object) {
        return new KirigamiStyleHints(object);
    });
}

Plugin::~Plugin() = default;

Kirigami::Platform::PlatformTheme *Plugin::createPlatformTheme(QObject *parent)
{
    Q_UNUSED(parent);
    // TODO: Implement a proper C++ version of PlatformTheme. This relies on fallback
    // behaviour in Kirigami to load the Theme.qml file.
    return new PlasmaTheme(parent);
}

Kirigami::Platform::Units *Plugin::createUnits(QObject *parent)
{
    Q_ASSERT(parent);
    return new Units(parent);
}

#include "moc_plugin.cpp"
