from cmd import Cmd
from pygtrie import StringTrie
from typing import Tuple
from colorama import Style, Fore
from global_variables import UNSUCESS_COMMAND_MSG


class RAFTCmd(Cmd):
    """Welcome to RAFT command line.
 The command line supports a large number of commands.
 You can get full details about each command using the special character '?'"""

    def __init__(self, commands_trie: StringTrie, commands_info_trie: StringTrie, special_words_dict: dict,
                 separator=' ', prompt_name="RAFT-CLI>"):
        super(RAFTCmd, self).__init__()

        self.__separator = separator
        self.prompt = prompt_name
        self.__commands_trie = commands_trie
        self.__special_words_dict = special_words_dict
        self.__special_words_size_list = []
        self.__max_special_words_size = 2
        self.__commands_info_trie = commands_info_trie

    # Checking values received from a programmer. Check if they are by format.
    def preloop(self):
        try:
            for value in self.__special_words_dict.values():
                check_types_flag = all([value_type in [int, float, str] for value_type in value[1]])
                if not (type(value[0]) is int and type(value[1]) is list and check_types_flag):
                    raise ValueError("The 'special_words_dict' is not in the following format: list(number, "
                                     "list(type, type)")

            for func in self.__commands_trie.values():
                if not callable(func):
                    raise ValueError("The 'commands_trie' must include function in node")
        except Exception as e:
            raise e
        self.__special_words_size_list = [value[0] for value in self.__special_words_dict.values()]
        self.__special_words_size_list.sort()
        print("{}{}". format(Style.BRIGHT, Fore.CYAN), RAFTCmd.__doc__, Style.RESET_ALL)

    def do_help(self, *arg):
        print("The system support this function:")
        [print("{}. - {}".format(idx + 1, func)) for idx, func in enumerate(self.__commands_trie.keys())]

    def do_EOF(self, *args) -> bool:
        self.__warning_msg("Close the RAFT command line")
        return True

    def do_close(self, *args) -> bool:
        self.__warning_msg("Close the RAFT command line")
        return True

    def do_exit(self, *args) -> bool:
        self.__warning_msg("Close the RAFT command line")
        return True

    def do_quit(self, *args) -> bool:
        self.__warning_msg("Close the RAFT command line")
        return True

    def __has_valid_command(self, command: str) -> bool:
        return command in self.__commands_trie

    def __has_valid_info_command(self, command: str) -> bool:
        return command in self.__commands_info_trie

    def __command_complete(self, command: str):
        if not self.__has_valid_info_command(command):
            self.__error_msg("Unrecognized command")
            return False

        commands_list = self.__commands_info_trie.keys(prefix=command)
        # if commands_list[-3] in self.__special_words_dict or:

        if len(commands_list) == 0:
            self.__error_msg("Unrecognized command")
        else:
            command_size = len(command.split(self.__separator)) + 1
            next_commands_list = [command_temp.split(self.__separator)[-1] for command_temp in commands_list
                                  if len(command_temp.split(self.__separator)) == command_size]

            command += self.__separator
            for next_command in next_commands_list:
                print("{:20}{}".format(next_command, self.__commands_info_trie[command + next_command]))

    @staticmethod
    def __check_value_type(type_var: type, value: str):
        try:
            if type_var is str:
                return str(value)
            elif type_var is int:
                return int(value)
            elif type_var is float:
                return float(value)
        except Exception as _:
            raise ValueError("ValueTypeError: The type of variable in the command is not according to the format "
                             "specified!")
        return None

    def __parser_and_run_command(self, command: str) -> Tuple[bool, list, str, str]:
        command_key_list = command.split(self.__separator)
        try:
            for special_word_size in self.__special_words_size_list:
                if special_word_size + 1 > len(command_key_list):
                    break
                special_word = command_key_list[-1 - special_word_size]
                if special_word in self.__special_words_dict:
                    if self.__has_valid_command(self.__separator.join(command_key_list[:-special_word_size])):
                        has_valid_command_flag, parameters_list = True, []
                        for idx in range(special_word_size):
                            parameters_list.append(self.__check_value_type(
                                self.__special_words_dict[special_word][1][idx],
                                command_key_list[-special_word_size + idx]))
                            if parameters_list[-1] is None:
                                has_valid_command_flag = False
                                break

                        if has_valid_command_flag is True:
                            return True, parameters_list, special_word, \
                                   self.__separator.join(command_key_list[:-special_word_size])
        except Exception as e:
            self.__error_msg(e)
        return False, [], "", ""

    @staticmethod
    def __success_msg(msg):
        print("{}{}{}{}".format(Style.BRIGHT, Fore.GREEN, msg,  Style.RESET_ALL))

    @staticmethod
    def __warning_msg(msg):
        print("{}{}{}{}".format(Style.BRIGHT, Fore.YELLOW, msg,  Style.RESET_ALL))

    @staticmethod
    def __error_msg(msg):
        print("{}{}{}{}".format(Style.BRIGHT, Fore.RED, msg,  Style.RESET_ALL))

    def default(self, command: str) -> None:
        command = command.lower()

        if '?' == command[-1]:
            command, is_command_complete_flag = command[:-1].strip(), True
            if self.__has_valid_command(command):
                if len(self.__commands_info_trie.keys(prefix=command)) == 1:
                    print("<cr>")
                    is_command_complete_flag = False
            if is_command_complete_flag:
                self.__command_complete(command)
        elif self.__has_valid_command(command):
            special_word = command.split(self.__separator)[-1]
            if special_word in self.__special_words_dict:
                self.__error_msg("The command expects to receive '{}' arguments of the following types '{}'".format(
                    *self.__special_words_dict[special_word]))
            else:
                self.__commands_trie[command]()
            # if not self.__commands_trie[command]():
            #     self.__error_msg("The command was not executed!")

        else:
            has_valid_command_flag, command_args_list, special_word, command = self.__parser_and_run_command(command)
            is_command_success_flag = False
            if has_valid_command_flag and self.__commands_trie[command](special_word, command_args_list):
                    is_command_success_flag = True

            if not is_command_success_flag:
                self.__error_msg("Unrecognized command")


