/*
    SPDX-FileCopyrightText: 2006-2007 Aaron Seigo <aseigo@kde.org>
    SPDX-FileCopyrightText: 2013 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PLASMA_THEME_P_H
#define PLASMA_THEME_P_H

#include "theme.h"

#include <KColorScheme>
#include <KConfigGroup>
#include <KConfigWatcher>
#include <KPluginMetaData>
#include <KSvg/ImageSet>

#include <QDebug>
#include <QHash>
#include <QTimer>

#include <config-plasma.h>

#include "private/blureffectwatcher_p.h"

namespace Plasma
{
class Theme;

// NOTE: Default wallpaper can be set from the theme configuration
#define DEFAULT_WALLPAPER_THEME "default"

class ThemePrivate : public QObject, public QSharedData
{
    Q_OBJECT

public:
    explicit ThemePrivate(QObject *parent = nullptr);
    ~ThemePrivate() override;

    KConfigGroup &config();

    void scheduleThemeChangeNotification();
    void setThemeName(const QString &themeName, bool writeSettings, bool emitChanged);
    void processWallpaperSettings(const KSharedConfigPtr &metadata);
    void processContrastSettings(const KSharedConfigPtr &metadata);
    void processAdaptiveTransparencySettings(const KSharedConfigPtr &metadata);
    void processBlurBehindSettings(const KSharedConfigPtr &metadata);

    QColor color(Theme::ColorRole role, Kirigami::Platform::PlatformTheme::ColorSet group) const;

    void updateKSvgSelectors();

public Q_SLOTS:
    void colorsChanged();
    void notifyOfChanged();
    void settingsChanged(bool emitChanges);

Q_SIGNALS:
    void themeChanged();
    void applicationPaletteChange();

public:
    static const char defaultTheme[];

    static BlurEffectWatcher *s_blurEffectWatcher;

    // Ref counting of ThemePrivate instances
    static ThemePrivate *globalTheme;
    static QHash<QString, ThemePrivate *> themes;

    std::unique_ptr<KSvg::ImageSet> kSvgImageSet;
    QString themeName;
    KPluginMetaData pluginMetaData;
    QList<QString> fallbackThemes;
    KSharedConfigPtr colors;
    KColorScheme colorScheme;
    KColorScheme selectionColorScheme;
    KColorScheme buttonColorScheme;
    KColorScheme viewColorScheme;
    KColorScheme complementaryColorScheme;
    KColorScheme headerColorScheme;
    KColorScheme tooltipColorScheme;
    QPalette palette;
    bool eventFilter(QObject *watched, QEvent *event) override;
    KConfigGroup cfg;
    KConfigWatcher::Ptr plasmaRcWatcher;
    QString defaultWallpaperTheme;
    QTimer *selectorsUpdateTimer;
    QTimer *updateNotificationTimer;

    bool compositingActive : 1;
    bool backgroundContrastActive : 1;
    bool isDefault : 1;
    bool useGlobal : 1;
    bool fixedName : 1;

    qreal backgroundContrast;
    qreal backgroundIntensity;
    qreal backgroundSaturation;
    bool backgroundContrastEnabled;
    bool adaptiveTransparencyEnabled;
    bool blurBehindEnabled;

    // Version number of Plasma the Theme has been designed for
    int apiMajor;
    int apiMinor;
    int apiRevision;
};

}

#endif

extern const QString s;
