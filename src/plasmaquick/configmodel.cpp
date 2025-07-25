/*
    SPDX-FileCopyrightText: 2013 Marco Martin <mart@kde.org>
    SPDX-FileCopyrightText: 2015 Eike Hein <hein@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "configmodel.h"

#include "applet.h"
#include "configcategory_p.h"

#include <QDebug>
#include <QPointer>

namespace PlasmaQuick
{
//////////////////////////////ConfigModel

class ConfigModelPrivate
{
public:
    ConfigModelPrivate(ConfigModel *model);
    ~ConfigModelPrivate();

    ConfigModel *q;
    QList<ConfigCategory *> categories;
    QPointer<Plasma::Applet> appletInterface;

    void appendCategory(ConfigCategory *c);
    void removeCategory(ConfigCategory *c);
    void removeCategoryAt(int index);
    void clear();
    QVariant get(int row) const;

    static ConfigCategory *categories_at(QQmlListProperty<ConfigCategory> *prop, qsizetype index);
    static qsizetype categories_count(QQmlListProperty<ConfigCategory> *prop);
    static void categories_append(QQmlListProperty<ConfigCategory> *prop, ConfigCategory *o);
    static void categories_clear(QQmlListProperty<ConfigCategory> *prop);
};

ConfigModelPrivate::ConfigModelPrivate(ConfigModel *model)
    : q(model)
{
}

ConfigModelPrivate::~ConfigModelPrivate()
{
}

ConfigCategory *ConfigModelPrivate::categories_at(QQmlListProperty<ConfigCategory> *prop, qsizetype index)
{
    ConfigModel *model = qobject_cast<ConfigModel *>(prop->object);
    if (!model || index >= model->d->categories.count() || index < 0) {
        return nullptr;
    } else {
        return model->d->categories.at(index);
    }
}

void ConfigModelPrivate::categories_append(QQmlListProperty<ConfigCategory> *prop, ConfigCategory *o)
{
    ConfigModel *model = qobject_cast<ConfigModel *>(prop->object);
    if (!o || !model) {
        return;
    }

    if (o->parent() == prop->object) {
        o->setParent(nullptr);
    }

    o->setParent(prop->object);
    model->d->appendCategory(o);
}

qsizetype ConfigModelPrivate::categories_count(QQmlListProperty<ConfigCategory> *prop)
{
    ConfigModel *model = qobject_cast<ConfigModel *>(prop->object);
    if (model) {
        return model->d->categories.count();
    } else {
        return 0;
    }
}

void ConfigModelPrivate::categories_clear(QQmlListProperty<ConfigCategory> *prop)
{
    ConfigModel *model = qobject_cast<ConfigModel *>(prop->object);
    if (!model) {
        return;
    }

    model->clear();
}

void ConfigModelPrivate::clear()
{
    q->beginResetModel();
    while (!categories.isEmpty()) {
        categories.first()->setParent(nullptr);
        categories.pop_front();
    }
    q->endResetModel();
    Q_EMIT q->countChanged();
}

void ConfigModelPrivate::appendCategory(ConfigCategory *c)
{
    if (!c) {
        return;
    }

    q->beginInsertRows(QModelIndex(), categories.size(), categories.size());
    categories.append(c);

    auto emitChange = [this, c] {
        const int row = categories.indexOf(c);
        if (row > -1) {
            QModelIndex modelIndex = q->index(row);
            Q_EMIT q->dataChanged(modelIndex, modelIndex);
        }
    };

    QObject::connect(c, &ConfigCategory::nameChanged, q, emitChange);
    QObject::connect(c, &ConfigCategory::iconChanged, q, emitChange);
    QObject::connect(c, &ConfigCategory::sourceChanged, q, emitChange);
    QObject::connect(c, &ConfigCategory::pluginNameChanged, q, emitChange);
    QObject::connect(c, &ConfigCategory::visibleChanged, q, emitChange);

    q->endInsertRows();
    Q_EMIT q->countChanged();
}

void ConfigModelPrivate::removeCategory(ConfigCategory *c)
{
    const int index = categories.indexOf(c);
    if (index > -1) {
        removeCategoryAt(index);
    }
}

void ConfigModelPrivate::removeCategoryAt(int index)
{
    if (index < 0 || index >= categories.count()) {
        return;
    }

    q->beginRemoveRows(QModelIndex(), index, index);

    ConfigCategory *c = categories.takeAt(index);
    if (c->parent() == q) {
        c->deleteLater();
    }

    q->endRemoveRows();
    Q_EMIT q->countChanged();
}

QVariant ConfigModelPrivate::get(int row) const
{
    QVariantMap value;
    if (row < 0 || row >= categories.count()) {
        return value;
    }

    value[QStringLiteral("name")] = categories.at(row)->name();
    value[QStringLiteral("icon")] = categories.at(row)->icon();
    value[QStringLiteral("pluginName")] = categories.at(row)->pluginName();
    value[QStringLiteral("source")] = q->data(q->index(row, 0), ConfigModel::SourceRole);
    value[QStringLiteral("visible")] = categories.at(row)->visible();

    return value;
}

ConfigModel::ConfigModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new ConfigModelPrivate(this))
{
}

ConfigModel::~ConfigModel()
{
    delete d;
}

int ConfigModel::rowCount(const QModelIndex &index) const
{
    if (index.column() > 0) {
        return 0;
    }
    return d->categories.count();
}

QVariant ConfigModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= d->categories.count()) {
        return QVariant();
    }
    switch (role) {
    case NameRole:
        return d->categories.at(index.row())->name();
    case IconRole:
        return d->categories.at(index.row())->icon();
    case SourceRole: {
        const QString source = d->categories.at(index.row())->source();
        // Quick check if source is an absolute path or not
        if (d->appletInterface && !source.isEmpty() && !(source.startsWith(QLatin1Char('/')) && source.endsWith(QLatin1String("qml")))) {
            return d->appletInterface.data()->fileUrl(source);
        } else {
            return source;
        }
    }
    case PluginNameRole:
        return d->categories.at(index.row())->pluginName();
    case VisibleRole:
        return d->categories.at(index.row())->visible();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> ConfigModel::roleNames() const
{
    return {
        {NameRole, "name"},
        {IconRole, "icon"},
        {SourceRole, "source"},
        {PluginNameRole, "pluginName"},
        {VisibleRole, "visible"},
    };
}

QVariant ConfigModel::get(int row) const
{
    return d->get(row);
}

void ConfigModel::appendCategory(const QString &iconName, const QString &name, const QString &path, const QString &pluginName)
{
    ConfigCategory *cat = new ConfigCategory(this);
    cat->setIcon(iconName);
    cat->setName(name);
    cat->setSource(path);
    cat->setPluginName(pluginName);
    d->appendCategory(cat);
}

void ConfigModel::appendCategory(const QString &iconName, const QString &name, const QString &path, const QString &pluginName, bool visible)
{
    ConfigCategory *cat = new ConfigCategory(this);
    cat->setIcon(iconName);
    cat->setName(name);
    cat->setSource(path);
    cat->setPluginName(pluginName);
    cat->setVisible(visible);
    d->appendCategory(cat);
}

void ConfigModel::appendCategory(ConfigCategory *category)
{
    d->appendCategory(category);
}

void ConfigModel::removeCategory(ConfigCategory *category)
{
    d->removeCategory(category);
}

void ConfigModel::removeCategoryAt(int index)
{
    d->removeCategoryAt(index);
}

void ConfigModel::clear()
{
    d->clear();
}

void ConfigModel::setApplet(Plasma::Applet *interface)
{
    d->appletInterface = interface;
}

Plasma::Applet *ConfigModel::applet() const
{
    return d->appletInterface.data();
}

QQmlListProperty<ConfigCategory> ConfigModel::categories()
{
    return QQmlListProperty<ConfigCategory>(this,
                                            nullptr,
                                            ConfigModelPrivate::categories_append,
                                            ConfigModelPrivate::categories_count,
                                            ConfigModelPrivate::categories_at,
                                            ConfigModelPrivate::categories_clear);
}

}

#include "moc_configmodel.cpp"
