// SPDX-FileCopyrightText: %{CURRENT_YEAR} %{AUTHOR} <%{EMAIL}>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>
#include <QString>
#include <QQmlEngine>

class HelloWorld : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QString message READ message CONSTANT)

public:
    static HelloWorld *create(QQmlEngine *, QJSEngine *);

    QString message() const;

private:
    HelloWorld();
};
