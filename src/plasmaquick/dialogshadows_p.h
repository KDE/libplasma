/*
    SPDX-FileCopyrightText: 2011 Aaron Seigo <aseigo@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PLASMA_DIALOGSHADOWS_H
#define PLASMA_DIALOGSHADOWS_H

#include <QSet>

#include <KSvg/FrameSvg>
#include <KSvg/Svg>

class DialogShadows : public KSvg::Svg
{
    Q_OBJECT

public:
    ~DialogShadows() override;

    static void addWindow(QWindow *window, KSvg::FrameSvg::EnabledBorders enabledBorders = KSvg::FrameSvg::AllBorders, const QString &prefix = QLatin1String("dialogs/background"));
    static void removeWindow(QWindow *window, const QString &prefix = QLatin1String("dialogs/background"));
    static void setEnabledBorders(QWindow *window, KSvg::FrameSvg::EnabledBorders enabledBorders = KSvg::FrameSvg::AllBorders, const QString &prefix = QLatin1String("dialogs/background"));

private:
    class Private;
    Private *const d;

    static DialogShadows *instance(const QString &prefix);
    explicit DialogShadows(const QString &prefix);

    Q_PRIVATE_SLOT(d, void updateShadows())
    Q_PRIVATE_SLOT(d, void windowDestroyed(QObject *deletedObject))
};

#endif
