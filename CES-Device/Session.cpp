#include "Session.h"
#include <stdio.h>
#include "mainwindow.h"
Session::Session(int identifier, int sg, int st, int i)
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

int Session::getIntensity()
{
    return intensity;
}

int Session::setIntensity(int i)
{
    intensity = i;
    setRecord();
}

int Session::getSG()
{
    return sessionGroup;
}

int Session::setSG(int sg)
{
    sessionGroup = sg;
    setRecord();
}

int Session::getST()
{
    return sessionType;
}

int Session::setST(int st)
{
    sessionType = st;
    setRecord();
}

char* Session::getRecord()
{
    return sessionRecord;
}

void Session::setRecord()
{
    sprintf(sessionRecord, ":%d:%d:%d:%d:", id, sessionGroup, sessionType, intensity);
}

Session::~Session()
{
}
