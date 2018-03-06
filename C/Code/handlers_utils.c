#include "../Headers/handlers_utils.h"

int relevant_event_bits[STATE_AMMOUNT][EVENT_AMMOUNT] = {
						/*FOLLOWER*/{1,1,1,0,0,1,1,0,1,0},
						/*CANDIDATE*/{1,1,0,0,0,0,0,1,1,0},
						/*LEADER*/{1,1,0,1,1,0,0,1,1,1}
						};

char const * const states_name[] = (char const *[]){"follower","candidate","leader"};

int update_DB(char * DB_flag,char * key, int value){
#if DEBUG_MODE == 1
	WRITE_TO_LOGGER(DEBUG_LEVEL,"update DB",CHARS_VALUES,2,LOG(DB_flag),LOG(key));
	WRITE_TO_LOGGER(DEBUG_LEVEL,"update DB",INT_VALUES,1,LOG(value));
#endif
	char value_str[MAX_ELEM_SIZE];
	//if value is not raft state value is a state name flag
	if(value<MIN_RAFT_STATE_VALUE){
		sprintf(value_str,"%s",states_name[value + STATE_NAME_SHIFT]);
	}
	else{
		sprintf(value_str,"%d",value);
	}
	sharedRaftData.python_functions.update_DB(DB_flag,key,value_str);
}

int calculate_raft_rand_timeout(){
    return MIN_RAFT_TIMEOUT + rand()%(MAX_RAFT_TIMEOUT-MIN_RAFT_TIMEOUT);
}

void create_timeout_event(int timeout){
#if DEBUG_MODE == 1
	WRITE_TO_LOGGER(DEBUG_LEVEL,"create new event timer",INT_VALUES,2,LOG(sharedRaftData.raft_state.current_state),LOG(timeout));
#endif
    struct itimerval timer;
    memset(&timer,0,sizeof(struct itimerval));
    //if timeout is bigger then one seconds
    if(timeout>=1000){
		timer.it_value.tv_sec = timeout/1000;
		timer.it_interval.tv_sec = timeout/1000;
		timer.it_value.tv_usec = 0;
		timer.it_interval.tv_usec = 0;
	}
	else{
		timer.it_value.tv_sec = 0;
		timer.it_interval.tv_sec = 0;
		timer.it_value.tv_usec = timeout * MILISEC_CONVERT /*1000 for milliseconds*/;
		timer.it_interval.tv_usec = timeout * MILISEC_CONVERT /*1000 for milliseconds*/;
	}
    //disable all previous timers
    alarm(0);
    setitimer(ITIMER_REAL, &timer, NULL);
}


//main.c
void time_out_hendler(int sig){
    Queue_node_data new_node;
    new_node.event  = TIMEOUT;
    push_queue(&new_node);
    
#if DEBUG_MODE == 1
	WRITE_TO_LOGGER(DEBUG_LEVEL,"add new timeout event",INT_VALUES,1,LOG(sharedRaftData.raft_state.current_state));
#endif
}

/*
int is_relevant_for_leader(Queue_node_data * node_message)
{
    if( (((node_message->event == SYNC_RES) && (node_message->message_sent_to != sharedRaftData.raft_state.server_id)))
    {
        return 0;
    }
    return 1;
}
*/
int is_relevant_for_candidate(Queue_node_data * node_message)
{
    if(((node_message->event == VOTE) &&(node_message->message_sent_to != sharedRaftData.raft_state.server_id)))
    {
        return 0;
    }
    return 1;
}

int is_relevant_message(Queue_node_data * node_message)
{
    int is_relevant = relevant_event_bits[sharedRaftData.raft_state.current_state][node_message->event];
	int is_relevant_term = node_message->term >= sharedRaftData.raft_state.term;
	int is_not_my_message = node_message->message_sent_by != sharedRaftData.raft_state.server_id;
	if(!is_relevant_term){
		WRITE_TO_LOGGER(INFO_LEVEL,"got msg with smaller term", INT_VALUES,4,
		LOG(sharedRaftData.raft_state.term),LOG(sharedRaftData.raft_state.current_state),
		LOG(node_message->term),LOG(node_message->event));
	}
#if DEBUG_MODE == 1
		int is_relevant_rv;
#endif

    if(sharedRaftData.raft_state.current_state == CANDIDATE)
    {
#if DEBUG_MODE == 1
		is_relevant_rv = is_not_my_message & is_relevant & is_relevant_term & is_relevant_for_candidate(node_message);
#else
		return is_not_my_message & is_relevant & is_relevant_term & is_relevant_for_candidate(node_message);
#endif
    }
#if DEBUG_MODE == 1
	else{
				is_relevant_rv = is_not_my_message & is_relevant & is_relevant_term;
	}
	WRITE_TO_LOGGER(DEBUG_LEVEL,"check if msg is relevant",INT_VALUES,3,
		LOG(sharedRaftData.raft_state.current_state),LOG(node_message->event),
		LOG(is_relevant_rv));
	return is_relevant_rv;
#else
    return is_not_my_message & is_relevant & is_relevant_term;
#endif

};
