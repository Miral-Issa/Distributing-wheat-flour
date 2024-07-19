/*
 * The distribution worker
 */

#include "local.h"

int initialEnergy=0,saveInitial=0;
int energy1=0;
int energy2=0;
double prop_of_mry;

void handle_alarm(int signum) {
        srand(time(NULL));
        // Generate a random number between strt_of_dec and end_of_dec
        int to_dec = rand() % (energy2 - energy1 + 1) + energy1;
        initialEnergy -= to_dec;
        prop_of_mry += 0.06;
        
        if (initialEnergy <= 0) {
            printf("!!!!!!! Distributing Worker out energy%d!!!!\n",initialEnergy);
            //inform parent
            kill(getppid(),SIGINT);
            //replace worker with new one
            initialEnergy = saveInitial;
            prop_of_mry=0.1;    
        }
        //printf("Distributing Worker prob of mry: %lf\n",prop_of_mry); 
   

    // Set the alarm for the next 2 seconds
    alarm(5);
}

int sighold(int sig);
int sigrelse(int sig);
int main(int argc, char *argv[])
{
    char *fifoName = "safeHouse";
    int fifod;
    key_t  key,fkey;
    int msqid,semid;
    MESSAGE  msg;
    int numFamylies; //number of families in the simulation
    int bags_hold=0,allowed_bags;
    prop_of_mry= 0.1;
    
    if (argc != 6) {
        printf("worker need number of families and number of bags for each trip\n");
        exit(1);
    }
    numFamylies = atoi(argv[1]);
    allowed_bags = atoi(argv[2]);
    initialEnergy = atoi(argv[3]);
    saveInitial = initialEnergy;
    energy1 = atoi(argv[4]);
    energy2 = atoi(argv[5]);
    
    if ( signal(SIGALRM, handle_alarm) == SIG_ERR ) {
    perror("Sigset can not set SIGALRM");
    exit(SIGQUIT);
    }
    alarm(5); // Set the initial alarm for 2 seconds
    unsigned short famStar[numFamylies];
    
    //open safeHouse fifo
    if( (fifod = open(fifoName, O_RDONLY)) ==-1)
    {
	perror("disWorker open fifo");
	exit(1);
    }else
    {
	//printf("worker opened fifo\n");
    }
    
    //generate the message queue key
    if ((key = ftok(".", QSEED)) == -1) 
    {    
    	perror("Parent: key generation");
    	printf("Error number: %d\n", errno);
    	exit(1);
    }
    
    //open the message queue
    if( (msqid = msgget(key, 0660)) == -1)
    {
  	perror("disworker: open message queue");
    	printf("Error number: %d\n", errno);
    	exit(2);
    }else
    {
  	printf("worker opened message queue\n");
    }
  
   //generate the families semaphore key
   if ((fkey = ftok(".", 'f')) == -1) 
   {    
    	perror("family: key generation for semaphore");
    	printf("Error number: %d\n", errno);
    	exit(1);
   }
    //open the semaphore
    if ( (semid = semget(fkey, 0, 0)) == -1 ) {
    perror("semget -- family -- access");
    exit(3);
   }
   
   
    int weaht;
    int dataread;
    srand(getpid());
    while(( dataread=read(fifod,&weaht,sizeof(int)) ) >= 0)
    {
    	if(dataread == 0)
    	  continue;
    	  
    	//printf("worker took from safeHouse %d kg weaht\n",weaht);
    	bags_hold++;
    	
    	if(bags_hold >= allowed_bags)
    	{
    		sighold(SIGALRM);
    		if(semctl(semid,0,GETALL,famStar) == -1)
    		{
    		 	perror("disWorker: getting semaphore values");
    			exit(4);
    		}
    		sigrelse(SIGALRM);
    		//find most starved family
    		int max=0,index=0;
    		printf("&&&&&&&\n");
    		for(int i=0;i<numFamylies;i++)
    		{
    			printf("%d,",famStar[i]);
    			if(max<famStar[i])
    			{
    				max=famStar[i];
    				index=i;
    			}	
    		}
    		printf("\n&&&&&&&\n");
    		//send the bag of wheat to a family
    		msg.msg_type=index +1; //depend on what family the worker want to send the weaht
    		msg.msg_data=bags_hold;
    	
    		sighold(SIGALRM);
    		if(msgsnd(msqid, &msg, sizeof(msg), 0) == -1)
    		{
    			perror("worker: msgsend");
    	  		exit(3);
    		}else
    		{
    			//printf("message sent to family %ld\n",msg.msg_type);
    			bags_hold=0;
    			if (initialEnergy <= 0||prop_of_mry== 0.9) {
               			printf("Distributing Worker has to be changed!!!!!!!!!!!!!\n");  
               			//inform parent
            			kill(getppid(),SIGINT);
            			//replace worker with new one
            			initialEnergy = saveInitial;
            			prop_of_mry=0.1;   
            		}else if(initialEnergy<=5){
                		prop_of_mry=0.85;
                		initialEnergy -= 3; // Decrease energy by 3
                		printf("!!!!!!!!!!!Distributing Worker energy: %d\n", initialEnergy); // Print energy level
                		printf("!!!!!!!!!!!Distributing Worker probability of martyring: %2f\n", prop_of_mry); 
            		}
            		initialEnergy -= 3; // Decrease energy by 3
             		prop_of_mry+=0.02;
            		printf("!!!!!!!!!!Distributing Worker  energy: %d\n", initialEnergy); // Print energy level
            		printf("!!!!!!!!!!Distributing Worker probability of martyring: %2f\n",  prop_of_mry);
    		}
    		sigrelse(SIGALRM);
    		//sleep(1); 
    	}//end of if	
    }//end of while
    
    close(fifod);
    pause();
}
