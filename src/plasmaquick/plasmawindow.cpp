/*
    SPDX-FileCopyrightText: 2023 David Edmundson <davidedmundson@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plasmawindow.h"
#include "dialogbackground_p.h"
#include "plasmashellwaylandintegration.h"

#include <QMarginsF>
#include <QQuickItem>

#include <KSvg/FrameSvg>
#include <KWindowEffects>
#include <Plasma/Theme>

using namespace Plasma;

namespace PlasmaQuick
{
class PlasmaWindowPrivate
{
public:
    PlasmaWindowPrivate(PlasmaWindow *_q)
        : q(_q)
    {
    }
    void handleFrameChanged();
    void updateMainItemGeometry();
    QMarginsF shadowMargins() const;
    PlasmaWindow *q;
    // Keep a theme instance as a member to create one as soon as possible,
    // as Theme creation will set KSvg to correctly fetch images form the Plasma Theme.
    // This makes sure elements are correct, both in the dialog surface and the shadows.
    Plasma::Theme theme;
    QPointer<QQuickItem> mainItem;
    DialogBackground *shadows;
    DialogBackground *dialogBackground;
    PlasmaWindow::BackgroundHints backgroundHints = PlasmaWindow::StandardBackground;
};

PlasmaWindow::PlasmaWindow(const QString &svgPrefix)
    : QQuickWindow()
    , d(new PlasmaWindowPrivate(this))
{
    setColor(QColor(Qt::transparent));
    setFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    d->shadows = new DialogBackground(contentItem());
    d->shadows->setObjectName(QStringLiteral("dialogshadow")); // TODO for debugging, remove?
    d->shadows->setImagePath(svgPrefix);
    d->shadows->setPrefix(QStringLiteral("shadow"));
    connect(d->shadows, &DialogBackground::marginsChanged, this, [this]() {
        d->updateMainItemGeometry();
        d->handleFrameChanged();
        Q_EMIT paddingChanged();
    });

    d->dialogBackground = new DialogBackground(contentItem());
    d->dialogBackground->setObjectName(QStringLiteral("dialogbackground")); // TODO for debugging, remove?
    d->dialogBackground->setImagePath(svgPrefix);
    connect(d->dialogBackground, &DialogBackground::fixedMarginsChanged, this, [this]() {
        d->updateMainItemGeometry();
        Q_EMIT paddingChanged();
    });
    connect(d->dialogBackground, &DialogBackground::maskChanged, this, [this]() {
        d->handleFrameChanged();
    });
}

PlasmaWindow::~PlasmaWindow()
{
}

void PlasmaWindow::setMainItem(QQuickItem *mainItem)
{
    if (d->mainItem == mainItem)
        return;

    if (d->mainItem) {
        d->mainItem->setParentItem(nullptr);
    }

    d->mainItem = mainItem;
    Q_EMIT mainItemChanged();

    if (d->mainItem) {
        mainItem->setParentItem(contentItem());
        d->updateMainItemGeometry();
    }
}

QQuickItem *PlasmaWindow::mainItem() const
{
    return d->mainItem;
}

static KSvg::FrameSvg::EnabledBorders edgeToBorder(Qt::Edges edges)
{
    KSvg::FrameSvg::EnabledBorders borders = KSvg::FrameSvg::EnabledBorders(0);

    if (edges & Qt::Edge::TopEdge) {
        borders |= KSvg::FrameSvg::EnabledBorder::TopBorder;
    }
    if (edges & Qt::Edge::BottomEdge) {
        borders |= KSvg::FrameSvg::EnabledBorder::BottomBorder;
    }
    if (edges & Qt::Edge::LeftEdge) {
        borders |= KSvg::FrameSvg::EnabledBorder::LeftBorder;
    }
    if (edges & Qt::Edge::RightEdge) {
        borders |= KSvg::FrameSvg::EnabledBorder::RightBorder;
    }

    return borders;
}

static Qt::Edges bordersToEdge(KSvg::FrameSvg::EnabledBorders borders)
{
    Qt::Edges edges;
    if (borders & KSvg::FrameSvg::EnabledBorder::TopBorder) {
        edges |= Qt::Edge::TopEdge;
    }
    if (borders & KSvg::FrameSvg::EnabledBorder::BottomBorder) {
        edges |= Qt::Edge::BottomEdge;
    }
    if (borders & KSvg::FrameSvg::EnabledBorder::LeftBorder) {
        edges |= Qt::Edge::LeftEdge;
    }
    if (borders & KSvg::FrameSvg::EnabledBorder::RightBorder) {
        edges |= Qt::Edge::RightEdge;
    }
    return edges;
}

void PlasmaWindow::setBorders(Qt::Edges bordersToShow)
{
    const auto borders = edgeToBorder(bordersToShow);
    d->dialogBackground->setEnabledBorders(borders);
    d->shadows->setEnabledBorders(borders);
    d->updateMainItemGeometry();
    d->handleFrameChanged();
    Q_EMIT bordersChanged();
    Q_EMIT paddingChanged();
}

Qt::Edges PlasmaWindow::borders()
{
    return bordersToEdge(d->dialogBackground->enabledBorders());
}

void PlasmaWindow::showEvent(QShowEvent *e)
{
    QQuickWindow::showEvent(e);
}

void PlasmaWindow::resizeEvent(QResizeEvent *e)
{
    QQuickWindow::resizeEvent(e);
    const QSize windowSize = e->size();
    d->shadows->setSize(windowSize);

    const auto margins = d->shadowMargins();
    d->dialogBackground->setSize(windowSize.toSizeF().shrunkBy(margins));

    if (d->mainItem) {
        const QSize contentSize = windowSize.shrunkBy(padding());
        d->mainItem->setSize(contentSize);
    }
}

void PlasmaWindowPrivate::handleFrameChanged()
{
    const auto shadow = shadowMargins();
    dialogBackground->setPosition(QPointF{shadow.left(), shadow.top()});
    const QSize windowSize = q->size();
    dialogBackground->setSize(windowSize.toSizeF().shrunkBy(shadow));
    const QRegion mask = dialogBackground->mask().translated(shadow.left(), shadow.top());
    KWindowEffects::enableBlurBehind(q, theme.blurBehindEnabled(), mask);
    KWindowEffects::enableBackgroundContrast(q,
                                             theme.backgroundContrastEnabled(),
                                             theme.backgroundContrast(),
                                             theme.backgroundIntensity(),
                                             theme.backgroundSaturation(),
                                             mask);

    q->setMask(mask);
}

QMarginsF PlasmaWindowPrivate::shadowMargins() const
{
    return {
        shadows->enabledBorders() & KSvg::FrameSvg::LeftBorder ? shadows->leftMargin() : 0,
        shadows->enabledBorders() & KSvg::FrameSvg::TopBorder ? shadows->topMargin() : 0,
        shadows->enabledBorders() & KSvg::FrameSvg::RightBorder ? shadows->rightMargin() : 0,
        shadows->enabledBorders() & KSvg::FrameSvg::BottomBorder ? shadows->bottomMargin() : 0,
    };
}

void PlasmaWindowPrivate::updateMainItemGeometry()
{
    if (!mainItem) {
        return;
    }
    const QMargins frameMargins = q->padding();
    const QSize windowSize = q->size();

    mainItem->setPosition(QPoint(frameMargins.left(), frameMargins.top()));

    const QSize contentSize = windowSize.shrunkBy(frameMargins);
    mainItem->setSize(contentSize);
}

QMargins PlasmaWindow::padding() const
{
    return QMargins(leftPadding(), topPadding(), rightPadding(), bottomPadding());
}

PlasmaWindow::BackgroundHints PlasmaWindow::backgroundHints() const
{
    return d->backgroundHints;
}

void PlasmaWindow::setBackgroundHints(BackgroundHints hints)
{
    if (d->backgroundHints == hints) {
        return;
    }
    d->backgroundHints = hints;

    auto prefix = QStringLiteral("");
    if (d->backgroundHints == PlasmaWindow::SolidBackground) {
        prefix = QStringLiteral("solid/");
    }
    d->dialogBackground->setImagePath(prefix + QStringLiteral("dialogs/background"));

    Q_EMIT backgroundHintsChanged();
}

qreal PlasmaWindow::topPadding() const
{
    return d->dialogBackground->topFixedMargin() + d->shadowMargins().top();
}

qreal PlasmaWindow::bottomPadding() const
{
    return d->dialogBackground->bottomFixedMargin() + d->shadowMargins().bottom();
}

qreal PlasmaWindow::leftPadding() const
{
    return d->dialogBackground->leftFixedMargin() + d->shadowMargins().left();
}

qreal PlasmaWindow::rightPadding() const
{
    return d->dialogBackground->rightFixedMargin() + d->shadowMargins().right();
}

qreal PlasmaWindow::topBackgroundPadding() const
{
    return d->dialogBackground->topFixedMargin();
}

qreal PlasmaWindow::bottomBackgroundPadding() const
{
    return d->dialogBackground->bottomFixedMargin();
}

qreal PlasmaWindow::leftBackgroundPadding() const
{
    return d->dialogBackground->leftFixedMargin();
}

qreal PlasmaWindow::rightBackgroundPadding() const
{
    return d->dialogBackground->rightFixedMargin();
}
}

#include "moc_plasmawindow.cpp"
