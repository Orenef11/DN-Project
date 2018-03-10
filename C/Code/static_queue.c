#include "../Headers/static_queue.h"

int first_time = 1;


int init_raft_queue()
{
    int rv = 0;
    if(first_time)
    {

#if DEBUG_MODE == 1
	WRITE_TO_LOGGER(DEBUG_LEVEL,"first time",NO_VALUES,0);
#endif

        rv|=pthread_mutex_init(&sharedRaftData.Raft_queue.mutex_queue,NULL);
    }

    if(!first_time)
    {

#if DEBUG_MODE == 1
	WRITE_TO_LOGGER(DEBUG_LEVEL,"not first time",NO_VALUES,0);
#endif

        rv|=pthread_mutex_lock(&sharedRaftData.Raft_queue.mutex_queue);
    }

    sharedRaftData.Raft_queue.queue_start = 0;
    sharedRaftData.Raft_queue.queue_end = 0;
    sharedRaftData.Raft_queue.queue_elements_amount = 0;
    rv|=sem_init(&sharedRaftData.Raft_queue.sem_queue,0,0); //sem is also a type of amount, so we need to init it back to 0 (when clearing queue)

    if(!first_time)
    {
        rv|=pthread_mutex_unlock(&sharedRaftData.Raft_queue.mutex_queue);
    }

    if(first_time)
    {
        first_time =0;
    }
    return rv;
}



void push_queue(Queue_node_data *data)
{
    if(sharedRaftData.Raft_queue.queue_elements_amount != QUEUE_SIZE -1)
    {
        pthread_mutex_lock(&sharedRaftData.Raft_queue.mutex_queue);

        memcpy(&sharedRaftData.Raft_queue.queue[sharedRaftData.Raft_queue.queue_end], data, sizeof(Queue_node_data));

        sharedRaftData.Raft_queue.queue_end = (sharedRaftData.Raft_queue.queue_end + 1) % QUEUE_SIZE;
        
        sharedRaftData.Raft_queue.queue_elements_amount++;

        pthread_mutex_unlock(&sharedRaftData.Raft_queue.mutex_queue);

        sem_post(&sharedRaftData.Raft_queue.sem_queue);
    }

    else
    {
		WRITE_TO_LOGGER(FATAL_LEVEL,"queue is full",NO_VALUES,0);
    }
}



void pop_queue(Queue_node_data *ret)
{
    if(sharedRaftData.Raft_queue.queue_elements_amount != 0)
    {
        pthread_mutex_lock(&sharedRaftData.Raft_queue.mutex_queue);

        memcpy(ret, &sharedRaftData.Raft_queue.queue[sharedRaftData.Raft_queue.queue_start], sizeof(Queue_node_data));

        sharedRaftData.Raft_queue.queue_start = (sharedRaftData.Raft_queue.queue_start + 1) % QUEUE_SIZE;
        
        sharedRaftData.Raft_queue.queue_elements_amount--;

        pthread_mutex_unlock(&sharedRaftData.Raft_queue.mutex_queue);
    }

    else
    {
		WRITE_TO_LOGGER(FATAL_LEVEL,"try to pop from an empty queue",NO_VALUES,0);
    }
}


void clear_queue()
{
    init_raft_queue();
}



void create_new_queue_node_data(eventType event, Queue_node_data* msg_data_memory)
{

#if DEBUG_MODE == 1
	WRITE_TO_LOGGER(DEBUG_LEVEL,"create new msg",INT_VALUES,1,
			LOG(event),LOG(sharedRaftData.raft_state.current_state));
#endif

    msg_data_memory->event = event;
    msg_data_memory->term = sharedRaftData.raft_state.term;
    msg_data_memory->message_sent_to = 0;
    if(event == KEEP_ALIVE_HB)
    {
        //msg_data is union so the filed is shared between the 3 struct
        msg_data_memory->msg_data.keep_alive_hb_msg.last_log_id = sharedRaftData.raft_state.last_log_index;
    }
    else if(event == COMMIT_OK){
		msg_data_memory->msg_data.commit_ok_msg.last_log_index = sharedRaftData.raft_state.last_log_index;
	}
	else if(event == SET_LOG_RES){
		msg_data_memory->msg_data.set_log_res_msg.to_be_commit_index = sharedRaftData.raft_state.last_log_index;
	}
    else if(event == SET_LOG_HB)
    {
        //CMD,KEY,VALUE is already in msg_data_memory
        msg_data_memory->msg_data.set_log_hb_msg.commit_id = sharedRaftData.raft_state.last_log_index;
    }
    else if(event == SYNC_REQ)
    {
        msg_data_memory->msg_data.sync_req_msg.start_log_index = 0;
        msg_data_memory->msg_data.sync_req_msg.last_log_id = sharedRaftData.raft_state.last_log_index;
    }
    else if(event == VOTE)
    {
        msg_data_memory->message_sent_to =msg_data_memory->message_sent_by;
    }
    msg_data_memory->message_sent_by = sharedRaftData.raft_state.server_id;
}
