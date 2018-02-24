from pygtrie import StringTrie
from typing import Tuple

SUCCESS = 1
FAILS = 0


def show_logs(special_word, args):
    """
    DAL.set({function_name: (sub_method, low_range_value, high_range_value)})
    Add function get dict.
    A key indicates which action to do and the value is a tuple of parameters

    :param special_word: Sub-function must be run to get the data
    :param args: The arguments that the function accepts
    :return:
    """
    if special_word == "":
        print("show all logs in cluster")
    elif len(args) == 2:
        if special_word == "range":
            print("show all range between {}-{}".format(args[0], args[1]))
            # DAL.get({log: [special_word, low_range_value, high_range_value]})

    elif len(args) == 1 and special_word in ["top", "last", "edit"]:
        print("{} command, key: {}".format(special_word, args[0]))
        # DAL.get({log: [special_word, value]})
    else:
        return None


def show_log():
    # DAL.get({log: ["last", 1]})
    print("Show last log")


def show_node_status():
    # DAL.get({"status": ["node"])})
    print("Show node status")


def show_system_status():
    # DAL.get({"status": ["system"])})
    print("Show system status")


def add_new_entry(special_word, args):
    if len(args) == 1:
        if special_word in ["timer", "ip", "edit"]:
            print("{} command, key: {}".format(special_word, args))
            # DAL.add({special_word: [key, value]})
        elif special_word == "delete":
            print("Delete, key: {}".format(args))
            # DAL.delete({"log": [key, value]})
    elif len(args) == 2 and special_word == "add":
        print("{} command, key: {}".format(special_word, args))
        # DAL.add({special_word: [key, value]})


def init_trie_function_and_info(separator: str) -> Tuple[StringTrie, StringTrie, dict]:
    if len(separator) != 1:
        raise ValueError("SeparatorError: the separator must be char and not string!")

    commands_trie, commands_info_trie = StringTrie(separator=separator), StringTrie(separator=separator)

    # Defining commands according to the amount of words in the command
    # 2 words command
    commands_trie["show log"] = show_log
    commands_trie["show logs"] = show_logs
    commands_trie["set timer"] = add_new_entry

    # 3 words command
    commands_trie["show logs last"] = show_logs
    commands_trie["show logs top"] = show_logs
    commands_trie["show logs range"] = show_logs
    commands_trie["show node status"] = show_node_status
    commands_trie["show system status"] = show_system_status
    commands_trie["set multicast ip"] = add_new_entry
    commands_trie["log value add"] = add_new_entry
    commands_trie["log value delete"] = add_new_entry
    commands_trie["log value edit"] = add_new_entry

    # Defines the explanations at each node and what values it receives
    # 1 word command
    commands_info_trie["show"] = "Printing data on CLI"
    commands_info_trie["set"] = "Set a value in the database"
    commands_info_trie["log"] = "Displays actions on LOGS in RAFT algorithm"

    # 2 words command
    commands_info_trie["show node"] = "action on node parameters"
    commands_info_trie["show system"] = "action on system parameters (all nodes in RAFT)"
    commands_info_trie["show log"] = "show last log in node"
    commands_info_trie["show logs"] = "show all logs in node"
    commands_info_trie["set multicast"] = "action on multicast ip"
    commands_info_trie["set timer"] = "Number between 0.150 to 0.300"
    commands_info_trie["log value"] = "Set of actions functions like add, delete, edit ..."

    # 3 words command
    commands_info_trie["show logs last"] = "A positive number greater than 0"
    commands_info_trie["show log top"] = "A positive number greater than 0"
    commands_info_trie["show logs range"] = "Range between two positive numbers"
    commands_info_trie["show node status"] = "show all node parameters in cluster"
    commands_info_trie["show system status"] = "show all nodes parameters in system"
    commands_info_trie["set multicast ip"] = "ip A.B.C.D according to the protocol"
    commands_info_trie["log value add"] = "Add a new log <key, value> to the system"
    commands_info_trie["log value delete"] = "Delete log <key> from the system"
    commands_info_trie["log value edit"] = "Edit log <key> from the system"

    special_words_dict = {"range": (2, [int, int]), "add": (2, [str, int]), "ip": (1, [str]), "timer": (1, [float]),
                          "delete": (1, [str]), "edit": (2, [str, int]), "top": (1, [int]), "last": (1, [int])}
    # "node": (1, [int]), "status": (1, [int])

    return commands_trie, commands_info_trie, special_words_dict
