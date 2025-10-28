/*
    SPDX-FileCopyrightText: 2023 David Edmundson <davidedmundson@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QMargins>
#include <QObject>
#include <memory>

#include <plasmaquick/plasmaquick_export.h>

class QWindow;

class WindowResizeHandlerPrivate;

namespace PlasmaQuick
{

class PLASMAQUICK_EXPORT WindowResizeHandler : public QObject
{
    Q_OBJECT
public:
    WindowResizeHandler(QWindow *parent);
    ~WindowResizeHandler() override;

    void setMargins(const QMargins &margins);
    QMargins margins() const;

    void setActiveEdges(Qt::Edges edges);
    Qt::Edges activeEdges() const;

    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    std::unique_ptr<WindowResizeHandlerPrivate> d;
};

}
