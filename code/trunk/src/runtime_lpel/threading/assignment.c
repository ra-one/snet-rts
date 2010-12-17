
#include <pthread.h>

#include "snettypes.h"
#include "bool.h"

#include "lpel.h"
#include "scheduler.h"
#include "task.h"


static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static int num_workers;
static int box_last;


/**
 * Initialize assignment
 */
void AssignmentInit(int lpel_num_workers)
{
  num_workers = lpel_num_workers;
  box_last = -1;
}

/**
 * Assign a task to a worker
 */
void AssignmentAssign(task_t *t, bool is_box)
{
  int target;

  pthread_mutex_lock( &lock);
  {
    if (is_box) {
      box_last += 1;
      if (box_last == num_workers) box_last = 0;
    }
    target = box_last;
  }
  pthread_mutex_unlock( &lock);

  if (target == -1) target = 0;

  SchedAssignTask( t, target);
}
