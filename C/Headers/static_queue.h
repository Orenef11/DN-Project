
#ifndef RAFT_PROJECT_STATIC_QUEUE_H
#define RAFT_PROJECT_STATIC_QUEUE_H

#include "general.h"

//inline int get_const_queue_msg_size();


int init_raft_queue();

void push_queue(Queue_node_data *data);

void pop_queue(Queue_node_data *ret);
void clear_queue();
void create_new_queue_node_data(eventType event, Queue_node_data* msg_data_memory);

#endif //RAFT_PROJECT_STATIC_QUEUE_H
