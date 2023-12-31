/*
    Copyright 2011 Marco Martin <notmart@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "appbackgroundprovider_p.h"

#include <QLatin1Literal>
#include <QPixmap>
#include <QSize>
#include <QStandardPaths>

#include <Plasma/Theme>

AppBackgroundProvider::AppBackgroundProvider()
  : QQuickImageProvider(QQuickImageProvider::Image)
{
    m_theme = new Plasma::Theme();
}

AppBackgroundProvider::~AppBackgroundProvider()
{
    m_theme->deleteLater();
}

QImage AppBackgroundProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(size)
    Q_UNUSED(requestedSize)
    QString search = QLatin1Literal("desktoptheme/") % m_theme->themeName() % QLatin1Literal("/appbackgrounds/") % id % ".png";
    search = QStandardPaths::locate(QStandardPaths::GenericDataLocation, search);
    return QImage(search);
}

