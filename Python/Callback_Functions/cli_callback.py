from pygtrie import StringTrie
from typing import Tuple
import global_variables
from Callback_Functions import raft_python_callback


def __show_logs_all() -> bool:
    if not global_variables.redis_db_obj.is_valid_command("logs", None):
        print(global_variables.UNKNOW_REDIS_DB_MSG.format("logs"))

    if len(global_variables.redis_db_obj["logs"]) == 0:
        print("The '{}' database is empty".format("logs"))
    else:
        print(global_variables.redis_db_obj["logs"])

    return True


def __show_logs(special_word: str, args: list) -> bool:
    """
    DAL.set({function_name: (sub_method, low_range_value, high_range_value)})
    Add function get dict.
    A key indicates which action to do and the value is a tuple of parameters

    :param special_word: Sub-function must be run to get the data
    :param args: The arguments that the function accepts
    :return:
    """
    if not global_variables.redis_db_obj.is_valid_command("logs", None):
        print(global_variables.UNKNOW_REDIS_DB_MSG.format("logs"))
        return True

    if len(args) == 2:
        if special_word == "range":
            if len(global_variables.redis_db_obj["logs"]) >= args[1]:
                [print("{}.\t{}".format(idx + 1, log_line)) for idx, log_line in
                 enumerate(global_variables.redis_db_obj["logs"][args[0] + 1: args[1] + 1])]
                return True

    elif len(args) == 1:
        if len(global_variables.redis_db_obj["logs"]) >= args[0]:
            if special_word == "top":
                [print("{}.\t{}".format(idx + 1, log_line)) for idx, log_line in
                 enumerate(global_variables.redis_db_obj["logs"][: args[0]])]
                return True
            elif special_word == "last":
                [print("{}.\t{}".format(idx + 1, log_line)) for idx, log_line in
                 enumerate(global_variables.redis_db_obj["logs"][-args[0]:])]
                return True

    return False


def __show_log() -> bool:
    if not global_variables.redis_db_obj.is_valid_command("logs", None):
        print(global_variables.UNKNOW_REDIS_DB_MSG.format("logs"))

    if len(global_variables.redis_db_obj["logs"]) >= 1:
        print(global_variables.redis_db_obj["logs"][-1])
    else:
        print("The '{}' database is empty".format("logs"))

    return True


def __show_node_status() -> bool:
    if not global_variables.redis_db_obj.is_valid_command("status", None):
        print(global_variables.UNKNOW_REDIS_DB_MSG.format("status"))
    else:
        if len(global_variables.redis_db_obj["status"]) == 0:
            print("The '{}' database is empty".format("status"))
        else:
            print(global_variables.redis_db_obj["status"])

    return True


def __show_system_status():
    # DAL.get({"status": ["system"])})
    print("Show system status")


def __add_new_entry(special_word: str, args: list) -> bool:
    if special_word in ["timer", "ip"]:
        if not (global_variables.redis_db_obj.is_valid_command("config", special_word)):
            print(global_variables.UNKNOW_VAR_REDIS_MSG.format(special_word, "config"))
        if (special_word == "timer" and global_variables.LOW_RANGE_TIMER <= args[0]
                <= global_variables.HIGH_RANGE_TIMER) or special_word == "ip":
            global_variables.redis_db_obj["config"][special_word] = args[0]

    elif special_word in ["add", "edit", "delete"]:
        if not global_variables.redis_db_obj["status"]["status"] == "leader":
            print(global_variables.PERMISSION_DENIED)
        # elif not (global_variables.redis_db_obj.is_valid_command("values", None)):
        #     print(global_variables.UNKNOW_VAR_REDIS_MSG.format(args[0], "values"))
        else:
            log_id = len(global_variables.redis_db_obj["logs"])
            if len(args) == 1:
                args.append(None)

            result = raft_python_callback.start_commit_process(log_id, special_word, args[0], args[1])
            if result:
                print("command was successfully executed!")
            else:
                print("Oh no! something went wrong... your command was not executed.")
    else:
        return False
    return True


def init_trie_functions_and_info(separator: str) -> Tuple[StringTrie, StringTrie, dict]:
    if len(separator) != 1:
        raise ValueError("SeparatorError: the separator must be char and not string!")

    commands_trie, commands_info_trie = StringTrie(separator=separator), StringTrie(separator=separator)

    # Defining commands according to the amount of words in the command
    # 2 words command
    commands_trie["show log"] = __show_log
    # commands_trie["show logs"] = __show_logs
    commands_trie["set timer"] = __add_new_entry

    # 3 words command
    commands_trie["show logs all"] = __show_logs_all
    commands_trie["show logs last"] = __show_logs
    commands_trie["show logs top"] = __show_logs
    commands_trie["show logs range"] = __show_logs
    commands_trie["show node status"] = __show_node_status
    # commands_trie["show system status"] = show_system_status
    commands_trie["set multicast ip"] = __add_new_entry
    commands_trie["log value add"] = __add_new_entry
    commands_trie["log value delete"] = __add_new_entry
    commands_trie["log value edit"] = __add_new_entry

    # Defines the explanations at each node and what values it receives
    # 1 word command
    commands_info_trie["show"] = "Prints data on CLI"
    commands_info_trie["set"] = "Sets a value in the database"
    commands_info_trie["log"] = "Displays actions on LOGS in RAFT algorithm"

    # 2 words command
    commands_info_trie["show node"] = "action on node parameters"
    # commands_info_trie["show system"] = "action on system parameters (all nodes in RAFT)"
    commands_info_trie["show log"] = "shows last log in node"
    commands_info_trie["show logs"] = "shows all logs in node"
    commands_info_trie["set multicast"] = "action on multicast IP"
    commands_info_trie["set timer"] = "A number between 0.150 to 0.300"
    commands_info_trie["log value"] = "A set of actions functions like add, delete, edit ..."

    # 3 words command
    commands_info_trie["show node all"] = "Shows all logs in cluster"
    commands_info_trie["show logs last"] = "A positive number greater than 0"
    commands_info_trie["show logs top"] = "A positive number greater than 0"
    commands_info_trie["show logs range"] = "A range between two positive numbers"
    commands_info_trie["show node status"] = "Shows all node parameters in cluster"
    # commands_info_trie["show system status"] = "Shows all nodes parameters in system"
    commands_info_trie["set multicast ip"] = "IP A.B.C.D according to the protocol"
    commands_info_trie["log value add"] = "Adds a new log <key, value> to the cluster"
    commands_info_trie["log value delete"] = "Deletes log <key> from the cluster"
    commands_info_trie["log value edit"] = "Edits log <key> in the cluster"

    special_words_dict = {"range": (2, [int, int]), "add": (2, [str, int]), "ip": (1, [str]), "timer": (1, [float]),
                          "delete": (1, [str]), "edit": (2, [str, int]), "top": (1, [int]), "last": (1, [int])}
    # "node": (1, [int]), "status": (1, [int])

    return commands_trie, commands_info_trie, special_words_dict
