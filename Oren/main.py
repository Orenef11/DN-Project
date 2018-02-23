import DAL
from RAFTCmd import RAFTCmd

def main():
    print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
          "Main function for testing!!!!!\n"
          "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n")
    commands_trie, commands_info_trie = DAL.init_trie_function_and_info(' ')
    RAFTCmd(commands_trie, commands_info_trie).cmdloop()


if __name__ == "__main__":
    main()
