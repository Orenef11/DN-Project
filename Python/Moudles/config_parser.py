from configparser import ConfigParser
from os import path
from ast import literal_eval
import logging

def __check_config_variables_type_and_get_values(parser, section_name, config_variables_type_list):
    variables_dict, var_type = {}, ""
    for idx, var_key in enumerate(parser[section_name].keys()):
        try:
            var_type = config_variables_type_list[idx]
            if var_type is bool:
                var = parser.getboolean(section_name, var_key)
            elif var_type is int:
                var = parser.getint(section_name, var_key)
            elif var_type is float:
                var = parser.getfloat(section_name, var_key)
            elif var_type is str:
                var = parser.get(section_name, var_key)
            else:
                try:
                    var = literal_eval(parser.get(section_name, var_key))
                except Exception as _:
                    logging.debug("\nThe value of the variable '{}' in '{}' section is unknown to the system ("
                                "not of the types supported by Python)".format(var_key, section_name))
                    exit()
                var_type = type(var)
            # if type(var) in [dict, set, list, str, float, int, bool]:
            try:
                variables_dict[var_key] = var
            except Exception as e:
                logging.debug("\nValue type is not supported by system (we support 'int', 'float', 'bool', 'list'"
                              ", 'dict' and 'set')")
                exit()
        except Exception as e:
            logging.debug("\nError: The '{}' variable type in section '{}' not supported! (Expected to '{}', get '{}'"
                          ")".format(var_key, section_name, config_variables_type_list[idx], type(var_key)))
            logging.debug("\nSystem error message:\n{}".format(str(e)))
            exit()
    return variables_dict


def __config_parser(config_file_path, config_variables_type_dict):
    parser, config_variables_dict = ConfigParser(), {}

    if not path.isfile(config_file_path):
        logging.debug("\nFileNotExist: The '{}' file that not found!".format(config_file_path))
        exit()

    parser.read(config_file_path)
    for section_name in parser.sections():
        if section_name in config_variables_type_dict.keys():
            config_variables_dict[section_name.lower()] = __check_config_variables_type_and_get_values(
                parser, section_name, config_variables_type_dict[section_name])

    return config_variables_dict


def get_config_variables(config_file_path):
    config_variables_type_dict = {"Multicast": [str, int],
                                  "CommandLine": [str, str],
                                  "RAFT": [str, int, int, float],
                                  "Debug": [bool]}
    return __config_parser(config_file_path, config_variables_type_dict)
