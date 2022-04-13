#ifndef SESSION_H
#define SESSION_H

//define session group constants. all session groups start with a 2
#define TWENTY_MIN 20.0
#define FOURTY_FIVE_MIN 45.0

//define session type constants. all session types start with a 3
#define DELTA 30
#define THETA 31
#define ALPHA 32
#define ONE_HUNDRED 33

//define hertz measurement for every session type
#define DELTA_HZ 3.75
#define THETA_HZ 7.0
#define ALPHA_HZ 10
#define ONE_HUNDRED_HZ 100.0


class Session
{

public:
    Session(int, float, int, float, float);
    Session(const Session *sesh);            //copy constructor for saving a session
    ~Session();
    char* getRecord();
    int id;
    float getHertz();
    float getSG();
    int getST();
    float setHertz(float);
    float setSG(float);
    int setST(int);
    void setId(int);
    float getIntensity();
    void setIntensity(float);

private:
    float Hertz;
    float intensity;
    int sessionType;
    float sessionGroup;
    char* sessionRecord;
    void setRecord();
};

#endif // SESSION_H
