#ifndef BASE_LOCK_H_
#define BASE_LOCK_H_

#include "base/macros.h"

#include <pthread.h>

namespace base {

class Lock { 
 public:
  using NativeHandle =  pthread_mutex_t;

  Lock();
  ~Lock();

  bool Try();
  void Acquire();
  void Release();             
             
  NativeHandle* native_handle() { return &native_handle_; } 

  static bool PriorityInheritanceAvailable();

 private:
  NativeHandle native_handle_;

  DISALLOW_COPY_AND_ASSIGN(Lock);
};

} // namespace base
#endif // BASE_LOCK_H_
