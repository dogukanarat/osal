# OSAL Layer Project
This project aims to create layer between cpp applications and operating system. In this way, even if the operation system is changed or low-level functionality is updated, the application is aimed to be stable and abstacted from operation system and hardware.

# Example Usage
Plant thread implements Mutex and periodic thread functionalities

```cpp
#include "main.h"

using namespace OSAL;

OS::Mutex printMutex;

void *server( void *arg )
{
    UNUSED( arg );

    Needmon::Buffer messageBuffer;
    Needmon::Frame messageFrame;
    Packets::Periodic periodicPacket;

    OS::display("[SERVER] Controller server has been started! ");

    Needmon::Ethernet* insTcp         = new Needmon::TCP("127.0.0.1", 5001);
    Needmon::Communication* insServer = new Needmon::Server(insTcp);

    Needmon::ErrorNo errorNo = true;

    errorNo = insServer->Connect();

    if( errorNo == true )
    {
        errorNo = insServer->Process();
    } else {}

    uint32_t aliveCounter = 0;

    while( errorNo == true )
    {
        errorNo = insServer->Read(messageBuffer);

        if( errorNo == true )
        {
            messageFrame.Parse(messageBuffer);
            periodicPacket.Decode(messageFrame);

            OS::print("[SERVER] Message is received | Message: %d\n", periodicPacket.Data.data1 );
        }
    }

    return 0;
}

void *plant( void *arg )
{
    UNUSED( arg );

    OS::TimePoint currentStartTime{};
    OS::TimePoint nextStartTime{};

    const OS::MilliSecond intervalMillis{100};

    uint32_t aliveCounter = 0;

    while( true )
    {
        currentStartTime = OS::Now();

        printMutex.Lock();
        OS::print("[PLANT] Alive Counter: %d\n", aliveCounter++);
        printMutex.Unlock();

        nextStartTime = currentStartTime + intervalMillis;

        OS::SleepUntil( nextStartTime );

    }

    return 0;
}

int main()
{
    OS::display("[PROCESS] Controller has been started!");

    /**
     * @brief 
     * 
     */
    OS::thread_t thread_server;
    OS::thread_t thread_plant;

    /**
     * @brief Construct a new os thread create object
     * 
     */
    OS::create( &thread_server, server );
    OS::create( &thread_plant, plant );

    /**
     * @brief Construct a new os thread join object
     * 
     */
    OS::join( thread_server );
    OS::join( thread_plant );
  
    return 0;
}
```
