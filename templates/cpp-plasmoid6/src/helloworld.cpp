// SPDX-FileCopyrightText: %{CURRENT_YEAR} %{AUTHOR} <%{EMAIL}>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "helloworld.h"

#include <KLocalizedString>

QString HelloWorld::message() const
{
    return i18nc("@info", "Hello World");
}

HelloWorld *HelloWorld::create(QQmlEngine *, QJSEngine *)
{
    static HelloWorld instance;
    QQmlEngine::setObjectOwnership(&instance, QQmlEngine::CppOwnership);
    return &instance;
}

HelloWorld::HelloWorld()
    : QObject(nullptr)
{}
