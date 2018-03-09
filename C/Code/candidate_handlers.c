
#include "../Headers/candidate_handlers.h"



void candidate_vote_for_me_handler(Queue_node_data * node)
{
    if(node->term == sharedRaftData.raft_state.term)
    {
        sharedRaftData.raft_state.vote_counter++;

        //candidate got majority increase vote_counter and check majority
        if(sharedRaftData.raft_state.vote_counter >= ((sharedRaftData.raft_state.members_amount/2) + 1))
        {
            sharedRaftData.raft_state.current_state = LEADER;
            clear_queue();
#if DEBUG_MODE == 1
        WRITE_TO_LOGGER(DEBUG_LEVEL,"candidate got majority- becoming leader",INT_VALUES,6,LOG(node->event),LOG(sharedRaftData.raft_state.term),
                        LOG(sharedRaftData.raft_state.current_state),LOG(sharedRaftData.raft_state.vote_counter),
                        LOG(node->message_sent_by),LOG(node->term));
#endif

            //change timeout for leader
            sharedRaftData.raft_state.timeout = sharedRaftData.raft_configuration.leader_timeout;
            create_alarm_timer(sharedRaftData.raft_state.timeout);

            sharedRaftData.raft_state.vote_counter = 0;
            sharedRaftData.raft_state.wakeup_counter = 0;
            sharedRaftData.raft_state.did_I_vote = 0;

            //update DAL
            sharedRaftData.raft_state.leader_id = sharedRaftData.raft_state.server_id;

            update_DB(DB_STATUS,STATUS,LEADER_VALUE);
            update_DB(DB_STATUS, LEADER_ID, sharedRaftData.raft_state.leader_id);

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
        sharedRaftData.raft_state.current_state = FOLLOWER;
        sharedRaftData.raft_state.vote_counter = 0;
        sharedRaftData.raft_state.wakeup_counter = 0;
        sharedRaftData.raft_state.did_I_vote = 0;

        sharedRaftData.raft_state.leader_id = node->message_sent_by;
        update_DB(DB_STATUS, LEADER_ID, sharedRaftData.raft_state.leader_id);
        update_DB(DB_STATUS, STATUS, FOLLOWER_VALUE);

        clear_queue();
        create_alarm_timer(sharedRaftData.raft_state.timeout);
    }

#if DEBUG_MODE == 1
	WRITE_TO_LOGGER(DEBUG_LEVEL,"end of vote event",INT_VALUES,1,LOG(sharedRaftData.raft_state.current_state));
#endif

}



void candidate_keep_alive_hb_handler(Queue_node_data *node)
{

#if DEBUG_MODE == 1
	WRITE_TO_LOGGER(DEBUG_LEVEL,"keep_alive HB event",INT_VALUES,4,LOG(node->event),LOG(sharedRaftData.raft_state.term),
	LOG(node->message_sent_by),LOG(node->term));
#endif

    if(node->term >= sharedRaftData.raft_state.term)
    {
        sharedRaftData.raft_state.current_state = FOLLOWER;
        sharedRaftData.raft_state.vote_counter = 0;
        sharedRaftData.raft_state.wakeup_counter = 0;
        sharedRaftData.raft_state.did_I_vote = 0;

        sharedRaftData.raft_state.leader_id = node->message_sent_by;
        update_DB(DB_STATUS, LEADER_ID, sharedRaftData.raft_state.leader_id);
        update_DB(DB_STATUS, STATUS, FOLLOWER_VALUE);
        
        clear_queue();
        create_alarm_timer(sharedRaftData.raft_state.timeout);

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

        send_raft_message(node,CONST_QUEUE_MSG_SIZE);
        sharedRaftData.raft_state.did_I_vote = 1;

        sharedRaftData.raft_state.vote_counter = 0;


        sharedRaftData.raft_state.leader_id = node->message_sent_by;
        update_DB(DB_STATUS, LEADER_ID, sharedRaftData.raft_state.leader_id);
        sharedRaftData.raft_state.current_state = FOLLOWER;
        update_DB(DB_STATUS, STATUS, FOLLOWER_VALUE);
        
        clear_queue();
        create_alarm_timer(sharedRaftData.raft_state.timeout);
    }

}



void candidate_time_out_handler(Queue_node_data* node)
{

#if DEBUG_MODE == 1
	WRITE_TO_LOGGER(DEBUG_LEVEL,"timeout event",INT_VALUES,1,LOG(sharedRaftData.raft_state.wakeup_counter));
#endif

    //if(++sharedRaftData.raft_state.wakeup_counter >= 2 )
    //{
        sharedRaftData.raft_state.term++;
        sharedRaftData.raft_state.vote_counter =1;
        sharedRaftData.raft_state.did_I_vote = 1;
        sharedRaftData.raft_state.wakeup_counter = 0;

        update_DB(DB_STATUS, TERM, sharedRaftData.raft_state.term);

        create_new_queue_node_data(REQUEST_FOR_VOTE,node);

#if DEBUG_MODE == 1
		WRITE_TO_LOGGER(DEBUG_LEVEL,"candidate sending request for vote msg",INT_VALUES,1,
                        LOG(sharedRaftData.raft_state.term));
#endif

        send_raft_message(node,CONST_QUEUE_MSG_SIZE);
        //change the timer, Increase the propabilty that some candidate will be the leader
        sharedRaftData.raft_state.timeout = calculate_raft_rand_timeout();
        create_alarm_timer(sharedRaftData.raft_state.timeout);
    //}

}

