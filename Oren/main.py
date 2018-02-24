from RAFTCmd import RAFTCmd
from sys import stderr
from traceback import print_exc

from cli_functions import init_trie_function_and_info


def main():
    print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
          "Main function for testing!!!!!\n"
          "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n")
    try:
        separator = ' '
        commands_trie, commands_info_trie, special_words_dict = init_trie_function_and_info(separator)
        RAFTCmd(commands_trie, commands_info_trie, special_words_dict, separator).cmdloop()
    except Exception as e:
        print_exc()
        print("{}: {}".format(type(e), str(e)), file=stderr)


if __name__ == "__main__":
    main()
