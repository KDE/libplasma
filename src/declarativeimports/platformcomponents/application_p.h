/*
 *   Copyright (C) 2013 Ivan Cukic <ivan.cukic(at)kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2,
 *   or (at your option) any later version, as published by the Free
 *   Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef APPLICATION_P_H
#define APPLiCAtION_P_H

#include "application.h"

#include <QProcess>

class Application::Private: public QObject
{
    Q_OBJECT
public:
    Private(Application *);

    QString application;
    QProcess process;
    bool running;

private Q_SLOTS:
    void stateChanged(QProcess::ProcessState newState);
    void errorFound(QProcess::ProcessError error);

private:
    Application *const q;
};

#endif /* APPLICATION_P_H */

