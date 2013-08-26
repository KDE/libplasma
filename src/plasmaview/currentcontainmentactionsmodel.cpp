/*
 *   Copyright 2013 Marco Martin <mart@kde.org>
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

#include "currentcontainmentactionsmodel.h"

#include <QMouseEvent>

#include <QDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>

#include <KAboutData>
#include <KAboutApplicationDialog>
#include <KLocalizedString>

#include <Plasma/Corona>
#include <Plasma/Containment>
#include <Plasma/ContainmentActions>
#include <Plasma/PluginLoader>


CurrentContainmentActionsModel::CurrentContainmentActionsModel(Plasma::Containment *cotainment, QObject *parent)
    : QStandardItemModel(parent),
      m_containment(cotainment)
{
    QHash<int, QByteArray> roleNames;
    roleNames[ActionRole] = "action";
    roleNames[PluginRole] = "plugin";

    setRoleNames(roleNames);

    m_baseCfg = KConfigGroup(m_containment->corona()->config(), "ActionPlugins");

    QHash<QString, Plasma::ContainmentActions*> actions = cotainment->containmentActions();

    QHashIterator<QString, Plasma::ContainmentActions*> i(actions);
    while (i.hasNext()) {
        i.next();

        QStandardItem *item = new QStandardItem();
        item->setData(i.key(), ActionRole);
        item->setData(i.value()->pluginInfo().pluginName(), PluginRole);
        appendRow(item);
        m_plugins[i.key()] = Plasma::PluginLoader::self()->loadContainmentActions(m_containment, i.value()->pluginInfo().pluginName());
        m_plugins[i.key()]->setContainment(m_containment);
        KConfigGroup cfg(&m_baseCfg, i.key());
        m_plugins[i.key()]->restore(cfg);
    }
}

CurrentContainmentActionsModel::~CurrentContainmentActionsModel()
{
}

QString CurrentContainmentActionsModel::mouseEventString(int mouseButton, int modifiers)
{
    QMouseEvent *mouse = new QMouseEvent(QEvent::MouseButtonRelease, QPoint(), (Qt::MouseButton)mouseButton, (Qt::MouseButton)mouseButton, (Qt::KeyboardModifiers) modifiers);

    QString string = Plasma::ContainmentActions::eventToString(mouse);

    delete mouse;

    return string;
}

QString CurrentContainmentActionsModel::wheelEventString(const QPointF &delta, int mouseButtons, int modifiers)
{
    QWheelEvent *wheel = new QWheelEvent(QPointF(), QPointF(), delta.toPoint(), QPoint(), 0, Qt::Vertical, (Qt::MouseButtons)mouseButtons, (Qt::KeyboardModifiers) modifiers);

    QString string = Plasma::ContainmentActions::eventToString(wheel);

    delete wheel;

    return string;
}

bool CurrentContainmentActionsModel::append(const QString &action, const QString &plugin)
{
    if (m_plugins.contains(action)) {
        return false;
    }

    QStandardItem *item = new QStandardItem();
    item->setData(action, ActionRole);
    item->setData(plugin, PluginRole);
    appendRow(item);
    m_plugins[action] = Plasma::PluginLoader::self()->loadContainmentActions(m_containment, plugin);
    KConfigGroup cfg(&m_baseCfg, action);
    m_plugins[action]->setContainment(m_containment);
    m_plugins[action]->restore(cfg);
    return true;
}

void CurrentContainmentActionsModel::update(int row, const QString &action, const QString &plugin)
{
    const QString oldPlugin = itemData(index(row, 0)).value(PluginRole).toString();

    QModelIndex idx = index(row, 0);

    if (idx.isValid()) {
        setData(idx, action, ActionRole);
        setData(idx, plugin, PluginRole);

        if (m_plugins.contains(action) && oldPlugin != plugin) {
            delete m_plugins[action];
            m_plugins[action] = Plasma::PluginLoader::self()->loadContainmentActions(m_containment, plugin);
        }
    }
}

void CurrentContainmentActionsModel::remove(int row)
{
    const QString action = itemData(index(row, 0)).value(ActionRole).toString();
    removeRows(row, 1);

    if (m_plugins.contains(action)) {
        delete m_plugins[action];
        m_plugins.remove(action);
    }
}

void CurrentContainmentActionsModel::showConfiguration(int row)
{
    const QString action = itemData(index(row, 0)).value(ActionRole).toString();

    if (!m_plugins.contains(action)) {
        return;
    }

    QDialog *configDlg = new QDialog();
    configDlg->setAttribute(Qt::WA_DeleteOnClose);
    QLayout *lay = new QVBoxLayout(configDlg);
    configDlg->setLayout(lay);
    configDlg->setWindowModality(Qt::WindowModal);

    //put the config in the dialog
    QWidget *w = m_plugins[action]->createConfigurationInterface(configDlg);
    QString title;
    if (w) {
        lay->addWidget(w);
        title = w->windowTitle();
    }

    configDlg->setWindowTitle(title.isEmpty() ? i18n("Configure Plugin") :title);
    //put buttons below
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                        Qt::Horizontal, configDlg);
    lay->addWidget(buttons);

    connect(buttons, SIGNAL(accepted()), this, SLOT(acceptConfig()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(rejectConfig()));


    configDlg->show();
}

void CurrentContainmentActionsModel::showAbout(int row)
{
    const QString action = itemData(index(row, 0)).value(ActionRole).toString();

    if (!m_plugins.contains(action)) {
        return;
    }

    KPluginInfo info = m_plugins[action]->pluginInfo();

    KAboutData aboutData(info.name().toUtf8(),
            info.name().toUtf8(),
            ki18n(info.name().toUtf8()).toString(),
            info.version().toUtf8(), ki18n(info.comment().toUtf8()).toString(),
            KAboutLicense::byKeyword(info.license()).key(), ki18n(QByteArray()).toString(), ki18n(QByteArray()).toString(), info.website().toLatin1(),
            info.email().toLatin1());

    aboutData.setProgramIconName(info.icon());

    aboutData.addAuthor(ki18n(info.author().toUtf8()).toString(), ki18n(QByteArray()).toString(), info.email().toLatin1());

    KAboutApplicationDialog *aboutDialog = new KAboutApplicationDialog(aboutData, qobject_cast<QWidget*>(parent()));
    aboutDialog->show();
}

void CurrentContainmentActionsModel::save()
{

    //TODO: this configuration save is still a stub, not completely "correct" yet
    //clean old config, just i case
    foreach (const QString &group, m_baseCfg.groupList()) {
        KConfigGroup cfg = KConfigGroup(&m_baseCfg, group);
        cfg.deleteGroup();

        if (m_plugins.contains(group)) {
            m_containment->setContainmentActions(group, QString());
        }
    }

    QHashIterator<QString, Plasma::ContainmentActions*> i(m_plugins);
    while (i.hasNext()) {
        m_containment->setContainmentActions(i.key(), i.value()->pluginInfo().pluginName());
        i.next();
        KConfigGroup cfg(&m_baseCfg, i.key());
        i.value()->save(cfg);
    }
}

#include "moc_currentcontainmentactionsmodel.cpp"
