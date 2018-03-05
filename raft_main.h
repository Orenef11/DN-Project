#ifndef RAFT_PROJECT_MAIN_H
#define RAFT_PROJECT_MAIN_H

#include "C/Headers/state_machine.h"
//#include "static_queue.h"
//#include "raft_server.h"
int run_raft(char* raft_ip,int raft_port,int server_id,int members_num,int leader_timeout,void(*set_callback_function)(void));

void transfer_callback_function(void (*add_to_log_DB)(int log_id,char* cmd,char* key,char* value),
							void (*update_DB)(char * DB_flag,char * key,char* value),
                               char** (*get_log_by_diff)(int from,int to),
							   void (*write_to_logger)(int logger_level,char * logger_info),
							void (*execute_log)(int),
							void (*clear_log_from_log_id)(int));
#endif //RAFT_PROJECT_MAIN_H
