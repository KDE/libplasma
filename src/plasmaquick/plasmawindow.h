/*
    SPDX-FileCopyrightText: 2023 David Edmundson <davidedmundson@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PLASMAWINDOW_H
#define PLASMAWINDOW_H

#include <QQuickWindow>

#include <plasmaquick/plasmaquick_export.h>

namespace Plasma
{
class FrameSvgItem;
}

namespace PlasmaQuick
{
class PlasmaWindowPrivate;

/*!
 * \qmltype Window
 * \inqmlmodule org.kde.plasma.core
 * \nativetype PlasmaQuick::PlasmaWindow
 *
 * \brief Creates a QQuickWindow themed in a Plasma style with background
 */

/*!
 * \qmlproperty Item Window::mainItem
 *
 * The main QML item that will be displayed in the Dialog
 */

/*!
 * \qmlproperty BackgroundHints Window::backgroundHints
 *
 * Defines the background used for the window
 */

/*!
 * \qmlproperty real Window::topPadding
 */

/*!
 * \qmlproperty real Window::bottomPadding
 */

/*!
 * \qmlproperty real Window::leftPadding
 */

/*!
 * \qmlproperty real Window::rightPadding
 */

/*!
 * \class PlasmaQuick::PlasmaWindow
 * \inheaderfile PlasmaQuick/PlasmaWindow
 * \inmodule PlasmaQuick
 *
 * \brief Creates a QQuickWindow themed in a Plasma style with background.
 */
class PLASMAQUICK_EXPORT PlasmaWindow : public QQuickWindow
{
    Q_OBJECT

    /*!
     * \property PlasmaQuick::PlasmaWindow::mainItem
     * The main QML item that will be displayed in the Dialog
     */
    Q_PROPERTY(QQuickItem *mainItem READ mainItem WRITE setMainItem NOTIFY mainItemChanged)

    /*!
     * \property PlasmaQuick::PlasmaWindow::backgroundHints
     * Defines the background used for the window
     */
    Q_PROPERTY(BackgroundHints backgroundHints READ backgroundHints WRITE setBackgroundHints NOTIFY backgroundHintsChanged)

    /*!
     * \property PlasmaQuick::PlasmaWindow::borders
     * Tells what borders are enabled of its background
     */
    Q_PROPERTY(Qt::Edges borders READ borders NOTIFY bordersChanged)

    /*!
     * \property PlasmaQuick::PlasmaWindow::topPadding
     */
    Q_PROPERTY(qreal topPadding READ topPadding NOTIFY paddingChanged)

    /*!
     * \property PlasmaQuick::PlasmaWindow::bottomPadding
     */
    Q_PROPERTY(qreal bottomPadding READ bottomPadding NOTIFY paddingChanged)

    /*!
     * \property PlasmaQuick::PlasmaWindow::leftPadding
     */
    Q_PROPERTY(qreal leftPadding READ leftPadding NOTIFY paddingChanged)

    /*!
     * \property PlasmaQuick::PlasmaWindow::rightPadding
     */
    Q_PROPERTY(qreal rightPadding READ rightPadding NOTIFY paddingChanged)

public:
    /*!
     * \value StandardBackground The standard background from the theme is drawn
     * \value SolidBackground The solid version of the background is preferred
     */
    enum BackgroundHints {
        StandardBackground = 0,
        SolidBackground = 1,
    };
    Q_ENUM(BackgroundHints)

    PlasmaWindow(const QString &svgPrefix = QStringLiteral("dialogs/background"));
    ~PlasmaWindow() override;

    /*!
     * The main QML item that will be displayed in the Dialog
     */
    void setMainItem(QQuickItem *mainItem);

    QQuickItem *mainItem() const;

    /*!
     * Changes which rounded corners are shown on the window.
     * Margins remain the same
     * The default is all borders
     */
    void setBorders(Qt::Edges bordersToShow);

    Qt::Edges borders();

    /*!
     * Returns the padding that are placed around the mainItem
     * When setting size hints on the window this should be factored in.
     */
    QMargins padding() const;

    BackgroundHints backgroundHints() const;
    void setBackgroundHints(BackgroundHints hints);

    qreal topPadding() const;
    qreal bottomPadding() const;
    qreal leftPadding() const;
    qreal rightPadding() const;

Q_SIGNALS:
    void mainItemChanged();
    void bordersChanged();
    void backgroundHintsChanged();
    void paddingChanged();

protected:
    void showEvent(QShowEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;

private:
    const std::unique_ptr<PlasmaWindowPrivate> d;
};
}

#endif
