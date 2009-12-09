/***********************************************************************/
/* rotationstacked.h                                                     */
/*                                                                     */
/* Copyright(C) 2009 Igor Trindade Oliveira <igor.oliveira@indt.org.br>*/
/*                                                                     */
/* This library is free software; you can redistribute it and/or       */
/* modify it under the terms of the GNU Lesser General Public	       */
/* License as published by the Free Software Foundation; either	       */
/* version 2.1 of the License, or (at your option) any later version.  */
/*                                                                     */
/* This library is distributed in the hope that it will be useful,     */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of      */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU   */
/* Lesser General Public License for more details.                     */
/*                                                                     */
/* You should have received a copy of the GNU Lesser General Public    */
/* License along with this library; if not, write to the Free Software */
/* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA       */
/* 02110-1301  USA                                                     */
/***********************************************************************/

#include "rotationstacked_p.h"
#include <QGraphicsRotation>
#include <QSequentialAnimationGroup>
#include <QWeakPointer>
#include <kdebug.h>

#include "stackedlayout.h"
#include "plasma.h"

namespace Plasma
{

RotationStackedAnimation::RotationStackedAnimation(QObject *parent)
    : Animation(parent)
{
    backRotation = new QGraphicsRotation(this);
    frontRotation = new QGraphicsRotation(this);
    sLayout = new StackedLayout;
}

RotationStackedAnimation::~RotationStackedAnimation()
{
    /* TODO: test what is lacking a parent and delete it */
}

void RotationStackedAnimation::setMovementDirection(const qint8 &direction)
{
    animDirection = static_cast<Plasma::AnimationDirection>(direction);
}

qint8 RotationStackedAnimation::movementDirection() const
{
    return static_cast<qint8>(animDirection);
}


void RotationStackedAnimation::setReference(const qint8 &reference)
{
    m_reference = reference;
}

qint8 RotationStackedAnimation::reference() const
{
    return m_reference;
}

QGraphicsWidget *RotationStackedAnimation::backWidget()
{
    return m_backWidget.data();
}

void RotationStackedAnimation::setBackWidget(QGraphicsWidget *backWidget)
{
    m_backWidget = backWidget;

    if(widgetToAnimate()) {
        sLayout->addWidget(widgetToAnimate());
        sLayout->addWidget(m_backWidget.data());
    }
}

QGraphicsLayoutItem *RotationStackedAnimation::layout()
{
    return sLayout;
}

void RotationStackedAnimation::updateState(
        QAbstractAnimation::State newState, QAbstractAnimation::State oldState)
{
    if (!backWidget()) {
        return;
    }

    QPair<QGraphicsWidget *,QGraphicsWidget *> widgets = qMakePair(widgetToAnimate(), backWidget());

    const qreal widgetFrontWidth = widgets.first->size().width();
    const qreal widgetFrontHeight = widgets.first->size().height();

    const qreal widgetBackWidth = widgets.second->size().width();
    const qreal widgetBackHeight = widgets.second->size().height();

    QPair<QVector3D, QVector3D> vector;

    if (reference() == Center) {

        vector.first = QVector3D(widgetFrontWidth/2, widgetFrontHeight/2, 0);
        vector.second = QVector3D(widgetBackWidth/2, widgetBackHeight/2, 0);

        if (animDirection == MoveLeft || animDirection == MoveRight) {
            frontRotation->setAxis(Qt::YAxis);
            backRotation->setAxis(Qt::YAxis);

            if (animDirection == MoveLeft) {
                /* TODO: the order way */

            } else {
                frontStartAngle = 0;
                frontEndAngle = 90;
                backStartAngle = 265; //hack
                backEndAngle = 360;
            }
        }
    }

    frontRotation->setOrigin(vector.first);
    backRotation->setOrigin(vector.second);

    QList<QGraphicsTransform *> backTransformation;
    QList<QGraphicsTransform *> frontTransformation;

    frontTransformation.append(frontRotation);
    backTransformation.append(backRotation);

    widgets.first->setTransformations(frontTransformation);
    widgets.second->setTransformations(backTransformation);

    if (oldState == Stopped && newState == Running) {
        frontRotation->setAngle(direction() == Forward ? frontStartAngle : frontEndAngle);
        backRotation->setAngle(direction() == Forward ? backStartAngle : backEndAngle);
    } else if(newState == Stopped) {
        frontRotation->setAngle(direction() == Forward ? frontEndAngle : frontStartAngle);
        backRotation->setAngle(direction() == Forward ? backEndAngle : backStartAngle);
    }
}

void RotationStackedAnimation::updateCurrentTime(int currentTime)
{
    QGraphicsWidget *w = widgetToAnimate();
    if(w) {
        qreal delta;
        if (currentTime <= duration()/2) {
            delta = Animation::easingCurve().valueForProgress(
                    (currentTime * 2) / qreal(duration()));
            sLayout->setCurrentWidgetIndex(0);
            delta = frontEndAngle * delta;
            frontRotation->setAngle(delta);
        } else {
            delta = Animation::easingCurve().valueForProgress(
                    (currentTime/2) / qreal(duration()));
            sLayout->setCurrentWidgetIndex(1);
            delta = backEndAngle * delta;
            backRotation->setAngle(delta);
        }
    }
}
}
