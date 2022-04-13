#include "Session.h"
#include <stdio.h>
#include "mainwindow.h"
Session::Session(int identifier, int sg, int st, float i)
{
     id = identifier;
     sessionGroup = sg;
     sessionType = st;
     intensity = i;
     sessionRecord = (char*) malloc(20);
     setRecord();
}

Session::Session(const Session *sesh)
{
    id = sesh->id;
    sessionGroup = sesh->sessionGroup;
    sessionType = sesh->sessionType;
    intensity = sesh->intensity;
    sessionRecord = (char*) malloc(20);
    setRecord();
}

float Session::getIntensity()
{
    return intensity;
}

float Session::setIntensity(float i)
{
    intensity = i;
    setRecord();
    return intensity;
}

int Session::getSG()
{
    return sessionGroup;
}

int Session::setSG(int sg)
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
    sprintf(sessionRecord, ":%d:%d:%d:%2.2f:", id, sessionGroup, sessionType, intensity);
}

Session::~Session()
{
}
