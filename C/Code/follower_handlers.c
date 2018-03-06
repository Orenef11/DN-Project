#include "../Headers/follower_handlers.h"


void follower_hb_set_log_handler(Queue_node_data* node)
{

#if DEBUG_MODE == 1
	WRITE_TO_LOGGER(DEBUG_LEVEL,"follower get set log msg",CHARS_VALUES,3,
			LOG(node->msg_data.set_log_hb_msg.cmd),LOG(node->msg_data.set_log_hb_msg.key),
			LOG(node->msg_data.set_log_hb_msg.value));
	WRITE_TO_LOGGER(DEBUG_LEVEL,"follower get set log msg",INT_VALUES,4,
			LOG(sharedRaftData.raft_state.term),LOG(sharedRaftData.raft_state.last_commit_index),
			LOG(node->msg_data.set_log_hb_msg.commit_id),LOG(node->term));

#endif
    sharedRaftData.raft_state.wakeup_counter = 0;

    //compare leaders term to my term
    if(node->term == sharedRaftData.raft_state.term)
    {
        //I don't need an updated
        if(sharedRaftData.raft_state.last_commit_index+1 == node->msg_data.set_log_hb_msg.commit_id)
        {
            //add the log to redis
            sharedRaftData.raft_state.last_log_index = node->msg_data.set_log_hb_msg.commit_id;
            sharedRaftData.python_functions.add_to_log_DB(node->msg_data.set_log_hb_msg.commit_id,
                                                           node->msg_data.set_log_hb_msg.cmd,
                                                           node->msg_data.set_log_hb_msg.key,
                                                           node->msg_data.set_log_hb_msg.value);
            update_DB(DB_STATUS,LAST_APPLIED,node->msg_data.set_log_hb_msg.commit_id);


            //send set log res to leader
            create_new_queue_node_data(SET_LOG_RES, node);
#if DEBUG_MODE == 1
		WRITE_TO_LOGGER(DEBUG_LEVEL,"follower sending set log res msg",NO_VALUES,0);
#endif
            send_raft_message(node, CONST_QUEUE_MSG_SIZE + sizeof(node->msg_data.set_log_res_msg));//TBD - check returned value
        }

        //I need an update
        else if(sharedRaftData.raft_state.last_commit_index +1 < node->msg_data.set_log_hb_msg.commit_id)
        {
            //send an update request to the leader
            create_new_queue_node_data(SYNC_REQ, node);
#if DEBUG_MODE == 1
		WRITE_TO_LOGGER(DEBUG_LEVEL,"follower sending sync request msg",NO_VALUES,0);
#endif
            send_raft_message(node,CONST_QUEUE_MSG_SIZE + sizeof(node->msg_data.sync_req_msg));//TBD - check returned value
        }
        //fatal error
        else{
			WRITE_TO_LOGGER(INFO_LEVEL,"follower had much bigger commit id than the leader",
						INT_VALUES,2,LOG(node->msg_data.set_log_hb_msg.commit_id),LOG(sharedRaftData.raft_state.last_commit_index));
		}

    }

    else if(node->term > sharedRaftData.raft_state.term)
    {
        if(sharedRaftData.raft_state.last_commit_index+1 < node->msg_data.set_log_hb_msg.commit_id)
        {

            sharedRaftData.raft_state.term = node->term;
            update_DB(DB_STATUS,TERM,node->term);

            //send an update request to the leader
            create_new_queue_node_data(SYNC_REQ, node);
#if DEBUG_MODE == 1
		WRITE_TO_LOGGER(DEBUG_LEVEL,"follower sending sync request msg",NO_VALUES,0);
#endif
            send_raft_message(node,CONST_QUEUE_MSG_SIZE + sizeof(node->msg_data.sync_req_msg));//check returned value

        }
    }

}



void follower_commit_ok_handler(Queue_node_data* node)
{
#if DEBUG_MODE == 1
		WRITE_TO_LOGGER(DEBUG_LEVEL,"follower get commit ok msg",INT_VALUES,4,
			LOG(sharedRaftData.raft_state.term),LOG(sharedRaftData.raft_state.last_log_index),
			LOG(node->msg_data.commit_ok_msg.last_log_index),LOG(node->term));

#endif
    sharedRaftData.raft_state.wakeup_counter = 0;

    if(node->term == sharedRaftData.raft_state.term)
    {
        sharedRaftData.raft_state.last_commit_index++ ;
        update_DB(DB_STATUS,COMMIT_INDEX,node->term);
        sharedRaftData.python_functions.execute_log(sharedRaftData.raft_state.last_log_index);
    }

}



void follower_sync_res_handler(Queue_node_data* node)
{
#if DEBUG_MODE == 1
	WRITE_TO_LOGGER(DEBUG_LEVEL,"follower get sync response msg",CHARS_VALUES,3,
			LOG(node->msg_data.sync_res_msg.cmd),LOG(node->msg_data.sync_res_msg.key),
			LOG(node->msg_data.sync_res_msg.value));
		WRITE_TO_LOGGER(DEBUG_LEVEL,"follower get sync response msg",INT_VALUES,6,
			LOG(sharedRaftData.raft_state.term),LOG(sharedRaftData.raft_state.last_log_index),
			LOG(sharedRaftData.raft_state.last_commit_index),LOG(node->msg_data.sync_res_msg.commit_id),
			LOG(node->term),LOG(node->message_sent_by));

#endif
    sharedRaftData.raft_state.wakeup_counter = 0;

    if (sharedRaftData.raft_state.server_id == node->message_sent_to)
    {
        if(node->msg_data.sync_res_msg.commit_id == sharedRaftData.raft_state.last_commit_index+1)
        {
            //accept the sunc - add the log to redis and execute the log
            sharedRaftData.raft_state.last_log_index++ ;
            sharedRaftData.python_functions.add_to_log_DB(node->msg_data.set_log_hb_msg.commit_id,
                                                          node->msg_data.set_log_hb_msg.cmd,
                                                          node->msg_data.set_log_hb_msg.key,
                                                          node->msg_data.set_log_hb_msg.value);

            sharedRaftData.raft_state.last_commit_index++ ;
            update_DB(DB_STATUS,LAST_APPLIED,node->msg_data.set_log_hb_msg.commit_id);
            sharedRaftData.python_functions.execute_log(sharedRaftData.raft_state.last_log_index);

        }

    }

};




void follower_vote_req_handler(Queue_node_data* node)
{
#if DEBUG_MODE == 1
		WRITE_TO_LOGGER(DEBUG_LEVEL,"follower get vote request msg",INT_VALUES,4,
			LOG(sharedRaftData.raft_state.term),LOG(sharedRaftData.raft_state.did_I_vote),
			LOG(node->term),LOG(node->message_sent_by));

#endif
    sharedRaftData.raft_state.wakeup_counter = 0;

    if(!sharedRaftData.raft_state.did_I_vote)//I didnt vote
    {

        if(node->term > sharedRaftData.raft_state.term)
        {

            sharedRaftData.raft_state.term = node->term;
            sharedRaftData.raft_state.did_I_vote = 1;
            update_DB(DB_STATUS,TERM,sharedRaftData.raft_state.term);

            create_new_queue_node_data(VOTE, node);
#if DEBUG_MODE == 1
		WRITE_TO_LOGGER(DEBUG_LEVEL,"follower sending vote msg",NO_VALUES,0);
#endif
            send_raft_message(node,CONST_QUEUE_MSG_SIZE /*+ sizeof(node->msg_data.vote_msg)*/);//check returned value

        }
        // else ignore
    }
    // else ignore

}


void follower_hb_keep_alive_handler(Queue_node_data* node)
{
    sharedRaftData.raft_state.wakeup_counter = 0;

    if(node->term < sharedRaftData.raft_state.term)
    {
        return ;
    }

    else if(node->term > sharedRaftData.raft_state.term)
    {
        sharedRaftData.raft_state.term = node->term;
        update_DB(DB_STATUS,TERM,sharedRaftData.raft_state.term);
        sharedRaftData.raft_state.did_I_vote = 0;

    }

    if(node->msg_data.keep_alive_hb_msg.last_log_id > sharedRaftData.raft_state.last_log_index)
    {
        sharedRaftData.raft_state.last_log_index = node->msg_data.keep_alive_hb_msg.last_log_id;
        create_new_queue_node_data(SYNC_REQ, node);
#if DEBUG_MODE == 1
		WRITE_TO_LOGGER(DEBUG_LEVEL,"follower sending sync request msg",NO_VALUES,0);
#endif
        send_raft_message(node,CONST_QUEUE_MSG_SIZE + sizeof(node->msg_data.sync_req_msg));//TBD - check returned value
    }
    //the leader cancle the commit proccess and this follower already get the update
	else if(node->msg_data.keep_alive_hb_msg.last_log_id < sharedRaftData.raft_state.last_commit_index){
		sharedRaftData.raft_state.last_commit_index = node->msg_data.keep_alive_hb_msg.last_log_id;
		sharedRaftData.python_functions.clear_log_from_log_id(sharedRaftData.raft_state.last_commit_index + 1);
		update_DB(DB_STATUS,LAST_APPLIED,sharedRaftData.raft_state.last_commit_index);
		update_DB(DB_STATUS,COMMIT_INDEX,sharedRaftData.raft_state.last_commit_index);
	}

}



void follower_time_out_handler(Queue_node_data * node)
{
    sharedRaftData.raft_state.wakeup_counter++;

    if(sharedRaftData.raft_state.wakeup_counter == 2 )
    {
        clear_queue();
        sharedRaftData.raft_state.current_state = CANDIDATE;
        sharedRaftData.raft_state.wakeup_counter = 0;
        sharedRaftData.raft_state.vote_counter = 1;
        sharedRaftData.raft_state.did_I_vote = 1;

        sharedRaftData.raft_state.term++;
        update_DB(DB_STATUS, TERM, sharedRaftData.raft_state.term);
        update_DB(DB_STATUS, STATUS, CANDIDATE_VALUE);
/*
        #if DEBUG_MODE == 1
			if(sharedRaftData.raft_state.members_amount == 1)
			{
				sharedRaftData.raft_state.current_state = LEADER;
				sharedRaftData.raft_state.wakeup_counter = 0;
				sharedRaftData.raft_state.vote_counter = 0;
				sharedRaftData.raft_state.did_I_vote = 0;
				
	            //change timeout for leader
				sharedRaftData.raft_state.timeout = sharedRaftData.raft_configuration.leader_timeout;
				create_timeout_event(sharedRaftData.raft_state.timeout);

				//update DAL
				sharedRaftData.raft_state.current_state = LEADER;
				update_DB(DB_STATUS,STATUS,LEADER_VALUE);
				update_DB(DB_STATUS, LEADER_ID, sharedRaftData.raft_state.server_id);

				create_new_queue_node_data(KEEP_ALIVE_HB,node);
			}
		#endif
*/
        create_new_queue_node_data(REQUEST_FOR_VOTE, node);
#if DEBUG_MODE == 1
        WRITE_TO_LOGGER(DEBUG_LEVEL, "follower became candidate and is sending request for vote msg", NO_VALUES, 0);
#endif
        send_raft_message(node, CONST_QUEUE_MSG_SIZE /*+ sizeof(node->msg_data.req_for_vote_msg)*/);


        //return CANDIDATE;
    }

}


