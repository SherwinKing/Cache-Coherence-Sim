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
#define NUM_ELE 128

char* HELP_MSG = "-h for help\n-n number of threads to generate\n-c number of each thread to count to\n";

////// array lock ///////
typedef struct array_lock {
    volatile int status[NUM_ELE];
    volatile int head;
} array_lock_t;

void array_lock_init(array_lock_t* l){
    for(int i=0; i<NUM_ELE; i++)
        l->status[i] = 1;
    l->status[0] = 0;
    l->head = 0;
}

void array_LOCK(array_lock_t* l, int* my_element){
    *my_element = __sync_fetch_and_add(&l->head, 1) % NUM_ELE;
    while(l->status[*my_element]);
}

void array_UNLOCK(array_lock_t* l, int* my_element){
    l->status[*my_element] = 1;
    int nxt_element = ((*my_element)+1) % NUM_ELE;
    l->status[nxt_element] = 0;
}


typedef struct thread_arg {
    int id;
    array_lock_t* lock;
} thread_arg_t;


void* thread_fn(void* vargp){
    thread_arg_t* arg =(thread_arg_t*)vargp;
    int id = arg->id;
    array_lock_t* lock = arg->lock;

    cpu_set_t cpuset;
    pthread_t thread;
    thread = pthread_self();
    CPU_ZERO(&cpuset);
    CPU_SET(id, &cpuset);
    int s = pthread_setaffinity_np(thread, sizeof(cpuset), &cpuset);
    if(s){
        fprintf(stderr, "thread set cpu affinity error\n");
        exit(1);
    }
    
    if(verbose)
        printf("threadID %d set to CPU %d\n", id, id);

    for(int i=0; i<COUNT_TO; i++){
        int my_element;
        array_LOCK(lock, &my_element);
        counter++;
        sleep(1);
        array_UNLOCK(lock, &my_element);
    }

    return NULL;
}

int main(int argc, char* argv[]){

    /*
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

    */

    array_lock_t lock;
    array_lock_init(&lock);
    
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
