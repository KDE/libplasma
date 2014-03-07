/***************************************************************************
 *   Copyright 2011 Marco Martin <mart@kde.org>                            *
 *   Copyright 2013 Sebastian Kügler <sebas@kde.org>                       *
 *   Copyright 2014 Martin Gräßlin <mgraesslin@kde.org>                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#include "dialog.h"
#include "framesvgitem.h"
#include "dialogshadows_p.h"

#include <QApplication>
#include <QQuickItem>
#include <QDesktopWidget>
#include <QTimer>
#include <QLayout>
#include <QScreen>

#include <kwindowsystem.h>
#include <KWindowSystem/KWindowInfo>

#include <kwindoweffects.h>
#include <Plasma/Plasma>
#include <Plasma/Corona>

#include <QDebug>

#include <config-plasma.h>
#if HAVE_XCB_SHAPE
#include <QX11Info>
#include <xcb/shape.h>
#endif

//Unfortunately QWINDOWSIZE_MAX is not exported
#define DIALOGSIZE_MAX ((1<<24)-1)


DialogProxy::DialogProxy(QQuickItem *parent)
    : QQuickWindow(parent ? parent->window() : 0),
      m_location(Plasma::Types::BottomEdge),
      m_type(Normal),
      m_hideOnWindowDeactivate(false),
      m_outputOnly(false),
      m_componentComplete(parent == 0)
{
    QSurfaceFormat format;
    format.setAlphaBufferSize(8);
    setFormat(format);
    setClearBeforeRendering(true);
    setColor(QColor(Qt::transparent));
    setFlags(Qt::FramelessWindowHint);

    m_syncTimer = new QTimer(this);
    m_syncTimer->setSingleShot(true);
    m_syncTimer->setInterval(0);
    connect(m_syncTimer, &QTimer::timeout, this,  &DialogProxy::syncToMainItemSize);

    connect(this, &QWindow::xChanged, [=]() {
        requestSyncToMainItemSize(true);
    });
    connect(this, &QWindow::yChanged, [=]() {
        requestSyncToMainItemSize(true);
    });
    connect(this, &QWindow::visibleChanged, this, &DialogProxy::updateInputShape);
    connect(this, &DialogProxy::outputOnlyChanged, this, &DialogProxy::updateInputShape);
//    connect(this, &QWindow::visibleChanged, this, &DialogProxy::onVisibleChanged);
    //HACK: this property is invoked due to the initialization that gets done to contentItem() in the getter
    property("data");
    //Create the FrameSvg background.
    m_frameSvgItem = new Plasma::FrameSvgItem(contentItem());
    m_frameSvgItem->setImagePath("dialogs/background");

    connect(&m_theme, &Plasma::Theme::themeChanged, this, &DialogProxy::updateContrast);

    //m_frameSvgItem->setImagePath("widgets/background"); // larger borders, for testing those
}

DialogProxy::~DialogProxy()
{
    if (!qApp->closingDown()) {
        DialogShadows::self()->removeWindow(this);
    }
}

QQuickItem *DialogProxy::mainItem() const
{
    return m_mainItem.data();
}

void DialogProxy::setMainItem(QQuickItem *mainItem)
{
    if (m_mainItem.data() != mainItem) {
        if (m_mainItem) {
            m_mainItem.data()->setParent(parent());
        }

        m_mainItem = mainItem;

        if (mainItem) {
            mainItem->setParent(contentItem());
            mainItem->setProperty("parent", QVariant::fromValue(contentItem()));

            if (mainItem->metaObject()->indexOfSignal("widthChanged")) {
                connect(mainItem, &QQuickItem::widthChanged, [=]() {
                    m_syncTimer->start(0);
                });
            }
            if (mainItem->metaObject()->indexOfSignal("heightChanged")) {
                connect(mainItem, &QQuickItem::heightChanged, [=]() {
                    m_syncTimer->start(0);
                });
            }
            requestSyncToMainItemSize();

            //Extract the representation's Layout, if any
            QObject *layout = 0;

            //Search a child that has the needed Layout properties
            //HACK: here we are not type safe, but is the only way to access to a pointer of Layout
            foreach (QObject *child, mainItem->children()) {
                //find for the needed property of Layout: minimum/maximum/preferred sizes and fillWidth/fillHeight
                if (child->property("minimumWidth").isValid() && child->property("minimumHeight").isValid() &&
                    child->property("preferredWidth").isValid() && child->property("preferredHeight").isValid() &&
                    child->property("maximumWidth").isValid() && child->property("maximumHeight").isValid() &&
                    child->property("fillWidth").isValid() && child->property("fillHeight").isValid()
                ) {
                    layout = child;
                }
            }
            m_mainItemLayout = layout;

            if (layout) {
                connect(layout, SIGNAL(minimumWidthChanged()), this, SLOT(updateMinimumWidth()));
                connect(layout, SIGNAL(minimumHeightChanged()), this, SLOT(updateMinimumHeight()));
                connect(layout, SIGNAL(maximumWidthChanged()), this, SLOT(updateMaximumWidth()));
                connect(layout, SIGNAL(maximumHeightChanged()), this, SLOT(updateMaximumHeight()));

                updateMinimumWidth();
                updateMinimumHeight();
                updateMaximumWidth();
                updateMaximumHeight();
            }

        }

        //if this is called in Component.onCompleted we have to wait a loop the item is added to a scene
        emit mainItemChanged();
    }
}

QQuickItem *DialogProxy::visualParent() const
{
    return m_visualParent.data();
}

void DialogProxy::setVisualParent(QQuickItem *visualParent)
{
    if (m_visualParent.data() == visualParent) {
        return;
    }

    m_visualParent = visualParent;
    emit visualParentChanged();
    if (visualParent) {
        requestSyncToMainItemSize();
    }
}

void DialogProxy::updateVisibility(bool visible)
{
    if (visible) {
        if (location() == Plasma::Types::FullScreen) {
            m_frameSvgItem->setEnabledBorders(Plasma::FrameSvg::NoBorder);

            // We cache the original size of the item, to retrieve it
            // when the dialog is switched back from fullscreen.
            if (geometry() != screen()->availableGeometry()) {
                m_cachedGeometry = geometry();
            }
            setGeometry(screen()->availableGeometry());
        } else {
            if (!m_cachedGeometry.isNull()) {
                resize(m_cachedGeometry.size());
                syncMainItemToSize();
                m_cachedGeometry = QRect();
            }
            syncToMainItemSize();
        }
    }

    if (!(flags() & Qt::ToolTip)) {
        KWindowEffects::SlideFromLocation slideLocation = KWindowEffects::NoEdge;

        switch (m_location) {
        case Plasma::Types::TopEdge:
            slideLocation = KWindowEffects::TopEdge;
            break;
        case Plasma::Types::LeftEdge:
            slideLocation = KWindowEffects::LeftEdge;
            break;
        case Plasma::Types::RightEdge:
            slideLocation = KWindowEffects::RightEdge;
            break;
        case Plasma::Types::BottomEdge:
            slideLocation = KWindowEffects::BottomEdge;
            break;
            //no edge, no slide
        default:
            break;
        }

        KWindowEffects::slideWindow(winId(), slideLocation, -1);
    }

    if (visible) {
        raise();

        if (m_type != Normal) {
            KWindowSystem::setType(winId(), (NET::WindowType)m_type);
        } else {
            setFlags(Qt::FramelessWindowHint|flags());
        }
        if (m_type == Dock) {
            KWindowSystem::setOnAllDesktops(winId(), true);
        } else {
            KWindowSystem::setOnAllDesktops(winId(), false);
        }
    }
}

QPoint DialogProxy::popupPosition(QQuickItem *item, const QSize &size, Qt::AlignmentFlag alignment)
{
    if (!item) {
        //If no item was specified try to align at the center of the parent view
        QQuickItem *parentItem = qobject_cast<QQuickItem *>(parent());
        if (parentItem) {
            QScreen *screen = screenForItem(parentItem);

            switch (m_location) {
            case Plasma::Types::TopEdge:
                return QPoint(screen->availableGeometry().center().x() - size.width()/2, screen->availableGeometry().y());
                break;
            case Plasma::Types::LeftEdge:
                return QPoint(screen->availableGeometry().x(), screen->availableGeometry().center().y() - size.height()/2);
                break;
            case Plasma::Types::RightEdge:
                return QPoint(screen->availableGeometry().right() - size.width(), screen->availableGeometry().center().y() - size.height()/2);
                break;
            case Plasma::Types::BottomEdge:
                return QPoint(screen->availableGeometry().center().x() - size.width()/2, screen->availableGeometry().bottom() -size.height());
                break;
                //Default center in the screen
            default:
                return screen->geometry().center() - QPoint(size.width()/2, size.height()/2);
            }
        } else {
            return QPoint();
        }
    }

    QPointF pos = item->mapToScene(QPointF(0, 0));

    if (item->window()) {
        pos = item->window()->mapToGlobal(pos.toPoint());
    } else {
        return QPoint();
    }

    //swap direction if necessary
    if (QApplication::isRightToLeft() && alignment != Qt::AlignCenter) {
        if (alignment == Qt::AlignRight) {
            alignment = Qt::AlignLeft;
        } else {
            alignment = Qt::AlignRight;
        }
    }

    //if the item is in a dock or in a window that ignores WM we want to position the popups outside of the dock
    const KWindowInfo winInfo = KWindowSystem::windowInfo(item->window()->winId(), NET::WMWindowType);
    const bool outsideParentWindow = (winInfo.windowType(NET::AllTypesMask) == NET::Dock) || (item->window()->flags() & Qt::X11BypassWindowManagerHint);

    //flag shows if the popup should be placed inside or outside the parent item
    //i.e if the parent item is the desktop we want to position the dialog to the left edge of
    //the parent, not just outside the parent
    const bool locateInsideParent = winInfo.windowType(NET::AllTypesMask) == NET::Desktop;

    QRect parentGeometryBounds;
    if (outsideParentWindow) {
        parentGeometryBounds = item->window()->geometry();
    } else {
        parentGeometryBounds = QRect(pos.toPoint(), QSize(item->width(), item->height()));
    }

    if (locateInsideParent) {
        //pretend the parent is smaller so that positioning to the outside edge of the parent is
        //aligned on the inside edge of the real parent
        parentGeometryBounds.adjust(size.width(), size.height(), -size.width(), -size.height());
    }

    const QPoint topPoint(pos.x() + (item->boundingRect().width() - size.width())/2,
                                   parentGeometryBounds.top() - size.height());
    const QPoint bottomPoint(pos.x() + (item->boundingRect().width() - size.width())/2,
                             parentGeometryBounds.bottom());

    const QPoint leftPoint(parentGeometryBounds.left() - size.width(),
                           pos.y() + (item->boundingRect().height() - size.height())/2);

    const QPoint rightPoint(parentGeometryBounds.right(),
                            pos.y() + (item->boundingRect().height() - size.height())/2);

    QPoint dialogPos;
    if (m_location == Plasma::Types::BottomEdge) {
        dialogPos = bottomPoint;
    } else if (m_location == Plasma::Types::LeftEdge) {
        dialogPos = leftPoint;
    } else if (m_location == Plasma::Types::RightEdge) {
        dialogPos = rightPoint;
    } else { // Types::TopEdge
        dialogPos = topPoint;
    }

    //find the correct screen for the item
    //we do not rely on item->window()->screen() because
    //QWindow::screen() is always only the screen where the window gets first created
    //not actually the current window. See QWindow::screen() documentation
    QRect avail = screenForItem(item)->availableGeometry();

    if (outsideParentWindow && m_frameSvgItem->enabledBorders() != Plasma::FrameSvg::AllBorders) {
        //make the panel look it's inside the panel, in order to not make it look cutted
        switch (m_location) {
        case Plasma::Types::LeftEdge:
        case Plasma::Types::RightEdge:
            avail.setTop(qMax(avail.top(), parentGeometryBounds.top()));
            avail.setBottom(qMin(avail.bottom(), parentGeometryBounds.bottom()));
            break;
        default:
            avail.setLeft(qMax(avail.left(), parentGeometryBounds.left()));
            avail.setRight(qMin(avail.right(), parentGeometryBounds.right()));
            break;
        }
    }

    if (dialogPos.x() < avail.left()) {
        // popup hits lhs
        if (m_location == Plasma::Types::TopEdge || m_location == Plasma::Types::BottomEdge) {
            // move it
            dialogPos.setX(avail.left());
        } else {
            // swap edge
            dialogPos.setX(rightPoint.x());
        }
    }
    if (dialogPos.x() + size.width() > avail.right()) {
        // popup hits rhs
        if (m_location == Plasma::Types::TopEdge || m_location == Plasma::Types::BottomEdge) {
            dialogPos.setX(avail.right() - size.width());
        } else {
            dialogPos.setX(leftPoint.x());
        }
    }
    if (dialogPos.y() < avail.top()) {
        // hitting top
        if (m_location == Plasma::Types::LeftEdge || m_location == Plasma::Types::RightEdge) {
            dialogPos.setY(avail.top());
        } else {
            dialogPos.setY(bottomPoint.y());
        }
    }

    if (dialogPos.y() + size.height() > avail.bottom()) {
        // hitting bottom
        if (m_location == Plasma::Types::TopEdge || m_location == Plasma::Types::BottomEdge) {
            dialogPos.setY(topPoint.y());
        } else {
            dialogPos.setY(avail.bottom() - size.height());
        }
    }

    return dialogPos;
}

Plasma::Types::Location DialogProxy::location() const
{
    return m_location;
}

void DialogProxy::setLocation(Plasma::Types::Location location)
{
    if (m_location == location) {
        return;
    }
    m_location = location;
    emit locationChanged();
    requestSyncToMainItemSize();
}


QObject *DialogProxy::margins() const
{
    return m_frameSvgItem->margins();
}

void DialogProxy::setFramelessFlags(Qt::WindowFlags flags)
{
    setFlags(Qt::FramelessWindowHint|flags);
    emit flagsChanged();
}

void DialogProxy::adjustGeometry(const QRect &geom)
{
    setGeometry(geom);
}

void DialogProxy::resizeEvent(QResizeEvent *re)
{
    syncMainItemToSize();
    QQuickWindow::resizeEvent(re);
}

void DialogProxy::syncMainItemToSize()
{
    m_frameSvgItem->setX(0);
    m_frameSvgItem->setY(0);
    m_frameSvgItem->setWidth(width());
    m_frameSvgItem->setHeight(height());

    KWindowEffects::enableBlurBehind(winId(), true, m_frameSvgItem->frameSvg()->mask());
    updateContrast();

    if (m_mainItem) {
        m_mainItem.data()->setX(m_frameSvgItem->margins()->left());
        m_mainItem.data()->setY(m_frameSvgItem->margins()->top());
        m_mainItem.data()->setWidth(width() - m_frameSvgItem->margins()->left() - m_frameSvgItem->margins()->right());
        m_mainItem.data()->setHeight(height() - m_frameSvgItem->margins()->top() - m_frameSvgItem->margins()->bottom());
    }
}

void DialogProxy::syncToMainItemSize()
{
    //if manually sync a sync timer was running cancel it so we don't get called twice
    m_syncTimer->stop();

    if (!m_mainItem) {
        return;
    }
    syncBorders();
    const QSize s = QSize(m_mainItem.data()->width(), m_mainItem.data()->height()) +
                    QSize(m_frameSvgItem->margins()->left() + m_frameSvgItem->margins()->right(),
                          m_frameSvgItem->margins()->top() + m_frameSvgItem->margins()->bottom());

    if (visualParent()) {
        const QRect geom(popupPosition(visualParent(), s, Qt::AlignCenter), s);

        if (geom == geometry()) {
            return;
        }
        adjustGeometry(geom);
    } else {
        resize(s);
    }
}

void DialogProxy::requestSyncToMainItemSize(bool delayed)
{
    if (!m_componentComplete) {
        return;
    }

    if (delayed && !m_syncTimer->isActive()) {
        m_syncTimer->start(150);
    } else {
        m_syncTimer->start(0);
    }
}

void DialogProxy::updateContrast()
{
    KWindowEffects::enableBackgroundContrast(winId(), m_theme.backgroundContrastEnabled(),
                                                      m_theme.backgroundContrast(),
                                                      m_theme.backgroundIntensity(),
                                                      m_theme.backgroundSaturation(),
                                                      m_frameSvgItem->frameSvg()->mask());
}

void DialogProxy::setType(WindowType type)
{
    if (type == m_type) {
        return;
    }

    m_type = type;
    if (m_type != Normal) {
        KWindowSystem::setType(winId(), (NET::WindowType)type);
    } else {
        setFlags(Qt::FramelessWindowHint|flags());
    }

    if (type == Dock) {
        KWindowSystem::setOnAllDesktops(winId(), true);
    } else {
        KWindowSystem::setOnAllDesktops(winId(), false);
    }

    emit typeChanged();
}

DialogProxy::WindowType DialogProxy::type() const
{
    return m_type;
}

Plasma::FrameSvgItem *DialogProxy::frameSvgItem()
{
    return m_frameSvgItem;
}

void DialogProxy::focusInEvent(QFocusEvent *ev)
{
    QQuickWindow::focusInEvent(ev);
}

void DialogProxy::focusOutEvent(QFocusEvent *ev)
{
    if (m_hideOnWindowDeactivate) {
        qDebug( ) << "DIALOG:  hiding dialog.";
        setVisible(false);
    }
    QQuickWindow::focusOutEvent(ev);
}

void DialogProxy::showEvent(QShowEvent *event)
{
    DialogShadows::self()->addWindow(this, m_frameSvgItem->enabledBorders());
    QQuickWindow::showEvent(event);
}

bool DialogProxy::event(QEvent *event)
{
    if (event->type() == QEvent::Show) {
        updateVisibility(true);
    } else if (event->type() == QEvent::Hide) {
        updateVisibility(false);
    }

    const bool retval = QQuickWindow::event(event);
    KWindowSystem::setState(winId(), NET::SkipTaskbar | NET::SkipPager);
    return retval;
}

void DialogProxy::hideEvent(QHideEvent *event)
{
    QQuickWindow::hideEvent(event);
}

void DialogProxy::classBegin()
{

}

void DialogProxy::componentComplete()
{
    m_componentComplete = true;
    syncToMainItemSize();
}

void DialogProxy::syncBorders()
{
    // FIXME: QWindow::screen() never ever changes if the window is moved across
    //        virtual screens (normal two screens with X), this seems to be intentional
    //        as it's explicitly mentioned in the docs. Until that's changed or some
    //        more proper way of howto get the current QScreen for given QWindow is found,
    //        we simply iterate over the virtual screens and pick the one our QWindow
    //        says it's at.
    QRect avail;
    QPoint pos = position();
    Q_FOREACH(QScreen *screen, screen()->virtualSiblings()) {
        if (screen->availableGeometry().contains(pos)) {
            avail = screen->availableGeometry();
            break;
        }
    }

    int borders = Plasma::FrameSvg::AllBorders;

    //Tooltips always have all the borders
    if (!(flags() & Qt::ToolTip)) {
        if (x() <= avail.x() || m_location == Plasma::Types::LeftEdge) {
            borders = borders & ~Plasma::FrameSvg::LeftBorder;
        }
        if (y() <= avail.y() || m_location == Plasma::Types::TopEdge) {
            borders = borders & ~Plasma::FrameSvg::TopBorder;
        }
        if (avail.right() <= x() + width() || m_location == Plasma::Types::RightEdge) {
            borders = borders & ~Plasma::FrameSvg::RightBorder;
        }
        if (avail.bottom() <= y() + height() || m_location == Plasma::Types::BottomEdge) {
            borders = borders & ~Plasma::FrameSvg::BottomBorder;
        }
    }

    m_frameSvgItem->setEnabledBorders((Plasma::FrameSvg::EnabledBorder)borders);

    if (isVisible()) {
        DialogShadows::self()->addWindow(this, m_frameSvgItem->enabledBorders());
    }
}

bool DialogProxy::hideOnWindowDeactivate() const
{
    return m_hideOnWindowDeactivate;
}

void DialogProxy::setHideOnWindowDeactivate(bool hide)
{
    if (flags() & Qt::X11BypassWindowManagerHint) {
        // doesn't get keyboard focus, so let's just ignore it
        return;
    }
    if (m_hideOnWindowDeactivate == hide) {
        return;
    }
    m_hideOnWindowDeactivate = hide;
    emit hideOnWindowDeactivateChanged();
}

bool DialogProxy::isOutputOnly() const
{
    return m_outputOnly;
}

void DialogProxy::setOutputOnly(bool outputOnly)
{
    if (m_outputOnly == outputOnly) {
        return;
    }
    m_outputOnly = outputOnly;
    emit outputOnlyChanged();
}

void DialogProxy::updateInputShape()
{
    if (!isVisible()) {
        return;
    }
#if HAVE_XCB_SHAPE
    if (QGuiApplication::platformName() == QStringLiteral("xcb")) {
        xcb_connection_t *c = QX11Info::connection();
        static bool s_shapeExtensionChecked = false;
        static bool s_shapeAvailable = false;
        if (!s_shapeExtensionChecked) {
            xcb_prefetch_extension_data(c, &xcb_shape_id);
            const xcb_query_extension_reply_t *extension = xcb_get_extension_data(c, &xcb_shape_id);
            if (extension->present) {
                // query version
                auto cookie = xcb_shape_query_version(c);
                QScopedPointer<xcb_shape_query_version_reply_t, QScopedPointerPodDeleter> version(xcb_shape_query_version_reply(c, cookie, Q_NULLPTR));
                if (!version.isNull()) {
                    s_shapeAvailable = (version->major_version * 0x10 + version->minor_version) >= 0x11;
                }
            }
            s_shapeExtensionChecked = true;
        }
        if (!s_shapeAvailable) {
            return;
        }
        if (m_outputOnly) {
            // set input shape, so that it doesn't accept any input events
            xcb_shape_rectangles(c, XCB_SHAPE_SO_SET, XCB_SHAPE_SK_INPUT,
                                 XCB_CLIP_ORDERING_UNSORTED, winId(), 0, 0, 0, NULL);
        } else {
            // delete the shape
            xcb_shape_mask(c, XCB_SHAPE_SO_INTERSECT, XCB_SHAPE_SK_INPUT,
                           winId(), 0, 0, XCB_PIXMAP_NONE);
        }
    }
#endif
}

void DialogProxy::setTransientParentAndNotify(QWindow *parent)
{
    if (parent == transientParent()) {
        return;
    }

    setTransientParent(parent);
    emit transientParentChanged();
}


//find the screen which contains the item
QScreen* DialogProxy::screenForItem(QQuickItem* item) const
{
    const QPoint globalPosition = item->window()->mapToGlobal(item->position().toPoint());
    foreach(QScreen *screen, QGuiApplication::screens()) {
        if (screen->geometry().contains(globalPosition)) {
            return screen;
        }
    }
    return QGuiApplication::primaryScreen();
}

void DialogProxy::updateMinimumWidth()
{
    if (m_mainItemLayout) {
        setMinimumWidth(m_mainItemLayout.data()->property("minimumWidth").toInt() + m_frameSvgItem->margins()->left() + m_frameSvgItem->margins()->right());
    } else {
        setMinimumWidth(-1);
    }
}

void DialogProxy::updateMinimumHeight()
{
    if (m_mainItemLayout) {
        setMinimumHeight(m_mainItemLayout.data()->property("minimumHeight").toInt() + m_frameSvgItem->margins()->top() + m_frameSvgItem->margins()->bottom());
    } else {
        setMinimumHeight(-1);
    }
}

void DialogProxy::updateMaximumWidth()
{
    if (m_mainItemLayout) {
        const int hint = m_mainItemLayout.data()->property("maximumWidth").toInt();
        if (hint > 0) {
            setMaximumWidth(hint + m_frameSvgItem->margins()->left() + m_frameSvgItem->margins()->right());
        } else {
            setMaximumWidth(DIALOGSIZE_MAX);
        }
    } else {
        setMaximumWidth(DIALOGSIZE_MAX);
    }
}

void DialogProxy::updateMaximumHeight()
{
    if (m_mainItemLayout) {
        const int hint = m_mainItemLayout.data()->property("maximumHeight").toInt();
        if (hint > 0) {
            setMaximumHeight(hint + m_frameSvgItem->margins()->top() + m_frameSvgItem->margins()->bottom());
        } else {
            setMaximumHeight(DIALOGSIZE_MAX);
        }
    } else {
        setMaximumHeight(DIALOGSIZE_MAX);
    }
}

#include "dialog.moc"

