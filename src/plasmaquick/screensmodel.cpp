/*
    SPDX-FileCopyrightText: 2026 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "screensmodel.h"

#include <QGuiApplication>
#include <QScreen>

using namespace Qt::StringLiterals;

namespace PlasmaQuick
{

ScreensModel::ScreensModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_screens.clear();
    for (QScreen *screen : QGuiApplication::screens()) {
        if (acceptsScreen(screen)) {
            m_screens.append(screen);
        }
    }

    connect(qGuiApp, &QGuiApplication::screenAdded, this, [this](QScreen *screen) {
        if (!acceptsScreen(screen)) {
            return;
        }

        const int row = m_screens.size();
        beginInsertRows({}, row, row);
        m_screens.append(screen);
        endInsertRows();
    });
    connect(qGuiApp, &QGuiApplication::screenRemoved, this, [this](QScreen *screen) {
        const int row = m_screens.indexOf(screen);
        if (row < 0) {
            return;
        }

        beginRemoveRows({}, row, row);
        m_screens.removeAt(row);
        endRemoveRows();
    });
}

int ScreensModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_screens.size();
}

QVariant ScreensModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_screens.size()) {
        return {};
    }

    QScreen *screen = m_screens.at(index.row());
    switch (role) {
    case Qt::DisplayRole:
        return screen->name();
    case ScreenRole:
        return QVariant::fromValue(screen);
    default:
        return {};
    }
}

QHash<int, QByteArray> ScreensModel::roleNames() const
{
    return {
        {Qt::DisplayRole, "name"_ba},
        {ScreenRole, "screenHandle"_ba},
    };
}

bool ScreensModel::acceptsScreen(QScreen *screen) const
{
    // Qt will create a fake placeholder screen when no screens are attached.
    // We can rely on this always having no name
    return !screen->name().isEmpty();
}
}
