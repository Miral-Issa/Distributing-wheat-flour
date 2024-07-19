/*
 * The PARENT
 */

#include "local.h"

// typedef void (*sighandler_t)(int);
// sighandler_t sigset(int sig, sighandler_t disp);

// functions
void cleanup();
void alarm_catcher(int);
void disCon_catcher(int);     // distroyed container
void collWorker_catcher(int); // collecting worker need to be replaced
void famDied_catcher(int);    // when a family died
void disWorker_catcher(int);  // distribution worker need to be replaced
int sighold(int sig);
int sigrelse(int sig);

// global vars
int ChildNum;
int *pids_pointer;

int semid, sec_semid, shmid, croaker;
char *shmptr;
char *fifoName = "safeHouse";
int msqid;

int No_bages, P_chrashed, W_blow, MaxC_martyred, MaxD_martyred, MaxF_martyred, MaxF_passed;
int splittingWork, commitee;
// simulation vars
int destroyed_containers = 0;
int collWorker_died = 0, disWorker_died = 0;
int dead_families = 0;
int gl_shmid;
struct sharedOpenGL *gl_shmptr;
int main(int argc, char *argv[])
{
  // sigset(SIGALRM, alarm_catcher);
  // sigset(SIGUSR1, disCon_catcher);
  if (signal(SIGALRM, alarm_catcher) == SIG_ERR)
  {
    perror("Sigset can not set SIGALRM");
    exit(SIGQUIT);
  }
  // alarm(10);
  if (signal(SIGUSR1, disCon_catcher) == SIG_ERR)
  {
    perror("Sigset can not set SIGUSR1");
    exit(SIGQUIT);
  }

  if (signal(SIGUSR2, collWorker_catcher) == SIG_ERR)
  {
    perror("Sigset can not set SIGUSR2");
    exit(SIGQUIT);
  }

  if (signal(SIGTRAP, famDied_catcher) == SIG_ERR)
  {
    perror("Sigset can not set SIGTRAP");
    exit(SIGQUIT);
  }

  if (signal(SIGINT, disWorker_catcher) == SIG_ERR)
  {
    perror("Sigset can not set SIGINT");
    exit(SIGQUIT);
  }

  static struct MEMORY memory;
  static ushort start_val[2] = {N_SLOTS, 0};
  // int            semid,sec_semid, shmid, croaker;
  // char          *shmptr;
  pid_t p_id, c_id, pid = getpid();
  union semun arg;

  memory.head = memory.tail = 0;

  //***********************************************************************************************
  // Reading From file

  char fileName[] = "user.txt"; // Define the file name

  // Open the file for reading

  FILE *fptr = fopen(fileName, "r");
  if (fptr == NULL)
  {
    printf("Error opening file.\n");
    return 1;
  }
  // Variables to store data read from the file
  int cargoPlanes, wheatContainers1, wheatContainers2, /*dropTime1, dropTime2,*/ refillPeriod1, refillPeriod2, /*probability, reliefCommittees,*/ reliefWorkers; // bagsPerTrip;
  // float energyPerWorker, thresholdTime, deathRate;
  // int planesCrashed, containersShotDown, committeeWorkersMartyred, familiesStarved, distributingWorkersMartyred;
  int Initialenergy, energy1, energy2, /*ContainerShot,*/ Tsleep1, Tsleep2, EndUp;
  int distance, /*collectComm,*/ CollectWork;
  // double splittingComm, DistributingComm, DistributingWork;
  int numberOfFamilies, maxDistance, weight, max_star_lvl;

  // Read data from the file
  char line[100];
  fgets(line, 100, fptr); // number of cargoPlanes
  char *token = strtok(line, " ");
  cargoPlanes = atoi(token);
  fgets(line, 100, fptr); // number of commitees
  char *token1 = strtok(line, " ");
  commitee = atoi(token1);
  fgets(line, 100, fptr); // range of wheatContainers1 from
  char *token2 = strtok(line, " ");
  wheatContainers1 = atoi(token2);
  fgets(line, 100, fptr); // range of wheatContainers1 to
  char *token3 = strtok(line, " ");
  wheatContainers2 = atoi(token3);
  fgets(line, 100, fptr); // range of dropTime1 from
  // char *token4 = strtok(line, " ");
  // dropTime1 = atoi(token4);
  fgets(line, 100, fptr); // range of dropTime1 to
  // char *token5 = strtok(line, " ");
  // dropTime2 = atoi(token5);
  fgets(line, 100, fptr); // range of refillPeriod1 from
  char *token6 = strtok(line, " ");
  refillPeriod1 = atoi(token6);
  fgets(line, 100, fptr); // range of refillPeriod1 to
  char *token7 = strtok(line, " ");
  refillPeriod2 = atoi(token7);
  fgets(line, 100, fptr); // probability of planes to be collide
  // char *token8 = strtok(line, " ");
  // probability = atoi(token8);
  fgets(line, 100, fptr); // distance from the ground
  char *token9 = strtok(line, " ");
  distance = atoi(token9);
  fgets(line, 100, fptr); // collecting relief committees
  // char *token10 = strtok(line, " ");
  // collectComm= atoi(token10);
  fgets(line, 100, fptr); //// relief Workers of collecting commitee
  char *token11 = strtok(line, " ");
  CollectWork = atoi(token11);
  fgets(line, 100, fptr); // Splitting relief committees
  // char *token12 = strtok(line, " ");
  // splittingComm = strtod(token12, NULL);
  fgets(line, 100, fptr); // relief Workers of Splitting commitee
  char *token13 = strtok(line, " ");
  splittingWork = strtod(token13, NULL);
  fgets(line, 100, fptr); // Distributing relief committees
  // char *token14 = strtok(line, " ");
  // DistributingComm = strtod(token14, NULL);
  fgets(line, 100, fptr); //// relief Workers of Distributing commitee
  char *token15 = strtok(line, " ");
  // DistributingWork =  strtod(token15, NULL);
  reliefWorkers = strtod(token15, NULL);
  fgets(line, 100, fptr); // distribution relief worker can handle a user-defined number of bags per distribution trip
  char *token16 = strtok(line, " ");
  No_bages = atoi(token16);
  fgets(line, 100, fptr); // initial energy for each worker
  char *token17 = strtok(line, " ");
  Initialenergy = atoi(token17);
  fgets(line, 100, fptr); // range of energy to be subtracted from
  char *token18 = strtok(line, " ");
  energy1 = atoi(token18);
  fgets(line, 100, fptr); // range of energy to be subtracted to
  char *token19 = strtok(line, " ");
  energy2 = atoi(token19);
  fgets(line, 100, fptr); // containersShotDown
  // char *token20 = strtok(line, " ");
  // ContainerShot = atoi(token20);
  fgets(line, 100, fptr); // time to sleep in shared memory 1
  char *token21 = strtok(line, " ");
  Tsleep1 = atoi(token21);
  fgets(line, 100, fptr); // time to sleep in shared memory 2
  char *token22 = strtok(line, " ");
  Tsleep2 = atoi(token22);
  fgets(line, 100, fptr); // threshold time for the game to end up
  char *token23 = strtok(line, " ");
  EndUp = atoi(token23);
  fgets(line, 100, fptr); // max number of planes to be crashed
  char *token24 = strtok(line, " ");
  P_chrashed = atoi(token24);
  fgets(line, 100, fptr); // max number of wheat flour to bw shoot
  char *token25 = strtok(line, " ");
  W_blow = atoi(token25);
  fgets(line, 100, fptr); // max number of collecting relief committee workers who have been martyred
  char *token26 = strtok(line, " ");
  MaxC_martyred = atoi(token26);
  fgets(line, 100, fptr); // max number of distributing relief committee workers who have been martyred
  char *token27 = strtok(line, " ");
  MaxD_martyred = atoi(token27);
  fgets(line, 100, fptr); // max number of families who passed away due to starvation
  char *token28 = strtok(line, " ");
  MaxF_passed = atoi(token28);
  // fgets(line,100,fptr); // The number of families in the simulation
  /*char *token29 = strtok(line, " ");
  numberOfFamilies= atoi(token28);
  printf("number of families= %d\n",numberOfFamilies);*/
  fgets(line, 100, fptr); // weight
  char *token29 = strtok(line, " ");
  weight = atoi(token29);
  fgets(line, 100, fptr); // max distance planes fly at
  char *token30 = strtok(line, " ");
  maxDistance = atoi(token30);
  fgets(line, 100, fptr); // The number of families in the simulation
  char *token31 = strtok(line, " ");
  numberOfFamilies = atoi(token31);
  fgets(line, 100, fptr); // max starvation level before the family dies
  char *token32 = strtok(line, " ");
  max_star_lvl = atoi(token32);
  // printf("number of families= %d\n",numberOfFamilies);
  // printf("container weight= %d\n",weight);
  // printf("max distance= %d\n",maxDistance);

  // Close the file
  fclose(fptr);

  alarm(EndUp);
  // finished reading userDefine.txt

  // if ( argc != 3 ) { //sleep duration will be random for planes and committees, but for now leave it as is
  // fprintf(stderr, "%s producer_time consumer_time\n", argv[0]);
  // exit(-1);
  //}

  /*
   * Create, attach and initialize the memory segment
   */
  if ((shmid = shmget((int)pid, sizeof(memory), // if needed the key could be changed from the parent's pid
                      IPC_CREAT | 0666)) != -1)
  {

    if ((shmptr = (char *)shmat(shmid, 0, 0)) == (char *)-1)
    {
      perror("shmptr -- parent -- attach");
      exit(1);
    }
    memcpy(shmptr, (char *)&memory, sizeof(memory));
  }
  else
  {
    perror("shmid -- parent -- creation");
    exit(2);
  }

  /*
   * Create and initialize the semaphores
   */
  if ((semid = semget((int)pid, 2, // if needed the key could be changed from the parent's pid
                      IPC_CREAT | 0660)) != -1)
  {
    arg.array = start_val;

    if (semctl(semid, 0, SETALL, arg) == -1)
    {
      perror("semctl -- parent -- initialization");
      exit(3);
    }
  }
  else
  {
    perror("semget -- parent -- creation");
    exit(4);
  }
  /*
   *ٍ SHARED MEMORY FOR OPENGL
   */

  // Define the key for the OpenGL shared memory segment
  key_t gl_shm_key = ftok(".", 'q');

  // Create the OpenGL shared memory segment
  if ((gl_shmid = shmget(gl_shm_key, sizeof(struct sharedOpenGL), IPC_CREAT | 0660)) != -1)
  {
    // Attach the shared memory segment
    if ((gl_shmptr = (struct sharedOpenGL *)shmat(gl_shmid, 0, 0)) == (struct sharedOpenGL *)-1)
    {
      perror("gl_shmptr -- parent -- attach");
      exit(5);
    }

    // Initialize the shared memory data
    gl_shmptr->numPlanes = cargoPlanes;
    gl_shmptr->numContainers = wheatContainers1;
    gl_shmptr->committeeNum = commitee;
    gl_shmptr->reliefNum = CollectWork;
    gl_shmptr->FamiliesNum = numberOfFamilies;
  }
  else
  {
    perror("gl_shmid -- parent -- creation");
    exit(6);
  }
  /*
   * Creat fifo safe hous
   */
  // char *fifoName = "safeHouse";
  if (mkfifo(fifoName, 0666) < 0)
  {
    printf("error in making safeHouse\n");
    perror(fifoName);
    exit(1);
  }

  /*
   *creat the message queue between families and workers
   */
  // create the key
  key_t key;
  if ((key = ftok(".", QSEED)) == -1)
  {
    perror("Parent: key generation");
    printf("Error number: %d\n", errno);
    exit(1);
  }
  // creat the message queue
  // int msqid;
  if ((msqid = msgget(key, IPC_CREAT | 0660)) == -1)
  {
    perror("Parent: make message queue");
    printf("Error number: %d\n", errno);
    exit(2);
  }
  else
  {
    printf("created message queue\n");
  }

  /*
   * Create and initialize the semaphores for families starvation
   */
  key_t fkey;
  if ((fkey = ftok(".", 'f')) == -1)
  {
    perror("Parent: key generation for families semaphore");
    printf("Error number: %d\n", errno);
    exit(1);
  }
  if ((sec_semid = semget(fkey, numberOfFamilies,
                          IPC_CREAT | 0660)) != -1)
  {
    // initilize the semaphore with zeros
    for (int i = 0; i < numberOfFamilies; i++)
    {
      if (semctl(sec_semid, i, SETVAL, 0) == -1)
      {
        perror("semctl (family)-- parent -- initialization");
        exit(3);
      }
    }
  }
  else
  {
    perror("semget -- parent -- creation");
    exit(4);
  }

  /*************************************************************************/
  // for children proccesses
  ChildNum = cargoPlanes + commitee + reliefWorkers + numberOfFamilies; // planes + committees + workers + a family
  // printf("workers = %d\tfamily = %d\n",reliefWorkers,numberOfFamilies);
  printf("children number = %d\n", ChildNum);
  int pids[ChildNum];
  pids_pointer = pids;
  /*
   * Fork the producer(plane) process
   */

  for (int i = 0; i < cargoPlanes; i++)
  {
    if ((p_id = fork()) == -1)
    {
      perror("fork -- producer");
      exit(5);
    }
    else if (p_id == 0)
    {
      char Tsleep1_str[32], distance_str[32], Tsleep2_str[32], weight_str[32], distanceMax_str[32];
      char wheatContainers1_str[32], wheatContainers2_str[32], refillPeriod2_str[32], refillPeriod1_str[32];
      sprintf(Tsleep1_str, "%d", Tsleep1);
      sprintf(distance_str, "%d", distance);
      sprintf(Tsleep2_str, "%d", Tsleep2);
      sprintf(wheatContainers1_str, "%d", wheatContainers1);
      sprintf(wheatContainers2_str, "%d", wheatContainers2);
      sprintf(refillPeriod2_str, "%d", refillPeriod2);
      sprintf(refillPeriod1_str, "%d", refillPeriod1);
      sprintf(weight_str, "%d", weight);
      sprintf(distanceMax_str, "%d", maxDistance);

      execl("./plane", "./plane", Tsleep2_str, distance_str, Tsleep1_str, wheatContainers1_str, wheatContainers2_str, refillPeriod2_str, refillPeriod1_str, weight_str, distanceMax_str, (char *)0);
      // execl only returns if there's an error
      perror("execl -- plane");
      exit(6);
    }
    else
    {
      pids[i] = p_id;
      gl_shmptr->planeID[i] = pids[i];
    }
  }
  pid_t pid_openGl = fork();
  char gl_shm_key_str[32]; // Assuming the key can be represented in 32 characters
  snprintf(gl_shm_key_str, sizeof(gl_shm_key_str), "%d", gl_shm_key);
  if (pid_openGl < 0)
  {
    // Error occurred during fork
    perror("Fork failed");
    exit(EXIT_FAILURE);
  }
  else if (pid_openGl == 0)
  {
    // Child process
    printf("HIIIIII\n");
    // Convert integers to strings
    execl(
        "./OpenGL",
        "./OpenGL",
        (char *)NULL);

    // If execl fails
    perror("OPENGL: Execl failed");

    exit(EXIT_FAILURE);
  }
  else
  {
    // Parent process
    printf("  process: PID=%d, Child PID=%d\n", getpid(), pid_openGl);
  }
  sleep(1);
  /*
   * Fork the consumer process
   */
  for (int i = 0; i < commitee; i++)
  {
    if ((c_id = fork()) == -1)
    {
      perror("fork -- consumer");
      exit(7);
    }
    else if (c_id == 0)
    {
      char initial_energyStr[32], energy1_str[32], energy2_str[32], CollectWork_str[32];
      sprintf(initial_energyStr, "%d", Initialenergy);
      sprintf(energy1_str, "%d", energy1);
      sprintf(energy2_str, "%d", energy2);
      sprintf(CollectWork_str, "%d", CollectWork);
      execl("./committee", "./committee", initial_energyStr, energy1_str, energy2_str, CollectWork_str, (char *)0);
      perror("execl -- committee");
      exit(8);
    }
    else
    {
      pids[i + cargoPlanes] = c_id;
      gl_shmptr->committeIDs[i] = c_id;

      printf("this is committe number %d\n", i);
    }
  }

  /*
   *  fork the distribution workers
   */

  int w_id;
  // reliefWorkers
  for (int i = 0; i < reliefWorkers; i++)
  {
    if ((w_id = fork()) == -1)
    {
      perror("fork -- worker");
      exit(5);
    }
    else if (w_id == 0)
    {
      char numfamilies[32], holdbags[32], initial_energyStr[32], energy1_str[32], energy2_str[32];
      sprintf(numfamilies, "%d", numberOfFamilies);
      sprintf(holdbags, "%d", No_bages);
      sprintf(initial_energyStr, "%d", Initialenergy);
      sprintf(energy1_str, "%d", energy1);
      sprintf(energy2_str, "%d", energy2);
      execl("./disWorker", "./disWorker", numfamilies, holdbags, initial_energyStr, energy1_str, energy2_str, (char *)0);
      perror("execl -- worker");
      exit(6);
    }
    pids[cargoPlanes + commitee + i] = w_id;
    gl_shmptr->workerIDs[i] = w_id;
  }
  /*
   *  fork the families
   */
  int f_id;
  // numberOfFamilies
  for (int i = 0; i < numberOfFamilies; i++)
  {
    if ((f_id = fork()) == -1)
    {
      perror("fork -- family");
      exit(5);
    }
    else if (f_id == 0)
    {
      char familyNum[2], max_star_lvl_str[32];
      sprintf(familyNum, "%d", i);
      sprintf(max_star_lvl_str, "%d", max_star_lvl);
      execl("./family", "./family", &familyNum, max_star_lvl_str, (char *)0);
      perror("execl -- family");
      exit(6);
    }
    pids[cargoPlanes + commitee + reliefWorkers + i] = f_id;
    gl_shmptr->familiesIDs[i] = f_id;
  }

  while (1)
  {
    printf("Waiting for signal\n");
    // sigpause(SIGUSR2);		/* Wait for USR1 signal */
    pause();
  }

  exit(0);
}

// signals catchers
void alarm_catcher(int the_sig)
{
  printf(">>>>>>>>>>>>>catched alarm signal<<<<<<<<<<<<<<<<\n");
  cleanup();
  exit(0);
}
void disCon_catcher(int the_sig)
{
  sighold(SIGUSR1);
  sighold(SIGALRM);

  printf("catch SIGUSR1 signal\n");
  destroyed_containers++;
  if (destroyed_containers >= W_blow)
  {
    gl_shmptr->terminatedFlag = 1;
    printf(">>>>>>>>>>>>>>>>>>>>>too many containers were distroyed<<<<<<<<<<<<<<<<<<<<<<\n");
    cleanup();
    exit(0);
  }
  printf("finished handling SIGUSR1\n");

  sigrelse(SIGUSR1);
  sigrelse(SIGALRM);
  return;
}

// functions
void cleanup() // called whenever the simulation ends to clean ups memory/processes before ending the program
{
  for (int i = 0; i < ChildNum; i++)
  {
    printf("kill process %d with pid %d\n", i, *(pids_pointer + i));
    kill(*(pids_pointer + i), SIGKILL);
  }

  shmdt(shmptr);
  shmctl(shmid, IPC_RMID, (struct shmid_ds *)0); // remove
  shmdt(gl_shmptr);
  shmctl(gl_shmid, IPC_RMID, (struct shmid_ds *)0); /* remove */

  semctl(semid, 0, IPC_RMID, 0);
  printf("parent removed shared memory and semaphore\n");

  // remove fifo
  if (unlink(fifoName) == 0)
  {
    printf("fifo: safeHouse has been removed\n");
  }

  // remove message queue
  msgctl(msqid, IPC_RMID, (struct msqid_ds *)0);
  printf("parent removed message queue\n");

  // remove family semaphore
  semctl(sec_semid, 0, IPC_RMID, 0);
  printf("parent removed family semaphore\n");

  return;
}

void collWorker_catcher(int)
{
  sighold(SIGINT);
  sighold(SIGUSR2);
  printf("aaaaaaaaaaaaaaaaaaaaaaa collecting worker died\n");
  collWorker_died++;

  if (collWorker_died >= MaxC_martyred)
  {
    gl_shmptr->terminatedFlag = 1;

    printf(">>>>>>>>>>>>>>>>>>>>>too many collecting workers died<<<<<<<<<<<<<<<<<<<<<<\n");
    cleanup();
    exit(0);
  }
  splittingWork--;

  if (splittingWork < commitee)
  {
    gl_shmptr->terminatedFlag = 1;

    printf(">>>>>>>>>>>>>>>>>>>>>too little splitting workers to continue<<<<<<<<<<<<<<<<<<<<<<\n");
    cleanup();
    exit(0);
  }
  sigrelse(SIGINT);
  sigrelse(SIGUSR2);
}
void famDied_catcher(int)
{
  printf("parent know a family died#################\n");
  dead_families++;

  if (dead_families >= MaxF_passed)
  {
    gl_shmptr->terminatedFlag = 1;

    printf(">>>>>>>>>>>>>>>>>>>>>too lmany families died from starvation<<<<<<<<<<<<<<<<<<<<<<\n");
    cleanup();
    exit(0);
  }
}
void disWorker_catcher(int)
{
  sighold(SIGINT);
  sighold(SIGUSR2);
  printf("!!!!!!!!!!!!! distribution worker died!!!!!!!\n");
  disWorker_died++;

  if (disWorker_died >= MaxD_martyred)
  {
    gl_shmptr->terminatedFlag = 1;
    printf(">>>>>>>>>>>>>>>>>>>>>too many distribution workers died<<<<<<<<<<<<<<<<<<<<<<\n");
    cleanup();
    exit(0);
  }
  splittingWork--;

  if (splittingWork < commitee)
  {
    gl_shmptr->terminatedFlag = 1;
    printf(">>>>>>>>>>>>>>>>>>>>>too little splitting workers to continue<<<<<<<<<<<<<<<<<<<<<<\n");
    cleanup();
    exit(0);
  }
  sigrelse(SIGINT);
  sigrelse(SIGUSR2);
}
