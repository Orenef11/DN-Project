import ctypes
import logging
import signal
import global_variables
from os import path, getcwd

commit_flag = False

_raft = ctypes.CDLL(path.join(getcwd(), "raft.so"))


def add_to_log_DB(log_id, command, key, val):
    #if global_variables.redis_db_obj.is_valid_command("logs", None):
    if len(global_variables.redis_db_obj["logs"]) == log_id:
        py_cmd = ctypes.string_at(command).decode("utf-8")
        py_key = ctypes.string_at(key).decode("utf-8")
        py_val = ctypes.string_at(val).decode("utf-8")
        global_variables.redis_db_obj["logs"]+=[(py_cmd, py_key, py_val)]
        return 0
    else:
        write_to_logger(4, "Trying to insert to invalid index to log_DB")
    return 1


# updates configuration or status db
def update_DB(db_flag, key, val):
    py_db_flag = ctypes.string_at(db_flag).decode("utf-8")
    py_key = ctypes.string_at(key).decode("utf-8")
    py_val = ctypes.string_at(val).decode("utf-8")
    if global_variables.redis_db_obj.is_valid_command(py_db_flag, None):
        global_variables.redis_db_obj[(py_db_flag, py_key)] = py_val
        return 0
    return 1


def get_log_by_diff(log_idx):
    if global_variables.redis_db_obj.is_valid_command("logs", None) and\
            len(global_variables.redis_db_obj["logs"]) >= log_idx:
            entry_data = global_variables.redis_db_obj["logs"][log_idx]
            entry_data.append(','.join(entry_data))
            return ctypes.c_char_p(entry_data)
    return ctypes.c_char_p()
# def get_log_by_diff(start, end):
#     # we need +2. 1 to get real size of the list and 1 mode for null pointer
#     c_log_array, log_list = (ctypes.c_wchar_p * (end - start + 1))(), []
#     if global_variables.redis_db_obj.is_valid_command("logs", None) and\
#             len(global_variables.redis_db_obj["logs"]) >= end:
#         for entry in range(start, end):
#
#             entry_data = [str(value) if type(value) is not str else value for value in
#                           global_variables.redis_db_obj["logs"][entry]]
#             log_list.append(','.join(entry_data))
#
#         c_log_array[:-1] = log_list
#         c_log_array[-1] = None
#     # log_list.append(ctypes.c_char_p)
#     return c_log_array[0]


def write_to_logger(logger_level, logger_message):
    py_logger_message = "\n"+ctypes.string_at(logger_message).decode("utf-8")
    levels = {0: logging.debug,
              1: logging.info,
              2: logging.warning,
              3: logging.error,
              4: logging.critical}
    levels.get(logger_level, logging.debug)(py_logger_message)
    return 1


def execute_log(log_id):
    if global_variables.redis_db_obj.is_valid_command("logs", None):
        if len(global_variables.redis_db_obj["logs"]) == log_id:
            cmd = global_variables.redis_db_obj["logs"][log_id][0]

            if cmd in ["add", "edit"]:
                key, val = global_variables.redis_db_obj["logs"][log_id][:2]
                global_variables.redis_db_obj[("values", key)] = val
                return 0

            elif cmd == "delete":
                key = global_variables.redis_db_obj["logs"][log_id][1]
                del global_variables.redis_db_obj["values"][key]
                return 0

            else:
                write_to_logger(4, "The command we tried to execute from the log is invalid")
        else:
            write_to_logger(4, "Trying to insert to invalid index to log_DB")

    return 1


def clear_log_from_log_id(log_id):
    return 1


def set_callback_funcs():
    global _raft
    _raft.transfer_callback_function(callback_func1, callback_func2, callback_func3, callback_func4, callback_func5, callback_func6)


# -----------------------------------------------------------------------------------------

# python transfer func pointers to c:



callback_type1 = ctypes.CFUNCTYPE(ctypes.c_int,
                                  ctypes.c_int,
                                  ctypes.c_char_p,
                                  ctypes.c_char_p,
                                  ctypes.c_char_p)
callback_func1 = callback_type1(add_to_log_DB)

callback_type2 = ctypes.CFUNCTYPE(ctypes.c_int,
                                  ctypes.c_char_p,
                                  ctypes.c_char_p,
                                  ctypes.c_char_p)
callback_func2 = callback_type2(update_DB)

callback_type3 = ctypes.CFUNCTYPE(ctypes.c_char_p, ctypes.c_int)
callback_func3 = callback_type3(get_log_by_diff)

callback_type4 = ctypes.CFUNCTYPE(ctypes.c_int,
                                  ctypes.c_int,
                                  ctypes.c_char_p)
callback_func4 = callback_type4(write_to_logger)

callback_type5 = ctypes.CFUNCTYPE(ctypes.c_int,
                                  ctypes.c_int)
callback_func5 = callback_type5(execute_log)

callback_type6 = ctypes.CFUNCTYPE(ctypes.c_int,ctypes.c_int)
callback_func6 = callback_type6(clear_log_from_log_id)

callback_type7 = ctypes.CFUNCTYPE(ctypes.c_void_p)
c_set_callback_funcs = callback_type7(set_callback_funcs)


def python_run_raft(raft_ip: bytes, raft_port: int, server_id: int, members_num: int, leader_timeout: int):
    global _raft, c_set_callback_funcs
    return _raft.run_raft(ctypes.c_char_p(raft_ip),
                          ctypes.c_int(raft_port),
                          ctypes.c_int(server_id),
                          ctypes.c_int(members_num),
                          ctypes.c_int(leader_timeout),
                          c_set_callback_funcs)


# -----------------------------------------------------------------------------------------


def sig_handler(signum, frame):
    global commit_flag
    print(signum)
    if signum == 10:
        commit_flag = True

    else:
        commit_flag = False


def start_commit_process(log_id, cmd, key, val):
    global _raft
    global commit_flag
    commit_flag = False
    if len(global_variables.redis_db_obj["logs"]) == log_id:
        _raft.start_commit_process(ctypes.c_int(log_id),
                                   ctypes.c_char_p(str.encode(cmd)),
                                   ctypes.c_char_p(str.encode(key)),
                                   ctypes.c_char_p(str.encode(str(val))))

    #signal.sigwait([signal.SIGUSR1, signal.SIGUSR2])

    # if commit_flag:
    #     return True
    # return False
    return True


# -----------------------------------------------------------------------------------------

# def python_run_raft():
#     run_raft(b"224.1.1.1", 6060, 1, 2, 1000, c_set_callback_funcs)
#
# if __name__ == '__main__':
#     main()
