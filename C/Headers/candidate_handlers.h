
#ifndef RAFT_PROJECT_CANDIDATE_HANDLERS_H
#define RAFT_PROJECT_CANDIDATE_HANDLERS_H

#include "handlers_utils.h"
#include "static_queue.h"
#include "raft_server.h"

void candidate_vote_for_me_handler(Queue_node_data * node);

void candidate_keep_alive_hb_handler(Queue_node_data *node);

void candidate_time_out_handler(Queue_node_data* node);

void candidate_vote_req_handler(Queue_node_data* node);

#endif 
