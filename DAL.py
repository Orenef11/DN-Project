from redis import Redis
from collections import MutableMapping
from pickle import loads, dumps


class RedisDB(MutableMapping):
    _instances = {}

    def __call__(cls, *args, **kwargs):
        if cls not in cls._instances:
            cls._instances[cls] = super(RedisDB, cls).__call__(*args, **kwargs)
        return cls._instances[cls]

    def __init__(self,  host: str, port: int, db_name_and_type_list: list):
        self.__db_data = Redis(host=host, port=port)

        self.__db_data.flushdb()
        for (db_name, db_type) in db_name_and_type_list:
            if type(db_type) is dict:
                self.__db_data[dumps(db_name)] = dumps({})
            elif type(db_type) is list:
                self.__db_data[dumps(db_name)] = dumps([])

    def __getitem__(self, key):
        return loads(self.__db_data[dumps(key.lower)])

    def __setitem__(self, key, value):
        self.__db_data[dumps(key.lower())] = dumps(value)

    def __delitem__(self, key):
        del self.__db_data[dumps(key.lower())]

    def __iter__(self):
        return iter(self.keys())

    def __len__(self):
        return len(self.__db_data.keys())

    def __recursion_parser(self, value_obj, key):
        temp_list = []
        if type(value_obj) is dict:
            temp_list += self.__parser_to_string(value_obj, key)
        elif type(value_obj) is list:
            temp_list += self.__parser_to_string(value_obj, key)
        elif type(value_obj) is tuple:
            temp_list.append("'{}' <{}>".format(key, ','.join(str(value) for value in value_obj)))
        else:
            temp_list.append("Unsupported object type! The type of '{}' is '{}'".format(key, type(value_obj)))

        return temp_list

    def __parser_to_string(self, obj_data, obj_name):
        temp_list = []
        if type(obj_data) is dict:
            temp_list.append("\nThe '{}' dictionary data:".format(obj_name))
            for obj_name, obj_value in obj_data.items():
                if type(obj_value) in [int, float, str]:
                    temp_list.append("<{}, {}>".format(obj_name, obj_value))
                else:
                    temp_list += self.__recursion_parser(obj_value, obj_name)

        elif type(obj_data) is list:
            temp_list.append("\nThe '{}' list data:".format(obj_name))
            for obj_name, obj_value in enumerate(obj_data):
                if type(obj_value) in [int, float, str]:
                    temp_list.append("<{}, {}>".format(obj_name, obj_value))
                else:
                    temp_list += self.__recursion_parser(obj_value, obj_name)

        elif type(obj_data) is tuple:
            temp_list.append("\nThe '{}' tuple data:".format(obj_name))
            for obj_name, obj_value in enumerate(obj_data):
                if type(obj_value) in [int, float, str]:
                    temp_list.append("<{}, {}>".format(obj_name, obj_value))
                else:
                    temp_list += self.__recursion_parser(obj_value, obj_name)

        return temp_list

    def __str__(self):
        str_print_list = ["The Redis database:"]
        for obj_name, obj_value in self.items():
            str_print_list += self.__recursion_parser(obj_value, obj_name)

        # str_print_list.append("The '{}' is regular type and the value is {}:".format(key, value_obj))
        return '\n'.join(str_print_list)

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
    # def update_value(self,  sub_db_name, key_name, value) -> bool:
    #     if sub_db_name not in self.__db_data or key_name in self.__db_data[sub_db_name]:
    #         return False
    #
    #     self.__db_data[sub_db_name][key_name] = value
    #     self.__db_data["logs"].append(("update", key_name, value))
    #     return True

#
#
# d = RedisDB("localhost", 7777, [("logs", []), ("data", {}), ("config", {}), ("status", {})])
# d["logs"] = [("add", "x", 3), ("delete", "X"), ("edit", "x", 10)]
# print(d)
# d["data"] = {'x': 3}
# d["config"] = {"member_size": -1, "ip_multicast": "", "port": -1, "leader_timeout": -1, "debug_mode": False}
# d["status"] = {"node": {"status": "", "leader_id": 2, "applied_last_idx": -1, "commit_idx": -1},
#                "system": {}}
#
#
# x = RedisDB("localhost", 7777)
# y = RedisDB("localhost", 7777)
# #
# print(y["logs"])
# y["logs"] = [(), ()]
# print(y["logs"])
