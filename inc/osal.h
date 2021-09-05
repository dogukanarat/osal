#include "os_memory.h"
#include "os_thread.h"
#include "os_stream.h"
#include "os_timer.h"
#include "os_mutex.h"
#include "os_chrono.h"

namespace OSAL
{
    class OS
    : public Memory
    , public Thread
    , public Stream
    , public Timer
    , public Mutex
    , public ThChrono
    { };
}