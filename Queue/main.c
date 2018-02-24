#include "Queue.h"
#include <pthread.h>
#include <signal.h>
#include <string.h>

typedef void (*function)(void);


// Two glboal variables to store address of front and rear nodes.
struct QueueNode* front = NULL;
struct QueueNode* rear = NULL;

pthread_mutex_t mutex_queue;

void initialize_raft_protocol()
{

}

//handles inner calls (multicast from other servers)
void *inner_raft_handler(void *p)
{

}


//handles the queue messages
void *queue_handler(void *p)
{

    while(1)
    {
        if( pthread_mutex_trylock(&mutex_queue) == 0) //acquired mutex
        {


            pthread_mutex_unlock(&mutex_queue);
        }

    }

}



int main()
{
    /* Drive code to test the implementation. */
    // Printing elements in Queue after each Enqueue or Dequeue
    Enqueue("ido"); Print();
    Enqueue("schwartz"); Print();
    Enqueue("is"); Print();
    Dequeue();  Print();
    Enqueue("awesome"); Print();

    /*
    int i;
    pthread_t arrThread[2];
    function arr_func[2] = {inner_raft_handler, queue_handler};
    int ret[2] = {0,0};
    struct sigaction action;
    sigset_t emptymask;

    /* Set up signal handling for SIGINT

    sigemptyset(&emptymask);
    memset(&action, 0 , sizeof(action));

    action.sa_mask = emptymask;
    action.sa_flags = SA_SIGINFO;
    action.sa_handler = signal_timeout_handler;
    sigaction(SIGINT, &action, NULL); //for ctrl +C ------------------> change signal!


    Create the producer and consumer creators thread

    pthread_create(&arrThread[0], NULL, inner_raft_handler, NULL);
    pthread_create(&arrThread[1], NULL, queue_handler, NULL);
    for(i=0; i<2; i++)
    {
        pthread_join(arrThread[i],&ret[i]);
    }

    return 0;
*/
}
