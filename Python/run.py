#!bin/bash/

from os import path, getcwd, makedirs
import signal
from Callback_Functions.raft_python_callback import python_run_raft
from time import sleep
from shutil import rmtree
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
    config_dict = config_parser.get_config_variables(path.join(getcwd(), "Python", "Configuration", "config.ini"))
    if config_dict["commandline"]["separator"] == '':
        config_dict["commandline"]["separator"] = ' '

    config_dict["multicast"]["ip"] = str.encode(config_dict["multicast"]["ip"])
    commands_trie, commands_info_trie, special_words_dict = \
        cli_callback.init_trie_functions_and_info(config_dict["commandline"]["separator"])
    global_variables.raft_cmd_obj = RAFTCmd.RAFTCmd(commands_trie, commands_info_trie, special_words_dict)
    name_and_value_db_list = \
        [("config", {}), ("logs", [("add", "x", 1), ("add", "y", 1)]), ("values", {}),
         ("status", {"status": "follower", "leader_id": -1, "applied_last_idx": -1, "commit_idx": -1,"term":-1})]
    global_variables.redis_db_obj = \
        RedisDB.RedisDB(config_dict["redis"]["ip"], config_dict["redis"]["port"], name_and_value_db_list)
    
    signal.signal(signal.SIGUSR1, signal.SIG_IGN)
    signal.signal(signal.SIGUSR2, signal.SIG_IGN)

    with ThreadPoolExecutor(max_workers=2) as e:
        e.submit(python_run_raft, config_dict["multicast"]["ip"], config_dict["multicast"]["port"],
                 config_dict["raft"]["my_id"], config_dict["raft"]["members_size"],
                 config_dict["raft"]["leader_timeout"]).done()
        e.submit(global_variables.raft_cmd_obj.cmdloop).done()

    print("exit successfully")


if __name__ == "__main__":
    main()
