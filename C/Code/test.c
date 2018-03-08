#include "../Headers/test.h"

#if MAIN_TEST == 1


void init_logger()
{
}


void _test_write_to_logger(int logger_level,char * logger_info)
{
}

void _test_add_to_log_DB(int log_id,char* cmd,char* key,char* value)
{
	WRITE_TO_LOGGER(INFO_LEVEL,"WORK WORK WORK WORK WORK _test_add_to_log_DB",NO_VALUES,0);
}


void _test_update_DB(char * DB_flag,char * key,char* value)
{
	WRITE_TO_LOGGER(INFO_LEVEL,"WORK WORK WORK WORK WORK _test_update_DB",NO_VALUES,0);
}


char* _test_get_log_by_diff(int flog_index)
{
	WRITE_TO_LOGGER(INFO_LEVEL,"WORK WORK WORK WORK WORK _test_get_log_by_diff",NO_VALUES,0);
	char **abc = NULL;
	return abc;
}


void _test_execute_log(int command_id)
{
	WRITE_TO_LOGGER(INFO_LEVEL,"WORK WORK WORK WORK WORK _test_execute_log",NO_VALUES,0);
}


void _test_clear_log_from_log_id(int log_to_delete)
{
	WRITE_TO_LOGGER(INFO_LEVEL,"WORK WORK WORK WORK WORK _test_clear_log_from_log_id",NO_VALUES,0);
}


void _test_set_callback_function()
{
	transfer_callback_function(_test_add_to_log_DB,
							_test_update_DB,
                               _test_get_log_by_diff,
							   _test_write_to_logger,
							_test_execute_log,
							_test_clear_log_from_log_id);
}


int main(int argc,char **argv)
{
	init_logger();
	run_raft(_TEST_MULTICAST_IP,_TEST_MULTICAST_PORT,_TEST_SERVER_ID,_TEST_MEMBERS_NUM,_TEST_LEADER_TIMEOUT,_test_set_callback_function);
	return 0;
}
#endif
