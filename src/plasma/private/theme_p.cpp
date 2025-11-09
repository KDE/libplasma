/*
    SPDX-FileCopyrightText: 2006-2007 Aaron Seigo <aseigo@kde.org>
    SPDX-FileCopyrightText: 2013 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "theme_p.h"
#include "debug_p.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>

#include <KIconLoader>
#include <KSharedConfig>
#include <KWindowEffects>
#include <KWindowSystem>
#include <KX11Extras>

namespace Plasma
{
const char ThemePrivate::defaultTheme[] = "default";

BlurEffectWatcher *ThemePrivate::s_blurEffectWatcher = nullptr;

ThemePrivate *ThemePrivate::globalTheme = nullptr;
QHash<QString, ThemePrivate *> ThemePrivate::themes = QHash<QString, ThemePrivate *>();
using QSP = QStandardPaths;

KSharedConfig::Ptr configForTheme(const QString &theme)
{
    const QString baseName = QLatin1String(PLASMA_RELATIVE_DATA_INSTALL_DIR "/desktoptheme/") % theme;
    QString configPath = QSP::locate(QSP::GenericDataLocation, baseName + QLatin1String("/plasmarc"));
    if (!configPath.isEmpty()) {
        return KSharedConfig::openConfig(configPath, KConfig::SimpleConfig);
    }
    // Because we have desktop file compat code further below too, this is still needed in KF6
    QString metadataPath = QSP::locate(QSP::GenericDataLocation, baseName + QLatin1String("/metadata.desktop"));
    return KSharedConfig::openConfig(metadataPath, KConfig::SimpleConfig);
}

KPluginMetaData metaDataForTheme(const QString &theme)
{
    const QString packageBasePath =
        QSP::locate(QSP::GenericDataLocation, QLatin1String(PLASMA_RELATIVE_DATA_INSTALL_DIR "/desktoptheme/") % theme, QSP::LocateDirectory);
    if (packageBasePath.isEmpty()) {
        qWarning(LOG_PLASMA) << "Could not locate plasma theme" << theme << "in" << PLASMA_RELATIVE_DATA_INSTALL_DIR "/desktoptheme/"
                             << "using search path" << QSP::standardLocations(QSP::GenericDataLocation);
        return {};
    }
    if (QFileInfo::exists(packageBasePath + QLatin1String("/metadata.json"))) {
        return KPluginMetaData::fromJsonFile(packageBasePath + QLatin1String("/metadata.json"));
    } else if (QString metadataPath = packageBasePath + QLatin1String("/metadata.desktop"); QFileInfo::exists(metadataPath)) {
        KConfigGroup cg(KSharedConfig::openConfig(metadataPath, KConfig::SimpleConfig), QStringLiteral("Desktop Entry"));
        QJsonObject obj = {};
        for (const QString &key : cg.keyList()) {
            obj[key] = cg.readEntry(key);
        }
        qWarning(LOG_PLASMA) << "The theme" << theme
                             << "uses the legacy metadata.desktop. Consider contacting the author and asking them update it to use the newer JSON format.";
        return KPluginMetaData(obj, metadataPath);
    } else {
        qCWarning(LOG_PLASMA) << "Could not locate metadata for theme" << theme;
        return {};
    }
}

ThemePrivate::ThemePrivate(QObject *parent)
    : QObject(parent)
    , colorScheme(QPalette::Active, KColorScheme::Window, KSharedConfigPtr(nullptr))
    , selectionColorScheme(QPalette::Active, KColorScheme::Selection, KSharedConfigPtr(nullptr))
    , buttonColorScheme(QPalette::Active, KColorScheme::Button, KSharedConfigPtr(nullptr))
    , viewColorScheme(QPalette::Active, KColorScheme::View, KSharedConfigPtr(nullptr))
    , complementaryColorScheme(QPalette::Active, KColorScheme::Complementary, KSharedConfigPtr(nullptr))
    , headerColorScheme(QPalette::Active, KColorScheme::Header, KSharedConfigPtr(nullptr))
    , tooltipColorScheme(QPalette::Active, KColorScheme::Tooltip, KSharedConfigPtr(nullptr))
    , defaultWallpaperTheme(QStringLiteral(DEFAULT_WALLPAPER_THEME))
    , compositingActive(true)
    , backgroundContrastActive(KWindowEffects::isEffectAvailable(KWindowEffects::BackgroundContrast))
    , isDefault(true)
    , useGlobal(true)
    , fixedName(false)
    , backgroundContrast(qQNaN())
    , backgroundIntensity(qQNaN())
    , backgroundSaturation(qQNaN())
    , backgroundContrastEnabled(true)
    , adaptiveTransparencyEnabled(false)
    , blurBehindEnabled(true)
    , apiMajor(1)
    , apiMinor(0)
    , apiRevision(0)
{
    if (KWindowSystem::isPlatformX11()) {
        compositingActive = KX11Extras::self()->compositingActive();
    }

    kSvgImageSet = std::unique_ptr<KSvg::ImageSet>(new KSvg::ImageSet);
    kSvgImageSet->setBasePath(QStringLiteral(PLASMA_RELATIVE_DATA_INSTALL_DIR "/desktoptheme/"));

    // Event compress updateKSvgSelectors, because when compositing changes,
    // compositingactive and effect available will both happen in short succession
    // in a not really deterministic order
    selectorsUpdateTimer = new QTimer(this);
    selectorsUpdateTimer->setSingleShot(true);
    selectorsUpdateTimer->setInterval(600);
    QObject::connect(selectorsUpdateTimer, &QTimer::timeout, this, [this]() {
        updateKSvgSelectors();
        scheduleThemeChangeNotification();
    });

    updateNotificationTimer = new QTimer(this);
    updateNotificationTimer->setSingleShot(true);
    updateNotificationTimer->setInterval(100);
    QObject::connect(updateNotificationTimer, &QTimer::timeout, this, &ThemePrivate::notifyOfChanged);

    if (QPixmap::defaultDepth() > 8) {
        // watch for background contrast effect property changes as well
        if (!s_blurEffectWatcher) {
            s_blurEffectWatcher = new BlurEffectWatcher();
        }

        QObject::connect(s_blurEffectWatcher, &BlurEffectWatcher::effectChanged, selectorsUpdateTimer, qOverload<>(&QTimer::start));
    }
    QCoreApplication::instance()->installEventFilter(this);

    plasmaRcWatcher = KConfigWatcher::create(KSharedConfig::openConfig(QStringLiteral("plasmarc")));

    connect(plasmaRcWatcher.get(), &KConfigWatcher::configChanged, this, [this] {
        settingsChanged(true);
    });

    QObject::connect(KIconLoader::global(), &KIconLoader::iconChanged, this, [this]() {
        scheduleThemeChangeNotification();
    });

    if (KWindowSystem::isPlatformX11()) {
        connect(KX11Extras::self(), &KX11Extras::compositingChanged, selectorsUpdateTimer, qOverload<>(&QTimer::start));
    }
    updateKSvgSelectors();
}

ThemePrivate::~ThemePrivate()
{
}

KConfigGroup &ThemePrivate::config()
{
    if (!cfg.isValid()) {
        QString groupName = QStringLiteral("Theme");

        if (!useGlobal) {
            QString app = QCoreApplication::applicationName();

            if (!app.isEmpty()) {
#ifndef NDEBUG
                // qCDebug(LOG_PLASMA) << "using theme for app" << app;
#endif
                groupName.append(QLatin1Char('-')).append(app);
            }
        }
        cfg = plasmaRcWatcher->config()->group(groupName);
    }

    return cfg;
}

void ThemePrivate::updateKSvgSelectors()
{
#if HAVE_X11
    if (KWindowSystem::isPlatformX11()) {
        compositingActive = KX11Extras::compositingActive();
    } else {
        compositingActive = true;
    }
#else
    compositingActive = true;
#endif
    backgroundContrastActive = s_blurEffectWatcher->isEffectActive();

    if (compositingActive) {
        if (backgroundContrastActive) {
            kSvgImageSet->setSelectors({QStringLiteral("translucent")});
        } else {
            kSvgImageSet->setSelectors({});
        }
    } else {
        kSvgImageSet->setSelectors({QStringLiteral("opaque")});
    }
}

void ThemePrivate::colorsChanged()
{
    // in the case the theme follows the desktop settings, refetch the colorschemes
    if (colors != nullptr) {
        colors->reparseConfiguration();
    } else {
        KSharedConfig::openConfig()->reparseConfiguration();
    }
    colorScheme = KColorScheme(QPalette::Active, KColorScheme::Window, colors);
    buttonColorScheme = KColorScheme(QPalette::Active, KColorScheme::Button, colors);
    viewColorScheme = KColorScheme(QPalette::Active, KColorScheme::View, colors);
    selectionColorScheme = KColorScheme(QPalette::Active, KColorScheme::Selection, colors);
    complementaryColorScheme = KColorScheme(QPalette::Active, KColorScheme::Complementary, colors);
    headerColorScheme = KColorScheme(QPalette::Active, KColorScheme::Header, colors);
    tooltipColorScheme = KColorScheme(QPalette::Active, KColorScheme::Tooltip, colors);
    palette = KColorScheme::createApplicationPalette(colors);
    scheduleThemeChangeNotification();
    Q_EMIT applicationPaletteChange();
}

void ThemePrivate::scheduleThemeChangeNotification()
{
    updateNotificationTimer->start();
}

void ThemePrivate::notifyOfChanged()
{
    Q_EMIT themeChanged();
}

void ThemePrivate::settingsChanged(bool emitChanges)
{
    if (fixedName) {
        return;
    }
    // qCDebug(LOG_PLASMA) << "Settings Changed!";
    KConfigGroup cg = config();
    setThemeName(cg.readEntry("name", ThemePrivate::defaultTheme), false, emitChanges);
}

QColor ThemePrivate::color(Theme::ColorRole role, Kirigami::Platform::PlatformTheme::ColorSet group) const
{
    const KColorScheme *scheme = nullptr;

    // Before 5.0 Plasma theme really only used Normal and Button
    // many old themes are built on this assumption and will break
    // otherwise
    if (apiMajor < 5 && group != Kirigami::Platform::PlatformTheme::Window) {
        group = Kirigami::Platform::PlatformTheme::ColorSet::Button;
    }

    switch (group) {
    case Kirigami::Platform::PlatformTheme::ColorSet::Button: {
        scheme = &buttonColorScheme;
        break;
    }

    case Kirigami::Platform::PlatformTheme::ColorSet::View: {
        scheme = &viewColorScheme;
        break;
    }

    // this doesn't have a real kcolorscheme
    case Kirigami::Platform::PlatformTheme::ColorSet::Complementary: {
        scheme = &complementaryColorScheme;
        break;
    }

    case Kirigami::Platform::PlatformTheme::ColorSet::Header: {
        scheme = &headerColorScheme;
        break;
    }

    case Kirigami::Platform::PlatformTheme::ColorSet::Tooltip: {
        scheme = &tooltipColorScheme;
        break;
    }

    case Kirigami::Platform::PlatformTheme::ColorSet::Window:
    default: {
        scheme = &colorScheme;
        break;
    }
    }

    switch (role) {
    case Theme::TextColor:
        return scheme->foreground(KColorScheme::NormalText).color();

    case Theme::BackgroundColor:
        return scheme->background(KColorScheme::NormalBackground).color();

    case Theme::HoverColor:
        return scheme->decoration(KColorScheme::HoverColor).color();

    case Theme::HighlightColor:
        return selectionColorScheme.background(KColorScheme::NormalBackground).color();

    case Theme::FocusColor:
        return scheme->decoration(KColorScheme::FocusColor).color();

    case Theme::LinkColor:
        return scheme->foreground(KColorScheme::LinkText).color();

    case Theme::VisitedLinkColor:
        return scheme->foreground(KColorScheme::VisitedText).color();

    case Theme::HighlightedTextColor:
        return selectionColorScheme.foreground(KColorScheme::NormalText).color();

    case Theme::PositiveTextColor:
        return scheme->foreground(KColorScheme::PositiveText).color();
    case Theme::NeutralTextColor:
        return scheme->foreground(KColorScheme::NeutralText).color();
    case Theme::NegativeTextColor:
        return scheme->foreground(KColorScheme::NegativeText).color();
    case Theme::DisabledTextColor:
        return scheme->foreground(KColorScheme::InactiveText).color();
    case Theme::ActiveTextColor:
        return scheme->foreground(KColorScheme::ActiveText).color();

    case Theme::ActiveBackgroundColor:
        return scheme->background(KColorScheme::ActiveBackground).color();
    case Theme::PositiveBackgroundColor:
        return scheme->background(KColorScheme::PositiveBackground).color();
    case Theme::NeutralBackgroundColor:
        return scheme->background(KColorScheme::NeutralBackground).color();
    case Theme::NegativeBackgroundColor:
        return scheme->background(KColorScheme::NegativeBackground).color();
    }

    return QColor();
}

void ThemePrivate::processWallpaperSettings(const KSharedConfigPtr &metadata)
{
    if (!defaultWallpaperTheme.isEmpty() && defaultWallpaperTheme != QLatin1String(DEFAULT_WALLPAPER_THEME)) {
        return;
    }

    KConfigGroup cg;
    if (metadata->hasGroup(QStringLiteral("Wallpaper"))) {
        // we have a theme color config, so let's also check to see if
        // there is a wallpaper defined in there.
        cg = KConfigGroup(metadata, QStringLiteral("Wallpaper"));
    } else {
        // since we didn't find an entry in the theme, let's look in the main
        // theme config
        cg = config();
    }

    defaultWallpaperTheme = cg.readEntry("defaultWallpaperTheme", DEFAULT_WALLPAPER_THEME);
}

void ThemePrivate::processContrastSettings(const KSharedConfigPtr &metadata)
{
    KConfigGroup cg;
    if (metadata->hasGroup(QStringLiteral("ContrastEffect"))) {
        cg = KConfigGroup(metadata, QStringLiteral("ContrastEffect"));
        backgroundContrastEnabled = cg.readEntry("enabled", false);

        backgroundContrast = cg.readEntry("contrast", qQNaN());
        backgroundIntensity = cg.readEntry("intensity", qQNaN());
        backgroundSaturation = cg.readEntry("saturation", qQNaN());
    } else {
        backgroundContrastEnabled = false;
    }
}

void ThemePrivate::processAdaptiveTransparencySettings(const KSharedConfigPtr &metadata)
{
    KConfigGroup cg;
    if (metadata->hasGroup(QStringLiteral("AdaptiveTransparency"))) {
        cg = KConfigGroup(metadata, QStringLiteral("AdaptiveTransparency"));
        adaptiveTransparencyEnabled = cg.readEntry("enabled", false);
    } else {
        adaptiveTransparencyEnabled = false;
    }
}

void ThemePrivate::processBlurBehindSettings(const KSharedConfigPtr &metadata)
{
    KConfigGroup cg;
    if (metadata->hasGroup(QStringLiteral("BlurBehindEffect"))) {
        cg = KConfigGroup(metadata, QStringLiteral("BlurBehindEffect"));
        blurBehindEnabled = cg.readEntry("enabled", true);
    } else {
        blurBehindEnabled = true;
    }
}

void ThemePrivate::setThemeName(const QString &theme, bool writeSettings, bool emitChanged)
{
    if (theme.isEmpty()) {
        qCWarning(LOG_PLASMA) << "Empty theme name passed to ThemePrivate::setThemeName";
        return;
    }

    if (themeName == theme) {
        return;
    }

    kSvgImageSet->setImageSetName(theme);

    pluginMetaData = metaDataForTheme(theme);
    if (!pluginMetaData.isValid()) {
        // metaDataForTheme already prints a warning
        return;
    }

    themeName = theme;

    // load the color scheme config
    const QString colorsFile = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                      QLatin1String(PLASMA_RELATIVE_DATA_INSTALL_DIR "/desktoptheme/") % theme % QLatin1String("/colors"));

    // qCDebug(LOG_PLASMA) << "we're going for..." << colorsFile << "*******************";

    if (colorsFile.isEmpty()) {
        colors = nullptr;
    } else {
        colors = KSharedConfig::openConfig(colorsFile);
    }

    colorScheme = KColorScheme(QPalette::Active, KColorScheme::Window, colors);
    selectionColorScheme = KColorScheme(QPalette::Active, KColorScheme::Selection, colors);
    buttonColorScheme = KColorScheme(QPalette::Active, KColorScheme::Button, colors);
    viewColorScheme = KColorScheme(QPalette::Active, KColorScheme::View, colors);
    complementaryColorScheme = KColorScheme(QPalette::Active, KColorScheme::Complementary, colors);
    headerColorScheme = KColorScheme(QPalette::Active, KColorScheme::Header, colors);
    tooltipColorScheme = KColorScheme(QPalette::Active, KColorScheme::Tooltip, colors);
    palette = KColorScheme::createApplicationPalette(colors);

    // load the wallpaper settings, if any
    KSharedConfigPtr metadata = configForTheme(theme);

    processContrastSettings(metadata);
    processBlurBehindSettings(metadata);
    processAdaptiveTransparencySettings(metadata);

    processWallpaperSettings(metadata);

    KConfigGroup cg(metadata, QStringLiteral("Settings"));
    QString fallback = cg.readEntry("FallbackTheme", QString());

    fallbackThemes.clear();
    while (!fallback.isEmpty() && !fallbackThemes.contains(fallback)) {
        fallbackThemes.append(fallback);

        KSharedConfigPtr metadata = configForTheme(fallback);
        KConfigGroup cg(metadata, QStringLiteral("Settings"));
        fallback = cg.readEntry("FallbackTheme", QString());
    }

    if (!fallbackThemes.contains(QLatin1String(ThemePrivate::defaultTheme))) {
        fallbackThemes.append(QLatin1String(ThemePrivate::defaultTheme));
    }

    for (const QString &theme : std::as_const(fallbackThemes)) {
        KSharedConfigPtr metadata = configForTheme(theme);
        processWallpaperSettings(metadata);
    }

    // Check for what Plasma version the theme has been done
    // There are some behavioral differences between KDE4 Plasma and Plasma 5
    const QString apiVersion = pluginMetaData.value(u"X-Plasma-API");
    apiMajor = 1;
    apiMinor = 0;
    apiRevision = 0;
    if (!apiVersion.isEmpty()) {
        const QList<QStringView> parts = QStringView(apiVersion).split(QLatin1Char('.'));
        if (!parts.isEmpty()) {
            apiMajor = parts.value(0).toInt();
        }
        if (parts.count() > 1) {
            apiMinor = parts.value(1).toInt();
        }
        if (parts.count() > 2) {
            apiRevision = parts.value(2).toInt();
        }
    }

    if (isDefault && writeSettings) {
        // we're the default theme, let's save our status
        KConfigGroup &cg = config();
        cg.writeEntry("name", themeName);
        cg.sync();
    }

    if (emitChanged) {
        scheduleThemeChangeNotification();
    }
}

bool ThemePrivate::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == QCoreApplication::instance()) {
        if (event->type() == QEvent::ApplicationPaletteChange) {
            colorsChanged();
        }
    }
    return QObject::eventFilter(watched, event);
}

}

#include "moc_theme_p.cpp"
