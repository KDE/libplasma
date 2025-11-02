/*
    SPDX-FileCopyrightText: 2006-2007 Aaron Seigo <aseigo@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "quicktheme.h"

namespace Plasma
{
QuickTheme::QuickTheme(QObject *parent)
    : Theme(parent)
{
    connect(this, &Theme::themeChanged, this, &QuickTheme::themeChangedProxy);
}

QuickTheme::~QuickTheme()
{
}

QColor QuickTheme::textColor() const
{
    return Plasma::Theme::color(Plasma::Theme::TextColor);
}

QColor QuickTheme::highlightColor() const
{
    return Plasma::Theme::color(Plasma::Theme::HighlightColor);
}

QColor QuickTheme::highlightedTextColor() const
{
    return Plasma::Theme::color(Plasma::Theme::HighlightedTextColor);
}

QColor QuickTheme::positiveTextColor() const
{
    return Plasma::Theme::color(Plasma::Theme::PositiveTextColor);
}

QColor QuickTheme::neutralTextColor() const
{
    return Plasma::Theme::color(Plasma::Theme::NeutralTextColor);
}

QColor QuickTheme::negativeTextColor() const
{
    return Plasma::Theme::color(Plasma::Theme::NegativeTextColor);
}

QColor QuickTheme::disabledTextColor() const
{
    return Plasma::Theme::color(Plasma::Theme::DisabledTextColor);
}

QColor QuickTheme::backgroundColor() const
{
    return Plasma::Theme::color(Plasma::Theme::BackgroundColor);
}

QColor QuickTheme::buttonTextColor() const
{
    return Plasma::Theme::color(Plasma::Theme::TextColor, Kirigami::Platform::PlatformTheme::Button);
}

QColor QuickTheme::buttonBackgroundColor() const
{
    return Plasma::Theme::color(Plasma::Theme::BackgroundColor, Kirigami::Platform::PlatformTheme::Button);
}

QColor QuickTheme::buttonPositiveTextColor() const
{
    return Plasma::Theme::color(Plasma::Theme::TextColor, Kirigami::Platform::PlatformTheme::Button);
}

QColor QuickTheme::buttonNeutralTextColor() const
{
    return Plasma::Theme::color(Plasma::Theme::TextColor, Kirigami::Platform::PlatformTheme::Button);
}

QColor QuickTheme::buttonNegativeTextColor() const
{
    return Plasma::Theme::color(Plasma::Theme::TextColor, Kirigami::Platform::PlatformTheme::Button);
}

QColor QuickTheme::linkColor() const
{
    return Plasma::Theme::color(Plasma::Theme::LinkColor);
}

QColor QuickTheme::visitedLinkColor() const
{
    return Plasma::Theme::color(Plasma::Theme::VisitedLinkColor);
}

QColor QuickTheme::buttonHoverColor() const
{
    return Plasma::Theme::color(Plasma::Theme::HoverColor, Kirigami::Platform::PlatformTheme::Button);
}

QColor QuickTheme::buttonFocusColor() const
{
    return Plasma::Theme::color(Plasma::Theme::FocusColor, Kirigami::Platform::PlatformTheme::Button);
}

QColor QuickTheme::buttonHighlightedTextColor() const
{
    return Plasma::Theme::color(Plasma::Theme::HighlightedTextColor, Kirigami::Platform::PlatformTheme::Button);
}

QColor QuickTheme::viewTextColor() const
{
    return Plasma::Theme::color(Plasma::Theme::TextColor, Kirigami::Platform::PlatformTheme::View);
}

QColor QuickTheme::viewBackgroundColor() const
{
    return Plasma::Theme::color(Plasma::Theme::BackgroundColor, Kirigami::Platform::PlatformTheme::View);
}

QColor QuickTheme::viewHoverColor() const
{
    return Plasma::Theme::color(Plasma::Theme::HoverColor, Kirigami::Platform::PlatformTheme::View);
}

QColor QuickTheme::viewFocusColor() const
{
    return Plasma::Theme::color(Plasma::Theme::FocusColor, Kirigami::Platform::PlatformTheme::View);
}

QColor QuickTheme::viewHighlightedTextColor() const
{
    return Plasma::Theme::color(Plasma::Theme::HighlightedTextColor, Kirigami::Platform::PlatformTheme::View);
}

QColor QuickTheme::viewPositiveTextColor() const
{
    return Plasma::Theme::color(Plasma::Theme::TextColor, Kirigami::Platform::PlatformTheme::View);
}

QColor QuickTheme::viewNeutralTextColor() const
{
    return Plasma::Theme::color(Plasma::Theme::TextColor, Kirigami::Platform::PlatformTheme::View);
}

QColor QuickTheme::viewNegativeTextColor() const
{
    return Plasma::Theme::color(Plasma::Theme::TextColor, Kirigami::Platform::PlatformTheme::View);
}

QColor QuickTheme::complementaryTextColor() const
{
    return Plasma::Theme::color(Plasma::Theme::TextColor, Kirigami::Platform::PlatformTheme::Complementary);
}

QColor QuickTheme::complementaryBackgroundColor() const
{
    return Plasma::Theme::color(Plasma::Theme::BackgroundColor, Kirigami::Platform::PlatformTheme::Complementary);
}

QColor QuickTheme::complementaryHoverColor() const
{
    return Plasma::Theme::color(Plasma::Theme::HoverColor, Kirigami::Platform::PlatformTheme::Complementary);
}

QColor QuickTheme::complementaryFocusColor() const
{
    return Plasma::Theme::color(Plasma::Theme::FocusColor, Kirigami::Platform::PlatformTheme::Complementary);
}

QColor QuickTheme::complementaryHighlightedTextColor() const
{
    return Plasma::Theme::color(Plasma::Theme::HighlightedTextColor, Kirigami::Platform::PlatformTheme::Complementary);
}

QColor QuickTheme::complementaryPositiveTextColor() const
{
    return Plasma::Theme::color(Plasma::Theme::TextColor, Kirigami::Platform::PlatformTheme::Complementary);
}

QColor QuickTheme::complementaryNeutralTextColor() const
{
    return Plasma::Theme::color(Plasma::Theme::TextColor, Kirigami::Platform::PlatformTheme::Complementary);
}

QColor QuickTheme::complementaryNegativeTextColor() const
{
    return Plasma::Theme::color(Plasma::Theme::TextColor, Kirigami::Platform::PlatformTheme::Complementary);
}

QColor QuickTheme::headerTextColor() const
{
    return Plasma::Theme::color(Plasma::Theme::TextColor, Kirigami::Platform::PlatformTheme::Header);
}

QColor QuickTheme::headerBackgroundColor() const
{
    return Plasma::Theme::color(Plasma::Theme::BackgroundColor, Kirigami::Platform::PlatformTheme::Header);
}

QColor QuickTheme::headerHoverColor() const
{
    return Plasma::Theme::color(Plasma::Theme::HoverColor, Kirigami::Platform::PlatformTheme::Header);
}

QColor QuickTheme::headerFocusColor() const
{
    return Plasma::Theme::color(Plasma::Theme::FocusColor, Kirigami::Platform::PlatformTheme::Header);
}

QColor QuickTheme::headerHighlightedTextColor() const
{
    return Plasma::Theme::color(Plasma::Theme::HighlightedTextColor, Kirigami::Platform::PlatformTheme::Header);
}

QColor QuickTheme::headerPositiveTextColor() const
{
    return Plasma::Theme::color(Plasma::Theme::TextColor, Kirigami::Platform::PlatformTheme::Header);
}

QColor QuickTheme::headerNeutralTextColor() const
{
    return Plasma::Theme::color(Plasma::Theme::TextColor, Kirigami::Platform::PlatformTheme::Header);
}

QColor QuickTheme::headerNegativeTextColor() const
{
    return Plasma::Theme::color(Plasma::Theme::TextColor, Kirigami::Platform::PlatformTheme::Header);
}
}

#include "moc_quicktheme.cpp"
