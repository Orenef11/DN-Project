from cmd import Cmd
from pygtrie import StringTrie


class RAFTCmd(Cmd):
    """    Welcome to RAFT command line.
    The command line supports a large number of commands.
    Can you get full details about each command using the special character '?'"""

    def __init__(self, commands_trie, commands_info_trie, prompt_name="RAFT-CLI>", separator=' '):
        print(RAFTCmd.__doc__)
        super(RAFTCmd, self).__init__()

        self.__separator = separator
        self.prompt = prompt_name
        self.__commands_trie = commands_trie
        self.__commands_info_trie = commands_info_trie

    def __has_valid_command(self, command):
        return command in self.__commands_trie

    def __command_complete(self, command):
        if not self.__has_valid_command(command):
            print("Unrecognized command")
            return False

        commands_list = self.__commands_trie.keys(command)
        if len(commands_list) == 0:
            print("Unrecognized command")
        else:
            command_size = len(command.split(self.__separator)) + 1
            next_commands_list = [command_temp.split(self.__separator)[-1] for command_temp in commands_list
                                  if len(command_temp.split(self.__separator)) == command_size]

            command += self.__separator
            for next_command in next_commands_list:
                print("{:20}{}".format(next_command, self.__commands_info_trie[command + next_command]))

    def default(self, command):
        command = command.lower()
        if command in ["exit", "exit()", "close", "close()"]:
            exit()
        if '?' == command[-1]:
            command = command[:-1].strip()
            self.__command_complete(command)
        if self.__has_valid_command(command):
            self.__commands_trie[command]()

