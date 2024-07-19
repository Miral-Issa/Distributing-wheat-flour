/*
 * The PRODUCER
 */

#include "local.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int generateRandomNumber(int min, int max);
double stillGoodCont(int pblow, int distanceContainer8, int maxDistance);
int main(int argc, char *argv[])
{
  static struct container local_buffer;
  pid_t ppid = getppid();
  int semid, shmid;
  char *shmptr;
  struct MEMORY *memptr;
  struct sharedOpenGL *memo;

  if (argc != 10)
  {
    fprintf(stderr, "Usage: %s <arg1> <arg2> <arg3>\n", argv[0]);
    return 1;
  }
  // Tsleep2_str,distance_str, Tsleep1_str,wheatContainers1,wheatContainers2
  //,refillPeriod2 ,refillPeriod1,
  int Tsleep1 = atoi(argv[1]);
  int distance = atoi(argv[2]);
  int Tsleep2 = atoi(argv[3]);
  int wheatContainers1 = atoi(argv[4]);
  int wheatContainers2 = atoi(argv[5]);
  int refillPeriod2 = atoi(argv[6]);
  int refillPeriod1 = atoi(argv[7]);
  int weight = atoi(argv[8]);
  int maxDistance = atoi(argv[9]);
  distance--;

  /*
   * Access, attach and reference the shared memory
   */
  if ((shmid = shmget((int)ppid, 0, 0)) != -1)
  {
    if ((shmptr = (char *)shmat(shmid, (char *)0, 0)) == (char *)-1)
    {
      perror("shmat -- producer -- attach");
      exit(1);
    }
    memptr = (struct MEMORY *)shmptr;
  }
  else
  {
    perror("shmget -- producer -- access");
    exit(2);
  }

  /*
   * Access the semaphore set
   */
  if ((semid = semget((int)ppid, 2, 0)) == -1)
  {
    perror("semget -- producer -- access");
    exit(3);
  }
  /*
   * OPENGL SHARED MEMORY
   */
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

  srand((unsigned)getpid());
  int containerNum = 0;

  // start of while(1)
  while (1)
  {
    int wheatContainers = generateRandomNumber(wheatContainers1, wheatContainers2);
    int distanceContiner = generateRandomNumber(0, maxDistance);

    for (int j = 0; j < wheatContainers; j++)
    {
      local_buffer.plane_pid = 0;
      local_buffer.containerID = 0;
      local_buffer.weight = 0;
      local_buffer.pBlow = 0;
      local_buffer.distance = 0;
      local_buffer.stillGood = 0;

      // generate container
      struct container temp;
      temp.plane_pid = getpid();
      temp.containerID = containerNum;
      containerNum++;
      temp.weight = weight;                   // make it user define
      temp.pBlow = (double)rand() / RAND_MAX; // keep it random (as is)
      temp.distance = distanceContiner;       /// make it random but less than the maximum given by user and depend on pBlow
      temp.stillGood = stillGoodCont(temp.pBlow, temp.distance, maxDistance);

      // strcat(local_buffer, struct container temp);
      memo->dropFLAG = 1;
      memo->cont->containerID = temp.containerID;
      memo->cont->plane_pid = temp.plane_pid;

      local_buffer = temp;

      acquire.sem_num = AVAIL_SLOTS;

      if (semop(semid, &acquire, 1) == -1)
      {
        perror("semop -- producer -- acquire");
        exit(4);
      }

      memptr->cont[memptr->tail] = local_buffer;
      // printf("P: [%d] %s.\n", memptr->tail, memptr->buffer[memptr->tail]);

      memptr->tail = (memptr->tail + 1) % N_SLOTS;

      release.sem_num = TO_CONSUME;

      if (semop(semid, &release, 1) == -1)
      {

        perror("semop -- producer -- release");
        exit(5);
      }
      memo->recievedFlag = 1;
      memo->P_id = getpid();
      sleep(generateRandomNumber(Tsleep1, Tsleep2));
    }
    int refillPeriod = generateRandomNumber(refillPeriod2, refillPeriod1);
    memo->refillFLAG = 1;
    sleep(refillPeriod);
    printf(" refill plane \n");

    memo->refillFLAG = 0;
  }
  // put all of it a while(1) so after refile the plane continue with new containers

  exit(0);
}
int generateRandomNumber(int min, int max)
{
  return min + rand() % (max - min + 1);
}
// make a function that chooses the container atribute carefully

double stillGoodCont(int pblow, int distanceContainer, int maxDistance)
{
  double random_value = (double)rand() / RAND_MAX;
  double biased_value = random_value * (1 - pblow) * (1 - (double)distanceContainer / maxDistance);
  return biased_value;
}

/*
double biased_value = random_value * (1 - pBlow) * (1 - (double)distance / 50);
*/
