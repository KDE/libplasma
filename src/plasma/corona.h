/*
    SPDX-FileCopyrightText: 2007 Aaron Seigo <aseigo@kde.org>
    SPDX-FileCopyrightText: 2007 Matt Broadstone <mbroadst@gmail.com>
    SPDX-FileCopyrightText: 2012 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PLASMA_CORONA_H
#define PLASMA_CORONA_H

#include <plasma/containment.h>
#include <plasma/plasma.h>
#include <plasma/plasma_export.h>

class QAction;

namespace Plasma
{
class CoronaPrivate;

/*!
 * \class Plasma::Corona
 * \inheaderfile Plasma/Corona
 * \inmodule Plasma
 *
 * \brief A bookkeeping Scene for Plasma::Applets.
 */
class PLASMA_EXPORT Corona : public QObject
{
    Q_OBJECT

    /*!
     * \property Plasma::Corona::isStartupCompleted
     */
    Q_PROPERTY(bool isStartupCompleted READ isStartupCompleted NOTIFY startupCompleted)

    /*!
     * \property Plasma::Corona::editMode
     */
    Q_PROPERTY(bool editMode READ isEditMode WRITE setEditMode NOTIFY editModeChanged)

    /*!
     * \property Plasma::Corona::kPackage
     */
    Q_PROPERTY(KPackage::Package kPackage READ kPackage NOTIFY kPackageChanged)

public:
    /*!
     * Constructor
     */
    explicit Corona(QObject *parent = nullptr);
    ~Corona() override;

    /*!
     * Accessor for the associated Package object if any.
     * A Corona package defines how Containments are laid out in a View,
     * ToolBoxes, default layout, error messages
     * and in general all the furniture specific of a particular
     * device form factor.
     *
     * Returns the package object, or an invalid one if none
     * \since 5.5
     **/
    KPackage::Package kPackage() const;

    /*!
     * Setting the package for the corona
     * \since 5.5
     */
    void setKPackage(const KPackage::Package &package);

    /*!
     * Returns all containments on this Corona
     */
    QList<Containment *> containments() const;

    /*!
     * Returns true when the startup is over, and
     * all the ui graphics has been instantiated
     */
    bool isStartupCompleted() const;

    /*!
     * Returns the config file used to store the configuration for this Corona
     */
    KSharedConfig::Ptr config() const;

    /*!
     * Adds a Containment to the Corona
     *
     * \a name the plugin name for the containment, as given by
     *        KPluginInfo::pluginName(). If an empty string is passed in, the default
     *        containment plugin will be used (usually DesktopContainment). If the
     *        string literal "null" is passed in, then no plugin will be loaded and
     *        a simple Containment object will be created instead.
     *
     * \a args argument list to pass to the containment
     *
     * Returns a pointer to the containment on success, or 0 on failure. Failure can be
     * caused by too restrictive of an Immutability type, as containments cannot be added
     * when widgets are locked.
     * If the requested containment plugin can not be located or successfully loaded, the Containment will have an invalid pluginInfo().
     */
    Containment *createContainment(const QString &name, const QVariantList &args = QVariantList());

    /*!
     * Returns the Containment for a given physical screen and desktop, creating one
     * if none exists
     *
     * \a screen number of the physical screen to locate
     *
     * \a activity the activity id of the containment we want,
     *                 and empty string if the activity is not important
     * \a defaultPluginIfNonExistent the plugin to load by default; "null" won't
     * create it and "default" creates the default plugin
     *
     * \a defaultArgs optional arguments to pass in when creating a Containment if needed
     *
     * \since 5.45
     */
    Containment *
    containmentForScreen(int screen, const QString &activity, const QString &defaultPluginIfNonExistent, const QVariantList &defaultArgs = QVariantList());

    /*!
     * Returns all containments which match a particular activity, for any screen
     * \a activity the activity id we want
     * Returns the list of matching containments if any, empty if activity is an empty string
     * \since 5.45
     */
    QList<Containment *> containmentsForActivity(const QString &activity);

    /*!
     * Returns all containments which match a particular screen, for any activity
     *
     * \a screen the screen number we want
     *
     * Returns the list of matching containments if any, empty if screen is < 0
     *
     * \since 5.45
     */
    QList<Containment *> containmentsForScreen(int screen);

    /*!
     * Returns the number of screens available to plasma.
     * Subclasses should override this method as the default
     * implementation returns a meaningless value.
     */
    virtual int numScreens() const;

    /*!
     * Returns the geometry of a given screen.
     * Valid screen ids are 0 to numScreen()-1, or -1 for the full desktop geometry.
     * Subclasses should override this method as the default
     * implementation returns a meaningless value.
     */
    virtual QRect screenGeometry(int id) const = 0;

    /*!
     * Returns the available region for a given screen.
     * The available region excludes panels and similar windows.
     * Valid screen ids are 0 to numScreens()-1.
     * By default this method returns a rectangular region
     * equal to screenGeometry(id); subclasses that need another
     * behavior should override this method.
     */
    virtual QRegion availableScreenRegion(int id) const;

    /*!
     * Returns the available rect for a given screen.
     * The difference between this and availableScreenRegion()
     * is that this method returns only a rectangular
     * available space (it doesn't care if your panel is not 100% width).
     * The available rect excludes panels and similar windows.
     * Valid screen ids are 0 to numScreens()-1.
     * By default this method returns a rectangular region
     * equal to screenGeometry(id); subclasses that need another
     * behavior should override this method.
     */
    virtual QRect availableScreenRect(int id) const;

    /*!
     * This method is useful in order to retrieve the list of available
     * screen edges for panel type containments.
     *
     * \a screen the id of the screen to look for free edges.
     *
     * Returns a list of free edges not filled with panel type containments.
     */
    QList<Plasma::Types::Location> freeEdges(int screen) const;

    /*!
     * Returns The action with the given name, if any
     */
    Q_INVOKABLE QAction *action(const QString &name) const;

    /*!
     * Defines a new action with the given name in the internal collection
     */
    void setAction(const QString &name, QAction *action);

    /*!
     * Remove the action with the given name, if exists
     */
    void removeAction(const QString &name);

    /*!
     * Returns all the actions supported by the corona
     */
    QList<QAction *> actions() const;

    /*!
     * Imports an applet layout from a config file. The results will be added to the
     * current set of Containments.
     *
     * \a config the name of the config file to load from,
     *               or the default config file if QString()
     *
     * Returns the list of containments that were loaded
     *
     * \since 4.6
     */
    QList<Plasma::Containment *> importLayout(const KConfigGroup &config);

    /*!
     * Exports a set of containments to a config file.
     *
     * \a config the config group to save to
     *
     * \a containments the list of containments to save
     *
     * \since 4.6
     */
    void exportLayout(KConfigGroup &config, QList<Containment *> containments);

    /*!
     * Returns the id of the screen which is showing \a containment
     * -1 is returned if the containment is not associated with a screen.
     */
    virtual int screenForContainment(const Containment *containment) const;

    /*!
     * Returns The type of immutability of this Corona
     */
    Types::ImmutabilityType immutability() const;

    /*!
     * Returns \c true if immutability() is not Types::Mutable
     * \since 6.4
     */
    bool immutable() const;

    /*!
     * Set the Corona globally into "edit mode"
     * Only when the corona is of mutable type can be set of edit mode.
     * This indicates the UI to make easy for the user to manipulate applets.
     *
     * \a edit
     *
     * \since 5.63
     */
    void setEditMode(bool edit);

    /*!
     * Returns true if the corona is in edit mode
     *
     * \since 5.63
     */
    bool isEditMode() const;

    // TODO: make them not slots anymore
public Q_SLOTS:
    /*!
     * Load applet layout from a config file. The results will be added to the
     * current set of Containments.
     *
     * \a config the name of the config file to load from,
     *               or the default config file if QString()
     */
    void loadLayout(const QString &config = QString());

    /*!
     * Save applets layout to file
     *
     * \a config the file to save to, or the default config file if QString()
     */
    void saveLayout(const QString &config = QString()) const;

    /*!
     * Sets the immutability type for this Corona (not immutable,
     * user immutable or system immutable)
     *
     * \a immutable the new immutability type of this applet
     */
    void setImmutability(const Types::ImmutabilityType immutable);

    /*!
     * Schedules a flush-to-disk synchronization of the configuration state
     * at the next convenient moment.
     */
    void requestConfigSync();

    /*!
     * Schedules a time sensitive flush-to-disk synchronization of the
     * configuration state. Since this method does not provide any sort of
     * event compression, it should only be used when an *immediate* disk
     * sync is *absolutely* required. Otherwise, use requestConfigSync()
     * which does do event compression.
     */
    void requireConfigSync();

Q_SIGNALS:
    /*!
     * This signal indicates a new containment has been added to
     * the Corona: it may occur after creation or restore from config
     */
    void containmentAdded(Plasma::Containment *containment);

    /*!
     * This signal indicates a new containment has been created
     * in the Corona. Compared to containmentAdded it can only happen
     * after the creation of a new containment.
     *
     * \sa containmentAdded
     * \since 5.16
     */
    void containmentCreated(Plasma::Containment *containment);

    /*!
     * This signal indicates that a containment has been newly
     * associated (or dissociated) with a physical screen.
     *
     * \a isScreen the screen it is now associated with
     */
    void screenOwnerChanged(int isScreen);

    /*!
     * This signal indicates that the configuration file was flushed to disk.
     */
    void configSynced();

    /*!
     * This signal indicates that a change in available screen geometry occurred.
     */
    void availableScreenRegionChanged(int id);

    /*!
     * This signal indicates that a change in available screen geometry occurred.
     */
    void availableScreenRectChanged(int id);

    /*!
     * This signal indicates that a change in geometry for the screen occurred.
     */
    void screenGeometryChanged(int id);

    /*!
     * emitted when immutability changes.
     * this is for use by things that don't get constraints events, like plasmaapp.
     * it's NOT for containments or applets or any of the other stuff on the scene.
     * if your code's not in shells/ it probably shouldn't be using it.
     */
    void immutabilityChanged(Plasma::Types::ImmutabilityType immutability);

    /*! This signal indicates the screen with the specified id was removed.
     * \since 5.40
     */
    void screenRemoved(int id);

    /*! This signal indicates a new screen with the specified id was added.
     * \since 5.40
     */
    void screenAdded(int id);

    /*!
     * emitted when the editMode state changes
     * \sa isEditMode()
     * \since 5.63
     */
    void editModeChanged(bool edit);

    /*!
     * Emitted when the package for this corona has been changed.
     * Shells must support changing the shell package on the fly (for instance due to device form factor changing)
     *
     * \a package the new package that defines the Corona furniture and behavior
     */
    void kPackageChanged(const KPackage::Package &package);

    /*!
     * Emitted when the startup phase has been completed
     */
    void startupCompleted();

protected:
    /*!
     * Loads the default (system wide) layout for this user
     **/
    virtual void loadDefaultLayout();

    /*!
     * Loads a containment with delayed initialization, primarily useful
     * for implementations of loadDefaultLayout. The caller is responsible
     * for all initialization, saving and notification of a new containment.
     *
     * \a name the plugin name for the containment, as given by
     *        KPluginInfo::pluginName(). If an empty string is passed in, the default
     *        containment plugin will be used (usually DesktopContainment). If the
     *        string literal "null" is passed in, then no plugin will be loaded and
     *        a simple Containment object will be created instead.
     *
     * \a args argument list to pass to the containment
     *
     * Returns a pointer to the containment on success, or 0 on failure. Failure can
     * be caused by the Immutability type being too restrictive, as containments can't be added
     * when widgets are locked, or if the requested containment plugin can not be located
     * or successfully loaded.
     * \sa createContainment
     **/
    Containment *createContainmentDelayed(const QString &name, const QVariantList &args = QVariantList());

private:
    CoronaPrivate *const d;

    Q_PRIVATE_SLOT(d, void containmentDestroyed(QObject *))
    Q_PRIVATE_SLOT(d, void syncConfig())
    Q_PRIVATE_SLOT(d, void toggleImmutability())
    Q_PRIVATE_SLOT(d, void containmentReady(bool))

    friend class CoronaPrivate;
    friend class View;
};

} // namespace Plasma

#endif
