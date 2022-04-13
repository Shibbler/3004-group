#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QTimer>
#include "Session.h"

#define MAX_SESSIONS 10



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QTimer *batteryTimer;
    QTimer *softOffTimer;
    QTimer *noSessionTimer;

    QTimer *incTimer;
    int timeSinceStart = 0;



private:
    Ui::MainWindow *ui;
    Session *savedSessions[10];
    Session *curSession;
    float connectionStrength; //one of three possible values (0.33,0.66,1) based on the colour groupings
    float batteryLevel;
    int numSessions;
    bool inSession;
    int softOffRow;
    bool powerStatus;
    void loadSessions();

    quint64 mLastPressTime=0;
    static const quint64 MY_LONG_PRESS_THRESHOLD=1000;

public slots:
    void powerClickedHeld();
    void drainBattery();
    void softOffFromButton();
    void softOffTimed();
    void startButton();
    void earlobeWeakButton();
    void earlobeStrongButton();
    void earlobeDetachButton();
    void powerDown();
    void loadSession();
    void saveSession();
    void increaseTime();

    void power_pressed();
    void power_released();

signals:




};
#endif // MAINWINDOW_H
