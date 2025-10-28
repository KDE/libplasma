/*
    SPDX-FileCopyrightText: 2009 Chani Armitage <chani@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PLASMA_CONTAINMENTACTIONS_H
#define PLASMA_CONTAINMENTACTIONS_H

#include <QList>

#include <plasma/plasma_export.h>

#include <plasma/plasma.h>

class QAction;
class KPluginMetaData;
class KConfigGroup;

namespace Plasma
{
class Containment;
class ContainmentActionsPrivate;

/*!
 * \class Plasma::ContainmentActions
 * \inheaderfile Plasma/ContainmentActions
 * \inmodule Plasma
 *
 * \brief The base ContainmentActions class.
 *
 * "ContainmentActions" are components that provide actions (usually displaying a contextmenu) in
 * response to an event with a position (usually a mouse event).
 *
 * ContainmentActions plugins are registered using .desktop files. These files should be
 * named using the following naming scheme:
 *
 * \c plasma-containmentactions-\<pluginname\>.desktop
 *
 */

class PLASMA_EXPORT ContainmentActions : public QObject
{
    Q_OBJECT

public:
    /*!
     * Default constructor for an empty or null containmentactions
     */
    explicit ContainmentActions(QObject *parent = nullptr);

    ~ContainmentActions() override;

    /*!
     * Returns the ID of this plugin
     *
     * \since 6.4
     */
    QString id() const;

    /*!
     * Returns whether this plugin has a configuration UI
     *
     * \since 6.4
     */
    bool hasConfigurationInterface() const;

    /*!
     * Returns the metadata for this ContainmentActions instance
     *         including name, pluginName and icon
     * \since 5.67
     */
    KPluginMetaData metadata() const;

    /*!
     * This method should be called once the plugin is loaded or settings are changed.
     *
     * \a config Config group to load settings
     * \sa init
     **/
    virtual void restore(const KConfigGroup &config);

    /*!
     * This method is called when settings need to be saved.
     *
     * \a config Config group to save settings
     **/
    virtual void save(KConfigGroup &config);

    /*!
     * Returns the widget used in the configuration dialog.
     * Add the configuration interface of the containmentactions to this widget.
     */
    virtual QWidget *createConfigurationInterface(QWidget *parent);

    /*!
     * This method is called when the user's configuration changes are accepted
     */
    virtual void configurationAccepted();

    /*!
     * Called when a "next" action is triggered (e.g. by mouse wheel scroll). This
     * can be used to scroll through a list of items this plugin manages such as
     * windows, virtual desktops, activities, etc.
     * \sa performPrevious
     */
    virtual void performNextAction();

    /*!
     * Called when a "previous" action is triggered (e.g. by mouse wheel scroll). This
     * can be used to scroll through a list of items this plugin manages such as
     * windows, virtual desktops, activities, etc.
     * \sa performNext
     */
    virtual void performPreviousAction();

    /*!
     * Implement this to provide a list of actions that can be added to another menu
     * for example, when right-clicking an applet, the "Activity Options" submenu is populated
     * with this.
     */
    virtual QList<QAction *> contextualActions();

    /*!
     * Turns a mouse or wheel event into a string suitable for a ContainmentActions
     * Returns the string representation of the event
     */
    static QString eventToString(QEvent *event);

    /*!
     * \a newContainment the containment the plugin should be associated with.
     * \since 4.6
     */
    void setContainment(Containment *newContainment);

    /*!
     * Returns the containment the plugin is associated with.
     */
    Containment *containment();

protected:
    /*!
     * This constructor is to be used with the plugin loading systems
     * found in KPluginInfo and KService. The argument list is expected
     * to have one element: the KService service ID for the desktop entry.
     *
     * \a parent a QObject parent; you probably want to pass in 0
     *
     * \a args a list of strings containing one entry: the service id
     */
    ContainmentActions(QObject *parent, const QVariantList &args);

private:
    ContainmentActionsPrivate *const d;
};

} // Plasma namespace

#endif // PLASMA_CONTAINMENTACTIONS_H
