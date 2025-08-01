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
#define DEFAULT_WALLPAPER_SUFFIX ".png"
static const int DEFAULT_WALLPAPER_WIDTH = 1920;
static const int DEFAULT_WALLPAPER_HEIGHT = 1200;

class ThemePrivate : public QObject, public QSharedData
{
    Q_OBJECT

public:
    explicit ThemePrivate(QObject *parent = nullptr);
    ~ThemePrivate() override;

    KConfigGroup &config();

    QString imagePath(const QString &theme, const QString &type, const QString &image);
    QString findInTheme(const QString &image, const QString &theme);
    void scheduleThemeChangeNotification();
    void setThemeName(const QString &themeName, bool writeSettings, bool emitChanged);
    void processWallpaperSettings(const KSharedConfigPtr &metadata);
    void processContrastSettings(const KSharedConfigPtr &metadata);
    void processAdaptiveTransparencySettings(const KSharedConfigPtr &metadata);
    void processBlurBehindSettings(const KSharedConfigPtr &metadata);

    QColor color(Theme::ColorRole role, Theme::ColorGroup group = Theme::NormalColorGroup) const;

    void updateKSvgSelectors();

public Q_SLOTS:
    void colorsChanged();
    void notifyOfChanged();
    void settingsChanged(bool emitChanges);

Q_SIGNALS:
    void themeChanged();
    void defaultFontChanged();
    void smallestFontChanged();
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
    QString defaultWallpaperSuffix;
    int defaultWallpaperWidth;
    int defaultWallpaperHeight;
    QTimer *selectorsUpdateTimer;
    QTimer *updateNotificationTimer;

    bool compositingActive : 1;
    bool backgroundContrastActive : 1;
    bool isDefault : 1;
    bool useGlobal : 1;
    bool hasWallpapers : 1;
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
