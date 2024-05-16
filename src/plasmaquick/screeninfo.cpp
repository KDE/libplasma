#include "screeninfo.h"
#include <QScreen>

Q_GLOBAL_STATIC(ScreenWorkAreaBackend, s_self);

ScreenWorkAreaBackend *ScreenWorkAreaBackend::self()
{
    return s_self;
}

class ScreenWorkAreaBackendPrivate
{
public:
    struct WorkAreaBlocker {
        QScreen *screen = nullptr;
        Qt::Edges edge;
        int size = 0;
    };
    QHash<QWindow *, WorkAreaBlocker> m_workAreaBlockers;
};

ScreenWorkAreaBackend::ScreenWorkAreaBackend()
    : d(new ScreenWorkAreaBackendPrivate)
{
}

ScreenWorkAreaBackend::~ScreenWorkAreaBackend()
{
}

void ScreenWorkAreaBackend::updateScreenEdgeRecord(QWindow *window, QScreen *screen, Qt::Edges edge, int size)
{
    ScreenWorkAreaBackendPrivate::WorkAreaBlocker &data = d->m_workAreaBlockers[window];
    QScreen *oldScreen = data.screen;

    data.screen = screen;
    data.edge = edge;
    data.size = size;

    if (oldScreen && oldScreen != screen) {
        Q_EMIT usableScreenAreaChanged(oldScreen);
    }
    Q_EMIT usableScreenAreaChanged(screen);
}

void ScreenWorkAreaBackend::removeScreenEdgeRecord(QWindow *window)
{
    ScreenWorkAreaBackendPrivate::WorkAreaBlocker data = d->m_workAreaBlockers.take(window);
    QScreen *oldScreen = data.screen;
    if (oldScreen) {
        Q_EMIT usableScreenAreaChanged(oldScreen);
    }
}

QMargins ScreenWorkAreaBackend::screenEdgeMargins(QScreen *screen)
{
    QMargins margins;
    for (const auto r : d->m_workAreaBlockers) {
        QMargins reservedEdge;
        if (r.screen != screen) {
            continue;
        }
        switch (r.edge) {
        case Qt::TopEdge:
            reservedEdge.setTop(r.size);
            break;
        case Qt::RightEdge:
            reservedEdge.setRight(r.size);
            break;
        case Qt::LeftEdge:
            reservedEdge.setLeft(r.size);
            break;
        case Qt::BottomEdge:
            reservedEdge.setBottom(r.size);
            break;
        }
        margins = margins | reservedEdge;
    }
    return margins;
}

QRect ScreenWorkAreaBackend::usableScreenArea(QScreen *screen)
{
    QRect screenRect = screen->geometry();
    return screenRect.marginsRemoved(screenEdgeMargins(screen));
}

ScreenWorkArea::ScreenWorkArea(QObject *parent)
    : QObject(parent)
{
    connect(ScreenWorkAreaBackend::self(), &ScreenWorkAreaBackend::usableScreenAreaChanged, this, [this](QScreen *screen) {
        if (screen != m_screen) {
            return;
        }
        Q_EMIT usableAreaChanged();
    });
}

QScreen *ScreenWorkArea::screen() const
{
    return m_screen;
}

void ScreenWorkArea::setScreen(QScreen *screen)
{
    if (m_screen == screen) {
        return;
    }

    m_screen = screen;
    Q_EMIT screenChanged();
    Q_EMIT usableAreaChanged();
}

QRect ScreenWorkArea::usableArea() const
{
    if (!m_screen) {
        return QRect();
    }
    return ScreenWorkAreaBackend::self()->usableScreenArea(m_screen);
}

int ScreenWorkArea::topMargin() const
{
    if (!m_screen) {
        return 0;
    }
    return ScreenWorkAreaBackend::self()->screenEdgeMargins(m_screen).top();
}

int ScreenWorkArea::bottomMargin() const
{
    if (!m_screen) {
        return 0;
    }
    return ScreenWorkAreaBackend::self()->screenEdgeMargins(m_screen).bottom();
}

int ScreenWorkArea::leftMargin() const
{
    if (!m_screen) {
        return 0;
    }
    return ScreenWorkAreaBackend::self()->screenEdgeMargins(m_screen).left();
}

int ScreenWorkArea::rightMargin() const
{
    if (!m_screen) {
        return 0;
    }
    return ScreenWorkAreaBackend::self()->screenEdgeMargins(m_screen).right();
}
