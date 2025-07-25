/*
    SPDX-FileCopyrightText: 2006-2007 Aaron Seigo <aseigo@kde.org>
    SPDX-FileCopyrightText: 2013 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PLASMA_THEME_H
#define PLASMA_THEME_H

#include <QFont>
#include <QGuiApplication>
#include <QObject>

#include <KSharedConfig>
#include <plasma/plasma_export.h>

class KPluginMetaData;

namespace Plasma
{
class ThemePrivate;
class SvgPrivate;

/*!
 * \class Plasma::Theme
 * \inheaderfile Plasma/Theme
 * \inmodule Plasma
 *
 * \brief Interface to the Plasma theme.
 *
 * Plasma::Theme provides access to a common and standardized set of graphic
 * elements stored in SVG format. This allows artists to create single packages
 * of SVGs that will affect the look and feel of all workspace components.
 *
 * Plasma::Svg uses Plasma::Theme internally to locate and load the appropriate
 * SVG data. Alternatively, Plasma::Theme can be used directly to retrieve
 * file system paths to SVGs by name.
 */
class PLASMA_EXPORT Theme : public QObject
{
    Q_OBJECT

    /*!
     * \property Plasma::Theme::themeName
     */
    Q_PROPERTY(QString themeName READ themeName NOTIFY themeChanged)

    /*!
     * \property Plasma::Theme::useGlobalSettings
     */
    Q_PROPERTY(bool useGlobalSettings READ useGlobalSettings NOTIFY themeChanged)

    /*!
     * \property Plasma::Theme::wallpaperPath
     */
    Q_PROPERTY(QString wallpaperPath READ wallpaperPath NOTIFY themeChanged)

    // fonts
    /*!
     * \property Plasma::Theme::defaultFont
     */
    Q_PROPERTY(QFont defaultFont READ defaultFont NOTIFY defaultFontChanged)

    /*!
     * \property Plasma::Theme::smallestFont
     */
    Q_PROPERTY(QFont smallestFont READ smallestFont NOTIFY smallestFontChanged)

    /*!
     * \property Plasma::Theme::palette
     */
    Q_PROPERTY(QPalette palette READ palette NOTIFY themeChanged)

    Q_PROPERTY(qreal backgroundContrast READ backgroundContrast NOTIFY themeChanged)
    Q_PROPERTY(qreal backgroundIntensity READ backgroundIntensity NOTIFY themeChanged)
    Q_PROPERTY(qreal backgroundSaturation READ backgroundSaturation NOTIFY themeChanged)

public:
    /*!
     * \value TextColor The text color to be used by items resting on the background
     * \value BackgroundColor The default background color
     * \value HighlightColor The text highlight color to be used by items resting on the background
     * \value HoverColor Color for hover effect on view
     * \value FocusColor Color for focus effect on view
     * \value LinkColor Color for clickable links
     * \value VisitedLinkColor Color visited clickable links
     * \value HighlightedTextColor Color contrasting with HighlightColor, to be used for instance with
     * \value PositiveTextColor Color of foreground objects with a "positive message" connotation (usually green)
     * \value NeutralTextColor Color of foreground objects with a "neutral message" connotation (usually yellow)
     * \value NegativeTextColor Color of foreground objects with a "negative message" connotation (usually red)
     * \value [since 5.64] DisabledTextColor color of disabled text
     */
    enum ColorRole {
        TextColor = 0,
        BackgroundColor = 1,
        HighlightColor = 2,
        HoverColor = 3,
        FocusColor = 4,
        LinkColor = 5,
        VisitedLinkColor = 6,
        HighlightedTextColor = 7,
        PositiveTextColor = 8,
        NeutralTextColor = 9,
        NegativeTextColor = 10,
        DisabledTextColor = 11,
        ActiveTextColor,
        ActiveBackgroundColor,
        PositiveBackgroundColor,
        NeutralBackgroundColor,
        NegativeBackgroundColor,
    };

    /*!
     * \value NormalColorGroup
     * \value ButtonColorGroup
     * \value ViewColorGroup
     * \value ComplementaryColorGroup
     * \value HeaderColorGroup
     * \value ToolTipColorGroup
     */
    enum ColorGroup {
        NormalColorGroup = 0,
        ButtonColorGroup = 1,
        ViewColorGroup = 2,
        ComplementaryColorGroup = 3,
        HeaderColorGroup,
        ToolTipColorGroup,
    };
    Q_ENUM(ColorGroup)

    /*!
     * Default constructor. It will be the global theme configured in plasmarc
     *
     * \a parent the parent object
     */
    explicit Theme(QObject *parent = nullptr);

    /*!
     * Construct a theme. It will be a custom theme instance of themeName.
     *
     * \a themeName the name of the theme to create
     *
     * \a parent the parent object
     *
     * \since 4.3
     */
    explicit Theme(const QString &themeName, QObject *parent = nullptr);

    ~Theme() override;

    /*!
     * Sets the current theme being used.
     */
    void setThemeName(const QString &themeName);

    /*!
     * Returns the name of the theme.
     */
    QString themeName() const;

    /*!
     * Retrieve the path for an SVG image in the current theme.
     *
     * \a name the name of the file in the theme directory (without the
     *           ".svg" part or a leading slash)
     *
     * Returns the full path to the requested file for the current theme
     */
    QString imagePath(const QString &name) const;

    /*!
     * Retrieves the default wallpaper associated with this theme.
     *
     * \a size the target height and width of the wallpaper; if an invalid size
     *           is passed in, then a default size will be provided instead.
     *
     * Returns the full path to the wallpaper image
     */
    QString wallpaperPath(const QSize &size = QSize()) const;

    /*!
     *
     */
    Q_INVOKABLE QString wallpaperPathForSize(int width = -1, int height = -1) const;

    /*!
     * Checks if this theme has an image named in a certain way
     *
     * \a name the name of the file in the theme directory (without the
     *           ".svg" part or a leading slash)
     *
     * Returns true if the image exists for this theme
     */
    bool currentThemeHasImage(const QString &name) const;

    /*!
     * Returns the color scheme configurationthat goes along this theme.
     *
     * This can be used with KStatefulBrush and KColorScheme to determine
     * the proper colours to use along with the visual elements in this theme.
     */
    KSharedConfigPtr colorScheme() const;

    /*!
     * Returns the text color to be used by items resting on the background
     *
     * \a role which role (usage pattern) to get the color for
     *
     * \a group which group we want a color of
     */
    QColor color(ColorRole role, ColorGroup group = NormalColorGroup) const;

    /*!
     * Tells the theme whether to follow the global settings or use application
     * specific settings
     *
     * \a useGlobal pass in true to follow the global settings
     */
    void setUseGlobalSettings(bool useGlobal);

    /*!
     * Returns true if the global settings are followed, false if application
     * specific settings are used.
     */
    bool useGlobalSettings() const;

    /*!
     * Returns a QPalette with the colors set as defined by the theme.
     * \since 5.68
     */
    QPalette palette() const;

    /*!
     * Returns plugin metadata for this theme, with information such as
     * name, description, author, website etc
     * \since 5.95
     */
    KPluginMetaData metadata() const;

    /*!
     * Returns The default application font
     * \since 5.0
     */
    QFont defaultFont() const;

    /*!
     * Returns The smallest readable font
     * \since 5.0
     */
    QFont smallestFont() const;

    /*! This method allows Plasma to enable and disable the background
     * contrast effect for a given theme, improving readability. The
     * value is read from the "enabled" key in the "ContrastEffect"
     * group in the Theme's metadata file.
     * The configuration in the metadata.desktop file of the theme
     * could look like this (for a lighter background):
     * \code
     * [ContrastEffect]
     * enabled=true
     * contrast=0.45
     * intensity=0.45
     * saturation=1.7
     * \endcode
     *
     * Returns Whether or not to enable the contrasteffect
     *
     * \since 5.0
     */
    bool backgroundContrastEnabled() const;

    /*! This method allows Plasma to enable and disable the adaptive
     * transparency option of the panel, which allows user to decide
     * whether the panel should be always transparent, always opaque
     * or only opaque when a window is maximized.
     * The configuration in the metadata.desktop file of the theme
     * could look like this (for a lighter background):
     * \code
     * [AdaptiveTransparency]
     * enabled=true
     * \endcode
     *
     * Returns Whether or not to enable the adaptive transparency
     *
     * \since 5.20
     */
    bool adaptiveTransparencyEnabled() const;

    /*! This method allows Plasma to set a background contrast effect
     * for a given theme, improving readability. The value is read
     * from the "contrast" key in the "ContrastEffect" group in the
     * Theme's metadata file.
     *
     * Returns The contrast provided to the contrasteffect
     * \since 5.0
     * \sa backgroundContrastEnabled
     */
    qreal backgroundContrast() const;

    /*! This method allows Plasma to set a background contrast effect
     * for a given theme, improving readability. The value is read
     * from the "intensity" key in the "ContrastEffect" group in the
     * Theme's metadata file.
     *
     * Returns The intensity provided to the contrasteffect
     *
     * \since 5.0
     * \sa backgroundContrastEnabled
     */
    qreal backgroundIntensity() const;

    /*! This method allows Plasma to set a background contrast effect
     * for a given theme, improving readability. The value is read
     * from the "saturation" key in the "ContrastEffect" group in the
     * Theme's metadata file.
     *
     * Returns The saturation provided to the contrasteffect
     *
     * \since 5.0
     * \sa backgroundContrastEnabled
     */
    qreal backgroundSaturation() const;

    /*! This method allows Plasma to enable and disable the blurring
     * of what is behind the background for a given theme. The
     * value is read from the "enabled" key in the "BlurBehindEffect"
     * group in the Theme's metadata file. Default is @c true.
     *
     * The configuration in the metadata.desktop file of the theme
     * could look like this:
     * \code
     * [BlurBehindEffect]
     * enabled=false
     * \endcode
     *
     * Returns Whether or not to enable blurring of what is behind
     * \since 5.57
     */
    bool blurBehindEnabled() const;

    /*!
     * Returns the size of the letter "M" as rendered on the screen with the given font.
     * This values gives you a base size that:
     * * scales dependent on the DPI of the screen
     * * Scales with the default font as set by the user
     * You can use it like this in QML Items:
     * \code
     * Item {
     *     width: PlasmaCore.Theme.mSize(PlasmaCore.Theme.defaultFont).height
     *     height: width
     * }
     * \endcode
     * This allows you to dynamically scale elements of your user interface with different font settings and
     * different physical outputs (with different DPI).
     *
     * \a font The font to use for the metrics.
     *
     * Returns The size of the letter "M" as rendered on the screen with the given font.
     *
     * \since 5.0
     */
    Q_INVOKABLE QSizeF mSize(const QFont &font = QGuiApplication::font()) const;

    /*!
     *
     */
    QString backgroundPath(const QString &image) const;

    /*!
     *
     */
    static QPalette globalPalette();

    /*!
     *
     */
    static KSharedConfigPtr globalColorScheme();

Q_SIGNALS:
    /*!
     * Emitted when the user changes the theme. Stylesheet usage, colors, etc. should
     * be updated at this point. However, SVGs should *not* be repainted in response
     * to this signal; connect to Svg::repaintNeeded() instead for that, as Svg objects
     * need repainting not only when themeChanged() is emitted; moreover Svg objects
     * connect to and respond appropriately to themeChanged() internally, emitting
     * Svg::repaintNeeded() at an appropriate time.
     */
    void themeChanged();

    /*! Notifier for change of defaultFont property */
    void defaultFontChanged();
    /*! Notifier for change of smallestFont property */
    void smallestFontChanged();

private:
    friend class SvgPrivate;
    friend class FrameSvg;
    friend class FrameSvgPrivate;
    friend class ThemePrivate;
    ThemePrivate *d;
};

} // Plasma namespace

#endif // multiple inclusion guard
