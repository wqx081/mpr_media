#include "base/lock.h"

#include <errno.h>
#include <string.h>

#include <glog/logging.h>

namespace base {
                
Lock::Lock() { 
  pthread_mutexattr_t mta;
  int rv = pthread_mutexattr_init(&mta);
  DCHECK_EQ(rv, 0) << ". " << strerror(rv);
  if (PriorityInheritanceAvailable()) { 
    rv = pthread_mutexattr_setprotocol(&mta, PTHREAD_PRIO_INHERIT);
    DCHECK_EQ(rv, 0) << ". " << strerror(rv);
  } 
  rv = pthread_mutex_init(&native_handle_, &mta);
  DCHECK_EQ(rv, 0) << ". " << strerror(rv);
  rv = pthread_mutexattr_destroy(&mta);
  DCHECK_EQ(rv, 0) << ". " << strerror(rv);
}

Lock::~Lock() { 
  int rv = pthread_mutex_destroy(&native_handle_);
  DCHECK_EQ(rv, 0) << ". " << strerror(rv);
}

bool Lock::Try() { 
  int rv = pthread_mutex_trylock(&native_handle_);
  DCHECK(rv == 0 || rv == EBUSY) << ". " << strerror(rv);
  return rv == 0;
}

void Lock::Acquire() { 
  int rv = pthread_mutex_lock(&native_handle_);
  DCHECK_EQ(rv, 0) << ". " << strerror(rv);
}

void Lock::Release() { 
  int rv = pthread_mutex_unlock(&native_handle_);
  DCHECK_EQ(rv, 0) << ". " << strerror(rv);
}

bool Lock::PriorityInheritanceAvailable() { 
  return false;
}

} // namespace base
