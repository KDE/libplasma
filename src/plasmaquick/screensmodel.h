/*
    SPDX-FileCopyrightText: 2026 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QAbstractListModel>
#include <QList>

#include <plasmaquick/plasmaquick_export.h>

class QScreen;

namespace PlasmaQuick
{

/*!
 * ScreensModel is a model of all active screens.
 * The order of screens has no semantic meaning.
 *
 * @note QGuiApplication::screens contains a placeholder fake screen when no real screens are connected. This model deliberately excludes them.
 */
class PLASMAQUICK_EXPORT ScreensModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        //! Returns the underlying QScreen* object
        ScreenRole = Qt::UserRole + 1,
    };
    Q_ENUM(Roles)

    /*!
     * Constructs a default screensmodel
     */
    explicit ScreensModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    bool acceptsScreen(QScreen *screen) const;
    QList<QScreen *> m_screens;
};

}
