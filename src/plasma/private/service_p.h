/*
 *   Copyright 2008 Aaron Seigo <aseigo@kde.org>
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

#ifndef SERVICE_P_H
#define SERVICE_P_H

#include "servicejob.h"
#include "service.h"

#include <QMap>
#include <QMultiHash>
#include <QWidget>
#include <QSet>

#include <kplugininfo.h>
#include <klocalizedstring.h>

#include <dnssd/publicservice.h>
#include <dnssd/servicebrowser.h>

#include "plasma/configloader.h"

namespace Plasma
{

class ConfigLoader;

class NullServiceJob : public ServiceJob
{
public:
    NullServiceJob(const QString &destination, const QString &operation, QObject *parent)
        : ServiceJob(destination, operation, QVariantMap(), parent)
    {
    }

    void start()
    {
        setErrorText(i18nc("Error message, tried to start an invalid service", "Invalid (null) service, can not perform any operations."));
        emitResult();
    }
};

class NullService : public Service
{
public:
    NullService(const QString &target, QObject *parent)
        : Service(parent)
    {
        setDestination(target);
        setName("NullService");
    }

    ServiceJob *createJob(const QString &operation, QVariantMap &)
    {
        return new NullServiceJob(destination(), operation, this);
    }
};

class ServicePrivate
{
public:
    ServicePrivate(Service *service)
        : q(service),
          dummyConfig(0),
          publicService(0)
    {        
    }

    ~ServicePrivate()
    {
        delete dummyConfig;
    }

    void associatedWidgetDestroyed(QObject *obj);

    void associatedItemDestroyed(QObject *obj);

    KConfigGroup dummyGroup();

    Service *q;
    QString destination;
    QString name;
    QString resourcename;
    QMap<QString, QVariantMap> operationsMap;
    KConfig *dummyConfig;
    DNSSD::PublicService *publicService;
    QSet<QString> disabledOperations;
};

} // namespace Plasma

#endif

