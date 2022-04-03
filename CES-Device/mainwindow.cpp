#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Session.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    //TESTING
    Session *test = new Session(0,TWENTY_MIN,DELTA,1);

    Session *toCopy = new Session(test);

    qInfo("%s", test->getRecord());
    qInfo("%s", toCopy->getRecord());
    //TESTING

    //if we want to 'save' a session, all ya need to do is delete old pointer and make a new one using copy constructor and current session. only thing to keep in mind is saving the right 'id'
    //at startup we can read the "DB" textfile and load in all the saved sessions. To start out, the DB file is filled with placeholder default sessions.


}

MainWindow::~MainWindow()
{
    delete ui;
}

