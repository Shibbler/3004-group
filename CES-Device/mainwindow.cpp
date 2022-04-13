#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Session.h"
#include <QDebug>
#include <QTapAndHoldGesture>

#include <QDateTime>
#include <stdio.h>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    //read saved sessions from DB textfile and store them in the array of sessions
    loadSessions();

    connect(ui->loadButton, SIGNAL(released()) , this, SLOT (loadSession()));
    connect(ui->saveButton, SIGNAL(released()) , this, SLOT (saveSession()));

    connect(ui->PowerButton, SIGNAL (pressed()) , this, SLOT (power_pressed()));
    connect(ui->PowerButton, SIGNAL (released()) , this, SLOT (power_released()));

    connect(ui->StartButton, SIGNAL (released()) , this, SLOT (startButton()));

    connect(ui->earButtonStrong, SIGNAL (released()) , this, SLOT (earlobeStrongButton()));

    connect(ui->earButtonWeak, SIGNAL (released()) , this, SLOT (earlobeWeakButton()));

    connect(ui->detachEars, SIGNAL (released()) , this, SLOT (earlobeDetachButton()));

    connect(ui->detachEars, SIGNAL (released()) , this, SLOT (earlobeDetachButton()));

    this->incTimer = new QTimer(this);
    this->batteryTimer = new QTimer(this);
    this->softOffTimer = new QTimer(this);
    this->noSessionTimer = new QTimer(this);
    connect(incTimer, SIGNAL (timeout()), this, SLOT (increaseTime()));
    connect(batteryTimer, SIGNAL (timeout()),this, SLOT (drainBattery()));
    connect(softOffTimer, SIGNAL (timeout()),this, SLOT (softOffTimed()));
    connect(noSessionTimer, SIGNAL(timeout()),this, SLOT (powerDown()));


    this->incTimer->start(1000); // Every 1000


    //set batteryPower to 100
    this->batteryLevel = 100.0;
    //set in session bool to false
    this->inSession = false;
    this->powerStatus = false;
    //this->softOffRow = 8;
    //start the timer to model battery depletion (always runs)
    batteryTimer->start(1000);


    //TESTING
    Session *test = new Session(0,20,33,100.0);
    this->connectionStrength = 1; //to model full connection
    this->savedSessions[1]=test;

    Session *toCopy = new Session(test);
    this->curSession = test;


    qInfo("%s", test->getRecord());
    qInfo("%s", toCopy->getRecord());
    //qInfo("%s", savedSessions[0]->getRecord());
    //TESTING
}

//SHOULD TURN OFF IN TWO MINS IF NO SESSION SELECTED

//works by loading the ith session of savedSessions, where i current index of the QComboBox (like a list, so session 1 =0

void MainWindow::loadSessions()
{
    //open the db file for reading
//    FILE *file;
//    file = fopen("database.txt", "r");

//    if (file == NULL)
//    {
//        qInfo("error reading the db file. exiting\n");
//        exit(1);
//    }

//    char* temp = (char*) malloc(20);


//    for (int i = 0; i < MAX_SESSIONS; i++)
//    {
//        if (fgets(temp, 20, file) != NULL)
//        {
//            int id, sg, st;
//            float i;

//            sscanf(temp, "%d%2.2f%d%2.2f", id, sg, st, i);
//            savedSessions[i] = new Session(id, sg, st, i);

//        }
//        else
//            break;
//    }

//    delete temp;
}

void MainWindow::saveSessions()
{
    //open db file for writing

    //clear contents of db file

    for (int i = 0; i < MAX_SESSIONS; i++)
    {
        //write the session from array to file
    }
}

void MainWindow::loadSession(){
    if (this->powerStatus){
        this->curSession = this->savedSessions[ui->sessionStore->currentIndex()];//might want to use model column
    }
}

void MainWindow::saveSession(){
    if (this->powerStatus){
        delete savedSessions[ui->sessionStore->currentIndex()]; //release memory allocated for previous session stored in the array
        savedSessions[ui->sessionStore->currentIndex()] = new Session(curSession);  //use cpy ctor to create a new session
    }
}

void MainWindow::increaseTime(){
    timeSinceStart++;
}

void MainWindow::powerDown(){
    this->powerStatus=false;
    this->noSessionTimer->stop();
    ui->powerLabel->setText("POWER: OFF");
}



void MainWindow::startButton(){
    if (this->powerStatus){
        this->inSession = true;
        this->noSessionTimer->stop();
        //should start the currently loaded session
    }
}

void MainWindow::earlobeStrongButton(){
    if (this->powerStatus){
        this->connectionStrength = 1;
        for (int i = 0; i< 8; i++){
            ui->SessionType_2->setCurrentRow(i, QItemSelectionModel::Deselect);
        }
        ui->SessionType_2->setCurrentRow(0, QItemSelectionModel::Select);
        ui->SessionType_2->setCurrentRow(1, QItemSelectionModel::Select);
        ui->SessionType_2->setCurrentRow(2, QItemSelectionModel::Select);
        qDebug() << "StrongConnection Pressed";
    }

}

void MainWindow::earlobeWeakButton(){
    if (this->powerStatus){
        this->connectionStrength = 0.66;
        for (int i = 0; i< 8; i++){
            ui->SessionType_2->setCurrentRow(i, QItemSelectionModel::Deselect);
        }
        ui->SessionType_2->setCurrentRow(3, QItemSelectionModel::Select);
        ui->SessionType_2->setCurrentRow(4, QItemSelectionModel::Select);
        ui->SessionType_2->setCurrentRow(5, QItemSelectionModel::Select);
    }
}

void MainWindow::earlobeDetachButton(){
    if (this->powerStatus){
        this->connectionStrength = 0.33;
        for (int i = 0; i< 8; i++){
            ui->SessionType_2->setCurrentRow(i, QItemSelectionModel::Deselect);
        }

        ui->SessionType_2->setCurrentRow(6, QItemSelectionModel::Select);
        ui->SessionType_2->setCurrentRow(7, QItemSelectionModel::Select);
    }
}


//function to be called for battery drainage (should be timed)
void MainWindow::drainBattery(){
    //battery drains: Hz/100 *connection (3 possible values (0.33, 0.66,1)) * lengthOfSession/60
    //qDebug() << "Session intensity: " << curSession->getIntensity() << ". Connection strength: " << this->connectionStrength << ". Length of Session: " << this->curSession->getLength();
    //qDebug() << "First piece of math /100 = " << (curSession->getIntensity()/100) << ". Second piece of math /60: " <<(this->curSession->getLength()/60);

    float amountToReduceBattery= ((curSession->getIntensity()/100) * this->connectionStrength * (this->curSession->getSG()/60));

    //qDebug() << amountToReduceBattery;
    if (inSession && powerStatus){//if we are in a session AND the power is on
        this->batteryLevel -= amountToReduceBattery;
        ui->batteryLabel_2->setText(QString::number(this->batteryLevel));
        if (this->batteryLevel <= 0){//if battery is reduced such that there is no more charge
            this->powerStatus = false;
            ui->powerLabel->setText("POWER: OFF");
            this->noSessionTimer->stop();
            ui->batteryLabel_2->setText("0");
        }
    }
}

//call the softoffbased on button press
void MainWindow::softOffFromButton(){
        //only call if inSession, otherwise softOff not required
        this->softOffRow = 0;
        ui->SessionType_2->setCurrentRow(this->softOffRow,QItemSelectionModel::Select);
        for (int i = 0; i< 8; i++){//start 5 timers (this will be a bit hacky, but it works.
            ui->SessionType_2->setCurrentRow(8);
            softOffTimer->start(500);
        }
        //ensure booleans are nice again
        this->powerDown();
        this->softOffRow = 0;
        ui->powerLabel->setText("POWER: OFF");
}

void MainWindow::softOffTimed(){
    if (this->softOffRow <=8){
       ui->SessionType_2->setCurrentRow(this->softOffRow,QItemSelectionModel::Deselect);
       this->softOffRow++;
       ui->SessionType_2->setCurrentRow(this->softOffRow, QItemSelectionModel::Select);
       //qDebug() << "The row is: " << this->softOffRow;
    }else{
        this->softOffTimer->stop();
    }
}

//models a very simple case of turning the  device on, or off by HOLDING the button, might need to be more complex RIGHT NOW JUST PRESSING
void MainWindow::powerClickedHeld(){ //we can call this a hard off
    if (this->powerStatus == false){//if trying to turn on
        if (this->batteryLevel >0){
            ui->powerLabel->setText("POWER: ON");
           // batteryTimer->start(1000); //starts the battery drain THIS SHOULD PROBABLY ONLY DRAIN WITH SESSION
            this->powerStatus = true;
            this->inSession= false;
            this->noSessionTimer->start(120000);
        }else{
            ui->powerLabel->setText("NO CHARGE");
        }
    }else{//if trying to turn off
        this->powerDown();
    }
}


void MainWindow::power_pressed(){
    mLastPressTime = timeSinceStart;
}

void MainWindow::power_released(){
    qDebug() << "in power_released()";
    qDebug() << mLastPressTime;
    if (timeSinceStart - mLastPressTime >= 1){
        qDebug() << "Power was held";
        // It was held, so this is a power on or a power off

        if (this->powerStatus == false){//if trying to turn on
            if (this->batteryLevel >0){
                ui->powerLabel->setText("POWER: ON");
               // batteryTimer->start(1000); //starts the battery drain THIS SHOULD PROBABLY ONLY DRAIN WITH SESSION
                this->powerStatus = true;
                this->inSession= false;
                this->noSessionTimer->start(120000);
            }else{
                ui->powerLabel->setText("NO CHARGE");
            }
        }else{//if trying to turn off
            this->powerDown();
        }
    }else{
        qDebug() << "Power was clicked";

        // Button was clicked, not held

        // Are we in a session already?
        if (this->inSession == true){
            softOffFromButton();
            qDebug() << "Called softOffFromButton, we were already in a session";
        }else{
            // Time to select a session


        }
    }
}


MainWindow::~MainWindow()
{
    //write current sessions stored in array into the DB file
    saveSessions();

    for (int i = 0; i < MAX_SESSIONS; i++)
    {
        delete savedSessions[i];
    }

    delete curSession;

    delete ui;
}

