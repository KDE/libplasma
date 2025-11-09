/*
    SPDX-FileCopyrightText: 2006-2007 Aaron Seigo <aseigo@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "theme.h"
#include "private/theme_p.h"

#include <QFile>
#include <QFileInfo>
#include <QMutableListIterator>
#include <QPair>
#include <QStringBuilder>
#include <QThread>
#include <QTimer>

#include "config-plasma.h"

#include <KColorScheme>
#include <KConfigGroup>
#include <KWindowEffects>
#include <QDebug>
#include <QStandardPaths>

#include "debug_p.h"

namespace Plasma
{
Theme::Theme(QObject *parent)
    : QObject(parent)
{
    if (!ThemePrivate::globalTheme) {
        ThemePrivate::globalTheme = new ThemePrivate;
        ThemePrivate::globalTheme->settingsChanged(false);
    }
    ThemePrivate::globalTheme->ref.ref();
    d = ThemePrivate::globalTheme;

    connect(d, &ThemePrivate::themeChanged, this, &Theme::themeChanged);
}

Theme::Theme(const QString &themeName, QObject *parent)
    : QObject(parent)
{
    auto &priv = ThemePrivate::themes[themeName];
    if (!priv) {
        priv = new ThemePrivate;
    }

    priv->ref.ref();
    d = priv;

    d->setThemeName(themeName, false, false);
    d->fixedName = true;
    connect(d, &ThemePrivate::themeChanged, this, &Theme::themeChanged);
}

Theme::~Theme()
{
    if (d == ThemePrivate::globalTheme) {
        if (!d->ref.deref()) {
            disconnect(ThemePrivate::globalTheme, nullptr, this, nullptr);
            delete ThemePrivate::globalTheme;
            ThemePrivate::globalTheme = nullptr;
            d = nullptr;
        }
    } else {
        if (!d->ref.deref()) {
            delete ThemePrivate::themes.take(d->themeName);
        }
    }
}

void Theme::setThemeName(const QString &themeName)
{
    if (d->themeName == themeName) {
        return;
    }

    if (d != ThemePrivate::globalTheme) {
        disconnect(QCoreApplication::instance(), nullptr, d, nullptr);
        if (!d->ref.deref()) {
            delete ThemePrivate::themes.take(d->themeName);
        }

        auto &priv = ThemePrivate::themes[themeName];
        if (!priv) {
            priv = new ThemePrivate;
        }
        priv->ref.ref();
        d = priv;
        connect(d, &ThemePrivate::themeChanged, this, &Theme::themeChanged);
    }

    d->setThemeName(themeName, true, true);
}

QString Theme::themeName() const
{
    return d->themeName;
}

QPalette Theme::palette() const
{
    return d->palette;
}

QPalette Theme::globalPalette()
{
    if (!ThemePrivate::globalTheme) {
        ThemePrivate::globalTheme = new ThemePrivate;
        ThemePrivate::globalTheme->settingsChanged(false);
    }
    return ThemePrivate::globalTheme->palette;
}

KSharedConfigPtr Theme::globalColorScheme()
{
    if (!ThemePrivate::globalTheme) {
        ThemePrivate::globalTheme = new ThemePrivate;
        ThemePrivate::globalTheme->settingsChanged(false);
    }
    return ThemePrivate::globalTheme->colors;
}

QString Theme::defaultWallpaperTheme() const
{
    return d->defaultWallpaperTheme;
}

KSharedConfigPtr Theme::colorScheme() const
{
    return d->colors;
}

QColor Theme::color(ColorRole role, Kirigami::Platform::PlatformTheme::ColorSet group) const
{
    return d->color(role, group);
}

void Theme::setUseGlobalSettings(bool useGlobal)
{
    if (d->useGlobal == useGlobal) {
        return;
    }

    d->useGlobal = useGlobal;
    d->cfg = KConfigGroup();
    d->themeName.clear();
    d->settingsChanged(true);
}

bool Theme::useGlobalSettings() const
{
    return d->useGlobal;
}

KPluginMetaData Theme::metadata() const
{
    return d->pluginMetaData;
}

bool Theme::backgroundContrastEnabled() const
{
    return d->backgroundContrastEnabled;
}

bool Theme::adaptiveTransparencyEnabled() const
{
    return d->adaptiveTransparencyEnabled;
}

qreal Theme::backgroundContrast() const
{
    if (qIsNaN(d->backgroundContrast)) {
        // Make up sensible default values, based on the background color
        // If we're using a dark background color, darken the background
        if (qGray(color(Plasma::Theme::BackgroundColor).rgb()) < 127) {
            return 0.45;
            // for a light theme lighten up the background
        } else {
            return 0.3;
        }
    }
    return d->backgroundContrast;
}

qreal Theme::backgroundIntensity() const
{
    if (qIsNaN(d->backgroundIntensity)) {
        if (qGray(color(Plasma::Theme::BackgroundColor).rgb()) < 127) {
            return 0.6;
        } else {
            return 1.4;
        }
    }
    return d->backgroundIntensity;
}

qreal Theme::backgroundSaturation() const
{
    if (qIsNaN(d->backgroundSaturation)) {
        return 1.7;
    }
    return d->backgroundSaturation;
}

bool Theme::blurBehindEnabled() const
{
    return d->blurBehindEnabled;
}

}

#include "moc_theme.cpp"
