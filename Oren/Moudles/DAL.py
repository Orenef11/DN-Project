from pygtrie import StringTrie
from typing import Tuple

SUCCESS = 1
FAILS = 0


def show_all_logs() -> None:
    print("Print all logs in database")


def show_range_logs(start: int, end: int) -> None:
    print("Print logs from {} to {}".format(start, end))


def show_node_status() -> None:
    print("Show node status")


def show_system_status() -> None:
    print("Show system status")


def add_log(log_key: str, log_value: int) -> int:
    print("The '{}' key successfully added to database".format(log_key))

    return SUCCESS


def delete_log(log_key: str, log_value: int) -> int:
    print("The '{}' key successfully removed from database".format(log_key))

    return SUCCESS


def edit_log(log_key: str, log_value: int) -> int:
    print("The '{}' key successfully edited from '{}' to '{}' value".format(log_key, "old value", log_value))

    return SUCCESS


def init_trie_function_and_info(separator: str) -> Tuple[StringTrie, StringTrie]:
    if len(separator) != 1:
        raise ValueError("SeparatorError: the separator must be char and not string!")

    print("Definde all function that cli support!")
    commands_trie, commands_info_trie = StringTrie(separator=separator), StringTrie(separator=separator)
    commands_trie["show logs"] = show_all_logs
    commands_trie["add logs"] = add_log
    commands_trie["edit logs"] = edit_log
    commands_trie["delete logs"] = delete_log

    commands_trie["show logs range"] = show_range_logs
    commands_info_trie["show system status"] = show_system_status

    commands_info_trie["show"] = "Printing data on CLI"
    commands_info_trie["add"] = "Add new entry to dataset"
    commands_info_trie["edit"] = "Edit entry by key in dataset"
    commands_info_trie["delete"] = "Delete entry by key in dataset"

    commands_info_trie["add logs"] = "entry <key, value>"
    commands_info_trie["edit logs"] = "entry <key, value>"
    commands_info_trie["delete logs"] = "entry <key, value>"
    commands_info_trie["show logs"] = "Displays logs from database"
    commands_info_trie["show system"] = "Displays information on RAFT"

    commands_info_trie["show logs range"] = "Displays logs in between [X, Y]"
    commands_info_trie["show system status"] = "Displays current status on RAFT"

    return commands_trie, commands_info_trie
