/*
    SPDX-FileCopyrightText: 2023 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "plasmashellwaylandintegration.h"

#include <QGuiApplication>
#include <QPlatformSurfaceEvent>
#include <QWaylandClientExtensionTemplate>
#include <QWindow>

#include <qpa/qplatformwindow_p.h>

#include <KWindowSystem>

class PlasmaShellManager : public QWaylandClientExtensionTemplate<PlasmaShellManager>, public QtWayland::org_kde_plasma_shell
{
public:
    PlasmaShellManager()
        : QWaylandClientExtensionTemplate<PlasmaShellManager>(8)
    {
        initialize();
    }
};

class PlasmaShellSurface : public QtWayland::org_kde_plasma_surface
{
public:
    PlasmaShellSurface(struct ::org_kde_plasma_surface *impl)
        : QtWayland::org_kde_plasma_surface(impl)
    {
    }
    ~PlasmaShellSurface()
    {
        destroy();
    }
};

class WaylandIntegrationSingleton
{
public:
    WaylandIntegrationSingleton();
    std::unique_ptr<PlasmaShellManager> shellManager;
    QHash<QWindow *, PlasmaShellWaylandIntegration *> windows;
};

WaylandIntegrationSingleton::WaylandIntegrationSingleton()
{
    if (KWindowSystem::isPlatformWayland()) {
        shellManager = std::make_unique<PlasmaShellManager>();
    }
}

Q_GLOBAL_STATIC(WaylandIntegrationSingleton, s_waylandIntegration)

class PlasmaShellWaylandIntegrationPrivate
{
public:
    PlasmaShellWaylandIntegrationPrivate(PlasmaShellWaylandIntegration *integration, QWindow *window);

    void platformSurfaceCreated(QWindow *window);
    void surfaceCreated();
    void surfaceDestroyed();

    PlasmaShellWaylandIntegration *q;
    QWindow *m_window = nullptr;
    std::optional<QPoint> m_position;
    QtWayland::org_kde_plasma_surface::panel_behavior m_panelBehavior = QtWayland::org_kde_plasma_surface::panel_behavior_always_visible;
    QtWayland::org_kde_plasma_surface::role m_role = QtWayland::org_kde_plasma_surface::role_normal;
    bool m_takesFocus = false;
    std::unique_ptr<PlasmaShellSurface> m_shellSurface;
};

PlasmaShellWaylandIntegrationPrivate::PlasmaShellWaylandIntegrationPrivate(PlasmaShellWaylandIntegration *integration, QWindow *window)
    : q(integration)
    , m_window(window)
{
}

void PlasmaShellWaylandIntegrationPrivate::platformSurfaceCreated(QWindow *window)
{
    auto waylandWindow = window->nativeInterface<QNativeInterface::Private::QWaylandWindow>();
    if (!waylandWindow) { // It may be null, e.g. when called within KWin
        return;
    }
    QObject::connect(waylandWindow, SIGNAL(surfaceCreated()), q, SLOT(surfaceCreated()));
    QObject::connect(waylandWindow, SIGNAL(surfaceDestroyed()), q, SLOT(surfaceDestroyed()));
    if (waylandWindow->surface()) {
        surfaceCreated();
    }
}

void PlasmaShellWaylandIntegrationPrivate::surfaceCreated()
{
    struct wl_surface *surface = nullptr;
    if (!s_waylandIntegration->shellManager || !s_waylandIntegration->shellManager->isActive()) {
        return;
    }

    if (auto waylandWindow = m_window->nativeInterface<QNativeInterface::Private::QWaylandWindow>()) {
        surface = waylandWindow->surface();
    }

    if (!surface) {
        return;
    }

    m_shellSurface = std::make_unique<PlasmaShellSurface>(s_waylandIntegration->shellManager->get_surface(surface));
    if (m_shellSurface) {
        if (m_position) {
            m_shellSurface->set_position(m_position->x(), m_position->y());
        }
        m_shellSurface->set_panel_takes_focus(m_takesFocus);
        m_shellSurface->set_role(m_role);
        m_shellSurface->set_skip_switcher(true);
        m_shellSurface->set_skip_taskbar(true);
    }
}

void PlasmaShellWaylandIntegrationPrivate::surfaceDestroyed()
{
    m_shellSurface.reset();
}

PlasmaShellWaylandIntegration *PlasmaShellWaylandIntegration::get(QWindow *window)
{
    PlasmaShellWaylandIntegration *&it = s_waylandIntegration->windows[window];
    if (!it) {
        it = new PlasmaShellWaylandIntegration(window);
    }
    return it;
}

PlasmaShellWaylandIntegration::~PlasmaShellWaylandIntegration()
{
    s_waylandIntegration->windows.remove(d->m_window);
}

PlasmaShellWaylandIntegration::PlasmaShellWaylandIntegration(QWindow *window)
    : QObject(window)
    , d(new PlasmaShellWaylandIntegrationPrivate(this, window))
{
    if (!KWindowSystem::isPlatformWayland()) {
        return;
    }
    d->m_window->installEventFilter(this);
    d->m_window->setProperty("_q_showWithoutActivating", !d->m_takesFocus);
    d->platformSurfaceCreated(window);
}

bool PlasmaShellWaylandIntegration::eventFilter(QObject *watched, QEvent *event)
{
    auto window = qobject_cast<QWindow *>(watched);
    if (!window) {
        return false;
    }
    if (event->type() == QEvent::PlatformSurface) {
        auto surfaceEvent = static_cast<QPlatformSurfaceEvent *>(event);
        if (surfaceEvent->surfaceEventType() == QPlatformSurfaceEvent::SurfaceCreated) {
            d->platformSurfaceCreated(window);
        }
    }
    return false;
}

void PlasmaShellWaylandIntegration::setPosition(const QPoint &position)
{
    if (position == d->m_position) {
        return;
    }

    d->m_position = position;
    if (d->m_shellSurface) {
        d->m_shellSurface->set_position(d->m_position->x(), d->m_position->y());
    }
}

void PlasmaShellWaylandIntegration::setPanelBehavior(QtWayland::org_kde_plasma_surface::panel_behavior panelBehavior)
{
    if (panelBehavior == d->m_panelBehavior) {
        return;
    }
    d->m_panelBehavior = panelBehavior;
    if (d->m_shellSurface) {
        d->m_shellSurface->set_panel_behavior(panelBehavior);
    }
}

void PlasmaShellWaylandIntegration::setRole(QtWayland::org_kde_plasma_surface::role role)
{
    if (role == d->m_role) {
        return;
    }
    d->m_role = role;
    if (d->m_shellSurface) {
        d->m_shellSurface->set_role(role);
    }
}

void PlasmaShellWaylandIntegration::setTakesFocus(bool takesFocus)
{
    if (takesFocus == d->m_takesFocus) {
        return;
    }
    d->m_takesFocus = takesFocus;
    d->m_window->setProperty("_q_showWithoutActivating", !d->m_takesFocus);
    if (d->m_shellSurface) {
        d->m_shellSurface->set_panel_takes_focus(takesFocus);
    }
}

#include "moc_plasmashellwaylandintegration.cpp"
