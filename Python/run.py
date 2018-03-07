#!bin/bash/
from sys import stderr
from traceback import print_exc
from os import path, getcwd, makedirs
import signal
from Callback_Functions import raft_thread
from time import sleep
from shutil import rmtree
from pygtrie import StringTrie
from typing import Tuple
from concurrent.futures import ThreadPoolExecutor

from Moudles import RedisDB
from Moudles import config_parser
from Moudles import RAFTCmd
from Moudles import logger
from Callback_Functions import cli_callback
import global_variables


def main():
    print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
          "Common Logic!!!!!\n"
          "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n")
    common_logic_file_path = path.join(getcwd(), "Log_Files", "common_logic.log")
    logs_folder_path = path.split(common_logic_file_path)[0]
    if path.isdir(logs_folder_path):
        rmtree(logs_folder_path)
        sleep(0.5)
    makedirs(logs_folder_path)

    logger.setting_up_logger("debug", "critical", common_logic_file_path)
    config_dict = config_parser.get_config_variables(path.join(getcwd(),"Python","Configuration", "config.ini"))
    if config_dict["commandline"]["separator"] == '':
        config_dict["commandline"]["separator"] = ' '

    commands_trie, commands_info_trie, special_words_dict = \
        cli_callback.init_trie_function_and_info(config_dict["commandline"]["separator"])
    global_variables.raft_cmd_obj = RAFTCmd.RAFTCmd(commands_trie, commands_info_trie, special_words_dict)
    name_and_value_db_list = \
        [("config", {}), ("logs", [("Delete", "Oren"), ("Delete", "Oren"), ("Delete", "Oren"), ("Delete", "Oren"),
                                   ("Delete", "Oren"), ("Delete", "Oren"), ("Delete", "Oren"), ("Delete", "Oren")]), ("values", {}),
         ("status", {"status": "leader", "leader_id": 2, "applied_last_idx": -1, "commit_idx": -1})]
    global_variables.redis_db_obj =\
        RedisDB.RedisDB(config_dict["raft"]["ip"], config_dict["raft"]["port"], name_and_value_db_list)

    signal.signal(signal.SIGUSR1, signal.SIG_IGN)
    signal.signal(signal.SIGUSR2, signal.SIG_IGN)
    
    with ThreadPoolExecutor(max_workers=2) as e:
        e.submit(raft_thread.python_run_raft).done()
        e.submit(global_variables.raft_cmd_obj.cmdloop).done()

    print("exit successfully")




if __name__ == "__main__":
    main()
