/*
    SPDX-FileCopyrightText: 2023 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PLASMAACTION_H
#define PLASMAACTION_H

#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QObject>
#include <qqml.h>
#include <qtclasshelpermacros.h>
#include <qtmetamacros.h>

class QQuickItem;

class ActionExtension;

class IconGroup : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged)
public:
    explicit IconGroup(ActionExtension *parent = nullptr);
    ~IconGroup() override;

    void setName(const QString &name);
    QString name() const;

    QIcon icon() const;
    void setIcon(const QIcon &icon);

Q_SIGNALS:
    void nameChanged();
    void iconChanged();

private:
    QAction *m_action;
    QString m_name;
};

/*!
 * \qmltype Action
 * \inqmlmodule org.kde.plasma.core
 * \inherits QtObject
 *
 * \brief Represents an action on a Plasmoid.
 *
 * Unlike Qt Quick Controls' Action type, this is backed by a \l QAction.
 *
 * Use this to define actions for a Plasmoid, such as \l {Plasma::Applet::contextualActions} {\c {Plasmoid.contextualActions}}
 * or \l {Plasma::Applet::setInternalAction} {\c {Plasmoid.setInternalAction}}
 *
 * Example usage:
 *
 * \qml
 * import org.kde.plasma.core as PlasmaCore
 *
 * PlasmaCore.Action {
 *     text: "Run"
 *     icon.name: "system-run-symbolic"
 *     onTriggered: runProcess()
 * }
 * \endqml
 */
class ActionExtension : public QObject
{
    Q_OBJECT

    /*!
     * \qmlproperty string Action::icon.name
     * \qmlproperty QIcon Action::icon.icon
     *
     * The icon for this action. Can be a system icon name (through \c{icon.name}) or a \l {QIcon} (through \c{icon.icon}).
     */
    Q_PROPERTY(IconGroup *icon MEMBER m_iconGroup CONSTANT)

    /*!
     * \qmlproperty bool Action::isSeparator
     *
     * Defaults to false.
     *
     * \sa QAction::isSeparator
     */
    Q_PROPERTY(bool isSeparator READ isSeparator WRITE setSeparator NOTIFY isSeparatorChanged)

    /*!
     * \qmlproperty QActionGroup Action::actionGroup
     *
     * \sa QAction::actionGroup
     */
    Q_PROPERTY(QActionGroup *actionGroup READ actionGroup WRITE setActionGroup NOTIFY actionGroupChanged)

    /*!
     * \qmlproperty variant Action::shortcut
     *
     * The shortcut that can be used to activate this action.
     * Can be a string describing a sequence (like \c {"Ctrl+Shift+X"}) or a \l {QKeySequence::StandardKey}.
     */
    Q_PROPERTY(QVariant shortcut READ shortcut WRITE setShortcut NOTIFY shortcutChanged)

    /*!
     * \qmlproperty QMenu Action::menu
     *
     * \sa QAction::setMenu
     */
    Q_PROPERTY(QMenu *menu READ menu WRITE setMenu NOTIFY menuChanged)

public:
    explicit ActionExtension(QObject *parent = nullptr);
    ~ActionExtension() override;

    bool isSeparator() const;
    void setSeparator(bool setSeparator);

    void setActionGroup(QActionGroup *group);
    QActionGroup *actionGroup() const;

    void setShortcut(const QVariant &var);
    QVariant shortcut() const;

    void setMenu(QMenu *menu);
    QMenu *menu();

    QAction *action() const;

Q_SIGNALS:
    void isSeparatorChanged();
    void actionGroupChanged();
    void shortcutChanged();
    void menuChanged();

private:
    QAction *m_action;
    IconGroup *m_iconGroup;
    QString m_icon;
};

class ActionGroup : public QActionGroup
{
    Q_OBJECT
    QML_ELEMENT
public:
    ActionGroup(QObject *parent = nullptr)
        : QActionGroup(parent)
    {
    }
};

#endif
