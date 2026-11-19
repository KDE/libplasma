// SPDX-FileCopyrightText: 2026 Noah Davis <noahadvs@gmail.comm>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "plasmacomponentsplugin.h"

PlasmaComponentsPlugin::PlasmaComponentsPlugin(QObject *parent)
    : QQmlExtensionPlugin(parent)
{
}

void PlasmaComponentsPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == s_plasmaComponentsUri);
    m_importUri = qgetenv("PLASMA_COMPONENTS_IMPORT_URI");
    if (m_importUri.isEmpty()) {
        m_importUri = s_plasmaComponentsUri;
    }
    qmlRegisterModuleImport(uri, QQmlModuleImportModuleAny, m_importUri.data(), QQmlModuleImportLatest, QQmlModuleImportAuto);
}

void PlasmaComponentsPlugin::unregisterTypes()
{
    qmlUnregisterModuleImport(s_plasmaComponentsUri, QQmlModuleImportModuleAny, m_importUri.data(), QQmlModuleImportLatest, QQmlModuleImportAuto);
}

#include "moc_plasmacomponentsplugin.cpp"
