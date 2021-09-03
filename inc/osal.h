#include "os_memory.h"
#include "os_thread.h"
#include "os_stream.h"
#include "os_timer.h"

namespace OSAL
{
    class OS : public Memory, public Thread, public Stream, public Timer
    {

    };
}