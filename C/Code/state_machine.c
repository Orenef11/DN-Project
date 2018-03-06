#include "../Headers/state_machine.h"

const char* statesName[] = {"FOLLOWER", "CANDIDATE", "LEADER"};

void operate_machine_state(Queue_node_data * node){
#if DEBUG_MODE == 1
	WRITE_TO_LOGGER(DEBUG_LEVEL,"enter state function",INT_VALUES,2,
			LOG(node->event),LOG(sharedRaftData.raft_state.current_state));
#endif
	puts("got event");
    handlers_functions_arr[sharedRaftData.raft_state.current_state][node->event](node);
    puts("handle event");
}

void do_nothing(Queue_node_data * node){
	WRITE_TO_LOGGER(FATAL_LEVEL,"enter with undefinned event",INT_VALUES,0,
			LOG(node->event),LOG(sharedRaftData.raft_state.current_state));
}

void init_state_functions_handler()
{
#if DEBUG_MODE == 1
	WRITE_TO_LOGGER(DEBUG_LEVEL,"start",NO_VALUES,0);
#endif

    /*FOLLOWER{1,1,1,0,0,1,1,0,1,0},
    /*CANDIDATE{1,1,0,0,0,0,0,1,1,0},
    /*LEADER{1,1,0,1,1,0,0,1,1,1}*/
    //init follower functions
    handlers_functions_arr[FOLLOWER][TIMEOUT] = follower_time_out_handler;
    handlers_functions_arr[FOLLOWER][KEEP_ALIVE_HB] = follower_hb_keep_alive_handler;
    handlers_functions_arr[FOLLOWER][SET_LOG_HB] = follower_hb_set_log_handler;
    handlers_functions_arr[FOLLOWER][SET_LOG_RES] = NO_FUNCTION;
    handlers_functions_arr[FOLLOWER][SYNC_REQ] = NO_FUNCTION;
    handlers_functions_arr[FOLLOWER][SYNC_RES] = follower_sync_res_handler;
    handlers_functions_arr[FOLLOWER][COMMIT_OK] = follower_commit_ok_handler;
    handlers_functions_arr[FOLLOWER][VOTE] = NO_FUNCTION;
    handlers_functions_arr[FOLLOWER][REQUEST_FOR_VOTE] = follower_vote_req_handler;
    handlers_functions_arr[FOLLOWER][PY_SEND_LOG] = NO_FUNCTION;

    handlers_functions_arr[CANDIDATE][TIMEOUT] = candidate_time_out_handler;
    handlers_functions_arr[CANDIDATE][KEEP_ALIVE_HB] = candidate_keep_alive_hb_handler;
    handlers_functions_arr[CANDIDATE][SET_LOG_HB] = NO_FUNCTION;
    handlers_functions_arr[CANDIDATE][SET_LOG_RES] = NO_FUNCTION;
    handlers_functions_arr[CANDIDATE][SYNC_REQ] = NO_FUNCTION;
    handlers_functions_arr[CANDIDATE][SYNC_RES] = NO_FUNCTION;
    handlers_functions_arr[CANDIDATE][COMMIT_OK] = NO_FUNCTION;
    handlers_functions_arr[CANDIDATE][VOTE] = candidate_vote_for_me_handler;
    handlers_functions_arr[CANDIDATE][REQUEST_FOR_VOTE] = candidate_vote_req_handler;
    handlers_functions_arr[CANDIDATE][PY_SEND_LOG] = NO_FUNCTION;

    handlers_functions_arr[LEADER][TIMEOUT] = leader_time_out_handler;
    handlers_functions_arr[LEADER][KEEP_ALIVE_HB] = leader_hb_handler;
    handlers_functions_arr[LEADER][SET_LOG_HB] = NO_FUNCTION;
    handlers_functions_arr[LEADER][SET_LOG_RES] = leader_log_res_handler;
    handlers_functions_arr[LEADER][SYNC_REQ] = leader_sync_req_handler;
    handlers_functions_arr[LEADER][SYNC_RES] = NO_FUNCTION;
    handlers_functions_arr[LEADER][COMMIT_OK] = NO_FUNCTION;
    handlers_functions_arr[LEADER][VOTE] = leader_vote_handler;
    handlers_functions_arr[LEADER][REQUEST_FOR_VOTE] = leader_vote_req_handler;
    handlers_functions_arr[LEADER][PY_SEND_LOG] = leader_send_log_hb_handler;
#if DEBUG_MODE == 1
	WRITE_TO_LOGGER(DEBUG_LEVEL,"end",NO_VALUES,0);
#endif
}

