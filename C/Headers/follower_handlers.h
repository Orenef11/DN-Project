
#ifndef RAFT_PROJECT_FOLLOWER_HANDLERS_H
#define RAFT_PROJECT_FOLLOWER_HANDLERS_H
#include "handlers_utils.h"
//#include "logger.h"
#include "static_queue.h"
#include "raft_server.h"
void follower_hb_set_log_handler(Queue_node_data* node);

void follower_commit_ok_handler(Queue_node_data* node);

void follower_sync_res_handler(Queue_node_data* node);

void follower_vote_req_handler(Queue_node_data* node);

void follower_hb_keep_alive_handler(Queue_node_data* node);

void follower_time_out_handler(Queue_node_data* node);


#endif //RAFT_PROJECT_FOLLOWER_HANDLERS_H
