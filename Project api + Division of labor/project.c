
struct state{
	char term;
	int last_log_id;
	int commit_id;
	State current_state;
	int member_num;
	int id;
	
	//for candidate use
	int vote_counter;
	
	//for leader use
	int commit_counter;
	
	//for follower use
	char did_vote_or_not;
	
	int timeout;
	//if wakeup_counter == 2 , follower -> candidate
	int wakeup_counter;
	
	//all 9 options
	Event event_type;
	//not nedded for now
	//State (*hendler)();

}

typedef struct State_server_config{
	int multy_cast_fd;
	char * multy_cast_ip;
	int port;
}state_server_config;

typedef struct Node_queue{
	Event event_type;
	char term;
	int last_log_id;
	int commit_id; 
	char command_log;
	char * key;
	char * value;
	int sender_id;
}nude_queue;

typedef struct Shared_raft_data{
	queue * raft_queue;
	state_server_config * server_config;
	state * raft_state;
	python_function * function;
}shared_raft_data;

shared_raft_data * raft_data;

///////////////////////////////
//define
//states
#define LEADER
#define CANDIDATE
#define FOLLOWER
//log levels
#define DEBUG_LEVEL
#define INFO_LEVEL
#define WARN_LEVEL
#define ERROR_LEVEL
#define FATAL_LEVEL
//message type for log
#define CANDIDATE_VOTE_FOR_ME
//message format for log
#define CANDIDATE_VOTE_FOR_ME_FORMAT "cd"
/////////////////////////////////
//python add command to raft 
//ido - add new queue event and start commit process [difficulte = 1 ]
void add_new_value(int commit_id,char * key, char * value);
//pyhton start raft process, call init_raft() [difficulte = 2 ]
//itay - 
int run_raft(char* ip,int port,int id,int members_num,int leader_timeout);


//c calls py func 
//itay - set the callback function in the shared_raft_data struct [difficulte = easy ]
int transfer_callback_function(void (*add_log_to_DB)(int),void(*update_current_state)(char * key,char* value),void (*clear_uncommit_log)(int last_commit));



//initialize parameters (socket etc) 
//itay [difficulte = 1 ]
int init_raft();
//malloc for quque , state, handlers, etc
//itay [difficulte = 1 ]
int init_raft_memory();
//return socket for multi cast
//itay [difficulte = 2 ]
int init_raft_server(char * ip, int port); 
//open 2 threads - main and multicast listener
// pthread_create (&thread1, null, run_raft_server, null)
// pthread_create (&thread2, null, raft_main, null)
//itay - read data from socket [difficulte = 3 ]
char* read_from_multy_socket(int socket_fd);
//itay - send data by socket [difficulte = 2 ]
int send_raft_message(char * message);


	
//listen to multicast 

// in while(1):
//dequeue and 
// parse message 
//call event function in state machine

/////////////////////////////////
//

//
//oren - while(1) , raft_listenter -> add_to_queue [difficulte = 2 ]
void run_raft_server();
//oren - read_message -> parse_message [difficulte = 1 ]
nude_queue * raft_listener();
//oren . according to shany doc [difficulte = 3 ]
char * create_new_massage(state * raft_state);
///////////////
//oren SIGARAM - add new timeout event to queue [difficulte = 1 ]
int time_out_hendler(int sig);
//oren - if relevant -> add_to_queue->sem post (call to ido queue code) [difficulte = 1 ]
int add_to_quque(nude_queue *);

//
//////////////////
//oren - wait to sem -> get message from queue -> change state -> run handler (call to oren state code) [difficulte = 2 ]
void raft_main(); 
//itay - wrap ido queue code [difficulte = 1 ]
nude_queue * pop_queue(queue * raft_queue);
//oren [difficulte = 3 ]
nude_queue * parse_massge(char * message);
//ido , remove all elements from queue -> this function will be called when the state was change [difficulte = 2 ]
int clear_queue(queue * raft_queue);
//shany [difficulte = 3 ]
int is_relevant_message(nude_queue * message);
//////////////////

___________________________________________



follower:
//ido. relevant fileds from Node_queue - byte id_candidate, byte term [difficulte = 2 ]
state follower_vote_req_handler()
//ido. byte term, int last_log_id [difficulte = 1 ]
state follower_hb_keep_alive_handler()
//shany byte term, int commit_id, byte cmd, 101bytes key_val [difficulte = 2 ]
state follower_hb_set_log_handler()
//shany byte term, int start log id, int last log id [difficulte = 2 ]
state follower_sync_res_handler()
//oren relevant fileds from state - wakeup_counter [difficulte = 1 ]
state follower_time_out()

void write_to_log(int level_msg,int message_type,const char *format, ...);
candidate:
//itay relevant fileds from Node_queue - byte term [difficulte = 1 ]
state candidate_vote_for_me(){
	//pop queue
	nude_queue * node = pop_queue(raft_data->raft_queue);
	//write log message (CANDIDATE_VOTE_FOR_ME_FORMAT="cd" becouse term is char and server_id is int)
	write_to_log(DEBUG_LEVEL,CANDIDATE_VOTE_FOR_ME,CANDIDATE_VOTE_FOR_ME_FORMAT,node->term,node->server_id);
	//increase vote_counter and check majurity
	if(++raft_data->raft_state->vote_counter > raft_data->raft_state->member_num/2+1){
		//remove unrelevant queue element (any time when state will change )
		clear_queue(raft_data->raft_queue);
		//change state to leader
		return LEADER;
	}
	//stay in the same state
	return CANDIDATE;
}
//ido byte term [difficulte = 1 ]
state cadidate_keep_alive_hb();
//shany relevant fileds from state term [difficulte = 1 ]
state candidte_time_out();



leader:
//ido relevant fileds from Node_queue - byte term, last_log_id, sender_id [difficulte = 3 ]
state leader_sync_req()
//ido term [difficulte = 1 ]
state leader_vote()
//shany send hb message [difficulte = 2 ]
state leader_time_out()
//shany term [difficulte = 1 ]
state leader_hb()
//oren commit_counter [difficulte = 2 ]
state leader_commit()
