#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Session.h"
#include <QDebug>
#include <QTapAndHoldGesture>
#include <QDateTime>
#include <stdio.h>
#include <QSlider>
#include <unistd.h>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    path = (char*) malloc(200);
    getcwd(path, 200);
    strcat(path, "/database.txt");

    //read saved sessions from DB textfile and store them in the array of sessions
    loadSessions();


    this->incTimer = new QTimer(this);
    this->batteryTimer = new QTimer(this);
    this->softOffTimer = new QTimer(this);
    this->noSessionTimer = new QTimer(this);
    this->sessionTimer = new QTimer(this);
    this->cesBlinkTimer = new QTimer(this);
    this->noConnectBlinkTimer = new QTimer(this);
    this->incTimer->start(1000); // Every 1000

    this->flashingConnectionTimer = new QTimer(this);

    //set batteryPower to 100
    this->batteryLevel = 100.0;
    //set in session bool to false
    this->inSession = false;
    this->powerStatus = false;
    //this->softOffRow = 8;
    //start the timer to model battery depletion (always runs)
    batteryTimer->start(1000);

    initSlots();

    //TESTING
    curSession = new Session(0,20.0,30,100.0, 1.0);
    //this->connectionStrength = 1; //to model full connection

    Session *toCopy = new Session(curSession);

    qInfo("%s", curSession->getRecord());
    qInfo("%s", toCopy->getRecord());
  //  qInfo("%s", savedSessions[0]->getRecord());
    //TESTING
}

void MainWindow::loadSession(){
    if (this->powerStatus){
        delete curSession;
        this->curSession = new Session(this->savedSessions[ui->sessionStore->currentIndex()]); //might want to use model column
        this->curSession->id = 0;
        setSelections();
    }
}

void MainWindow::saveSession(){
    if (this->powerStatus){

        if (inSession)
        {
            saveInSession = true;
            this->ui->saveButton->setEnabled(false);
        }
        else
        {
            curSession->id = savedSessions[ui->sessionStore->currentIndex()]->id;
            qInfo("id for cur session %d \n", curSession->id);
            qInfo("id for the saved session in the array %d \n", savedSessions[ui->sessionStore->currentIndex()]->id);
            qInfo("%s \n", curSession->getRecord());


            delete savedSessions[ui->sessionStore->currentIndex()]; //release memory allocated for previous session stored in the array
            savedSessions[ui->sessionStore->currentIndex()] = new Session(curSession);  //use cpy ctor to create a new session
            qInfo("%s \n", savedSessions[ui->sessionStore->currentIndex()]->getRecord());
        }
    }
}

void MainWindow::increaseTime(){
    timeSinceStart++;
}

void MainWindow::powerDown(){
    this->powerStatus=false;
    this->setSelections();
    this->noSessionTimer->stop();
    this->sessionTimer->stop();
    ui->powerLabel->setText("POWER: OFF");
}



void MainWindow::startButton(){
    if (this->powerStatus){
        ui->CESModeLight->setCheckable(true);
        this->cesBlinkTimer->start(500);
        //GREAT CONNECTION
        if (this->connectionStrength == 1){
            ui->SessionType_2->setCurrentRow(0, QItemSelectionModel::Select);
            ui->SessionType_2->setCurrentRow(1, QItemSelectionModel::Select);
            ui->SessionType_2->setCurrentRow(2, QItemSelectionModel::Select);
            this->inSession = true;
            this->sessionTimer->start(curSession->getSG() * 1000);
            this->noSessionTimer->stop();
            this->ui->loadButton->setEnabled(false);
        }
        //MID CONNECTION
        if (this->connectionStrength < 1 && this->connectionStrength > 0.5 ){
            ui->SessionType_2->setCurrentRow(3, QItemSelectionModel::Select);
            ui->SessionType_2->setCurrentRow(4, QItemSelectionModel::Select);
            ui->SessionType_2->setCurrentRow(5, QItemSelectionModel::Select);
            this->inSession = true;
            this->sessionTimer->start(curSession->getSG() * 1000);
            this->noSessionTimer->stop();
            this->ui->loadButton->setEnabled(false);
        }
        //NO CONNECTION
        else{
            this->noConnectBlinkTimer->start(500);
        }
    }
}

void MainWindow::blinkNoConnect(){
    //qDebug()<< this->connectionStrength;
    if (this->connectionStrength < 0.65){
        //qDebug() << "does this work?";
        if (!isBlinkNoConnect){//if false, select rows
            ui->SessionType_2->setCurrentRow(6, QItemSelectionModel::Select);
            ui->SessionType_2->setCurrentRow(7, QItemSelectionModel::Select);
        }
        else{//if true, deselect rows
            ui->SessionType_2->setCurrentRow(6, QItemSelectionModel::Deselect);
            ui->SessionType_2->setCurrentRow(7, QItemSelectionModel::Deselect);
        }
        isBlinkNoConnect = !isBlinkNoConnect;
    }
    else{//if the connection strength has been changed
        //qDebug() << "this doesnt work";
        this->noConnectBlinkTimer->stop();
        ui->SessionType_2->setCurrentRow(6, QItemSelectionModel::Deselect);
        ui->SessionType_2->setCurrentRow(7, QItemSelectionModel::Deselect);
        this->isBlinkNoConnect = false;
    }
}


void MainWindow::blinkCESMode(){
    if (this->numBlinks < 8){
        ui->CESModeLight->setChecked(!ui->CESModeLight->isChecked());
        this->numBlinks++;
    }else{
        this->cesBlinkTimer->stop();
        this->numBlinks = 0;
        ui->CESModeLight->setCheckable(false);
    }
}

void MainWindow::connectivityHighlight(int index){
    if (index == 0) {
         ui->earButtonStrong->setVisible(false);
         ui->earButtonWeak->setVisible(true);
         ui->detachEars->setVisible(true);
    }
    else if (index == 1){
         ui->earButtonStrong->setVisible(true);
         ui->earButtonWeak->setVisible(false);
         ui->detachEars->setVisible(true);
    }
    else {
        ui->earButtonStrong->setVisible(true);
        ui->earButtonWeak->setVisible(true);
        ui->detachEars->setVisible(false);

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
        connectivityHighlight(0);
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
        connectivityHighlight(1);
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
        connectivityHighlight(2);
        this->inSession=false;
        this->sessionTimer->stop();
    }
}

void MainWindow::updateCustomTime(){
    this->curCustomTime = ui->UserDesSlider->sliderPosition();
    ui->timeLabel->setText(QString::number(this->curCustomTime));
    if (ui->SessionGroup->currentRow() == 2)
        curSession->setSG((float)ui->UserDesSlider->sliderPosition());

    qDebug() << this->curCustomTime;
    if (this->sessionGroupRow == 2){//custom time is selected
        this->curSession->setSG(this->curCustomTime);
    }
}

//function to be called for battery drainage (should be timed)
void MainWindow::drainBattery(){
    //battery drains: Hz/100 *connection (3 possible values (0.33, 0.66,1)) * lengthOfSession/60
    //qDebug() << "Session Hertz: " << curSession->getHertz() << ". Connection strength: " << this->connectionStrength <<". Current Intensity: "<< this->curSession->getIntensity() << ". Length of Session: " << this->curSession->getSG();
    //qDebug() << "First piece of math /100 = " << (curSession->getHertz()/100) << ". Second piece of math /60: " <<(this->curSession->getSG()/60) << "third piece of math: "<< (this->connectionStrength +(this->curSession->getIntensity()/10));

    float amountToReduceBattery= ((curSession->getHertz()/100) * (this->connectionStrength +(this->curSession->getIntensity()/10)) * (this->curSession->getSG()/60));
    //qDebug() << amountToReduceBattery;
    if (inSession && powerStatus){//if we are in a session AND the power is on
        this->flashConnections();
        //this->flashingConnectionTimer->start();
        if (this->batteryLevel < 33){
            qDebug()<< "Battery Level less than 33";
            this->batteryStatus = 0;
        }
        else if(this->batteryLevel < 34 && this->batteryLevel > 32 && !this->lowBatteryReached){
            this->batteryStatus = 1;
            qDebug()<< "Battery Level at 33";
            this->lowBatteryReached = true;
        }
        else if(this->batteryLevel <= 66 && !this->midBatteryReached){
            qDebug()<< "Battery Level at 66";
            this->batteryStatus = 2;
            this->midBatteryReached = true;

        }
        else if(this->batteryLevel == 100 && !flashedHundo){
            qDebug()<< "Battery Level at 100";
            flashedHundo = true;
           this->batteryStatus = 3;
        }
        else{
            this->batteryStatus = -1;
        }

        this->batteryLevel -= amountToReduceBattery;
        ui->batteryLabel_2->setText(QString::number(this->batteryLevel));
        if (this->batteryLevel <= 0){//if battery is reduced such that there is no more charge
            this->powerStatus = false;
            ui->powerLabel->setText("POWER: OFF");
            this->noSessionTimer->stop();
            ui->batteryLabel_2->setText("0");
        }
    }
    //this->flashingConnectionTimer->stop();
}

void MainWindow::flashConnections(){
    if (batteryStatus!=-1 && !needToDeselect){//if false, select rows
        this->needToDeselect = true;
        switch(this->batteryStatus){
            case 0:
                ui->SessionType_2->setCurrentRow(7, QItemSelectionModel::Select);
            break;
            case 1:
                ui->SessionType_2->setCurrentRow(6, QItemSelectionModel::Select);
                ui->SessionType_2->setCurrentRow(7, QItemSelectionModel::Select);
            break;
            case 2:
                ui->SessionType_2->setCurrentRow(3, QItemSelectionModel::Select);
                ui->SessionType_2->setCurrentRow(4, QItemSelectionModel::Select);
                ui->SessionType_2->setCurrentRow(5, QItemSelectionModel::Select);
                ui->SessionType_2->setCurrentRow(6, QItemSelectionModel::Select);
                ui->SessionType_2->setCurrentRow(7, QItemSelectionModel::Select);
            break;
            case 3:
                ui->SessionType_2->setCurrentRow(0, QItemSelectionModel::Select);
                ui->SessionType_2->setCurrentRow(1, QItemSelectionModel::Select);
                ui->SessionType_2->setCurrentRow(2, QItemSelectionModel::Select);
                ui->SessionType_2->setCurrentRow(3, QItemSelectionModel::Select);
                ui->SessionType_2->setCurrentRow(4, QItemSelectionModel::Select);
                ui->SessionType_2->setCurrentRow(5, QItemSelectionModel::Select);
                ui->SessionType_2->setCurrentRow(6, QItemSelectionModel::Select);
                ui->SessionType_2->setCurrentRow(7, QItemSelectionModel::Select);
            break;
        }
    }
    else if (this->needToDeselect || this->batteryStatus == 0 || this->batteryStatus == 1){//if true, deselect rows

        ui->SessionType_2->setCurrentRow(0, QItemSelectionModel::Deselect);
        ui->SessionType_2->setCurrentRow(1, QItemSelectionModel::Deselect);
        ui->SessionType_2->setCurrentRow(2, QItemSelectionModel::Deselect);
        ui->SessionType_2->setCurrentRow(3, QItemSelectionModel::Deselect);
        ui->SessionType_2->setCurrentRow(4, QItemSelectionModel::Deselect);
        ui->SessionType_2->setCurrentRow(5, QItemSelectionModel::Deselect);
        ui->SessionType_2->setCurrentRow(6, QItemSelectionModel::Deselect);
        ui->SessionType_2->setCurrentRow(7, QItemSelectionModel::Deselect);
        this->needToDeselect = false;
    }
    //this->flashingConnectionTimer->stop();
}

//call the softoffbased on button press
void MainWindow::softOffFromButton(){
        //only call if inSession, otherwise softOff not required
        this->sessionTimer->stop();
        this->ui->loadButton->setEnabled(true);

        for (int i = 0; i< 8; i++){
            ui->SessionType_2->setCurrentRow(i,QItemSelectionModel::Deselect);
        }
        this->softOffRow = 0;
        ui->SessionType_2->setCurrentRow(this->softOffRow,QItemSelectionModel::Select);
        for (int i = 0; i< 8; i++){//start 5 timers (this will be a bit hacky, but it works.
            ui->SessionType_2->setCurrentRow(8);
            softOffTimer->start(500);
        }
        //ensure booleans are nice again
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
        this->inSession = false;
        if (saveInSession)
        {
            this->saveInSession = false;
            saveSession();
            this->ui->saveButton->setEnabled(true);
        }
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
                this->flashConnections();
                //this->flashingConnectionTimer->start();
                // batteryTimer->start(1000); //starts the battery drain THIS SHOULD PROBABLY ONLY DRAIN WITH SESSION
                this->powerStatus = true;
                setSelections();
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
            //qDebug() << "Called softOffFromButton, we were already in a session";
        }else{
            // Time to select a session this->sessionGroupRow
            if (this->powerStatus){
                this->sessionGroupRow= this->sessionGroupRow % 3;
                ui->SessionGroup->setCurrentRow(this->sessionGroupRow,QItemSelectionModel::Deselect);
                this->sessionGroupRow++;
                this->sessionGroupRow= this->sessionGroupRow % 3;
                ui->SessionGroup->setCurrentRow(this->sessionGroupRow,QItemSelectionModel::Select);
                switch(this->sessionGroupRow)
                {
                    case 0: this->curSession->setSG(TWENTY_MIN);
                    break;
                    case 1: this->curSession->setSG(FOURTY_FIVE_MIN);
                    break;
                    case 2: this->curSession->setSG(curCustomTime);
                    break;
                }
            }
        }
    }
}


void MainWindow::upButtonPressed(){
    if(powerStatus){
        if (inSession){//adjust the intensity
            if (this->intensityRow > 0){
                for (int i = 0; i< 8; i++){
                    ui->SessionType_2->setCurrentRow(i,QItemSelectionModel::Deselect);
                }
                ui->SessionType_2->setCurrentRow(this->intensityRow,QItemSelectionModel::Deselect);
                this->intensityRow--;
                ui->SessionType_2->setCurrentRow(this->intensityRow,QItemSelectionModel::Select);
                this->curSession->setIntensity(8-this->intensityRow);

            }
        }
        else{
            if (sessionTypeRow > 0){//selecting session type
                ui->SessionType->setCurrentRow(sessionTypeRow,QItemSelectionModel::Deselect);
                sessionTypeRow--;
                ui->SessionType->setCurrentRow(sessionTypeRow,QItemSelectionModel::Select);
                switch(this->sessionTypeRow){
                    case 0: this->curSession->setST(DELTA); this->curSession->setHertz(DELTA_HZ);
                    break;
                    case 1: this->curSession->setST(THETA); this->curSession->setHertz(THETA_HZ);
                    break;
                    case 2: this->curSession->setST(ALPHA); this->curSession->setHertz(ALPHA_HZ);
                    break;
                    case 3: this->curSession->setST(ONE_HUNDRED); this->curSession->setHertz(ONE_HUNDRED_HZ);
                }
            }
        }
    }
}

void MainWindow::downButtonPressed(){
    if(powerStatus){
        if (inSession){//adjust the intensity
            if (this->intensityRow < 7){
                for (int i = 0; i< 8; i++){
                    ui->SessionType_2->setCurrentRow(i,QItemSelectionModel::Deselect);
                }
                ui->SessionType_2->setCurrentRow(this->intensityRow,QItemSelectionModel::Deselect);
                this->intensityRow++;
                ui->SessionType_2->setCurrentRow(this->intensityRow,QItemSelectionModel::Select);
                this->curSession->setIntensity(8-this->intensityRow);
            }
        }
        else{//iterate through session types
            if (sessionTypeRow < 3){
                ui->SessionType->setCurrentRow(sessionTypeRow,QItemSelectionModel::Deselect);
                sessionTypeRow++;
                ui->SessionType->setCurrentRow(sessionTypeRow,QItemSelectionModel::Select);
                switch(this->sessionTypeRow){
                    case 0: this->curSession->setST(DELTA); this->curSession->setHertz(DELTA_HZ);
                    break;
                    case 1: this->curSession->setST(THETA); this->curSession->setHertz(THETA_HZ);
                    break;
                    case 2: this->curSession->setST(ALPHA); this->curSession->setHertz(ALPHA_HZ);
                    break;
                    case 3: this->curSession->setST(ONE_HUNDRED); this->curSession->setHertz(ONE_HUNDRED_HZ);
                }
            }
        }
    }
}

void MainWindow::setSelections()
{
    ui->SessionGroup->setCurrentRow(ui->SessionGroup->currentRow(), QItemSelectionModel::Deselect);
    ui->SessionType->setCurrentRow(ui->SessionType->currentRow(), QItemSelectionModel::Deselect);
    ui->SessionType_2->setCurrentRow(ui->SessionType_2->currentRow(), QItemSelectionModel::Deselect);
    if (powerStatus)
    {
        if (curSession->getSG() < TWENTY_MIN + 1.0 && curSession->getSG() > TWENTY_MIN - 1.0)
        {
            ui->SessionGroup->setCurrentRow(0, QItemSelectionModel::Select);
            sessionGroupRow = 0;
        }
        else if (curSession->getSG() < FOURTY_FIVE_MIN + 1.0 && curSession->getSG() > FOURTY_FIVE_MIN - 1.0)
        {
            ui->SessionGroup->setCurrentRow(1, QItemSelectionModel::Select);
            sessionGroupRow = 1;
        }
        else
        {
            sessionGroupRow = 2;
            ui->SessionGroup->setCurrentRow(2, QItemSelectionModel::Select);
            ui->UserDesSlider->setSliderPosition(curSession->getSG());
            this->curCustomTime = curSession->getSG();
            ui->timeLabel->setText(QString::number(curSession->getSG()));
        }


        switch (this->curSession->getST())
        {
            case DELTA:       ui->SessionType->setCurrentRow(0, QItemSelectionModel::Select); sessionTypeRow = 0;
            break;
            case THETA:       ui->SessionType->setCurrentRow(1, QItemSelectionModel::Select); sessionTypeRow = 1;
            break;
            case ALPHA:       ui->SessionType->setCurrentRow(2, QItemSelectionModel::Select); sessionTypeRow = 2;
            break;
            case ONE_HUNDRED: ui->SessionType->setCurrentRow(3, QItemSelectionModel::Select); sessionTypeRow = 3;
            break;
        }

        if (curSession->getIntensity() < 1.1 && curSession->getSG() > 0.9)
        {
            ui->SessionType_2->setCurrentRow(7, QItemSelectionModel::Select);
            this->intensityRow = 7;
        }
        else if (curSession->getIntensity() < 2.1 && curSession->getSG() > 1.9)
        {
            ui->SessionType_2->setCurrentRow(6, QItemSelectionModel::Select);
            this->intensityRow = 6;
        }
        else if (curSession->getIntensity() < 3.1 && curSession->getSG() > 2.9)
        {
            ui->SessionType_2->setCurrentRow(5, QItemSelectionModel::Select);
            this->intensityRow = 5;
        }
        else if (curSession->getIntensity() < 4.1 && curSession->getSG() > 3.9)
        {
            ui->SessionType_2->setCurrentRow(4, QItemSelectionModel::Select);
            this->intensityRow = 4;
        }
        else if (curSession->getIntensity() < 5.1 && curSession->getSG() > 4.9)
        {
            ui->SessionType_2->setCurrentRow(3, QItemSelectionModel::Select);
            this->intensityRow = 3;
        }
        else if (curSession->getIntensity() < 6.1 && curSession->getSG() > 5.9)
        {
            ui->SessionType_2->setCurrentRow(2, QItemSelectionModel::Select);
            this->intensityRow = 2;
        }
        else if (curSession->getIntensity() < 7.1 && curSession->getSG() > 6.9)
        {
            ui->SessionType_2->setCurrentRow(1, QItemSelectionModel::Select);
            this->intensityRow = 1;
        }
        else if (curSession->getIntensity() < 8.1 && curSession->getSG() > 7.9)
        {
            ui->SessionType_2->setCurrentRow(0, QItemSelectionModel::Select);
            this->intensityRow = 0;
        }
    }
}

void MainWindow::loadSessions()
{

    //open the db file for reading
    FILE *file;
    file = fopen(path, "r");

    if (file == NULL) //file does not exist yet. setup fresh db file for new project
    {
        file = fopen(path, "w");  //reopen the file as write only, since file does not exist, this creates it

        for (int i = 0; i < MAX_SESSIONS; i++)
        {
            savedSessions[i] = new Session(i+1, TWENTY_MIN, DELTA, DELTA_HZ, 1.0); //fill up saved sessions array with default values
            fputs(savedSessions[i]->getRecord(), file);                                  //write the session to the DB file
            fputs("\n", file);
        }
        fclose(file);
    }
    else
    {
        for (int i = 0; i < MAX_SESSIONS; i++)
        {
            char* temp = (char*) malloc(200);

            if (fgets(temp, 200, file) != NULL)
            {
                int id, st;
                float Hertz, sg, intensity;

                sscanf(temp, "%d %f %d %f %f", &id, &sg, &st, &Hertz, &intensity);

                savedSessions[i] = new Session(id, sg, st, Hertz, intensity);
                delete temp;
                continue;
            }

            delete temp;
            break;
        }
        fclose(file);
    }
}

void MainWindow::saveSessions()
{
    FILE *file;
    file = fopen(path, "w");
    //clear contents of db file

    for (int i = 0; i < MAX_SESSIONS; i++)
    {
        //write the session from array to file

        fputs(savedSessions[i]->getRecord(), file);
        fputs("\n", file);
    }
    fclose(file);

}

void MainWindow::initSlots()
{
    connect(ui->loadButton, SIGNAL(released()) , this, SLOT (loadSession()));
    connect(ui->saveButton, SIGNAL(released()) , this, SLOT (saveSession()));

    connect(ui->PowerButton, SIGNAL (pressed()) , this, SLOT (power_pressed()));
    connect(ui->PowerButton, SIGNAL (released()) , this, SLOT (power_released()));

    connect(ui->StartButton, SIGNAL (released()) , this, SLOT (startButton()));

    connect(ui->earButtonStrong, SIGNAL (released()) , this, SLOT (earlobeStrongButton()));

    connect(ui->earButtonWeak, SIGNAL (released()) , this, SLOT (earlobeWeakButton()));

    connect(ui->detachEars, SIGNAL (released()) , this, SLOT (earlobeDetachButton()));

    connect(ui->UserDesSlider, SIGNAL (sliderReleased()) , this, SLOT (updateCustomTime()));

    connect(ui->downButton, SIGNAL (released()) , this, SLOT (downButtonPressed()));
    connect(ui->upButton, SIGNAL (released()) , this, SLOT (upButtonPressed()));

    connect(incTimer, SIGNAL (timeout()), this, SLOT (increaseTime()));
    connect(batteryTimer, SIGNAL (timeout()),this, SLOT (drainBattery()));
    connect(softOffTimer, SIGNAL (timeout()),this, SLOT (softOffTimed()));
    connect(noSessionTimer, SIGNAL(timeout()),this, SLOT (powerDown()));
    connect(sessionTimer, SIGNAL(timeout()), this, SLOT(softOffFromButton()));
    connect(cesBlinkTimer, SIGNAL(timeout()), this, SLOT(blinkCESMode()));
    connect(noConnectBlinkTimer, SIGNAL(timeout()), this, SLOT(blinkNoConnect()));
    connect(flashingConnectionTimer,SIGNAL(timeout()),this,SLOT(flashConnections()));


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

    delete path;

    delete ui;
}

