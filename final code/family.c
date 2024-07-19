/*
 * The family in need
 */

#include "local.h"

int main(int argc, char *argv[])
{
  key_t key, fkey;
  int msqid, semid;
  MESSAGE msg;
  int familyNum;
  int starvation = 0, maxStarLvl;
  /*
   *SHARED MEMORY FOR OPENGL
   */
  struct sharedOpenGL *memo;
  key_t gl_shm_key = ftok(".", 'q');
  int gl_shmid;

  if ((gl_shmid = shmget(gl_shm_key, 0, 0)) != -1)
  {
    void *shmptr = shmat(gl_shmid, NULL, 0);
    if (shmptr == (void *)-1)
    {
      perror("shmat -- OPENGL -- attach");
      exit(1);
    }
    memo = (struct sharedOpenGL *)shmptr;
  }
  else
  {
    perror("shmget -- OPEN -- access1");
    exit(2);
  }
  if (argc != 3)
  {
    printf("family needs to know its number and max starvation level\n");
    exit(1);
  }
  familyNum = atoi(argv[1]);
  maxStarLvl = atoi(argv[2]);
  // printf("000000000000000family num %d\n",familyNum+1);
  // generate the message queue key
  if ((key = ftok(".", QSEED)) == -1)
  {
    perror("Parent: key generation");
    printf("Error number: %d\n", errno);
    exit(1);
  }

  // open the message queue
  if ((msqid = msgget(key, 0660)) == -1)
  {
    perror("family: open message queue");
    printf("Error number: %d\n", errno);
    exit(2);
  }
  else
  {
    printf("family opened message queue\n");
  }

  // generate the semaphore key
  if ((fkey = ftok(".", 'f')) == -1)
  {
    perror("family: key generation for semaphore");
    printf("Error number: %d\n", errno);
    exit(1);
  }
  // open the semaphore
  if ((semid = semget(fkey, 0, 0)) == -1)
  {
    perror("semget -- family -- access");
    exit(3);
  }

  srand(getpid());
  while (1)
  {

    // update starvation level
    starvation += rand() % 7 + 1;
    if (semctl(semid, familyNum, SETVAL, starvation) == -1)
    {
      perror("semctl (family)-- famiy -- update");
      exit(3);
    }

    if (starvation >= maxStarLvl)
    {
      printf("############family died due to starvation\n");
      kill(getppid(), SIGTRAP);
      memo->familyflag = 1;
    }
    // recieve weahtbags
    if (msgrcv(msqid, &msg, sizeof(msg), familyNum, 0) == -1)
    {
      perror("family: msgrcv");
      exit(3);
    }
    else
    {
      memo->familyID = familyNum + 1;

      // recieve weahtbags
      printf("##family number %d with starvation lvl=%d recieve %d weaht\n", familyNum + 1, starvation, msg.msg_data);
      starvation -= msg.msg_data;
      memo->distributedFlag = 1;
      memo->wet-=msg.msg_data;

      if (starvation < 0)
        starvation = 0;
    }
  }
  pause();
  return 0;
}
