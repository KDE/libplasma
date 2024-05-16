#pragma once

#include <QHash>
#include <QObject>
#include <QPointer>
#include <QRect>

#include <plasmaquick/plasmaquick_export.h>

class QWindow;
class QScreen;

class ScreenWorkAreaBackendPrivate;

// singleton data store, used by panel and that's it
// A bit like QScreen::availableGeometry but autohide panels still reserve space
// Also works on wayland

class PLASMAQUICK_EXPORT ScreenWorkAreaBackend : public QObject
{
    Q_OBJECT
public:
    ScreenWorkAreaBackend();
    ~ScreenWorkAreaBackend();

    static ScreenWorkAreaBackend *self();
    void updateScreenEdgeRecord(QWindow *window, QScreen *screen, Qt::Edges edge, int size);
    void removeScreenEdgeRecord(QWindow *window);

    QMargins screenEdgeMargins(QScreen *screen);
    QRect usableScreenArea(QScreen *screen);
Q_SIGNALS:
    void usableScreenAreaChanged(QScreen *screen);

private:
    std::unique_ptr<ScreenWorkAreaBackendPrivate> d;
};

// Easy to use read API, QML exposed
class PLASMAQUICK_EXPORT ScreenWorkArea : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QScreen *screen READ screen WRITE setScreen NOTIFY screenChanged)
    Q_PROPERTY(QRect usableArea READ usableArea NOTIFY usableAreaChanged)
    Q_PROPERTY(int topMargin READ topMargin NOTIFY usableAreaChanged)
    Q_PROPERTY(int bottomMargin READ bottomMargin NOTIFY usableAreaChanged)
    Q_PROPERTY(int leftMargin READ leftMargin NOTIFY usableAreaChanged)
    Q_PROPERTY(int rightMargin READ rightMargin NOTIFY usableAreaChanged)

public:
    ScreenWorkArea(QObject *parent = nullptr);
    ~ScreenWorkArea() = default;
    QScreen *screen() const;
    void setScreen(QScreen *screen);
    QRect usableArea() const;
    int topMargin() const;
    int bottomMargin() const;
    int leftMargin() const;
    int rightMargin() const;
Q_SIGNALS:
    void screenChanged();
    void usableAreaChanged();

private:
    QPointer<QScreen> m_screen;
};
