/*
 *   Copyright 2013 Marco Martin <notmart@gmail.com>
 *
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

#include "packageurlinterceptor.h"

#include <QDebug>
#include <QQmlEngine>
#include <QFile>
#include <QFileInfo>
#include <QFileSelector>
#include <QStandardPaths>

#include <Plasma/PluginLoader>
#include <Plasma/Package>
#include <KPackage/Package>

#include <kdeclarative/kdeclarative.h>

namespace PlasmaQuick
{

class PackageUrlInterceptorPrivate {
public:
    PackageUrlInterceptorPrivate(QQmlEngine *engine, PackageUrlInterceptor *interceptor, const KPackage::Package &p)
        : q(interceptor),
          package(p),
          engine(engine)
    {
        selector = new QFileSelector;
    }

    ~PackageUrlInterceptorPrivate()
    {
        engine->setUrlInterceptor(nullptr);
        delete selector;
    }

    PackageUrlInterceptor *q;
    KPackage::Package package;
    QStringList allowedPaths;
    QQmlEngine *engine;
    QFileSelector *selector;
    bool forcePlasmaStyle = false;
};


PackageUrlInterceptor::PackageUrlInterceptor(QQmlEngine *engine, const KPackage::Package &p)
    : QQmlAbstractUrlInterceptor(),
      d(new PackageUrlInterceptorPrivate(engine, this, p))
{
    //d->allowedPaths << d->engine->importPathList();
}

PackageUrlInterceptor::~PackageUrlInterceptor()
{
    delete d;
}

void PackageUrlInterceptor::addAllowedPath(const QString &path)
{
    d->allowedPaths << path;
}

void PackageUrlInterceptor::removeAllowedPath(const QString &path)
{
    d->allowedPaths.removeAll(path);
}

QStringList PackageUrlInterceptor::allowedPaths() const
{
    return d->allowedPaths;
}

bool PackageUrlInterceptor::forcePlasmaStyle() const
{
    return d->forcePlasmaStyle;
}

void PackageUrlInterceptor::setForcePlasmaStyle(bool force)
{
    d->forcePlasmaStyle = force;
}

QUrl PackageUrlInterceptor::intercept(const QUrl &path, QQmlAbstractUrlInterceptor::DataType type)
{
    //qDebug() << "Intercepted URL:" << path << type;

    const QString urlPath = path.path();
    // Don't intercept qmldir files, to prevent double interception
    if (urlPath.endsWith(QLatin1String("qmldir"))) {
        return path;
    }
    // We assume we never rewritten qml/qmldir files
    if (urlPath.endsWith(QLatin1String("qml"))
        || urlPath.endsWith(QLatin1String("/inline"))) {
        return d->selector->select(path);
    }
    const QString prefix = prefixForType(type, urlPath);
    // TODO KF6: Kill this hack
    QString plainPath = path.toString();
    const int index = plainPath.indexOf(QLatin1String("/ui/"));
    if (index != -1) {
        plainPath = plainPath.midRef(0, index)
                    + QLatin1Char('/') + prefix + QLatin1Char('/') + plainPath.midRef(index + 4);
        //search it in a resource or as a file on disk
        const QUrl url = QUrl(plainPath);
        const QString newPath = url.path();
        if (!(plainPath.contains(QLatin1String("qrc")) && QFile::exists(QLatin1Char(':') + newPath))
            && !QFile::exists(newPath)) {
            return d->selector->select(path);
        }
        qWarning() <<"Warning: all files used by qml by the plasmoid should be in ui/. The file in the path"
                   << plainPath << "was expected at" << path;
        // This deprecated code path doesn't support selectors
        return url;
    }
    return d->selector->select(path);
}

}
