#ifndef RAFT_PROJECT_UTILS_HANDLERS_
#define RAFT_PROJECT_UTILS_HANDLERS_
#include <unistd.h>

#include "general.h"
#include "static_queue.h"
#define FOLLOWER_VALUE -3
#define CANDIDATE_VALUE -2
#define LEADER_VALUE -1

#define STATE_NAME_SHIFT 3

#define MIN_RAFT_STATE_VALUE 0

int update_DB(char * DB_flag,char * key, int value);
int calculate_raft_rand_timeout();
void create_timeout_event(int timeout);
int is_relevant_message(Queue_node_data * node_message);
//main.c
void time_out_hendler(int sig);
#endif 
