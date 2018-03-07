from colorama import Style, Fore
#global raft_cmd_obj, redis_db_obj
raft_cmd_obj = None
redis_db_obj = None
SUCCESS = 1
FAILS = 0
LOW_RANGE_TIMER = 0.150
HIGH_RANGE_TIMER = 0.300
PERMISSION_DENIED = "{}{}Permission Denied!!!{}".format(Style.BRIGHT, Fore.RED, Style.RESET_ALL)

UNKNOW_VAR_REDIS_MSG = "The '{}' variable does not exist in the '{}' REDIS database"
UNKNOW_REDIS_DB_MSG = "The '{}' REDIS database does not exist"
UNSUCESS_COMMAND_MSG = "CommandFailed: Please check the entered values or check command exists ('?' char)"


def get_raft_cmd_obj():
    global raft_cmd_obj
    return raft_cmd_obj


def get_redis_db_obj():
    global redis_db_obj
    return redis_db_obj
