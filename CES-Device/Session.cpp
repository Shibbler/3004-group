#include "Session.h"
#include <stdio.h>
#include "mainwindow.h"
Session::Session(int identifier, float sg, int st, float hz, float i)
{
     id = identifier;
     sessionGroup = sg;
     sessionType = st;
     Hertz = hz;
     intensity = i;
     sessionRecord = (char*) malloc(100);
     setRecord();
}

Session::Session(const Session *sesh)
{
    id = sesh->id;
    sessionGroup = sesh->sessionGroup;
    sessionType = sesh->sessionType;
    Hertz = sesh->Hertz;
    intensity = sesh->intensity;
    sessionRecord = (char*) malloc(20);
    setRecord();
}

void Session::setIntensity(float i)
{
    intensity = i;
    setRecord();
}

float Session::getIntensity()
{
    return intensity;
}

void Session::setId(int i)
{
    id = i;
    setRecord();
}

float Session::getHertz()
{
    return Hertz;
}

float Session::setHertz(float i)
{
    Hertz = i;
    setRecord();
    return Hertz;
}

float Session::getSG()
{
    return sessionGroup;
}

float Session::setSG(float sg)
{
    sessionGroup = sg;
    setRecord();
    return sg;
}

int Session::getST()
{
    return sessionType;
}

int Session::setST(int st)
{
    sessionType = st;
    setRecord();
    return st;
}

char* Session::getRecord()
{
    return sessionRecord;
}

void Session::setRecord()
{
    sprintf(sessionRecord, "%d %2.2f %d %2.2f %2.2f", id, sessionGroup, sessionType, Hertz, intensity);
}

Session::~Session()
{
}
