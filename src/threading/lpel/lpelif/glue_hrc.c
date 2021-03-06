#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "glue_snet.h"
#include "threading.h"
#include "hrc_lpel.h"
#include "lpel_common.h"
#include <lpel/monitor.h>
#include "debug.h"

/* provisional assignment module */
#include "assign.h"

/* monitoring module */
#include "mon_snet.h"

/* put this into assignment/monitoring module */
#include "distribution.h"

#define TSK_DBG //

static int rec_lim = -1;

#ifdef USE_LOGGING
static int mon_flags = 0;
static FILE *mapfile = NULL;
static const char *filePath = "/shared/nil/Out/";
#endif

/**
 * use the Distributed S-Net placement operators for worker placement
 */
static bool sosi_placement = false;

int SNetThreadingInit(int argc, char **argv)
{
#ifdef USE_LOGGING
	char *mon_elts = NULL;
#endif

	lpel_config_t config;
	int i;
	int neg_demand = 0;
  int wait_win_found = 0;

	memset(&config, 0, sizeof(lpel_config_t));

	int priorf = 14;

	config.type = HRC_LPEL;

	config.flags = LPEL_FLAG_PINNED;
  config.wait_window_size = 1;
  config.wait_threshold = -1;
  config.proc_others = 0;
	for (i=0; i<argc; i++) {
		if(strcmp(argv[i], "-m") == 0 && i + 1 <= argc) {
			/* Monitoring level */
			i = i + 1;
#ifdef USE_LOGGING
			mon_elts = argv[i];
#endif
		} else if(strcmp(argv[i], "-excl") == 0 ) {
			/* Assign realtime priority to workers*/
			config.flags |= LPEL_FLAG_EXCLUSIVE;
		} else if(strcmp(argv[i], "-co") == 0 && i + 1 <= argc) {
			/* Number of cores for others */
			i = i + 1;
			config.proc_others = atoi(argv[i]);
		} else if(strcmp(argv[i], "-cw") == 0 && i + 1 <= argc) {
			/* Number of cores for others */
			i = i + 1;
			config.proc_workers = atoi(argv[i]);
		} else if(strcmp(argv[i], "-w") == 0 && i + 1 <= argc) {
			/* Number of workers */
			i = i + 1;
      config.num_workers = atoi(argv[i]);
      config.proc_workers = atoi(argv[i]);
		} else if(strcmp(argv[i], "-sosi") == 0) {
			sosi_placement = true;
		} else if (strcmp(argv[i], "-np") == 0) { // no pinned
			config.flags ^= LPEL_FLAG_PINNED;
		} else if(strcmp(argv[i], "-pf") == 0 && i + 1 <= argc) {
			i = i + 1;
			priorf = atoi(argv[i]);
		} else if(strcmp(argv[i], "-rl") == 0 && i + 1 <= argc) {
			i = i + 1;
			rec_lim = atoi(argv[i]);
		} else if(strcmp(argv[i], "-nd") == 0 && i + 1 <= argc) {
			i = i + 1;
			neg_demand = atoi(argv[i]);
    } else if(strcmp(argv[i], "-ws") == 0 && i + 1 <= argc) {
			i = i + 1;
			config.wait_window_size = atoi(argv[i]);
      wait_win_found = 1;
      //printf("glue_hrc: ws %d %d\n",config.wait_window_size,atoi(argv[i]));
    } else if(strcmp(argv[i], "-wt") == 0 && i + 1 <= argc) {
			i = i + 1;
			config.wait_threshold = atof(argv[i]);
      //printf("glue_hrc: wt %f %f\n",config.wait_threshold,atof(argv[i]));
		}    
	}
  
  // if wait window is not provided set it to number of workers
  if(wait_win_found == 0){
    config.wait_window_size = config.num_workers;
  }
  //printf("glue_hrc: wt %f, ws %d\n",config.wait_threshold,config.wait_window_size);
	LpelTaskSetPriorityFunc(priorf);
	LpelTaskSetNegLim(neg_demand);

#ifdef USE_LOGGING
	char fname[63+1];
	if (mon_elts != NULL) {
		if (strchr(mon_elts, MON_ALL_FLAG) != NULL) {
			mon_flags = (1<<7) - 1;
		} else {
			if (strchr(mon_elts, MON_MAP_FLAG) != NULL) mon_flags |= SNET_MON_MAP;
			if (strchr(mon_elts, MON_TIME_FLAG) != NULL) mon_flags |= SNET_MON_TIME;
			if (strchr(mon_elts, MON_WORKER_FLAG) != NULL) mon_flags |= SNET_MON_WORKER;
			if (strchr(mon_elts, MON_TASK_FLAG) != NULL) mon_flags |= SNET_MON_TASK;
			if (strchr(mon_elts, MON_STREAM_FLAG) != NULL) mon_flags |= SNET_MON_STREAM;
			if (strchr(mon_elts, MON_MESSAGE_FLAG) != NULL) mon_flags |= SNET_MON_MESSAGE;
			if (strchr(mon_elts, MON_LOAD_FLAG) != NULL) mon_flags |= SNET_MON_LOAD;
		}

		if ( mon_flags & SNET_MON_MAP) {
      snprintf(fname, 63, "%sn%02d_tasks.map", filePath,SNetDistribGetNodeId() );
			/* create a map file */
			mapfile = fopen(fname, "w");
			assert( mapfile != NULL);
			(void) fprintf(mapfile, "%s%c", LOG_FORMAT_VERSION, END_LOG_ENTRY);
		}
	}

	/* initialise monitoring module */
	SNetThreadingMonInit(&config.mon, SNetDistribGetNodeId(), mon_flags);
#endif

	LpelInit(&config);

	if (LpelStart(&config)) SNetUtilDebugFatal("Could not initialize LPEL!");
  if (!SNetDistribIsRootNode()) LpelCleanup();
	return 0;
}

/*****************************************************************************
 * Spawn a new task
 ****************************************************************************/
static void setTaskRecLimit(snet_entity_descr_t type, lpel_task_t *t){
	int limit;
	switch(type) {
	case ENTITY_sync:
	case ENTITY_nameshift:
	case ENTITY_other:
	case ENTITY_collect:
		limit = -1;
		break;
	default:
		limit = rec_lim;
		break;
	}
	LpelTaskSetRecLimit(t, limit);
}


int SNetThreadingSpawn(snet_entity_t *ent){
	snet_entity_descr_t type = SNetEntityDescr(ent);
	const char *name = SNetEntityName(ent);
	int location = LPEL_MAP_MASTER;
	int l1 = strlen(SNET_SOURCE_PREFIX);
	int l2 = strlen(SNET_SINK_PREFIX);
	if ((sosi_placement && (strnstr(name, SNET_SOURCE_PREFIX, l1) || strnstr(name, SNET_SINK_PREFIX, l2))) 	// sosi placemnet and entity is source/sink
			|| type == ENTITY_other)	// wrappers
		location = LPEL_MAP_OTHERS;

  TSK_DBG("\n\nSpawn Start Taskname: %s\n",name);
	lpel_task_t *t = LpelTaskCreate(
			location,
			//(lpel_taskfunc_t) func,
			EntityTask,
			ent,
			GetStacksize(type)
	);
	if (location != LPEL_MAP_OTHERS)
			setTaskRecLimit(type, t);
  
#ifdef USE_LOGGING
	if (mon_flags & SNET_MON_TASK){
		mon_task_t *mt = SNetThreadingMonTaskCreate(
				LpelTaskGetId(t),
				name
		);
		LpelTaskMonitor(t, mt);
		/* if we monitor the task, we make an entry in the map file */
	}
	if ((mon_flags & SNET_MON_MAP) && mapfile) {
		int tid = LpelTaskGetId(t);
		(void) fprintf(mapfile, "%d%s%c", tid, SNetEntityStr(ent), END_LOG_ENTRY);
	}
#endif

	LpelTaskStart(t);
  TSK_DBG("Spawn End Taskname: %s\n\n",name);
	return 0;
}

int SNetThreadingCleanup(void)
{
	SNetAssignCleanup();

#ifdef USE_LOGGING
	/* Cleanup monitoring module */
	SNetThreadingMonCleanup();
	if (mapfile) {
		(void) fclose(mapfile);
	}
#endif

	return 0;
}
