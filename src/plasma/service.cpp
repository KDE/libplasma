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

#include "service.h"
#include "private/service_p.h"

#include "config-plasma.h"

#include <QFile>
#include <QTimer>
#include <QQuickItem>

#include <kdebug.h>
#include <kservice.h>
#include <kservicetypetrader.h>
#include <ksharedconfig.h>

#include <dnssd/publicservice.h>
#include <dnssd/servicebrowser.h>
#include <qstandardpaths.h>

#include "configloader.h"
#include "version.h"
#include "private/configloader_p.h"
#include "private/configloaderhandler_p.h"
#include "pluginloader.h"

namespace Plasma
{

class ConfigLoaderHandlerMap : public ConfigLoaderHandler
{
public:
    ConfigLoaderHandlerMap(ConfigLoader *config, ConfigLoaderPrivate *d)
        : ConfigLoaderHandler(config, d)
    {}

    void addItem();
    const QMap<QString, QVariantMap> &groupsMap() const;

private:
    QMap<QString, QVariantMap> m_groupsMap;
};

void ConfigLoaderHandlerMap::addItem()
{
    if (name().isEmpty()) {
        if (key().isEmpty()) {
            return;
        }

        setName(key());
    } else if (key().isEmpty()) {
        if (name().isEmpty()) {
            return;
        }

        setKey(name());
    }

    if (!m_groupsMap.contains(currentGroup())) {
        m_groupsMap[currentGroup()] = QVariantMap();
        m_groupsMap[currentGroup()]["_name"] = currentGroup();
    }

    if (type() == "bool") {
        bool defaultVal = defaultValue().toLower() == "true";
        m_groupsMap[currentGroup()][key()] = defaultVal;
    } else if (type() == "color") {
        m_groupsMap[currentGroup()][key()] = QColor(defaultValue());
    } else if (type() == "datetime") {
        m_groupsMap[currentGroup()][key()] = QDateTime::fromString(defaultValue());
    } else if (type() == "enum") {
        key() = (key().isEmpty()) ? name() : key();
        m_groupsMap[currentGroup()][key()] = defaultValue().toUInt();
    } else if (type() == "font") {
        m_groupsMap[currentGroup()][key()] = QFont(defaultValue());
    } else if (type() == "int") {
        m_groupsMap[currentGroup()][key()] = defaultValue().toInt();
    } else if (type() == "password") {
        m_groupsMap[currentGroup()][key()] = defaultValue();
    } else if (type() == "path") {
        m_groupsMap[currentGroup()][key()] = defaultValue();
    } else if (type() == "string") {
        m_groupsMap[currentGroup()][key()] = defaultValue();
    } else if (type() == "stringlist") {
        //FIXME: the split() is naive and will break on lists with ,'s in them
        m_groupsMap[currentGroup()][key()] = defaultValue().split(',');
    } else if (type() == "uint") {
        m_groupsMap[currentGroup()][key()] = defaultValue().toUInt();
    } else if (type() == "url") {
        setKey((key().isEmpty()) ? name() : key());
        m_groupsMap[currentGroup()][key()] = QUrl::fromUserInput(defaultValue());
    } else if (type() == "double") {
        m_groupsMap[currentGroup()][key()] = defaultValue().toDouble();
    } else if (type() == "intlist") {
        QStringList tmpList = defaultValue().split(',');
        QList<int> defaultList;
        foreach (const QString &tmp, tmpList) {
            defaultList.append(tmp.toInt());
        }
        m_groupsMap[currentGroup()][key()] = QVariant::fromValue(defaultList);
    } else if (type() == "longlong") {
        m_groupsMap[currentGroup()][key()] = defaultValue().toLongLong();
    } else if (type() == "point") {
        QPoint defaultPoint;
        QStringList tmpList = defaultValue().split(',');
        if (tmpList.size() >= 2) {
            defaultPoint.setX(tmpList[0].toInt());
            defaultPoint.setY(tmpList[1].toInt());
        }
        m_groupsMap[currentGroup()][key()] = defaultPoint;
    } else if (type() == "rect") {
        QRect defaultRect;
        QStringList tmpList = defaultValue().split(',');
        if (tmpList.size() >= 4) {
            defaultRect.setCoords(tmpList[0].toInt(), tmpList[1].toInt(),
                                  tmpList[2].toInt(), tmpList[3].toInt());
        }
        m_groupsMap[currentGroup()][key()] = tmpList;
    } else if (type() == "size") {
        QSize defaultSize;
        QStringList tmpList = defaultValue().split(',');
        if (tmpList.size() >= 2) {
            defaultSize.setWidth(tmpList[0].toInt());
            defaultSize.setHeight(tmpList[1].toInt());
        }
        m_groupsMap[currentGroup()][key()] = tmpList;
    } else if (type() == "ulonglong") {
        m_groupsMap[currentGroup()][key()] = defaultValue().toULongLong();
    }
}

const QMap<QString, QVariantMap> &ConfigLoaderHandlerMap::groupsMap() const
{
    return m_groupsMap;
}

Service::Service(QObject *parent)
    : QObject(parent),
      d(new ServicePrivate(this))
{
}

Service::Service(QObject *parent, const QVariantList &args)
    : QObject(parent),
      d(new ServicePrivate(this))
{
    Q_UNUSED(args)
}

Service::~Service()
{
    delete d;
}

KConfigGroup ServicePrivate::dummyGroup()
{
    if (!dummyConfig) {
        dummyConfig = new KConfig(QString(), KConfig::SimpleConfig);
    }

    return KConfigGroup(dummyConfig, "DummyGroup");
}

void Service::setDestination(const QString &destination)
{
    d->destination = destination;
}

QString Service::destination() const
{
    return d->destination;
}

QStringList Service::operationNames() const
{
    if (d->operationsMap.keys().isEmpty()) {
#ifndef NDEBUG
        kDebug() << "No valid operations scheme has been registered";
#endif
        return QStringList();
    }

    return d->operationsMap.keys();
}

QVariantMap Service::operationDescription(const QString &operationName)
{
    if (d->operationsMap.keys().isEmpty()) {
#ifndef NDEBUG
        kDebug() << "No valid operations scheme has been registered";
#endif
        return QVariantMap();
    }

    //kDebug() << "operation" << operationName
    //         << "requested, has keys" << d->operationsMap.keys();
    return d->operationsMap.value(operationName);
}

ServiceJob *Service::startOperationCall(const QVariantMap &description, QObject *parent)
{
    // TODO: nested groups?
    ServiceJob *job = 0;
    const QString op = !description.isEmpty() ? description.value("_name").toString() : QString();

    if (d->operationsMap.keys().isEmpty()) {
#ifndef NDEBUG
        kDebug() << "No valid operations scheme has been registered";
#endif
    } else if (!op.isEmpty() && d->operationsMap.contains(op)) {
        if (d->disabledOperations.contains(op)) {
#ifndef NDEBUG
            kDebug() << "Operation" << op << "is disabled";
#endif
        } else {
            QVariantMap map = description;
            job = createJob(op, map);
        }
    } else {
#ifndef NDEBUG
    kDebug() << op << "is not a valid group; valid groups are:" << d->operationsMap.keys();
#endif
    }

    if (!job) {
        job = new NullServiceJob(d->destination, op, this);
    }

    job->setParent(parent ? parent : this);
    QTimer::singleShot(0, job, SLOT(autoStart()));
    return job;
}

QString Service::name() const
{
    return d->name;
}

void Service::setName(const QString &name)
{
    d->name = name;

    // now reset the config, which may be based on our name
    d->operationsMap.clear();

    delete d->dummyConfig;
    d->dummyConfig = 0;

    registerOperationsScheme();

    emit serviceReady(this);
}

void Service::setOperationEnabled(const QString &operation, bool enable)
{
    if (d->operationsMap.keys().isEmpty() || !d->operationsMap.contains(operation)) {
        return;
    }

    if (enable) {
        d->disabledOperations.remove(operation);
    } else {
        d->disabledOperations.insert(operation);
    }

    emit operationEnabledChanged(operation, enable);
}

bool Service::isOperationEnabled(const QString &operation) const
{
    return d->operationsMap.contains(operation) && !d->disabledOperations.contains(operation);
}

void Service::setOperationsScheme(QIODevice *xml)
{
    d->operationsMap.clear();

    delete d->dummyConfig;
    d->dummyConfig = 0;

    ConfigLoaderPrivate *configLoaderPrivate = new ConfigLoaderPrivate;
    configLoaderPrivate->setWriteDefaults(true);
    ConfigLoaderHandlerMap configLoaderHandler(0, configLoaderPrivate);
    QXmlInputSource source(xml);
    QXmlSimpleReader reader;
    reader.setContentHandler(&configLoaderHandler);
    reader.parse(&source, false);
    d->operationsMap = configLoaderHandler.groupsMap();
    delete configLoaderPrivate;
}

void Service::registerOperationsScheme()
{
    if (!d->operationsMap.keys().isEmpty()) {
        // we've already done our job. let's go home.
        return;
    }

    if (d->name.isEmpty()) {
#ifndef NDEBUG
        kDebug() << "No name found";
#endif
        return;
    }

    const QString path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "plasma/services/" + d->name + ".operations");

    if (path.isEmpty()) {
#ifndef NDEBUG
        kDebug() << "Cannot find operations description:" << d->name << ".operations";
#endif
        return;
    }

    QFile file(path);
    setOperationsScheme(&file);
}

} // namespace Plasma



#include "moc_service.cpp"
