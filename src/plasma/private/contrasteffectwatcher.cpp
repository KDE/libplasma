/*
    SPDX-FileCopyrightText: 2011 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "contrasteffectwatcher_p.h"

#include <QWaylandClientExtensionTemplate>

#include <KWindowSystem>

#if HAVE_X11
#include <X11/Xlib.h>
#endif

#include "qwayland-contrast.h"

namespace Plasma
{

class ContrastManager : public QWaylandClientExtensionTemplate<ContrastManager>, public QtWayland::org_kde_kwin_contrast_manager
{
public:
    ContrastManager()
        : QWaylandClientExtensionTemplate<ContrastManager>(2)
    {
    }
    ~ContrastManager()
    {
        if (object()) {
            org_kde_kwin_contrast_manager_destroy(object());
        }
    }
};

ContrastEffectWatcher::ContrastEffectWatcher(QObject *parent)
    : QObject(parent)
#if HAVE_X11
    , m_property(XCB_ATOM_NONE)
    , m_x11Interface(qGuiApp->nativeInterface<QNativeInterface::QX11Application>())
#endif
{
    if (KWindowSystem::isPlatformWayland()) {
        m_contrastManager = std::make_unique<ContrastManager>();
    }

    init();
}

ContrastEffectWatcher::~ContrastEffectWatcher()
{
}

void ContrastEffectWatcher::init()
{
    if (KWindowSystem::isPlatformWayland()) {
        connect(m_contrastManager.get(), &ContrastManager::activeChanged, this, [this]() {
            m_effectActive = m_contrastManager->isActive();
            Q_EMIT effectChanged(m_effectActive);
        });
        m_effectActive = m_contrastManager->isActive();
    } else if (KWindowSystem::isPlatformX11()) {
#if HAVE_X11
        if (!m_x11Interface) {
            return;
        }
        QCoreApplication::instance()->installNativeEventFilter(this);

        xcb_connection_t *c = m_x11Interface->connection();
        const QByteArray propertyName("_KDE_NET_WM_BACKGROUND_CONTRAST_REGION");
        xcb_intern_atom_cookie_t atomCookie = xcb_intern_atom_unchecked(c, false, propertyName.length(), propertyName.constData());
        xcb_get_window_attributes_cookie_t winAttrCookie = xcb_get_window_attributes_unchecked(c, DefaultRootWindow(m_x11Interface->display()));

        QScopedPointer<xcb_intern_atom_reply_t, QScopedPointerPodDeleter> atom(xcb_intern_atom_reply(c, atomCookie, nullptr));
        if (!atom.isNull()) {
            m_property = atom->atom;
        }

        m_effectActive = fetchEffectActive();

        QScopedPointer<xcb_get_window_attributes_reply_t, QScopedPointerPodDeleter> attrs(xcb_get_window_attributes_reply(c, winAttrCookie, nullptr));
        if (!attrs.isNull()) {
            uint32_t events = attrs->your_event_mask | XCB_EVENT_MASK_PROPERTY_CHANGE;
            xcb_change_window_attributes(c, DefaultRootWindow(m_x11Interface->display()), XCB_CW_EVENT_MASK, &events);
        }
#endif
    }
}

#if HAVE_X11
bool ContrastEffectWatcher::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
{
    if (KWindowSystem::isPlatformWayland()) {
        return false;
    }

    Q_UNUSED(message);
    Q_UNUSED(result);
    // A faster comparison than eventType != "xcb_generic_event_t"
    // given that eventType can only have the following values:
    // "xcb_generic_event_t", "windows_generic_MSG" and "mac_generic_NSEvent"
    // According to https://doc.qt.io/qt-5/qabstractnativeeventfilter.html
    if (eventType[0] != 'x') {
        return false;
    }
    xcb_generic_event_t *event = reinterpret_cast<xcb_generic_event_t *>(message);
    uint response_type = event->response_type & ~0x80;
    if (response_type != XCB_PROPERTY_NOTIFY || m_property == XCB_ATOM_NONE) {
        return false;
    }

    xcb_property_notify_event_t *prop_event = reinterpret_cast<xcb_property_notify_event_t *>(event);
    if (prop_event->atom == m_property) {
        bool nowEffectActive = fetchEffectActive();
        if (m_effectActive != nowEffectActive) {
            m_effectActive = nowEffectActive;
            Q_EMIT effectChanged(m_effectActive);
        }
    }

    return false;
}
#endif

bool ContrastEffectWatcher::isEffectActive() const
{
    return m_effectActive;
}

bool ContrastEffectWatcher::fetchEffectActive() const
{
    if (KWindowSystem::isPlatformWayland()) {
        return m_contrastManager->isActive();
    }

    if (m_property == XCB_ATOM_NONE || !m_x11Interface) {
        return false;
    }
    xcb_connection_t *c = m_x11Interface->connection();
    xcb_list_properties_cookie_t propsCookie = xcb_list_properties_unchecked(c, DefaultRootWindow(m_x11Interface->display()));
    QScopedPointer<xcb_list_properties_reply_t, QScopedPointerPodDeleter> props(xcb_list_properties_reply(c, propsCookie, nullptr));
    if (props.isNull()) {
        return false;
    }
    xcb_atom_t *atoms = xcb_list_properties_atoms(props.data());
    for (int i = 0; i < props->atoms_len; ++i) {
        if (atoms[i] == m_property) {
            return true;
        }
    }
    return false;
}

} // namespace Plasma
