#include "Socket.h"

atomic<int32> Socket::clientNumber = ATOMIC_VAR_INIT(1);
