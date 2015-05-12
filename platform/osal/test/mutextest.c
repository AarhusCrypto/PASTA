#include <osal.h>
#ifndef WINDOWS 
#include <unistd.h>
#endif

volatile MUTEX m = 0;

void * t(void * a) {
  OE oe = (OE)a;
  usleep(1000);
  oe->p("unlock m");
  oe->unlock(m);
  return 0;
}

static int mutextest(OE oe) {
  MUTEX m1 = 0;
  m1 = Mutex_new(0);
  Mutex_lock(m1);
  Mutex_lock(m1);
  Mutex_lock(m1);

  {
    OE oe = OperatingEnvironment_New();
    ThreadID tid = 0;
    m = oe->newmutex();
    
    oe->p("locking m");
    oe->lock(m);
    tid = oe->newthread(t, oe);
    
    oe->p("locking m again");
    oe->lock(m);
    oe->jointhread(tid);
  return 0;
  }
}
