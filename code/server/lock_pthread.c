#include "unpthread.h"
#include <sys/mman.h>

static pthread_mutex_t *mptr; /* actual mutex will be in shared memory */

void my_lock_init(char *pathname)
