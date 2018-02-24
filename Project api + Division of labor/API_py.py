class DAL(object):
	
	
	#key,value are string 
	def update_db(key,value):
		return bool
	
	def delete(key):
		return bool
	#from start to end inclusive
	def get_range_log(start_idx,end_idx)
		return list
	
	def show_DB():
		return list # of <key,value>
		
	def get_value_from_DB(key):
		return value
	
	#ip = string, port = int
	def init(redis_ip,redis_port):
		#void

class raft_run(object):
	def add_log_to_DB(commit_id):
		#commit
		#c call to this func
	
	def clear_uncommit_log(last_commit_index):
		#delete uncommit message from log
		#c call to this func
	def update_current_state(key,value):
		#c call to this func
	
	#int run_raft(char* ip,int port,int id,int members_num,int leader_timeout);
	def run_raft(raft_ip,raft_port,raft_id,members_num,leader_timeout):
		#just call to c
	
	def start_commit_process(key,value):
		#get last_log from redis
		#call to c with last,key,val
		#wait for signal SIGUSR1 for success , SIGUSR2 for failed
		return bool
	
	
		
class CLI(object):
	
	def get_input_from_user():
		#while TRUE
		return user_input
	#input is string
	def parse_input(user_input):
		return tuple_of_func_and_args_to_func ????
	
	#user_output can be list of string or list of tuple or just string accourding to output_type 
	def print_to_cli(user_output,output_type):
		pass
	
	def invalid_input(user_input):
		return invalid_input_message_for_user
	
	def translate_log_value_command(user_input_args):
		return function_for_add/edit or for delete
		
	def find_command_handler_in_dict(command):
		return function_handler
	
	#main
	def run_cli():
		pass
		
class Main(object):
	def get_args_from_user():
		pass
	
	def run():
		pass
