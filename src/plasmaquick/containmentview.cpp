/*
    SPDX-FileCopyrightText: 2013 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "containmentview.h"
#include "configview.h"
#include "debug_p.h"
#include "plasmaquick.h"
#include "plasmoid/plasmoiditem.h"

#include <KPackage/Package>
#include <QDebug>
#include <QLoggingCategory>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QScreen>
#include <QTimer>

#include <KLocalizedQmlContext>

namespace PlasmaQuick
{
class ContainmentViewPrivate
{
public:
    ContainmentViewPrivate(Plasma::Corona *corona, ContainmentView *view);
    ~ContainmentViewPrivate();

    void setContainment(Plasma::Containment *cont);
    Plasma::Types::FormFactor formFactor() const;
    Plasma::Types::Location location() const;
    void showConfigurationInterface(Plasma::Applet *applet);
    void updateDestroyed(bool destroyed);
    /*!
     * Reconnects the relevant signals after a screen change
     **/
    void reactToScreenChange();

    ContainmentView *q;
    friend class ContainmentView;
    Plasma::Corona *corona;
    QScreen *lastScreen;
    QPointer<Plasma::Containment> containment;
    QPointer<ConfigView> configContainmentView;
    std::shared_ptr<QQmlEngine> engine;
    std::unique_ptr<QQmlContext> qmlContext;
    std::unique_ptr<QQuickItem> rootObject;
    std::unique_ptr<KLocalizedQmlContext> i18nContext;
};

ContainmentViewPrivate::ContainmentViewPrivate(Plasma::Corona *cor, ContainmentView *view)
    : q(view)
    , corona(cor)
    , engine(PlasmaQuick::globalEngine())
    , qmlContext(std::make_unique<QQmlContext>(engine.get()))
{
    i18nContext = std::make_unique<KLocalizedQmlContext>();
    qmlContext->setContextObject(i18nContext.get());
    QQmlEngine::setContextForObject(i18nContext.get(), qmlContext.get());
}

ContainmentViewPrivate::~ContainmentViewPrivate()
{
}

void ContainmentViewPrivate::setContainment(Plasma::Containment *cont)
{
    if (containment == cont) {
        return;
    }

    Plasma::Types::Location oldLoc = location();
    Plasma::Types::FormFactor oldForm = formFactor();

    if (containment) {
        QObject::disconnect(containment, nullptr, q, nullptr);
        QObject *oldGraphicObject = AppletQuickItem::itemForApplet(containment);
        if (auto item = qobject_cast<QQuickItem *>(oldGraphicObject)) {
            // TODO: delete the item when needed instead of just hiding, but there are quite a lot of cornercases to manage beforehand
            item->setVisible(false);
        }
    }

    containment = cont;

    if (oldLoc != location()) {
        Q_EMIT q->locationChanged(location());
    }
    if (oldForm != formFactor()) {
        Q_EMIT q->formFactorChanged(formFactor());
    }

    Q_EMIT q->containmentChanged();

    // we are QuickViewSharedEngine::SizeRootObjectToView, but that's not enough, as
    // the root object isn't immediately resized (done at the resizeEvent handler).
    // by resizing it just before restoring the containment, it removes a chain of resizes at startup
    if (q->rootObject()) {
        q->rootObject()->setSize(q->size());
    }
    if (cont) {
        QObject::connect(cont, &Plasma::Containment::locationChanged, q, &ContainmentView::locationChanged);
        QObject::connect(cont, &Plasma::Containment::formFactorChanged, q, &ContainmentView::formFactorChanged);
        QObject::connect(cont, &Plasma::Containment::configureRequested, q, &ContainmentView::showConfigurationInterface);
        // TODO: move to PanelView
        QObject::connect(cont, SIGNAL(destroyedChanged(bool)), q, SLOT(updateDestroyed(bool)));
        auto screen = containment->screen();
        auto activity = containment->activity();

        if (cont->containmentType() == Plasma::Containment::Type::Desktop) {
            QObject::connect(cont, &QObject::destroyed, q, [this, screen, activity]() {
                q->setContainment(corona->containmentForScreen(screen, activity, {}));
                q->show();
            });
        } else if (cont->containmentType() == Plasma::Containment::Type::Panel || cont->containmentType() == Plasma::Containment::Type::CustomPanel) {
            // Panels are created invisible and the code below ensures they are only
            // shown once their contents have settled to avoid visual glitches on startup
            QObject::connect(cont, &Plasma::Containment::uiReadyChanged, q, [this, cont](bool ready) {
                if (ready && !cont->destroyed()) {
                    q->setVisible(true);
                }
            });

            q->setVisible(!cont->destroyed() && cont->isUiReady());
        }
    } else {
        return;
    }

    QQuickItem *graphicObject = AppletQuickItem::itemForApplet(containment);

    if (graphicObject) {
        //         qDebug() << "using as graphic containment" << graphicObject << containment.data();

        graphicObject->setFocus(true);
        // by resizing before adding, it will avoid some resizes in most cases
        graphicObject->setSize(q->size());
        graphicObject->setParentItem(q->rootObject());
        if (q->rootObject()) {
            q->rootObject()->setProperty("containment", QVariant::fromValue(graphicObject));
            QObject *wpGraphicObject = containment->property("wallpaperGraphicsObject").value<QObject *>();
            if (wpGraphicObject) {
                q->rootObject()->setProperty("wallpaper", QVariant::fromValue(wpGraphicObject));
            }
        } else {
            qCWarning(LOG_PLASMAQUICK) << "Could not set containment property on rootObject";
        }
    } else {
        qCWarning(LOG_PLASMAQUICK) << "Containment graphic object not valid";
    }
}

Plasma::Types::Location ContainmentViewPrivate::location() const
{
    if (!containment) {
        return Plasma::Types::Desktop;
    }
    return containment->location();
}

Plasma::Types::FormFactor ContainmentViewPrivate::formFactor() const
{
    if (!containment) {
        return Plasma::Types::Planar;
    }
    return containment->formFactor();
}

void ContainmentViewPrivate::showConfigurationInterface(Plasma::Applet *applet)
{
    if (configContainmentView) {
        if (configContainmentView->applet() != applet) {
            configContainmentView->hide();
            configContainmentView->deleteLater();
        } else {
            configContainmentView->raise();
            configContainmentView->requestActivate();
            return;
        }
    }

    if (!applet || !applet->containment()) {
        return;
    }

    configContainmentView = new ConfigView(applet);

    configContainmentView->init();
    configContainmentView->show();
}

void ContainmentViewPrivate::updateDestroyed(bool destroyed)
{
    q->setVisible(!destroyed);
}

void ContainmentViewPrivate::reactToScreenChange()
{
    QScreen *newScreen = q->screen();

    if (newScreen == lastScreen) {
        return;
    }

    QObject::disconnect(lastScreen, nullptr, q, nullptr);
    lastScreen = newScreen;
    QObject::connect(newScreen, &QScreen::geometryChanged, q,
                     &ContainmentView::screenGeometryChanged);
    Q_EMIT q->screenGeometryChanged();
}

ContainmentView::ContainmentView(Plasma::Corona *corona, QWindow *parent)
    : QQuickWindow(parent)
    , d(new ContainmentViewPrivate(corona, this))
{
    setColor(Qt::transparent);

    d->lastScreen = screen();
    QObject::connect(d->lastScreen, &QScreen::geometryChanged, this,
                     &ContainmentView::screenGeometryChanged);
    QObject::connect(this, &ContainmentView::screenChanged, this,
                     [this]() {
        d->reactToScreenChange();
    });

    if (corona->kPackage().isValid()) {
        const auto info = corona->kPackage().metadata();
        if (info.isValid()) {
            d->i18nContext->setTranslationDomain(QStringLiteral("plasma_shell_") + info.pluginId());
        } else {
            qCWarning(LOG_PLASMAQUICK) << "Invalid corona package metadata";
        }
    } else {
        qCWarning(LOG_PLASMAQUICK) << "Invalid home screen package";
    }
}

ContainmentView::~ContainmentView()
{
    delete d;
}

void ContainmentView::destroy()
{
    // it will hide and deallocate the window so that no visibility or geometry
    // changes will be emitted during the destructor, avoiding potential crash
    // situations
    QWindow::destroy();

    // TODO: do we need a version which does not create?
    QQuickItem *graphicObject = AppletQuickItem::itemForApplet(d->containment);
    if (auto item = qobject_cast<QQuickItem *>(graphicObject)) {
        item->setVisible(false);
        item->setParentItem(nullptr); // First, remove the item from the view
    }
    deleteLater(); // delete the view
}

Plasma::Corona *ContainmentView::corona() const
{
    return d->corona;
}

void ContainmentView::setContainment(Plasma::Containment *cont)
{
    d->setContainment(cont);
}

Plasma::Containment *ContainmentView::containment() const
{
    return d->containment;
}

void ContainmentView::setLocation(Plasma::Types::Location location)
{
    d->containment->setLocation(location);
}

Plasma::Types::Location ContainmentView::location() const
{
    return d->location();
}

Plasma::Types::FormFactor ContainmentView::formFactor() const
{
    return d->formFactor();
}

QRectF ContainmentView::screenGeometry()
{
    return screen()->geometry();
}

QQuickItem *ContainmentView::rootObject() const
{
    return d->rootObject.get();
}

QQmlContext *ContainmentView::rootContext() const
{
    return d->qmlContext.get();
}

void ContainmentView::setSource(const QUrl &url)
{
    if (url.isEmpty()) {
        qWarning(LOG_PLASMAQUICK) << "Empty file name passed to ContainmentView";
    }

    QQmlComponent component(d->engine.get());

    // Important! Some parts of Plasma are extremely sensitive to status changed
    // signal being emitted in exactly the same way QQmlComponent does it. So this
    // connection needs to happen before any loading of the component happens.
    QObject::connect(&component, &QQmlComponent::statusChanged, this, &ContainmentView::statusChanged, Qt::QueuedConnection);
    component.loadUrl(url);

    auto obj = component.create(d->qmlContext.get());

    auto item = qobject_cast<QQuickItem *>(obj);
    item->setParentItem(contentItem());
    item->setSize(QSize(width(), height()));

    d->rootObject = std::unique_ptr<QQuickItem>(item);
}

void ContainmentView::showConfigurationInterface(Plasma::Applet *applet)
{
    d->showConfigurationInterface(applet);
}

void ContainmentView::resizeEvent(QResizeEvent *e)
{
    d->rootObject->setSize(e->size());

    QQuickWindow::resizeEvent(e);
}
}

#include "moc_containmentview.cpp"
