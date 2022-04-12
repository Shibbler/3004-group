#ifndef SESSION_H
#define SESSION_H

//define session group constants. all session groups start with a 2
#define TWENTY_MIN 20
#define FOURTY_FIVE_MIN 21
#define CUSTOM 22

//define session type constants. all session types start with a 3
#define DELTA 30
#define THETA 31
#define ALPHA 32
#define ONE_HUNDRED 33

class Session
{

public:
    Session(int, int, int, float,float);
    Session(const Session *sesh);            //copy constructor for saving a session
    ~Session();
    char* getRecord();
    int id;
    float getIntensity();
    int getSG();
    int getST();
    float getLength();
    float setIntensity(float);
    int setSG(int);
    int setST(int);
    //float hertz;//necessary for us to model depletion, but we could put it as part of a session group or something NVM IM DUMB ITS IN INTENSITY

private:
    float intensity;
    int sessionType;
    int sessionGroup;
    int sessionLength;
    char* sessionRecord;
    void setRecord();
};

#endif // SESSION_H
