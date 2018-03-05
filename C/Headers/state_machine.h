#ifndef RAFT_PROJECT_STATE_MACHINE_H
#define RAFT_PROJECT_STATE_MACHINE_H

#include <stdio.h>
#include <malloc.h>

#include "candidate_handlers.h"
#include "follower_handlers.h"
#include "leader_handlers.h"
//#include "general.h"

#define STATE_MAX_SIZE 3
#define EVENT_MAX_SIZE 9

//in debug mode we want segmention fault
#if DEBUG_MODE == 1
	#define NO_FUNCTION NULL
#else
	#define NO_FUNCTION	do_nothing
#endif

void operate_machine_state(Queue_node_data * node);
void do_nothing(Queue_node_data * node);




void init_state_functions_handler();


void (*handlers_functions_arr[STATE_MAX_SIZE][EVENT_MAX_SIZE])(Queue_node_data *);//handlers for each state

#endif //RAFT_PROJECT_STATE_MACHINE_H
