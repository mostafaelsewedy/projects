#include <stdio.h>
#include <pthread.h>

#define NumberOfThreads 4
#define RoundTimeSlice 1

void *threadFunction(void *tid){
    long threadID = pthread_self();
    printf("My Thread ID is %li \n", threadID );
    for(int i=0;i<2;i++){
        for(int j=0; j<NumberOfThreads; j++){
            if(j== (int) tid)
                printf("Thread with ID: %li is running \n",threadID );
            else
                printf("Thread with ID: %li is waiting \n",threadID );
        }
    }
    printf("Now Exiting thread with ID %li \n", threadID);
    pthread_exit(NULL);
}

int main(){

    pthread_t myThreads[NumberOfThreads];
    pthread_attr_t attribute;
    pthread_attr_init(&attribute); //this is to intialize the thread attribute object 

    pthread_attr_setschedpolicy(&attribute,SCHED_RR); //we chose round robin but there is FIFO and many others

    printf("Scheduling Policy is set to Round Robin \n");

    for(int i=0; i<NumberOfThreads ; i++)
        pthread_create(&myThreads[i],&attribute,&threadFunction,(void *)i);

    for(int i=0 ; i<NumberOfThreads ; i++)
        pthread_join(myThreads[i],NULL); //to wait for all threads to be done before cleaning up

    pthread_attr_destroy(&attribute); //to clean up after usage (same as free function)

    return 0;
}