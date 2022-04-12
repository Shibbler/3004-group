#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Session.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    connect(ui->PowerButton, SIGNAL (pressed()) , this, SLOT (powerClickedHeld()));

    connect(ui->PowerButton, SIGNAL (released()) , this, SLOT (softOffFromButton()));

    connect(ui->StartButton, SIGNAL (pressed()) , this, SLOT (startButton();));

    connect(ui->earButtonStrong, SIGNAL (pressed()) , this, SLOT (earlobeStrongButton();));

    connect(ui->earButtonWeak, SIGNAL (pressed()) , this, SLOT (earlobeWeakButton();));

    this->batteryTimer = new QTimer(this);
    this->softOffTimer = new QTimer(this);
    connect(batteryTimer, SIGNAL (timeout()),this, SLOT (drainBattery()));
    connect(softOffTimer, SIGNAL (timeout()),this, SLOT (softOffTimed()));


    //set batteryPower to 100
    this->batteryLevel = 100.0;
    //set in session bool to false
    this->inSession = true; //ITS TRUE SO YOU CAN SEE THE SOFT POWER THING, OTHERWISE SHOULD BE FALSE
    //this->softOffRow = 8;
    //start the timer to model battery depletion (always runs)
    batteryTimer->start(1000);


    //TESTING
    Session *test = new Session(0,60,30,30);
    this->connectionStrength = 1; //to model full connection
    Session *toCopy = new Session(test);
    this->curSession = test;


    qInfo("%s", test->getRecord());
    qInfo("%s", toCopy->getRecord());
    //TESTING

    //if we want to 'save' a session, all ya need to do is delete old pointer and make a new one using copy constructor and current session. only thing to keep in mind is saving the right 'id'
    //at startup we can read the "DB" textfile and load in all the saved sessions. To start out, the DB file is filled with placeholder default sessions.


}


void MainWindow::startButton(){}

void MainWindow::earlobeStrongButton(){}

void MainWindow::earlobeWeakButton(){}




//function to be called for battery drainage (should be timed)
void MainWindow::drainBattery(){
    //battery drains: Hz/100 *connection (3 possible values (0.33, 0.66,1)) * lengthOfSession/60

    float amountToReduceBattery((curSession->getIntensity()/100) * this->connectionStrength * (this->curSession->getLength()/60));

    qDebug() << amountToReduceBattery;
    if (inSession && powerStatus){//if we are in a session AND the power is on
        this->batteryLevel -= amountToReduceBattery;
        ui->batteryLabel_2->setText(QString::number(this->batteryLevel));
    }
}

//call the softoffbased on button press
void MainWindow::softOffFromButton(){
    if (this->inSession == true){//only call if inSession, otherwise softOff not required
        this->softOffRow = 0;
        ui->SessionType_2->setCurrentRow(this->softOffRow,QItemSelectionModel::Select);
        for (int i = 0; i< 8; i++){//start 5 timers (this will be a bit hacky, but it works.
            ui->SessionType_2->setCurrentRow(8);
            softOffTimer->start(500);
        }
    }
    //ensure booleans are nice again
    this->inSession = false;
    this->powerStatus = false;
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

//models a very simple case of turning the  device on, or off by HOLDING the button, might need to be more complex
void MainWindow::powerClickedHeld(){ //we can call this a hard off
    if (this->powerStatus == false){//if trying to turn on
        if (this->batteryLevel >0){
            ui->powerLabel->setText("POWER: ON");
           // batteryTimer->start(1000); //starts the battery drain THIS SHOULD PROBABLY ONLY DRAIN WITH SESSION
            this->powerStatus = true;
        }else{
            ui->powerLabel->setText("NO CHARGE");
        }
    }else{//if trying to turn off
        this->powerStatus = false;
        ui->powerLabel->setText("POWER: OFF");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

