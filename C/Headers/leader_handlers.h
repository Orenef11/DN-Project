#ifndef RAFT_PROJECT_LEADER_HANDLERS_H
#define RAFT_PROJECT_LEADER_HANDLERS_H
#include "handlers_utils.h"
//#include "logger.h"
#include "static_queue.h"
#include "raft_server.h"

#define TIME_TO_CANCLE_COMMIT_PROC 10

void leader_sync_req_handler(Queue_node_data* node);

void leader_vote_handler(Queue_node_data* node);

void leader_time_out_handler(Queue_node_data* node);

void leader_hb_handler(Queue_node_data* node);

void leader_send_log_hb_handler(Queue_node_data* node);

void leader_log_res_handler(Queue_node_data* node);

void leader_vote_req_handler(Queue_node_data* node);

#endif //RAFT_PROJECT_LEADER_HANDLERS_H
