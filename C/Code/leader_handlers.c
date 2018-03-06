
#include "../Headers/leader_handlers.h"

///leader:

//ido relevant fileds from Node_queue - byte term, last_log_id, sender_id [difficulte = 3 ]
void leader_sync_req_handler(Queue_node_data *node)
{
#if DEBUG_MODE == 1
	WRITE_TO_LOGGER(DEBUG_LEVEL,"leader get sync request msg",INT_VALUES,5,
			LOG(sharedRaftData.raft_state.last_commit_index),LOG(sharedRaftData.raft_state.last_log_index),
			LOG(node->msg_data.sync_req_msg.start_log_index),LOG(node->msg_data.sync_req_msg.last_log_id),
			LOG(node->message_sent_by));
#endif
    int start_diff_index = node->msg_data.sync_req_msg.start_log_index;
    int end_diff_index = node->msg_data.sync_req_msg.last_log_id;
    int current_index = start_diff_index;
    char * msg_pointer, *elem;
    char** diff = sharedRaftData.python_functions.get_log_by_diff(start_diff_index, end_diff_index);
    node->msg_data.sync_res_msg.commit_id = start_diff_index;
    node->message_sent_to = node->message_sent_by;
    node->message_sent_by = sharedRaftData.raft_state.server_id;
    msg_pointer = (char *)&node->msg_data.sync_res_msg.cmd;
    //python return null at the end of the list
    while(*diff){
        elem = strtok(*diff, PYTHON_DELIMITER);
#if DEBUG_MODE == 1
	WRITE_TO_LOGGER(DEBUG_LEVEL,"first elemnt - command",CHARS_VALUES,1,
			LOG(elem));
#endif
        while(elem){
            strcpy(msg_pointer,elem);
            elem = strtok(NULL, PYTHON_DELIMITER);
#if DEBUG_MODE == 1
	WRITE_TO_LOGGER(DEBUG_LEVEL,"message pointer and element key/value",CHARS_VALUES,2,
			LOG(msg_pointer),LOG(elem));
#endif
            msg_pointer+=MAX_ELEM_SIZE;
        }
		node->msg_data.sync_res_msg.commit_id = current_index;
#if DEBUG_MODE == 1
	WRITE_TO_LOGGER(DEBUG_LEVEL,"leader send sync log msg",CHARS_VALUES,3,
			LOG(node->msg_data.sync_res_msg.cmd),LOG(node->msg_data.sync_res_msg.key),
			LOG(node->msg_data.sync_res_msg.value));
		WRITE_TO_LOGGER(DEBUG_LEVEL,"leader send sync log msg",INT_VALUES,2,
			LOG(node->msg_data.sync_res_msg.commit_id),LOG(current_index));	
#endif
#if DEBUG_MODE == 1
		WRITE_TO_LOGGER(DEBUG_LEVEL,"leader sending sync response msg",NO_VALUES,0);
#endif
        send_raft_message(node,CONST_QUEUE_MSG_SIZE + sizeof(node->msg_data.sync_res_msg));
        current_index++;
        diff++;
    }
}

//ido term [difficulte = 1 ]
void leader_vote_handler(Queue_node_data *node)
{
#if DEBUG_MODE == 1
		WRITE_TO_LOGGER(DEBUG_LEVEL,"leader got vote request msg",INT_VALUES,2,
			LOG(sharedRaftData.raft_state.term),LOG(node->term));	
#endif
    if(node->term > sharedRaftData.raft_state.term)
    {
        clear_queue();
        sharedRaftData.raft_state.commit_counter = 0;
        sharedRaftData.raft_state.wakeup_counter = 0;
        sharedRaftData.raft_state.did_I_vote = 0;

        sharedRaftData.raft_state.current_state = FOLLOWER;
        update_DB(DB_STATUS, STATUS, FOLLOWER_VALUE);

    }
    //else ignores and stays leader
}


void cancel_commit_proccess(Queue_node_data* message_mem) {
    WRITE_TO_LOGGER(INFO_LEVEL, "leader cancel commit", INT_VALUES, 5,
                    LOG(sharedRaftData.raft_state.current_state), LOG(sharedRaftData.raft_state.members_amount),
                    LOG(sharedRaftData.raft_state.commit_counter), LOG(sharedRaftData.raft_state.last_commit_index),
                    LOG(sharedRaftData.raft_state.last_log_index));
    //send signal to inform CLI user
    raise(SIGUSR2);
    //delete enrty from redis log and decrease last_log_index
    sharedRaftData.python_functions.clear_log_from_log_id(sharedRaftData.raft_state.last_log_index--);
    update_DB(DB_STATUS, LAST_APPLIED, sharedRaftData.raft_state.last_log_index);
    //send hb with new last_log_index to inform the follower to cance the last log entry
    if (sharedRaftData.raft_state.current_state == LEADER) {
        create_new_queue_node_data(KEEP_ALIVE_HB, message_mem);
    #if DEBUG_MODE == 1
        WRITE_TO_LOGGER(DEBUG_LEVEL, "leader sending keep alive hb msg", NO_VALUES, 0);
    #endif
        send_raft_message(message_mem, CONST_QUEUE_MSG_SIZE +
                                       sizeof(message_mem->msg_data.keep_alive_hb_msg));//TBD - check returned value
}
    sharedRaftData.raft_state.commit_counter = 0;
}

//shany send hb message [difficulte = 2 ]
void  leader_time_out_handler(Queue_node_data* node)
{
#if DEBUG_MODE == 1
		WRITE_TO_LOGGER(DEBUG_LEVEL,"leader timeout event",NO_VALUES,0);	
#endif
    create_new_queue_node_data(KEEP_ALIVE_HB, node);
#if DEBUG_MODE == 1
		WRITE_TO_LOGGER(DEBUG_LEVEL,"leader sending keep alive hb msg",NO_VALUES,0);
#endif
    send_raft_message(node, CONST_QUEUE_MSG_SIZE + sizeof(node->msg_data.keep_alive_hb_msg));//TBD - check returned value
    //we are in commit procces
    if(sharedRaftData.raft_state.last_log_index != sharedRaftData.raft_state.last_commit_index){
		if(++sharedRaftData.raft_state.wakeup_counter == TIME_TO_CANCLE_COMMIT_PROC){
			cancel_commit_proccess(node);
		}
	}
}

//shany term [difficulte = 1 ]
void  leader_hb_handler(Queue_node_data* node)
{
#if DEBUG_MODE == 1
		WRITE_TO_LOGGER(DEBUG_LEVEL,"leader got HB msg",INT_VALUES,2,
			LOG(sharedRaftData.raft_state.term),LOG(node->term));	
#endif
    if(node->term > sharedRaftData.raft_state.term)
    {
        clear_queue();
        sharedRaftData.raft_state.did_I_vote = 0;
        sharedRaftData.raft_state.commit_counter = 0;
        sharedRaftData.raft_state.wakeup_counter = 0;

        //change timeout for leader
        sharedRaftData.raft_state.timeout = calculate_raft_rand_timeout();
        create_alarm_timer(sharedRaftData.raft_state.timeout);

        sharedRaftData.raft_state.current_state = FOLLOWER;
        update_DB(DB_STATUS, STATUS, FOLLOWER_VALUE);

    }
    if(node->term == sharedRaftData.raft_state.term)
    {
		WRITE_TO_LOGGER(INFO_LEVEL,"leader got HB with the same term",INT_VALUES,3,
			LOG(sharedRaftData.raft_state.term),LOG(node->term),
			LOG(node->message_sent_by));    
	}
}


//SHANY(oren) send_message->increase last_applied ->update_dal_last_applied [difficulte = 2 ]
//receiving a command from CLI
void  leader_send_log_hb_handler(Queue_node_data* node)
{
#if DEBUG_MODE == 1
		int new_log_commit_id =node->msg_data.set_log_hb_msg.commit_id;
		WRITE_TO_LOGGER(DEBUG_LEVEL,"leader send new log enrty",INT_VALUES,4,
			LOG(sharedRaftData.raft_state.last_commit_index),LOG(sharedRaftData.raft_state.last_log_index),
			LOG(sharedRaftData.raft_state.commit_counter),LOG(new_log_commit_id));
		WRITE_TO_LOGGER(DEBUG_LEVEL,"leader send sync log msg",CHARS_VALUES,3,
			LOG(node->msg_data.set_log_hb_msg.cmd),LOG(node->msg_data.set_log_hb_msg.key),
			LOG(node->msg_data.set_log_hb_msg.value));
#endif
    sharedRaftData.raft_state.last_log_index++;
    sharedRaftData.raft_state.commit_counter++;//count himself for majority

    update_DB(DB_STATUS,LAST_APPLIED,sharedRaftData.raft_state.last_log_index);

    create_new_queue_node_data(SET_LOG_HB, node);
#if DEBUG_MODE == 1
		WRITE_TO_LOGGER(DEBUG_LEVEL,"leader sending set_log_hb msg",NO_VALUES,0);
#endif
    send_raft_message(node, CONST_QUEUE_MSG_SIZE + sizeof(node->msg_data.set_log_hb_msg));//TBD - check returned value
    //use wake up counter as a timer for commit 
    sharedRaftData.raft_state.wakeup_counter = 0;
};


//ido if majority -> update_dal_commit_id ->send_commit_ok ->send_signal_to_python (python update_dal_key-val_DB )
void  leader_log_res_handler(Queue_node_data* node)
{
#if DEBUG_MODE == 1
		WRITE_TO_LOGGER(DEBUG_LEVEL,"leader log response msg",INT_VALUES,3,
			LOG(sharedRaftData.raft_state.commit_counter),LOG(sharedRaftData.raft_state.members_amount),
			LOG(sharedRaftData.raft_state.last_commit_index));
#endif
    sharedRaftData.raft_state.commit_counter++;

    //check if he has the most votes and if he has, sends commit ok
    if( sharedRaftData.raft_state.commit_counter == ((sharedRaftData.raft_state.members_amount/2) +1) )
    {
        sharedRaftData.raft_state.last_commit_index++;

        create_new_queue_node_data(COMMIT_OK, node);
#if DEBUG_MODE == 1
		WRITE_TO_LOGGER(DEBUG_LEVEL,"leader sending commit ok msg",NO_VALUES,0);
#endif
        send_raft_message(node, CONST_QUEUE_MSG_SIZE + sizeof(node->msg_data.commit_ok_msg));//TBD - check returned value

        update_DB(DB_STATUS, COMMIT_INDEX,sharedRaftData.raft_state.last_commit_index );
        sharedRaftData.python_functions.execute_log(sharedRaftData.raft_state.last_log_index);

        sharedRaftData.raft_state.wakeup_counter = 0;
        //inform python that commit proccess success
        raise(SIGUSR1);
    }

}



void leader_vote_req_handler(Queue_node_data* node)
{
#if DEBUG_MODE == 1
    WRITE_TO_LOGGER(DEBUG_LEVEL,"leader get vote request msg",INT_VALUES,3,
                    LOG(sharedRaftData.raft_state.term),LOG(node->term),
                    LOG(node->message_sent_by));

#endif
    if(node->term > sharedRaftData.raft_state.term)
    {

        //leader is in the middle of commit process
        if(sharedRaftData.raft_state.last_log_index > sharedRaftData.raft_state.last_commit_index){
            //leader must become a follower because there is a candidate with a grater term
            sharedRaftData.raft_state.current_state = FOLLOWER;
            cancel_commit_proccess(node);
        }
        sharedRaftData.raft_state.wakeup_counter = 0;

        sharedRaftData.raft_state.term = node->term;
        update_DB(DB_STATUS,TERM,sharedRaftData.raft_state.term);


        create_new_queue_node_data(VOTE, node);
#if DEBUG_MODE == 1
        WRITE_TO_LOGGER(DEBUG_LEVEL,"leader sending vote msg",NO_VALUES,0);
#endif
        send_raft_message(node,CONST_QUEUE_MSG_SIZE /*+ sizeof(node->msg_data.vote_msg)*/);//check returned value
        sharedRaftData.raft_state.did_I_vote = 1;

        clear_queue();
        sharedRaftData.raft_state.vote_counter = 0;


        sharedRaftData.raft_state.current_state = FOLLOWER;
        update_DB(DB_STATUS, STATUS, FOLLOWER_VALUE);
    }

}

