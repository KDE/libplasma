/*
    SPDX-FileCopyrightText: 2023 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plasmatheme.h"
#include <KIconLoader>
#include <QDebug>
#include <QGuiApplication>
#include <QPalette>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickRenderControl>
#include <QQuickWindow>
#include <QScopeGuard>

#include <KColorScheme>
#include <KConfigGroup>
#include <KIconColors>

PlasmaTheme::PlasmaTheme(QObject *parent)
    : PlatformTheme(parent)
{
    setSupportsIconColoring(true);

    auto parentItem = qobject_cast<QQuickItem *>(parent);
    if (parentItem) {
        connect(parentItem, &QQuickItem::windowChanged, this, &PlasmaTheme::syncWindow);
        connect(parentItem, &QQuickItem::enabledChanged, this, &PlasmaTheme::syncColors);
        connect(parentItem, &QQuickItem::visibleChanged, this, [this, parentItem] {
            if (!parentItem->isVisible()) {
                return;
            }
            syncColors();
        });
    }

    setDefaultFont(qGuiApp->font());

    KSharedConfigPtr ptr = KSharedConfig::openConfig();
    KConfigGroup general(ptr->group(QStringLiteral("general")));

    setSmallFont(general.readEntry("smallestReadableFont", []() {
        auto smallFont = qApp->font();
#ifndef Q_OS_WIN
        if (smallFont.pixelSize() != -1) {
            smallFont.setPixelSize(smallFont.pixelSize() - 2);
        } else {
            smallFont.setPointSize(smallFont.pointSize() - 2);
        }
#endif
        return smallFont;
    }()));

    syncWindow();
    syncColors();
    connect(&m_theme, &Plasma::Theme::themeChanged, this, &PlasmaTheme::syncColors);
}

PlasmaTheme::~PlasmaTheme()
{
}

QIcon PlasmaTheme::iconFromTheme(const QString &name, const QColor &customColor)
{
    KIconColors colors(Plasma::Theme::globalPalette());
    KColorScheme colorScheme(QPalette::Active, KColorScheme::Window, Plasma::Theme::globalColorScheme());

    colors.setPositiveText(colorScheme.foreground(KColorScheme::PositiveText).color().name());
    colors.setNeutralText(colorScheme.foreground(KColorScheme::NeutralText).color().name());
    colors.setNegativeText(colorScheme.foreground(KColorScheme::NegativeText).color().name());

    if (customColor != Qt::transparent) {
        colors.setText(customColor);
    }

    return KDE::icon(name, colors);
}

void PlasmaTheme::syncWindow()
{
    if (m_window) {
        disconnect(m_window.data(), &QWindow::activeChanged, this, &PlasmaTheme::syncColors);
    }

    QWindow *window = nullptr;

    auto parentItem = qobject_cast<QQuickItem *>(parent());
    if (parentItem) {
        QQuickWindow *qw = parentItem->window();

        window = QQuickRenderControl::renderWindowFor(qw);
        if (!window) {
            window = qw;
        }
        if (qw) {
            connect(qw, &QQuickWindow::sceneGraphInitialized, this, &PlasmaTheme::syncWindow, Qt::UniqueConnection);
        }
    }
    m_window = window;

    if (window) {
        connect(m_window.data(), &QWindow::activeChanged, this, &PlasmaTheme::syncColors);
        syncColors();
    }
}

void PlasmaTheme::syncColors()
{
    if (QCoreApplication::closingDown()) {
        return;
    }

    QPalette::ColorGroup paletteGroup = (QPalette::ColorGroup)colorGroup();
    auto parentItem = qobject_cast<QQuickItem *>(parent());
    if (parentItem) {
        if (!parentItem->isVisible()) {
            return;
        }
        if (!parentItem->isEnabled()) {
            paletteGroup = QPalette::Disabled;
            // Why also check if the window is exposed?
            // in the case of QQuickWidget the window() will never be active
            // and the widgets will always have the inactive palette.
            // better to always show it active than always show it inactive
        } else if (m_window && !m_window->isActive() && m_window->isExposed()) {
            paletteGroup = QPalette::Inactive;
        }
    }

    // foreground
    if (paletteGroup == QPalette::Disabled) {
        setTextColor(m_theme.color(Plasma::Theme::DisabledTextColor, colorSet()));
    } else {
        setTextColor(m_theme.color(Plasma::Theme::TextColor, colorSet()));
    }
    setDisabledTextColor(m_theme.color(Plasma::Theme::DisabledTextColor, colorSet()));
    setHighlightedTextColor(m_theme.color(Plasma::Theme::HighlightedTextColor, colorSet()));
    // Plasma::Theme doesn't have ActiveText, use PositiveTextColor
    setActiveTextColor(m_theme.color(Plasma::Theme::PositiveTextColor, colorSet()));
    setLinkColor(m_theme.color(Plasma::Theme::LinkColor, colorSet()));
    setVisitedLinkColor(m_theme.color(Plasma::Theme::VisitedLinkColor, colorSet()));
    setNegativeTextColor(m_theme.color(Plasma::Theme::NegativeTextColor, colorSet()));
    setNeutralTextColor(m_theme.color(Plasma::Theme::NeutralTextColor, colorSet()));
    setPositiveTextColor(m_theme.color(Plasma::Theme::PositiveTextColor, colorSet()));

    // background
    setBackgroundColor(m_theme.color(Plasma::Theme::BackgroundColor, colorSet()));
    setHighlightColor(m_theme.color(Plasma::Theme::HighlightColor, colorSet()));
    // Plasma::Theme doesn't have AlternateBackground
    setAlternateBackgroundColor(m_theme.color(Plasma::Theme::BackgroundColor, colorSet()));
    setActiveBackgroundColor(m_theme.color(Plasma::Theme::ActiveBackgroundColor, colorSet()));
    // Plasma::Theme doesn't have link backgrounds.
    setLinkBackgroundColor(m_theme.color(Plasma::Theme::BackgroundColor, colorSet()));
    setVisitedLinkBackgroundColor(m_theme.color(Plasma::Theme::BackgroundColor, colorSet()));
    setNegativeBackgroundColor(m_theme.color(Plasma::Theme::NegativeBackgroundColor, colorSet()));
    setNeutralBackgroundColor(m_theme.color(Plasma::Theme::NeutralBackgroundColor, colorSet()));
    setPositiveBackgroundColor(m_theme.color(Plasma::Theme::PositiveBackgroundColor, colorSet()));

    // decoration
    setHoverColor(m_theme.color(Plasma::Theme::HoverColor, colorSet()));
    setFocusColor(m_theme.color(Plasma::Theme::FocusColor, colorSet()));
    setFrameContrast(KColorScheme::frameContrast());
}

void PlasmaTheme::syncFrameContrast()
{
    if (QCoreApplication::closingDown()) {
        return;
    }

    QPalette::ColorGroup paletteGroup = (QPalette::ColorGroup)colorGroup();
    auto parentItem = qobject_cast<QQuickItem *>(parent());
    if (parentItem) {
        if (!parentItem->isVisible()) {
            return;
        }
        if (!parentItem->isEnabled()) {
            paletteGroup = QPalette::Disabled;
        } else if (m_window && !m_window->isActive() && m_window->isExposed()) {
            paletteGroup = QPalette::Inactive;
        }
    }

    if (paletteGroup == QPalette::Disabled) {
        setTextColor(m_theme.color(Plasma::Theme::DisabledTextColor, colorSet()));
    } else {
        setTextColor(m_theme.color(Plasma::Theme::TextColor, colorSet()));
    }
    setBackgroundColor(m_theme.color(Plasma::Theme::BackgroundColor, colorSet()));
    setFrameContrast(KColorScheme::frameContrast());
}

bool PlasmaTheme::event(QEvent *event)
{
    if (event->type() == Kirigami::Platform::PlatformThemeEvents::ColorSetChangedEvent::type) {
        syncColors();
    }

    if (event->type() == Kirigami::Platform::PlatformThemeEvents::ColorGroupChangedEvent::type) {
        syncColors();
    }

    if (event->type() == Kirigami::Platform::PlatformThemeEvents::FrameContrastChangedEvent::type) {
        syncFrameContrast();
    }

    return PlatformTheme::event(event);
}

#include "moc_plasmatheme.cpp"
