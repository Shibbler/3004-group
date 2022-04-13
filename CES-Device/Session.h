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
    Session(int, float, int, float);
    Session(const Session *sesh);            //copy constructor for saving a session
    ~Session();
    char* getRecord();
    int id;
    float getIntensity();
    float getSG();
    int getST();
    float setIntensity(float);
    float setSG(float);
    int setST(int);

private:
    float intensity;
    int sessionType;
    float sessionGroup;
    char* sessionRecord;
    void setRecord();
};

#endif // SESSION_H
