#include "../Headers/follower_handlers.h"


void follower_hb_set_log_handler(Queue_node_data* node)
{

#if DEBUG_MODE == 1 || PRINT_SET_LOG_PROCESS == 1
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

#if DEBUG_MODE == 1 || PRINT_SET_LOG_PROCESS == 1
		WRITE_TO_LOGGER(DEBUG_LEVEL,"follower sending set log res msg",NO_VALUES,0);
#endif

            send_raft_message(node, CONST_QUEUE_MSG_SIZE + sizeof(node->msg_data.set_log_res_msg),MAX_RAFT_MESSAGE);
        }

        //I need an update
        else if(sharedRaftData.raft_state.last_commit_index +1 < node->msg_data.set_log_hb_msg.commit_id)
        {
            //send an update request to the leader
            create_new_queue_node_data(SYNC_REQ, node);

#if DEBUG_MODE == 1 || PRINT_SET_LOG_PROCESS == 1
		WRITE_TO_LOGGER(DEBUG_LEVEL,"follower sending sync request msg",NO_VALUES,0);
#endif

            send_raft_message(node,CONST_QUEUE_MSG_SIZE + sizeof(node->msg_data.sync_req_msg),MAX_RAFT_MESSAGE);
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
            send_raft_message(node,CONST_QUEUE_MSG_SIZE + sizeof(node->msg_data.sync_req_msg),MAX_RAFT_MESSAGE);//check returned value

        }
    }

}



void follower_commit_ok_handler(Queue_node_data* node)
{
#if DEBUG_MODE == 1
		WRITE_TO_LOGGER(DEBUG_LEVEL,"follower get commit ok msg",INT_VALUES,5,
			LOG(sharedRaftData.raft_state.term),LOG(sharedRaftData.raft_state.last_log_index),
			LOG(node->msg_data.commit_ok_msg.last_log_index),LOG(node->term),
			LOG(sharedRaftData.raft_state.last_commit_index));
#endif

    sharedRaftData.raft_state.wakeup_counter = 0;

    if(node->term == sharedRaftData.raft_state.term)
    {
        sharedRaftData.raft_state.last_commit_index++ ;
        update_DB(DB_STATUS,COMMIT_INDEX,sharedRaftData.raft_state.last_commit_index);
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
            //accept the sync - add the log to redis and execute the log
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

}




void follower_vote_req_handler(Queue_node_data* node)
{

#if DEBUG_MODE == 1
		WRITE_TO_LOGGER(DEBUG_LEVEL,"follower get vote request msg",INT_VALUES,4,
			LOG(sharedRaftData.raft_state.term),LOG(sharedRaftData.raft_state.did_I_vote),
			LOG(node->term),LOG(node->message_sent_by));
#endif

    sharedRaftData.raft_state.wakeup_counter = 0;

    if(!sharedRaftData.raft_state.did_I_vote)//I didn't vote
    {

        if(node->term >= sharedRaftData.raft_state.term)
        {

            sharedRaftData.raft_state.term = node->term;
            sharedRaftData.raft_state.did_I_vote = 1;
            update_DB(DB_STATUS,TERM,sharedRaftData.raft_state.term);

            create_new_queue_node_data(VOTE, node);

#if DEBUG_MODE == 1
		WRITE_TO_LOGGER(DEBUG_LEVEL,"follower sending vote msg",NO_VALUES,0);
#endif

            send_raft_message(node,CONST_QUEUE_MSG_SIZE,MAX_RAFT_MESSAGE);

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

    if(sharedRaftData.raft_state.leader_id != node->message_sent_by)
    {
        sharedRaftData.raft_state.leader_id = node->message_sent_by;
        update_DB(DB_STATUS, LEADER_ID, sharedRaftData.raft_state.leader_id);
    }

    if(node->msg_data.keep_alive_hb_msg.last_log_id > sharedRaftData.raft_state.last_log_index)
    {
        sharedRaftData.raft_state.last_log_index = node->msg_data.keep_alive_hb_msg.last_log_id;
        create_new_queue_node_data(SYNC_REQ, node);

#if DEBUG_MODE == 1
		WRITE_TO_LOGGER(DEBUG_LEVEL,"follower sending sync request msg",NO_VALUES,0);
#endif

        send_raft_message(node,CONST_QUEUE_MSG_SIZE + sizeof(node->msg_data.sync_req_msg),MAX_RAFT_MESSAGE);
    }
    //the leader cancel the commit process and this follower already get the update
	else if(node->msg_data.keep_alive_hb_msg.last_log_id < sharedRaftData.raft_state.last_commit_index)
    {
		sharedRaftData.raft_state.last_commit_index = node->msg_data.keep_alive_hb_msg.last_log_id;
		sharedRaftData.python_functions.clear_log_from_log_id(sharedRaftData.raft_state.last_commit_index + 1);
		update_DB(DB_STATUS,LAST_APPLIED,sharedRaftData.raft_state.last_commit_index);
		update_DB(DB_STATUS,COMMIT_INDEX,sharedRaftData.raft_state.last_commit_index);
	}
	//reset the timer
	create_alarm_timer(sharedRaftData.raft_state.timeout);
}



void follower_time_out_handler(Queue_node_data * node)
{
    sharedRaftData.raft_state.wakeup_counter++;

    if(sharedRaftData.raft_state.wakeup_counter == 2 )
    {
        sharedRaftData.raft_state.current_state = CANDIDATE;

        sharedRaftData.raft_state.wakeup_counter = 0;
        sharedRaftData.raft_state.vote_counter = 1;
        sharedRaftData.raft_state.did_I_vote = 1;

        sharedRaftData.raft_state.term++;
        update_DB(DB_STATUS, TERM, sharedRaftData.raft_state.term);
        update_DB(DB_STATUS, STATUS, CANDIDATE_VALUE);

        sharedRaftData.raft_state.leader_id = 0;
        update_DB(DB_STATUS, LEADER_ID, sharedRaftData.raft_state.leader_id);

        create_new_queue_node_data(REQUEST_FOR_VOTE, node);

#if DEBUG_MODE == 1
        WRITE_TO_LOGGER(DEBUG_LEVEL, "follower became candidate and is sending request for vote msg", NO_VALUES, 0);
#endif

        send_raft_message(node, CONST_QUEUE_MSG_SIZE,MAX_RAFT_MESSAGE);
        clear_queue();
    }

}


