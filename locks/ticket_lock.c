#define _GNU_SOURCE

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>

int counter = 0;
int NUM_PROCESSOR = 8;
int COUNT_TO = 2;
bool verbose = false;

char* HELP_MSG = "-h for help\n-n number of threads to generate\n-c number of each thread to count to\n";

////// test-and-set lock ///////
typedef struct ticket_lock {
    volatile int next_ticket;
    volatile int now_serving;
} ticket_lock_t;

void ticket_lock_init(ticket_lock_t* l){
    l->next_ticket = 0;
    l->now_serving = 0;
}

void ticket_LOCK(ticket_lock_t* l){
    int my_ticket = __sync_fetch_and_add(&l->next_ticket, 1);
    while(my_ticket != l->now_serving);
}

void ticket_UNLOCK(ticket_lock_t* l){
    l->now_serving++;
}


typedef struct thread_arg {
    int id;
    ticket_lock_t* lock;
} thread_arg_t;


void* thread_fn(void* vargp){
    thread_arg_t* arg =(thread_arg_t*)vargp;
    int id = arg->id;
    ticket_lock_t* lock = arg->lock;

    cpu_set_t cpuset;
    pthread_t thread;
    thread = pthread_self();
    CPU_ZERO(&cpuset);
    CPU_SET(id%8, &cpuset);
    int s = pthread_setaffinity_np(thread, sizeof(cpuset), &cpuset);
    if(s){
        fprintf(stderr, "thread set cpu affinity error\n");
        exit(1);
    }
    
    if(verbose)
        printf("threadID %d set to CPU %d\n", id, id);

    for(int i=0; i<COUNT_TO; i++){
        ticket_LOCK(lock);
        counter++;
        sleep(1);
        ticket_UNLOCK(lock);
    }

    return NULL;
}

int main(int argc, char* argv[]){

    extern char* optarg;
    extern int optind, opterr, optopt;
    char opt;

    while((opt = getopt(argc, argv, "vhn:c:")) != -1){
        switch(opt){
            case 'h':
                printf("%s", HELP_MSG);
                break;
            case 'v':
                verbose = true;
                break;
            case 'n':
                NUM_PROCESSOR = atoi(optarg);
                break;
            case 'c':
                COUNT_TO = atoi(optarg);
                break;
            default:
                printf("%s", HELP_MSG);
                exit(0);
        }
    }

    if(verbose)
        printf("NUM_PROCESSOR set to %d\nCOUNT_TO set to %d\n", NUM_PROCESSOR,
                                                                COUNT_TO);


    ticket_lock_t lock;
    ticket_lock_init(&lock);
    
    pthread_t tid[128];
    for(int i=0; i<NUM_PROCESSOR; i++){
        thread_arg_t* thread_arg = malloc(sizeof(thread_arg_t));
        thread_arg->id = i;
        thread_arg->lock = &lock;
        pthread_create(&tid[i], NULL, thread_fn, thread_arg);
    }

    for(int i=0; i<NUM_PROCESSOR; i++){
        pthread_join(tid[i], NULL);
    }

    printf("All threads finish\ncounter is %d\n", counter);

    return 0;
}
