/*
 *   Copyright 2009 by Alan Alpert <alan.alpert@nokia.com>
 *   Copyright 2010 by Ménard Alexis <menard@kde.org>

 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef DECLARATIVE_APPLETSCRIPT_H
#define DECLARATIVE_APPLETSCRIPT_H

#include <QQmlEngine>
#include <QSet>

#include <Plasma/AppletScript>

class AppletInterface;

class DeclarativeAppletScript : public Plasma::AppletScript
{
    Q_OBJECT

public:
    DeclarativeAppletScript(QObject *parent, const QVariantList &args);
    ~DeclarativeAppletScript();

    QString filePath(const QString &type, const QString &file) const;

    QList<QAction*> contextualActions();

    void constraintsEvent(Plasma::Types::Constraints constraints);

public Q_SLOTS:
    void executeAction(const QString &name);

protected:
    bool init();

Q_SIGNALS:
    void formFactorChanged();
    void locationChanged();
    void contextChanged();
    void uiReadyChanged(bool ready);

private:
    AppletInterface *m_interface;
    friend class AppletLoader;
    friend class AppletInterface;
    friend class ContainmentInterface;
};


#endif
