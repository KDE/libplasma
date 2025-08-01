/*
    SPDX-FileCopyrightText: 2010 Ryan Rix <ry@n.rix.si>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PLUGIN_LOADER_H
#define PLUGIN_LOADER_H

#include <plasma/plasma_export.h>

#include <QVariant>

class KPluginMetaData;
namespace Plasma
{
class Applet;
class Containment;
class ContainmentActions;

/*!
 * \class Plasma::PluginLoader
 * \inheaderfile Plasma/PluginLoader
 * \inmodule Plasma
 *
 * \brief Loader for Plasma plugins.
 *
 * This is an abstract base class which defines an interface to which Plasma's
 * Applet Loading logic can communicate with a parent application. The plugin loader
 * must be set before any plugins are loaded, otherwise (for safety reasons), the
 * default PluginLoader implementation will be used. The reimplemented version should
 * not do more than simply returning a loaded plugin. It should not init() it, and it should not
 * hang on to it. The associated methods will be called only when a component of Plasma
 * needs to load a _new_ plugin.
 *
 * \since 4.6
 **/
class PLASMA_EXPORT PluginLoader
{
public:
    /*!
     * Load an Applet plugin.
     *
     * \a name the plugin name, as returned by KPluginInfo::pluginName()
     *
     * \a appletId unique ID to assign the applet, or zero to have one
     *        assigned automatically.
     *
     * \a args to send the applet extra arguments
     *
     * Returns a pointer to the loaded applet, or 0 on load failure
     **/
    Applet *loadApplet(const QString &name, uint appletId = 0, const QVariantList &args = QVariantList());

    /*!
     * Load a ContainmentActions plugin.
     *
     * Returns a pointer to the containmentactions if successful.
     * The caller takes responsibility for the containmentactions, including
     * deleting it when no longer needed.
     *
     * \a parent the parent containment. Since 4.6 null is allowed.
     *
     * \a name the plugin name, as returned by KPluginInfo::pluginName()
     *
     * \a args to send the containmentactions extra arguments
     *
     * Returns a ContainmentActions object
     **/
    ContainmentActions *loadContainmentActions(Containment *parent, const QString &containmentActionsName, const QVariantList &args = QVariantList());

    /*!
     * Returns a list of all known applets.
     * This may skip applets based on security settings and ExcludeCategories in the application's config.
     *
     * \a category Only applets matching this category will be returned.
     *                 If "Misc" is passed in, then applets without a
     *                 Categories= entry are also returned.
     *                 If an empty string is passed in, all applets are
     *                 returned.
     * Returns list of applets
     *
     * \since 5.28
     **/
    QList<KPluginMetaData> listAppletMetaData(const QString &category);

    /*!
     * Returns a list of all known applets associated with a certain mimetype.
     *
     * \since 5.36
     **/
    QList<KPluginMetaData> listAppletMetaDataForMimeType(const QString &mimetype);

    /*!
     * Returns a list of all known applets associated with a certain URL.
     *
     * \since 5.36
     **/
    QList<KPluginMetaData> listAppletMetaDataForUrl(const QUrl &url);

    /*!
     * Returns a list of all known containments.
     *
     * \a filter An optional predicate that can be used for filtering.
     */
    static QList<KPluginMetaData> listContainmentsMetaData(std::function<bool(const KPluginMetaData &)> filter = {});

    /*!
     * Returns a list of containments of the specified type.
     *
     * \a type The target containment type
     */
    static QList<KPluginMetaData> listContainmentsMetaDataOfType(const QString &type);

    /*!
     * Returns a list of all known ContainmentActions.
     *
     * \a parentApp the application to filter ContainmentActions on. Uses the
     *                  X-KDE-ParentApp entry (if any) in the plugin metadata.
     *                  The default value of QString() will result in a
     *                  list of all ContainmentActions.
     * \since 5.77
     **/
    QList<KPluginMetaData> listContainmentActionsMetaData(const QString &parentApp);

    /*!
     * Return the active plugin loader
     **/
    static PluginLoader *self();

    PluginLoader() = default;
    virtual ~PluginLoader() = default;

private:
    void *d;
};

}

Q_DECLARE_METATYPE(Plasma::PluginLoader *)

#endif
