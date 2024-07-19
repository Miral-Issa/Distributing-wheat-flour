#ifndef __LOCAL_H_
#define __LOCAL_H_

/*
 * Common header file: parent, producer and consumer
 */

#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <wait.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>

//for the fifo
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

//for message queue
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
//for signal
#include <signal.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

//#define ROWS   5
//#define COLS   3

//#define SLOT_LEN  50
#define N_SLOTS  7 //should depend on plans,containers and committees number but leave it as is for now
#define QSEED 'm' //seed to retain message queue (between families and disWorkers) key
/* This declaration is *MISSING* is many solaris environments.
   It should be in the <sys/sem.h> file but often is not! If 
   you receive a duplicate definition error message for semun
   then comment out the union declaration.
   */

union semun {
  int              val;
  struct semid_ds *buf;
  ushort          *array; 
};

struct container
{
  int plane_pid; //process ID of the plane that generated this container
  int containerID; //the number of the container acourding to the plane which generated it
  //plan_id and containerID together make a uniqe key that difrentiate between all containers in the program
  int weight; //the weight of the container
  double pBlow; //probability the container was blowen
  int distance; //the distance the container blow on from ground
  double stillGood; //what percentig of the container still good to use
};

struct MEMORY {
  struct container cont[N_SLOTS];
  int head, tail;
};

// Array of workers
struct Worker {
        int energy;
        float martyring_prob;
    };
    
struct sembuf acquire = {0, -1, SEM_UNDO}, 
              release = {0,  1, SEM_UNDO};

enum {AVAIL_SLOTS, TO_CONSUME};

typedef struct {
  long msg_type;
  //char buffer[BUFSIZ];
  int msg_data;
} MESSAGE;

struct sharedOpenGL
{
  struct container cont[N_SLOTS];
  int numContainers; // New member for the number of containers
  int openGLID;
  int numPlanes;
  int flag_Containers;
  int planeID[N_SLOTS];
  int P_id;
  int containerID;
  int dropFLAG;
  int refillFLAG;
  int committeeNum;
  int committeIDs[N_SLOTS];
  int reliefNum;
  int recievedFlag;
  int wet;
  int committeID;
  int workerIDs[N_SLOTS];
  int energyCollect;
  int FamiliesNum;
  int familiesIDs[N_SLOTS];
  int distributedFlag;
  int familyID;
  int familyflag;
  int terminatedFlag;
  int refillID;
};
#endif

