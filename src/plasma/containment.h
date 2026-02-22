/*
    SPDX-FileCopyrightText: 2007 Aaron Seigo <aseigo@kde.org>
    SPDX-FileCopyrightText: 2008 Ménard Alexis <darktears31@gmail.com>
    SPDX-FileCopyrightText: 2009 Chani Armitage <chani@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PLASMA_CONTAINMENT_H
#define PLASMA_CONTAINMENT_H

#include <KPluginMetaData>
#include <KSharedConfig>
#include <plasma/applet.h>
#include <qtmetamacros.h>

class QQuickItem;

namespace Plasma
{
class Corona;
class ContainmentActions;
class ContainmentPrivate;

/*!
 * \class Plasma::Containment
 * \inheaderfile Plasma/Containment
 * \inmodule Plasma
 *
 * \brief The base class for plugins that provide backgrounds and applet grouping containers.
 *
 * Containment objects provide the means to group applets into functional sets.
 * They also provide the following:
 *
 * \list
 * \li creation of focusing event
 * \li drawing of the background image (which can be interactive)
 * \li form factors (e.g. panel, desktop, full screen, etc)
 * \li applet layout management
 * \endlist
 *
 * Since Containment is itself a Plasma::Applet, all the techniques used
 * for writing the visual presentation of Applets is applicable to Containtments.
 * Containments are differentiated from Applets by being marked with the ServiceType
 * of Plasma/Containment. Plugins registered with both the Applet and the Containment
 * ServiceTypes can be loaded for use in either situation.
 */
class PLASMA_EXPORT Containment : public Applet
{
    Q_OBJECT

    /*!
     * \property Plasma::Containment::applets
     * List of applets in this containment.
     */
    Q_PROPERTY(QList<Plasma::Applet *> applets READ applets NOTIFY appletsChanged) // KF6: this should be AppletQuickItem *

    /*!
     * \property Plasma::Containment::corona
     * The corona for this contaiment.
     */
    Q_PROPERTY(Plasma::Corona *corona READ corona CONSTANT)

    /*!
     * \property Plasma::Containment::containmentType
     * Type of this containment.
     */
    Q_PROPERTY(Plasma::Containment::Type containmentType READ containmentType NOTIFY containmentTypeChanged)

    /*!
     * \property Plasma::Containment::activity
     * Activity UID of this containment.
     */
    Q_PROPERTY(QString activity READ activity NOTIFY activityChanged)

    /*!
     * \property Plasma::Containment::activityName
     * Activity name of this containment.
     */
    Q_PROPERTY(QString activityName READ activityName NOTIFY activityNameChanged)

    /*!
     * \property Plasma::Containment::containmentDisplayHints
     */
    Q_PROPERTY(Plasma::Types::ContainmentDisplayHints containmentDisplayHints READ containmentDisplayHints WRITE setContainmentDisplayHints NOTIFY
                   containmentDisplayHintsChanged)

    /*!
     * \property Plasma::Containment::wallpaperPlugin
     */
    Q_PROPERTY(QString wallpaperPlugin READ wallpaperPlugin WRITE setWallpaperPlugin NOTIFY wallpaperPluginChanged)

    /*!
     * \property Plasma::Containment::wallpaperGraphicsObject
     */
    Q_PROPERTY(QObject *wallpaperGraphicsObject READ wallpaperGraphicsObject WRITE setWallpaperGraphicsObject NOTIFY wallpaperGraphicsObjectChanged)

    /*!
     * \property Plasma::Containment::isUiReady
     */
    Q_PROPERTY(bool isUiReady READ isUiReady NOTIFY uiReadyChanged)

    /*!
     * \property Plasma::Containment::screen
     * The screen number this containment is serving as the desktop for, or -1 if none
     */
    Q_PROPERTY(int screen READ screen NOTIFY screenChanged)

    /*!
     * \property Plasma::Containment::availableScreenRect
     * Screen area used by the Containment by itself (free of enveloping panels).
     *
     * The coordinates are relative to the containment (that is, x:0 and y:0 match the Containment's upper left corner),
     * and independent from the screen position.
     *
     * For more precise available geometry use availableScreenRegion().
     */
    Q_PROPERTY(QRectF availableScreenRect READ availableRelativeScreenRect NOTIFY availableRelativeScreenRectChanged)

    /*!
     * \property Plasma::Containment::availableScreenRegion
     * A list of rectangles matching the available region of this screen, panels excluded.
     */
    Q_PROPERTY(QList<QRectF> availableScreenRegion READ availableRelativeScreenRegion NOTIFY availableRelativeScreenRegionChanged)

    /*!
     * \property Plasma::Containment::screenGeometry
     * Provides access to the geometry of the screen the applet is in.
     *
     * Can be useful to figure out what's the absolute position of the applet.
     */
    Q_PROPERTY(QRectF screenGeometry READ screenGeometry NOTIFY screenGeometryChanged)

public:
    /*!
     * This constructor can be used with the KCoreAddons plugin loading system.
     * The argument list is expected to have contain the KPackage of the applet,
     * the meta data file path (for compatibility) and an applet ID which must be a base 10 number.
     *
     * \a parent a QObject parent; you probably want to pass in 0
     *
     * \a data KPluginMetaData used to create this plugin
     *
     * \a args a list of strings containing the applet id
     *
     * \since 5.86
     */
    explicit Containment(QObject *parentObject, const KPluginMetaData &data, const QVariantList &args);

    ~Containment() override;

    void init() override;

    /*!
     * The type of the Containment.
     *
     * This value is specified in the "X-Plasma-ContainmentType" JSON-metadata value of containments.
     *
     * \value NoContainment
     * \value Desktop
     *        Main containments that will own a screen in a mutually exclusive fashion.
     * \value Panel
     *        A desktop panel. Multiple can be present per screen.
     * \value Custom
     *        A containment that is neither a desktop nor a panel but something application specific.
     * \value CustomPanel
     *        A customized desktop panel.
     *        "CustomPanel" in metadata.
     * \value CustomEmbedded
     *        A customized containment embedded into another applet.
     */
    enum Type {
        NoContainment = -1,
        Desktop = 0,
        Panel,
        Custom = 127,
        CustomPanel = 128,
        CustomEmbedded = 129,
    };
    Q_ENUM(Type)

    /*!
     * Returns the type of containment
     */
    Type containmentType() const;

    /*!
     * Returns the Corona (if any) that this Containment is hosted by
     */
    Corona *corona() const;

    /*!
     * Adds an applet to this Containment
     *
     * \a name the plugin name for the applet, as given by
     *        KPluginInfo::pluginName()
     *
     * \a args argument list to pass to the plasmoid
     *
     * \a geometryHint an hint to pass to the GUI on the location
     *           and size we prefer for the newly created applet;
     *           the gui might choose whether to respect or not this hint.
     *           The default position is (-1, -1) and the default size
     *           is (0, 0).
     *
     * Returns a pointer to the applet on success, or 0 on failure
     */
    Applet *createApplet(const QString &name, const QVariantList &args = QVariantList(), const QRectF &geometryHint = QRectF(-1, -1, 0, 0));

    /*!
     * \brief Adds an existing \a applet to this Containment with the given \a geometryHint.
     *
     * The hint determines the preferred location and size for the newly created applet. The hint will not be respected if it's not possible to position it in the preferred location.
     */
    /*Q_INVOKABLE*/ void addApplet(std::unique_ptr<Applet> &&applet, const QRectF &geometryHint = QRectF());

    /*!
     * Returns the applets currently in this Containment
     */
    QList<Applet *> applets() const;

    /*!
     * Returns the screen number this containment is serving as the desktop for
     *         or -1 if none
     */
    int screen() const; // TODO KF6 virtual? this should be available to applet as well

    /*!
     * Returns the last screen number this containment had
     *         only returns -1 if it's never ever been on a screen
     * \since 4.5
     */
    int lastScreen() const;

    void save(KConfigGroup &group) const override;

    void restore(KConfigGroup &group) override;

    /*!
     * Sets wallpaper plugin.
     *
     * \a pluginName the name of the wallpaper to attempt to load
     */
    void setWallpaperPlugin(const QString &pluginName);

    /*!
     * Return wallpaper plugin.
     */
    QString wallpaperPlugin() const;

    /*!
     * Sets the current activity by id
     *
     * \a activityId the id of the activity
     */
    void setActivity(const QString &activityId);

    /*!
     * Returns the current activity id associated with this containment
     */
    QString activity() const; // TODO KF6: this should be available to Applet as well as a property... virtual?

    /*!
     * Returns Activity name corresponding to the activity UID
     */
    QString activityName() const;

    /*!
     * Sets a containmentactions plugin.
     *
     * \a trigger the mouse button (and optional modifier) to associate the plugin with
     *
     * \a pluginName the name of the plugin to attempt to load. blank = set no plugin.
     *
     * \since 4.4
     */
    void setContainmentActions(const QString &trigger, const QString &pluginName);

    /*!
     * Returns all the loaded containment action plugins, indexed by trigger name
     * \since 5.0
     */
    QHash<QString, ContainmentActions *> &containmentActions();

    /*!
     * Returns \c true when the ui of this containment is fully loaded, as well the ui of every applet in it
     */
    bool isUiReady() const;

    /*!
     * Returns The available screen rect (excluding panels) for the screen this containment is associated to,
     * empty rectangle if the containment is not active in a screen
     */
    QRectF availableRelativeScreenRect() const;

    /*!
     * Returns The available region of this screen, panels excluded. It's a list of rectangles
     */
    QList<QRectF> availableRelativeScreenRegion() const;

    /*!
     * Returns The geometry of the screen this containment is associated to
     */
    QRectF screenGeometry() const;

Q_SIGNALS:
    /*!
     * This signal is emitted when a new applet is added in the containment
     * It may happen in the following situations:
     * \list
     * \li The user created the applet
     * \li The applet was moved in from another containment
     * \li The applet got restored at startup
     * \endlist
     *
     * \a applet the applet that has been added
     *
     * \a geometryHint an hint to pass to the GUI on the location
     *           and size we prefer for the newly created applet;
     *           the gui might choose whether to respect or not this hint
     */
    void appletAdded(Plasma::Applet *applet, const QRectF &geometryHint);

    /*!
     * This signal is emitted right before appletAdded, it can be used
     * to do a preliminary setup on the applet before the handlers of appletAdded are executed.
     * Useful for instance to prepare the GUI for the applet
     *
     * \a applet the applet that is about to be added
     *
     * \a geometryHint an hint to pass to the GUI on the location
     *           and size we prefer for the newly created applet;
     *           the gui might choose whether to respect or not this hint
     */
    void appletAboutToBeAdded(Plasma::Applet *applet, const QRectF &geometryHint);

    /*!
     * This signal is emitted when an applet is destroyed
     */
    void appletRemoved(Plasma::Applet *applet);

    /*!
     * This signal is emitted right before appletRemoved, it can be used
     * to do a preliminary setup on the applet before the handlers of appletRemoved are executed.
     * Useful for instance to prepare or teardown the GUI for the applet
     */
    void appletAboutToBeRemoved(Plasma::Applet *applet);

    /*!
     * This signal is emitted when a new applet is created by the containment.
     * Compared to appletAdded, this gets emitted only when the user explicitly
     * creates a new applet, either via the widget explorer or the scripting
     * environment.
     *
     * \sa appletAdded
     * \since 5.16
     */
    void appletCreated(Plasma::Applet *applet, const QRectF &geometryHint);

    /*!
     * Emitted when the list of applets has changed, either added or removed
     */
    void appletsChanged();

    /*!
     * Emitted when the activity id has changed
     */
    void activityChanged(const QString &activity);

    /*!
     * Emitted when the activity name has changed
     */
    void activityNameChanged(const QString &name);

    /*!
     * Emitted when the containment requests an add widgets dialog is shown.
     * Usually only used for desktop containments.
     *
     * \a pos where in the containment this request was made from, or
     *            an invalid position (QPointF()) is not location specific
     */
    void showAddWidgetsInterface(const QPointF &pos);

    /*!
     * This signal indicates that a containment has been
     * associated (or dissociated) with a physical screen.
     *
     * \a newScreen the screen it is now associated with
     */
    void screenChanged(int newScreen);

    /*!
     * Emitted when the user wants to configure/change the containment, or an applet inside it.
     */
    void configureRequested(Plasma::Applet *applet);

    /*!
     * Emitted when the user wants to chose an alternative for this applet or containment.
     */
    void appletAlternativesRequested(Plasma::Applet *applet);

    /*!
     * Emitted when the wallpaper plugin is changed
     */
    void wallpaperPluginChanged();

    /*!
     * Emitted when the location has changed
     * \since 5.0
     */
    void locationChanged(Plasma::Types::Location location);

    /*!
     * Emitted when the formFactor has changed
     * \since 5.0
     */
    void formFactorChanged(Plasma::Types::FormFactor formFactor);

    /*!
     * Emitted when the ui has been fully loaded and is fully working
     *
     * \a uiReady true when the ui of the containment is ready, as well the ui of each applet in it
     */
    void uiReadyChanged(bool uiReady);

    /*!
     * emitted when the containment type changed
     */
    void containmentTypeChanged();

    /*!
     * Emitted when the available screen rectangle has changed
     */
    void availableRelativeScreenRectChanged(const QRectF &rect);

    /*!
     * Emitted when the available screen rectangle has changed
     */
    void availableRelativeScreenRegionChanged(const QList<QRectF> &region);

    /*!
     * Emitted when the screen geometry has changed
     */
    void screenGeometryChanged(const QRectF &rect);

    /*!
     * Emitted when the root wallpaper item has changed
     */
    void wallpaperGraphicsObjectChanged();

public Q_SLOTS:
    /*!
     * Informs the Corona as to what position it is in. This is informational
     * only, as the Corona doesn't change its actual location. This is,
     * however, passed on to Applets that may be managed by this Corona.
     *
     * \a location the new location of this Corona
     */
    void setLocation(Plasma::Types::Location location);

    /*!
     * Sets the form factor for this Containment. This may cause changes in both
     * the arrangement of Applets as well as the display choices of individual
     * Applets.
     */
    void setFormFactor(Plasma::Types::FormFactor formFactor);

    /*!
     * Set Display hints that come from the containment that suggest the applet how to look and behave.
     *
     * \a hints the new hints, as bitwise OR
     * \since 5.77
     */
    void setContainmentDisplayHints(Plasma::Types::ContainmentDisplayHints hints);

    void reactToScreenChange();

protected:
    /*!
     * Called when the contents of the containment should be saved. By default this saves
     * all loaded Applets
     *
     * \a group the KConfigGroup to save settings under
     */
    virtual void saveContents(KConfigGroup &group) const;

    /*!
     * Called when the contents of the containment should be loaded. By default this loads
     * all previously saved Applets
     *
     * \a group the KConfigGroup to save settings under
     */
    virtual void restoreContents(KConfigGroup &group);

private:
    /*!
     * \internal This constructor is to be used with the Package loading system.
     *
     * \a parent a QObject parent; you probably want to pass in 0
     * \since 4.3
     */
    Containment(const KPluginMetaData &md, uint appletId);

    /*!
     * \internal Return root wallpaper item
     */
    QObject *wallpaperGraphicsObject() const;
    void setWallpaperGraphicsObject(QObject *object);

    QUrl compactApplet() const;

    Q_PRIVATE_SLOT(d, void appletDeleted(Plasma::Applet *))
    Q_PRIVATE_SLOT(d, void triggerShowAddWidgets())
    Q_PRIVATE_SLOT(d, void checkStatus(Plasma::Types::ItemStatus))

    friend class Applet;
    friend class AppletPrivate;
    friend class CoronaPrivate;
    friend class ContainmentPrivate;
    friend class ContainmentActions;
    friend class PlasmaQuick::AppletQuickItem;
    ContainmentPrivate *const d;
};

} // Plasma namespace

#endif // multiple inclusion guard
