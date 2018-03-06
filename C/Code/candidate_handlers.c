
#include "../Headers/candidate_handlers.h"

///candidate:

//itay relevant fileds from Node_queue - byte term [difficulte = 1 ]
void candidate_vote_for_me_handler(Queue_node_data * node)
{
#if DEBUG_MODE == 1
	WRITE_TO_LOGGER(DEBUG_LEVEL,"vote event",INT_VALUES,6,LOG(node->event),LOG(sharedRaftData.raft_state.term),
	LOG(sharedRaftData.raft_state.current_state),LOG(sharedRaftData.raft_state.vote_counter),
	LOG(node->message_sent_by),LOG(node->term));
#endif
    if(node->term == sharedRaftData.raft_state.term)
    {
        sharedRaftData.raft_state.vote_counter++;

        //candidate got majority increase vote_counter and check majority
        if(sharedRaftData.raft_state.vote_counter > ((sharedRaftData.raft_state.members_amount/2) + 1))
        {
            //change timeout for leader
            sharedRaftData.raft_state.timeout = sharedRaftData.raft_configuration.leader_timeout;
            create_timeout_event(sharedRaftData.raft_state.timeout);

            clear_queue();
            sharedRaftData.raft_state.vote_counter = 0;
            sharedRaftData.raft_state.wakeup_counter = 0;
            sharedRaftData.raft_state.did_I_vote = 0;

            //update DAL
            sharedRaftData.raft_state.current_state = LEADER;
            update_DB(DB_STATUS,STATUS,LEADER_VALUE);
            update_DB(DB_STATUS, LEADER_ID, sharedRaftData.raft_state.server_id);

            create_new_queue_node_data(KEEP_ALIVE_HB,node);
#if DEBUG_MODE == 1
		WRITE_TO_LOGGER(DEBUG_LEVEL,"candidate sending keep alive hb msg",NO_VALUES,0);
#endif
            send_raft_message(node,CONST_QUEUE_MSG_SIZE + sizeof(node->msg_data.keep_alive_hb_msg));

        }
    }
    //another leader has been chosen
    else if(node->term > sharedRaftData.raft_state.term)
    {
        clear_queue();
        sharedRaftData.raft_state.vote_counter = 0;
        sharedRaftData.raft_state.wakeup_counter = 0;
        sharedRaftData.raft_state.did_I_vote = 0;

        sharedRaftData.raft_state.current_state = FOLLOWER;
        update_DB(DB_STATUS, STATUS, FOLLOWER_VALUE);


    }
#if DEBUG_MODE == 1
	WRITE_TO_LOGGER(DEBUG_LEVEL,"end of vote event",INT_VALUES,1,LOG(sharedRaftData.raft_state.current_state));
#endif
}


//ido byte term [difficulte = 1 ]
void candidate_keep_alive_hb_handler(Queue_node_data *node)
{
#if DEBUG_MODE == 1
	WRITE_TO_LOGGER(DEBUG_LEVEL,"keep_alive HB event",INT_VALUES,4,LOG(node->event),LOG(sharedRaftData.raft_state.term),
	LOG(node->message_sent_by),LOG(node->term));
#endif
    if(node->term >= sharedRaftData.raft_state.term)
    {
        clear_queue();
        sharedRaftData.raft_state.vote_counter = 0;
        sharedRaftData.raft_state.wakeup_counter = 0;
        sharedRaftData.raft_state.did_I_vote = 0;

        sharedRaftData.raft_state.current_state = FOLLOWER;
        update_DB(DB_STATUS, STATUS, FOLLOWER_VALUE);

    }
}


void candidate_vote_req_handler(Queue_node_data* node)
{
#if DEBUG_MODE == 1
    WRITE_TO_LOGGER(DEBUG_LEVEL,"candidate get vote request msg",INT_VALUES,4,
                    LOG(sharedRaftData.raft_state.term),LOG(sharedRaftData.raft_state.did_I_vote),
                    LOG(node->term),LOG(node->message_sent_by));

#endif
    if(node->term > sharedRaftData.raft_state.term)
    {

        sharedRaftData.raft_state.wakeup_counter = 0;

        sharedRaftData.raft_state.term = node->term;
        update_DB(DB_STATUS,TERM,sharedRaftData.raft_state.term);


        create_new_queue_node_data(VOTE, node);
#if DEBUG_MODE == 1
        WRITE_TO_LOGGER(DEBUG_LEVEL,"candidate sending vote msg",NO_VALUES,0);
#endif
        send_raft_message(node,CONST_QUEUE_MSG_SIZE /*+ sizeof(node->msg_data.vote_msg)*/);//check returned value
        sharedRaftData.raft_state.did_I_vote = 1;

        clear_queue();
        sharedRaftData.raft_state.vote_counter = 0;


        sharedRaftData.raft_state.current_state = FOLLOWER;
        update_DB(DB_STATUS, STATUS, FOLLOWER_VALUE);
    }

}



//relevant fileds from state term
void candidate_time_out_handler(Queue_node_data* node)
{
#if DEBUG_MODE == 1
	WRITE_TO_LOGGER(DEBUG_LEVEL,"timeout event",INT_VALUES,1,LOG(sharedRaftData.raft_state.wakeup_counter));
#endif
    if(++sharedRaftData.raft_state.wakeup_counter >= 2 )
    {
        sharedRaftData.raft_state.term++;
        sharedRaftData.raft_state.vote_counter =1;
        sharedRaftData.raft_state.did_I_vote = 1;

        sharedRaftData.raft_state.wakeup_counter = 0;

        create_new_queue_node_data(REQUEST_FOR_VOTE,node);
#if DEBUG_MODE == 1
		WRITE_TO_LOGGER(DEBUG_LEVEL,"candidate sending request for vote msg",NO_VALUES,0);
#endif
        send_raft_message(node,CONST_QUEUE_MSG_SIZE /*+ sizeof(node->msg_data.req_for_vote_msg)*/);
    }

}

