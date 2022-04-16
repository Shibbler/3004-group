#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Session.h"
#include <QDebug>
#include <QTapAndHoldGesture>
#include <QDateTime>
#include <stdio.h>
#include <QSlider>
#include <unistd.h>

//responsible for initiliazing slots, timers and variables, loading sessions,
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
    this->incTimer->start(1); // Every millisecond

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



    //qInfo("%s", curSession->getRecord());
    //qInfo("%s", toCopy->getRecord());
  //  qInfo("%s", savedSessions[0]->getRecord());
    //TESTING
}


//loads the currentlu selected session into the curSession variable
void MainWindow::loadSession(){
    if (this->powerStatus){//only load if device is on
        delete curSession;//clear the current session so new one can be loaded
        this->curSession = new Session(this->savedSessions[ui->sessionStore->currentIndex()]);
        this->curSession->id = 0;
        setSelections();
    }
}


//save the curSession into the session list at the approproiate location.
void MainWindow::saveSession(){
    if (this->powerStatus){//ensure power is on

        if (inSession)//if a session is running, shouldnt be able to save
        {
            saveInSession = true;
            this->ui->saveButton->setEnabled(false);//make the button unclickable
        }
        else//if not in session, allow saving
        {
            curSession->id = savedSessions[ui->sessionStore->currentIndex()]->id;//set the id to the current row's id
            //qInfo("id for cur session %d \n", curSession->id);
            //qInfo("id for the saved session in the array %d \n", savedSessions[ui->sessionStore->currentIndex()]->id);
            //qInfo("%s \n", curSession->getRecord());
            delete savedSessions[ui->sessionStore->currentIndex()]; //release memory allocated for previous session stored in the array
            savedSessions[ui->sessionStore->currentIndex()] = new Session(curSession);  //use cpy ctor to create a new session
            //qInfo("%s \n", savedSessions[ui->sessionStore->currentIndex()]->getRecord());
        }
    }
}

//simulates a clock running every second, allows for time to be represented between button press and release
void MainWindow::increaseTime(){
    timeSinceStart++;
}

//function to call when powering down, sets booleans and stops timers
void MainWindow::powerDown(){
    this->midBatteryReached = false;
    this->lowBatteryReached = false;
    this->needToDeselect =false;
    this->flashedHundo = false;
    this->powerStatus=false;
    this->setSelections();
    this->flashingConnectionTimer->stop();
    this->noSessionTimer->stop();
    this->sessionTimer->stop();
    ui->powerLabel->setText("POWER: OFF");
}


//function called when startButton is pressed
void MainWindow::startButton(){
    if (this->powerStatus){//only work if device is on
        ui->CESModeLight->setCheckable(true);//allow the CESModeLight to be checkable, so it can blink
        this->cesBlinkTimer->start(500);//start the CESModeLight blinker
        //GREAT CONNECTION
        //set appropriate rows to show current connection strength, start and stop appropriate timers and set load button to unclickable
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
        else{//if no connection, start the timer to blink the bottom rows to indicate no connection.
            this->noConnectBlinkTimer->start(500);
        }
    }
}
//function to model the blinking of the connectivity graph if there is no earlobe connection.
void MainWindow::blinkNoConnect(){
    //qDebug()<< this->connectionStrength;
    //if the connection is lower than 0.66 (means it will be 0.33, only possible value below 0.66
    if (this->connectionStrength < 0.65){
        if (!isBlinkNoConnect){//if false, select rows
            ui->SessionType_2->setCurrentRow(6, QItemSelectionModel::Select);
            ui->SessionType_2->setCurrentRow(7, QItemSelectionModel::Select);
        }
        else{//if true, deselect rows
            ui->SessionType_2->setCurrentRow(6, QItemSelectionModel::Deselect);
            ui->SessionType_2->setCurrentRow(7, QItemSelectionModel::Deselect);
        }
        isBlinkNoConnect = !isBlinkNoConnect;//reverse the blink variable.
    }
    else{//if the connection strength has been changed
        this->noConnectBlinkTimer->stop();//stop the timer, deselect the rows
        ui->SessionType_2->setCurrentRow(6, QItemSelectionModel::Deselect);
        ui->SessionType_2->setCurrentRow(7, QItemSelectionModel::Deselect);
        this->isBlinkNoConnect = false;
    }
}

//function to blink the CESModeLight radio button 8 times.
void MainWindow::blinkCESMode(){
    if (this->numBlinks < 8){//if the function has not yet been called 8 times
        ui->CESModeLight->setChecked(!ui->CESModeLight->isChecked());//set to opposite of whatever state it is in (simulates blinking)
        this->numBlinks++;
    }else{//total number of blinks achieved (8)
        this->cesBlinkTimer->stop();//stop the timer
        this->numBlinks = 0;//reset for later use
        ui->CESModeLight->setCheckable(false);//dont allow the CESModeLight to be selected by user
    }
}

//a function to remove the button simulating earlobe attachment level when selected
void MainWindow::connectivityHighlight(int index){
    if (index == 0) {//strong was clicked
         ui->earButtonStrong->setVisible(false);
         ui->earButtonWeak->setVisible(true);
         ui->detachEars->setVisible(true);
    }
    else if (index == 1){//medium was clicked
         ui->earButtonStrong->setVisible(true);
         ui->earButtonWeak->setVisible(false);
         ui->detachEars->setVisible(true);
    }
    else {//detach was clicked.
        ui->earButtonStrong->setVisible(true);
        ui->earButtonWeak->setVisible(true);
        ui->detachEars->setVisible(false);

    }
}


//function to run if the strong attachment was clicked
void MainWindow::earlobeStrongButton(){
    if (this->powerStatus){//only work if device is on
        this->connectionStrength = 1;//set the connection strength
        for (int i = 0; i< 8; i++){//deselected any other selected rows so display is clear
            ui->SessionType_2->setCurrentRow(i, QItemSelectionModel::Deselect);
        }//select the appropriate rows
        ui->SessionType_2->setCurrentRow(0, QItemSelectionModel::Select);
        ui->SessionType_2->setCurrentRow(1, QItemSelectionModel::Select);
        ui->SessionType_2->setCurrentRow(2, QItemSelectionModel::Select);

        connectivityHighlight(0);//call the function to hide the button
    }

}

//function to run if the weak attachment was clicked
void MainWindow::earlobeWeakButton(){
    if (this->powerStatus){//only work if the device is on
        this->connectionStrength = 0.66;//set the connection strength
        for (int i = 0; i< 8; i++){//deselected any other selected rows so display is clear
            ui->SessionType_2->setCurrentRow(i, QItemSelectionModel::Deselect);
        }//select the appropriate rows
        ui->SessionType_2->setCurrentRow(3, QItemSelectionModel::Select);
        ui->SessionType_2->setCurrentRow(4, QItemSelectionModel::Select);
        ui->SessionType_2->setCurrentRow(5, QItemSelectionModel::Select);
        connectivityHighlight(1);//call the function to hide the button
    }
}

//function to run if the detach was clicked
void MainWindow::earlobeDetachButton(){
    if (this->powerStatus){//only work if the device is on
        this->connectionStrength = 0.33;//connection strength set
        for (int i = 0; i< 8; i++){//deselected any other selected rows so display is clear
            ui->SessionType_2->setCurrentRow(i, QItemSelectionModel::Deselect);
        }//select the appropriate rows
        ui->SessionType_2->setCurrentRow(6, QItemSelectionModel::Select);
        ui->SessionType_2->setCurrentRow(7, QItemSelectionModel::Select);
        connectivityHighlight(2);//call the function to hide the button
        this->inSession=false;//stop a session if in progress
        this->sessionTimer->stop();//stop the sessionTimer
    }
}

//function to update the custom time value based on the position of the QSlider (0-60)
void MainWindow::updateCustomTime(){
    this->curCustomTime = ui->UserDesSlider->sliderPosition();//set the time equal to the position of the QSlider
    ui->timeLabel->setText(QString::number(this->curCustomTime));//display the value of the QSlider in the QLabel
    if (ui->SessionGroup->currentRow() == 2)//if the current selected row is 2, meaning the custom time is selected
        curSession->setSG((float)ui->UserDesSlider->sliderPosition());//set the curSession time to be equal to the sliderPOsition

    //qDebug() << this->curCustomTime;
    if (this->sessionGroupRow == 2){//custom time is selected
        this->curSession->setSG(this->curCustomTime);
    }
}
//function to flash the battery level when called
void MainWindow::batteryCheck(){
    //this->flashingConnectionTimer->start();
    if (this->batteryLevel < 33){
        //qDebug()<< "Battery Level less than 33";
        this->batteryStatus = 0;
    }
    else if(this->batteryLevel < 34 && this->batteryLevel > 32 && !this->lowBatteryReached){
        this->batteryStatus = 1;
        //qDebug()<< "Battery Level at 33";
        this->lowBatteryReached = true;
    }
    else if(this->batteryLevel <= 66 && !this->midBatteryReached){
        //qDebug()<< "Battery Level at 66";
        this->batteryStatus = 2;
        this->midBatteryReached = true;

    }
    else if(this->batteryLevel == 100 && !flashedHundo){
        //qDebug()<< "Battery Level at 100";
        flashedHundo = true;
        this->batteryStatus = 3;
    }
    else{
        this->batteryStatus = -1;
    }

}

//function to be called for battery drainage
void MainWindow::drainBattery(){
    //battery drains: Hz/100 *(connection (3 possible values (0.33, 0.66,1))+ intensity/10) * lengthOfSession/60
    //uncomment if you want to see the math
    //qDebug() << "Session Hertz: " << curSession->getHertz() << ". Connection strength: " << this->connectionStrength <<". Current Intensity: "<< this->curSession->getIntensity() << ". Length of Session: " << this->curSession->getSG();
    //qDebug() << "First piece of math /100 = " << (curSession->getHertz()/100) << ". Second piece of math /60: " <<(this->curSession->getSG()/60) << "third piece of math: "<< (this->connectionStrength +(this->curSession->getIntensity()/10));

    float amountToReduceBattery= ((curSession->getHertz()/100) * (this->connectionStrength +(this->curSession->getIntensity()/10)) * (this->curSession->getSG()/60));
    if (inSession && powerStatus){//if we are in a session AND the power is on
        this->batteryLevel -= amountToReduceBattery;
        ui->batteryLabel_2->setText(QString::number(this->batteryLevel));
        if (this->batteryLevel <= 0){//if battery is reduced such that there is no more charge
            this->powerStatus = false;
            ui->powerLabel->setText("POWER: OFF");
            this->flashingConnectionTimer->stop();
            this->sessionTimer->stop();
            this->inSession = false;
            this->noSessionTimer->stop();
            ui->batteryLabel_2->setText("0");//show that battery is now 0
        }
    }
    //this->flashingConnectionTimer->stop();
}

//function to flash the connection view for the battery level
void MainWindow::flashConnections(){
    batteryCheck();//call the batteryCheck function to get status
    if (batteryStatus!=-1 && !needToDeselect){
        this->needToDeselect = true;//set to show we need to deselect other rows
        switch(this->batteryStatus){//select and deselect appropriate rows to show battery level
            case 0:
                ui->SessionType_2->setCurrentRow(0, QItemSelectionModel::Deselect);
                ui->SessionType_2->setCurrentRow(1, QItemSelectionModel::Deselect);
                ui->SessionType_2->setCurrentRow(2, QItemSelectionModel::Deselect);
                ui->SessionType_2->setCurrentRow(3, QItemSelectionModel::Deselect);
                ui->SessionType_2->setCurrentRow(4, QItemSelectionModel::Deselect);
                ui->SessionType_2->setCurrentRow(5, QItemSelectionModel::Deselect);
                ui->SessionType_2->setCurrentRow(6, QItemSelectionModel::Deselect);
                ui->SessionType_2->setCurrentRow(7, QItemSelectionModel::Select);
            break;
            case 1:
                ui->SessionType_2->setCurrentRow(0, QItemSelectionModel::Deselect);
                ui->SessionType_2->setCurrentRow(1, QItemSelectionModel::Deselect);
                ui->SessionType_2->setCurrentRow(2, QItemSelectionModel::Deselect);
                ui->SessionType_2->setCurrentRow(3, QItemSelectionModel::Deselect);
                ui->SessionType_2->setCurrentRow(4, QItemSelectionModel::Deselect);
                ui->SessionType_2->setCurrentRow(5, QItemSelectionModel::Deselect);
                ui->SessionType_2->setCurrentRow(6, QItemSelectionModel::Select);
                ui->SessionType_2->setCurrentRow(7, QItemSelectionModel::Select);
            break;
            case 2:
                ui->SessionType_2->setCurrentRow(0, QItemSelectionModel::Deselect);
                ui->SessionType_2->setCurrentRow(1, QItemSelectionModel::Deselect);
                ui->SessionType_2->setCurrentRow(2, QItemSelectionModel::Deselect);
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
    }//if we need to deselect to show flash, just deselect all
    else if (this->needToDeselect || this->batteryStatus == 0 || this->batteryStatus == 1){

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
    batteryCheck();
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
        //ui->powerLabel->setText("POWER: OFF");
}
//function called from the timer, will scroll from 8-1
void MainWindow::softOffTimed(){
    if (this->softOffRow <=8){//if havent scrolled all rows yet
       ui->SessionType_2->setCurrentRow(this->softOffRow,QItemSelectionModel::Deselect);
       this->softOffRow++;
       ui->SessionType_2->setCurrentRow(this->softOffRow, QItemSelectionModel::Select);
       //qDebug() << "The row is: " << this->softOffRow;
    }else{//if all rows have been scrolled
        this->softOffTimer->stop();
        this->inSession = false;
        if (saveInSession)//save if need be
        {
            this->saveInSession = false;
            saveSession();
            this->ui->saveButton->setEnabled(true);
        }
        this->powerDown();//call power down
    }
}
//when power button is pressed, gets current time it was pressed, for later comparison against realse
void MainWindow::power_pressed(){
    mLastPressTime = timeSinceStart;
}

void MainWindow::power_released(){
    if (timeSinceStart - mLastPressTime >= 1000){
        //qDebug() << "Power was held";
        // It was held, so this is a power on or a power off

        if (this->powerStatus == false){//if trying to turn on
            if (this->batteryLevel >0){//ensure enough charger
                ui->powerLabel->setText("POWER: ON");
                this->flashingConnectionTimer->start(1000);
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
        // Button was clicked, not held@
        // Are we in a session already?
        if (this->inSession == true){
            softOffFromButton();
            //qDebug() << "Called softOffFromButton, we were already in a session";
        }else{//if not in a session, means we want to switch session group
            // Time to select a session this->sessionGroupRow
            if (this->powerStatus){//ensure device is on
                this->sessionGroupRow= this->sessionGroupRow % 3;
                ui->SessionGroup->setCurrentRow(this->sessionGroupRow,QItemSelectionModel::Deselect);
                this->sessionGroupRow++;
                this->sessionGroupRow= this->sessionGroupRow % 3;
                ui->SessionGroup->setCurrentRow(this->sessionGroupRow,QItemSelectionModel::Select);
                switch(this->sessionGroupRow)//set the time based on
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

//function to model the up button pressed
void MainWindow::upButtonPressed(){
    if(powerStatus){//ensure power on
        if (inSession){//adjust the intensity if in session
            if (this->intensityRow > 0){//make sure is valid range
                for (int i = 0; i< 8; i++){//deselect all rows
                    ui->SessionType_2->setCurrentRow(i,QItemSelectionModel::Deselect);
                }//cycle up through the intensity, shown on the connectivity graph
                ui->SessionType_2->setCurrentRow(this->intensityRow,QItemSelectionModel::Deselect);
                this->intensityRow--;
                ui->SessionType_2->setCurrentRow(this->intensityRow,QItemSelectionModel::Select);
                this->curSession->setIntensity(8-this->intensityRow);

            }
        }
        else{//selecting session type. not in session
            if (sessionTypeRow > 0){//make sure is valid range
                //cycle up through sessionType
                ui->SessionType->setCurrentRow(sessionTypeRow,QItemSelectionModel::Deselect);
                sessionTypeRow--;
                ui->SessionType->setCurrentRow(sessionTypeRow,QItemSelectionModel::Select);
                switch(this->sessionTypeRow){//set the current session hertz and type equal to the selected row
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
//function to model the down button pressed
void MainWindow::downButtonPressed(){
    if(powerStatus){//ensure power on
        if (inSession){//adjust the intensity if in session
            if (this->intensityRow < 7){//ensure valid range
                for (int i = 0; i< 8; i++){
                    ui->SessionType_2->setCurrentRow(i,QItemSelectionModel::Deselect);
                }//cycle down through the intensity, shown on the connectivity graph
                ui->SessionType_2->setCurrentRow(this->intensityRow,QItemSelectionModel::Deselect);
                this->intensityRow++;
                ui->SessionType_2->setCurrentRow(this->intensityRow,QItemSelectionModel::Select);
                this->curSession->setIntensity(8-this->intensityRow);
            }
        }
        else{//selecting session type. not in session
            if (sessionTypeRow < 3){//ensure valid range
                ui->SessionType->setCurrentRow(sessionTypeRow,QItemSelectionModel::Deselect);
                sessionTypeRow++;
                ui->SessionType->setCurrentRow(sessionTypeRow,QItemSelectionModel::Select);
                switch(this->sessionTypeRow){//set the current session hertz and type equal to the selected row
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

//function to show the selections of the session
void MainWindow::setSelections()
{//deselect the current selected rows
    ui->SessionGroup->setCurrentRow(ui->SessionGroup->currentRow(), QItemSelectionModel::Deselect);
    ui->SessionType->setCurrentRow(ui->SessionType->currentRow(), QItemSelectionModel::Deselect);
    ui->SessionType_2->setCurrentRow(ui->SessionType_2->currentRow(), QItemSelectionModel::Deselect);
    if (powerStatus)//ensure power is on
    {//set the appropriate rows to selected based on curSession values for the session group display
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

        //set the appropriate rows to selected based on curSession values for the session type display
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
        //set the appropriate rows to selected based on curSession values for the session intensity
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

//function to load the sessions from the db file, allows for saving sessions between operation.
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
    {//load every line of the session into saved sessions
        for (int i = 0; i < MAX_SESSIONS; i++)
        {
            char* temp = (char*) malloc(200);

            if (fgets(temp, 200, file) != NULL)//not at end of file
            {
                int id, st;
                float Hertz, sg, intensity;
                //scan the current line values
                sscanf(temp, "%d %f %d %f %f", &id, &sg, &st, &Hertz, &intensity);
                //load the line into the session pointer array
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
//function to save the sessions to the db file
void MainWindow::saveSessions()
{
    FILE *file;
    file = fopen(path, "w");
    //clear contents of db file
    for (int i = 0; i < MAX_SESSIONS; i++)//for every session in savedSessions
    {
        //write the session from array to file

        fputs(savedSessions[i]->getRecord(), file);
        fputs("\n", file);
    }
    fclose(file);

}
//function to initiate the slots needed for the ui and timer signals to connect properly with their slots.
void MainWindow::initSlots()
{
    //ui
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

    //timers
    connect(incTimer, SIGNAL (timeout()), this, SLOT (increaseTime()));
    connect(batteryTimer, SIGNAL (timeout()),this, SLOT (drainBattery()));
    connect(softOffTimer, SIGNAL (timeout()),this, SLOT (softOffTimed()));
    connect(noSessionTimer, SIGNAL(timeout()),this, SLOT (powerDown()));
    connect(sessionTimer, SIGNAL(timeout()), this, SLOT(softOffFromButton()));
    connect(cesBlinkTimer, SIGNAL(timeout()), this, SLOT(blinkCESMode()));
    connect(noConnectBlinkTimer, SIGNAL(timeout()), this, SLOT(blinkNoConnect()));
    connect(flashingConnectionTimer,SIGNAL(timeout()),this,SLOT(flashConnections()));


}


//destructor class
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

