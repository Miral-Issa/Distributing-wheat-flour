/*
 * The CONSUMER
 */

#include "local.h"

void splitToSafeHouse(int weaht);

int initialEnergy = 0;
int energy1 = 0;
int energy2 = 0;
int CollectWork = 0;

struct Worker *workers;

void handle_alarm(int signum)
{
  srand(time(NULL));

  for (int i = 0; i < CollectWork; i++)
  {
    // Generate a random number between strt_of_dec and end_of_dec
    int to_dec = rand() % (energy2 - energy1 + 1) + energy1;
    workers[i].energy -= to_dec;
    workers[i].martyring_prob += 0.04;

    if (workers[i].energy <= 0)
    {
      // printf("Worker %d outt energy!\n", i + 1);
      kill(getppid(), SIGUSR2);
      // new worker came in their place
      workers[i].energy = initialEnergy;
      workers[i].martyring_prob = 0.1;
    }
    printf("Worker %d energy: %d\n", i + 1, workers[i].energy); // Print energy level
  }

  // Set the alarm for the next 2 seconds
  alarm(2);
}
int sighold(int sig);
int sigrelse(int sig);

int main(int argc, char *argv[])
{

  if (argc != 5)
  {
    fprintf(stderr, "Usage: %s <arg1> <arg2> <arg3>\n", argv[0]);
    return 1;
  }

  initialEnergy = atoi(argv[1]);
  energy1 = atoi(argv[2]);
  energy2 = atoi(argv[3]);
  CollectWork = atoi(argv[4]);

  static struct container local_buffer;
  int semid, shmid;
  pid_t ppid = getppid();
  pid_t mypid = getpid();
  char *shmptr;
  struct MEMORY *memptr;
  struct sharedOpenGL *memo;

  // Allocate memory for the workers array dynamically
  workers = malloc(CollectWork * sizeof(struct Worker));
  if (workers == NULL)
  {
    perror("Failed to allocate memory for workers");
    exit(EXIT_FAILURE);
  }

  // Initialize the array of workers
  for (int i = 0; i < CollectWork; i++)
  {
    workers[i].energy = initialEnergy;
    workers[i].martyring_prob = 0.1;
  }

  /*
   * Access, attach and reference the shared memory
   */
  if ((shmid = shmget((int)ppid, 0, 0)) != -1)
  {
    if ((shmptr = (char *)shmat(shmid, (char *)0, 0)) == (char *)-1)
    {
      perror("shmat -- consumer -- attach");
      exit(1);
    }
    memptr = (struct MEMORY *)shmptr;
  }
  else
  {
    perror("shmget -- consumer -- access");
    exit(2);
  }
  /*
   * OPENGL SHARED MEMORY
   */
  key_t gl_shm_key = ftok(".", 'q');
  int gl_shmid;

  if ((gl_shmid = shmget(gl_shm_key, 0, 0)) != -1)
  {
    void *shmptr1 = shmat(gl_shmid, NULL, 0);
    if (shmptr1 == (void *)-1)
    {
      perror("shmat -- OPENGL -- attach");
      exit(1);
    }
    memo = (struct sharedOpenGL *)shmptr1;
  }
  else
  {
    perror("shmget -- OPEN -- access1");
    exit(2);
  }
  /*
   * Access the semaphore set
   */
  if ((semid = semget((int)ppid, 2, 0)) == -1)
  {
    perror("semget -- consumer -- access");
    exit(3);
  }

  srand((unsigned)getpid());

  while (1)
  {
    acquire.sem_num = TO_CONSUME;

    sighold(SIGALRM);
    if (semop(semid, &acquire, 1) == -1)
    {
      perror("semop -- consumer -- acquire");
      exit(4);
    }
    sigrelse(SIGALRM);

    for (int j = 0; j < CollectWork; j++)
    {
      if (workers[j].energy <= 0 || workers[j].martyring_prob == 0.9)
      {
        // printf("Worker %d has to be changed!\n", j + 1);
        kill(getppid(), SIGUSR2);
        workers[j].energy = initialEnergy;
        workers[j].martyring_prob = 0.1;
        memo->energyCollect = 1;
      }
      else if (workers[j].energy <= 5)
      {
        workers[j].martyring_prob = 0.75;
        workers[j].energy -= 3; // Decrease energy by 3
      }
      workers[j].energy -= 3; // Decrease energy by 3
      workers[j].martyring_prob += 0.02;
    }

    local_buffer = memptr->cont[memptr->head];
    if (local_buffer.stillGood > 0.1)
    {
      memo->recievedFlag = 1;
      memo->committeID = mypid;
    }

    // printf("C: [%d] %s.\n", memptr->head, local_buffer);
    printf("*******New Container********\n");
    printf("Container:\nplane ID = %d\tcontainer ID = %d\n", local_buffer.plane_pid, local_buffer.containerID);
    printf("weight = %d\tblown probability = %f\n", local_buffer.weight, local_buffer.pBlow);
    printf("distance blown from ground = %d\tstill good of it = %f\n", local_buffer.distance, local_buffer.stillGood);

    int splittedWeight = local_buffer.weight * local_buffer.stillGood;
    memo->wet += splittedWeight;

    if (splittedWeight == 0)
    {
      printf("!!the container was completely distroyed!!\n");
      // printf("parent id %d or %d",ppid,getppid());
      kill(ppid, SIGUSR1);
    }
    splitToSafeHouse(splittedWeight);

    memptr->head = (memptr->head + 1) % N_SLOTS;

    release.sem_num = AVAIL_SLOTS;

    sighold(SIGALRM);
    if (semop(semid, &release, 1) == -1)
    {
      perror("semop -- consumer -- release");
      exit(5);
    }
    sigrelse(SIGALRM);
    // sleep(rand() % sleep_limit + 1);
    signal(SIGALRM, handle_alarm);
    alarm(2); // Set the initial alarm for 2 seconds
  }
  free(workers);
  exit(0);
}

void splitToSafeHouse(int weaht)
{
  char *fifoName = "safeHouse";
  int fifod;
  int bag = 1;
  for (int i = 0; i < weaht; i++)
  {
    // open safeHouse fifo
    if ((fifod = open(fifoName, O_WRONLY)) == -1)
    {
      perror("open fifo");
      // exit(1);
    }
    else
    {
      // printf("committee open safe house\n");
    }

    // write on the safe house
    // int splittedWeight = local_buffer.weight * local_buffer.stillGood;
    if (write(fifod, &bag, sizeof(int)) == -1)
    {
      perror("writing on fifo");
      // exit(2);
    }
    else
    {
      // printf("committee put 1 kg weaht bag in the safeHouse\n");
    }
    // close fifo
    close(fifod);
  }
}
