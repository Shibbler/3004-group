#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QTimer>
#include "Session.h"
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


private:
    Ui::MainWindow *ui;
    Session *savedSessions[10];
    Session *curSession;
    float batteryLevel;
    int numSessions;
    bool inSession;
    int softOffRow;
    bool powerStatus;

public slots:
    void powerClickedHeld();
    void drainBattery();
    void softOffFromButton();
    void softOffTimed();
signals:




};
#endif // MAINWINDOW_H
