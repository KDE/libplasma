/*
    SPDX-FileCopyrightText: 2011 Viranch Mehta <viranch.mehta@gmail.com>
    SPDX-FileCopyrightText: 2013 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "qmenuitem.h"

QMenuItem::QMenuItem(QQuickItem *parent)
    : QQuickItem(parent),
      m_action(nullptr),
      m_section(false)
{
    setAction(new QAction(this));
}

QAction *QMenuItem::action() const
{
    return m_action;
}

void QMenuItem::setAction(QAction *a)
{
    if (m_action != a) {
        if (m_action) {
            disconnect(m_action, nullptr, this, nullptr);

            if (m_action->parent() == this) {
                delete m_action;
                m_action = nullptr;
            }
        }

        if (a) {
            m_action = a;
        } else {
            // don't end up with no action, create an invisible one instead
            m_action = new QAction(this);
            m_action->setVisible(false);
        }

        setVisible(m_action->isVisible());
        setEnabled(m_action->isEnabled());

        connect(m_action, &QAction::changed, this, &QMenuItem::textChanged);
        connect(m_action, &QAction::changed, this, &QMenuItem::checkableChanged);
        connect(m_action, &QAction::toggled, this, &QMenuItem::toggled);
        connect(m_action, &QAction::triggered, this, &QMenuItem::clicked);

        connect(this, &QQuickItem::visibleChanged, this, &QMenuItem::updateAction);
        connect(this, &QQuickItem::enabledChanged, this, &QMenuItem::updateAction);

        emit actionChanged();
    }
}

QVariant QMenuItem::icon() const
{
    return m_icon;
}

void QMenuItem::setIcon(const QVariant &i)
{
    m_icon = i;
    if (i.canConvert<QIcon>()) {
        m_action->setIcon(i.value<QIcon>());
    } else if (i.canConvert<QString>()) {
        m_action->setIcon(QIcon::fromTheme(i.toString()));
    }
    emit iconChanged();
}

bool QMenuItem::separator() const
{
    return m_action->isSeparator();
}

void QMenuItem::setSeparator(bool s)
{
    m_action->setSeparator(s);
}

bool QMenuItem::section() const
{
    return m_section;
}

void QMenuItem::setSection(bool s)
{
    m_section = s;
}

QString QMenuItem::text() const
{
    return m_action->text();
}

void QMenuItem::setText(const QString &t)
{
    if (m_action->text() != t) {
        m_action->setText(t);
        // signal comes from m_action
    }
}

bool QMenuItem::checkable() const
{
    return m_action->isCheckable();
}

void QMenuItem::setCheckable(bool checkable)
{
    m_action->setCheckable(checkable);
}

bool QMenuItem::checked() const
{
    return m_action->isChecked();
}

void QMenuItem::setChecked(bool checked)
{
    m_action->setChecked(checked);
}

void QMenuItem::updateAction()
{
    m_action->setVisible(isVisible());
    m_action->setEnabled(isEnabled());
}



