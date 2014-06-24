#ifndef SYSBACE_SINGLEMANAGER_H 
#define SYSBACE_SINGLEMANAGER_H

//////////////////////////////////////////////////////////////////////////
//µ¥¼þÄ£°å
template <typename T>
class CSingletonT
{
public:

    virtual ~CSingletonT(){}

protected:

    CSingletonT(){}

public:

    static T &Instance()
    {
        static T instance;
        return instance;
    } 
};

#endif