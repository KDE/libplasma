/******************************************************************************
*   Copyright 2013 Sebastian Kügler <sebas@kde.org>                           *
*                                                                             *
*   This library is free software; you can redistribute it and/or             *
*   modify it under the terms of the GNU Library General Public               *
*   License as published by the Free Software Foundation; either              *
*   version 2 of the License, or (at your option) any later version.          *
*                                                                             *
*   This library is distributed in the hope that it will be useful,           *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU          *
*   Library General Public License for more details.                          *
*                                                                             *
*   You should have received a copy of the GNU Library General Public License *
*   along with this library; see the file COPYING.LIB.  If not, write to      *
*   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,      *
*   Boston, MA 02110-1301, USA.                                               *
*******************************************************************************/

#include "dpitest.h"

#include <QDebug>

#include <plasma/theme.h>

#include <qcommandlineparser.h>
#include <QStringList>
#include <QTimer>
#include <iostream>
#include <iomanip>


namespace Plasma
{
class DPITestPrivate {
public:
    QString pluginName;
    QCommandLineParser *parser;
};

DPITest::DPITest(int& argc, char** argv, QCommandLineParser *parser) :
    QApplication(argc, argv)
{
    d = new DPITestPrivate;
    d->parser = parser;
    QTimer::singleShot(0, this, SLOT(runMain()));
}

DPITest::~DPITest()
{
    delete d;
}

void DPITest::runMain()
{
    qDebug() << "DPI test runs: ";
    exit(0);
    return;
}

}

#include "moc_dpitest.cpp"
