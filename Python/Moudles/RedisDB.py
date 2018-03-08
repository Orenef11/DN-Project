from redis import Redis
from collections import MutableMapping
from pickle import loads, dumps


class RedisDB(MutableMapping):
    def __init__(self,  host: str, port: int, db_name_and_obj_list: list):
        self.__db_data = Redis(host=host, port=port)
        self.__db_data.flushdb()

        for (db_name, db_obj) in db_name_and_obj_list:
            self.__setitem__(db_name, db_obj)

    def __getitem__(self, key):
        return loads(self.__db_data[dumps(key.lower())])

    def __setitem__(self, key, value):
        if type(key) is tuple or type(key) is list:
            keys_tuple = tuple(sub_key.lower() if type(sub_key) is True else sub_key for sub_key in key)
            sub_dict_temp = loads(self.__db_data[dumps(keys_tuple[0])])
            temp = sub_dict_temp
            for sub_key in keys_tuple[1: -1]:
                temp = temp[sub_key]
            temp[keys_tuple[-1]] = value
            self.__db_data[dumps(key[0])] = dumps(sub_dict_temp)
        else:
            self.__db_data[dumps(key.lower())] = dumps(value)

    def __delitem__(self, key):
        del self.__db_data[dumps(key.lower())]

    def __iter__(self):
        return iter(self.keys())

    def __len__(self):
        return len(self.__db_data.keys())

    def __str__(self):
        str_print_list = ["The Redis database:"]
        for obj_name, obj_value in self.items():
            str_print_list += self.__recursion_parser(obj_value, obj_name, "")

        return '\n'.join(str_print_list)

    def __recursion_parser(self, value_obj, key, spaces):
        temp_list, temp_spaces = [], spaces
        if type(value_obj) is dict:
            temp_list += self.__parser_to_string(value_obj, key, spaces)
            temp_list.append(temp_spaces + "End Dict")
        elif type(value_obj) is list:
            temp_list += self.__parser_to_string(value_obj, key, spaces)
            temp_list.append(temp_spaces + "End List")
        elif type(value_obj) is tuple:
            temp_list.append("{}'{}' <{}>".format(spaces, key, ','.join(str(value) for value in value_obj)))
        else:
            temp_list.append("Unsupported object type! The type of '{}' is '{}'".format(key, type(value_obj)))

        return temp_list

    def __parser_to_string(self, obj_data, obj_name, spaces):
        temp_list = []
        if type(obj_data) is dict:
            temp_list.append("{}The '{}' dictionary data:".format(spaces, obj_name))
            spaces += '\t'
            for obj_name, obj_value in obj_data.items():
                if type(obj_value) in [bool, int, float, str]:
                    temp_list.append("{}{}: {}".format(spaces, obj_name, obj_value))
                else:
                    temp_list += self.__recursion_parser(obj_value, obj_name, spaces)

        elif type(obj_data) is list:
            temp_list.append("{}The '{}' list data:".format(spaces, obj_name))
            spaces += '\t'
            for obj_name, obj_value in enumerate(obj_data):
                if type(obj_value) in [bool, int, float, str]:
                    temp_list.append("{}({}, {})".format(spaces, obj_name, obj_value))
                else:
                    temp_list += self.__recursion_parser(obj_value, obj_name, spaces)

        elif type(obj_data) is tuple:
            temp_list.append("{}The '{}' tuple data:".format(spaces, obj_name))
            spaces += '\t'
            for obj_name, obj_value in enumerate(obj_data):
                if type(obj_value) in [bool, int, float, str]:
                    temp_list.append("{}<{}, {}>".format(spaces, obj_name, obj_value))
                else:
                    temp_list += self.__recursion_parser(obj_value, obj_name, spaces)

        return temp_list

    def keys(self):
        return [loads(x) for x in self.__db_data.keys()]

    def clear(self):
        self.__db_data.flushdb()

    def is_valid_command(self, sub_db, key_name) -> bool:
        # logs list dont have key
        sub_db = dumps(sub_db)
        if sub_db in self.__db_data and (key_name is None or key_name in loads(self.__db_data[sub_db])):
            return True

        return False





