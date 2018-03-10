#include "../Headers/logger.h"

///////////////////////////////////////////////////////////////////
//#define WRITE_TO_LOGGER(logger_level,problem_desc,msg_type,args_num,...) ( \
				__WRITE_TO_LOGGER__(sharedRaftData.python_functions.write_to_logger,logger_level,problem_desc,msg_type,args_num,...) )
////////////////////////////////////////////////////////////////

typedef struct thread_data{
	char logger_msg[MAX_LOGGER_MSG];
	int logger_level;
	int (*write_to_logger)(int logger_level,char* logger_info);
}thread_data;

char * get_data_after_last_seperator(char * data,char seperator){
	char * pointer = data;
	int loc = 0,rv_loc=0;
	if(!data){
		return NULL_STRING;
	}
	while(*pointer){
		if(*pointer == seperator){
			rv_loc=loc+1;
		}
		loc++;
		pointer++;
	}
	return data +rv_loc;
}

void * write_to_logger_thread(void * args)
{
	thread_data *data = (thread_data *)args;
	data->write_to_logger(data->logger_level,data->logger_msg);
	free(data);
}
//do not call this function directly - call WRITE_TO_LOGGER
int add_logger_msg(int (*write_to_logger)(int logger_level,char* logger_info),int logger_level,const char * file,const char *func_name,const int line,
				const char* problem_desc, int msg_type,int args_num,...){
	pthread_t IO_thread;
/*
	thread_data *data= (thread_data *)malloc(sizeof(thread_data));
	if(!data){
		return 0;
	}
*/
	char logger_msg[MAX_LOGGER_MSG];
	int loc=0,is_valid = MAX_LOGGER_MSG;
	va_list ap;
	va_start(ap, args_num);
	loc=sprintf(logger_msg,"FILE:%s, FUNCTION:%s, LINE:%d\nMESSAGE:%s",
					file,func_name,line,problem_desc);
	if(args_num>0){
		loc+=sprintf(logger_msg+loc, "\nVARIABLES: ");
	}
	for(int i=0;i<args_num && (is_valid>0);i++){
		loc+= sprintf(logger_msg+loc," %s=",va_arg(ap,char*));
		if(msg_type == INT_VALUES){
			loc+= sprintf(logger_msg+loc, "%d ",va_arg(ap,int));
		}
		else {
			loc+= sprintf(logger_msg+loc, "%s ",va_arg(ap,char *));
		}
		if(args_num % 3 ==0 ){
			loc+=sprintf(logger_msg+loc, "\n\t");
			
		}
		is_valid=MAX_LOGGER_MSG-loc;
	}
	va_end(ap);
	if(is_valid<1){
		loc=MAX_LOGGER_MSG-5;
	}
	logger_msg[loc]='\n';
	logger_msg[loc+1]=0;
	//puts(logger_msg);
	//puts("1111111111111111111111111");
    	//pthread_create(&IO_thread, NULL, write_to_logger_thread, (void*)data);
	//return 0;
	return write_to_logger(logger_level,logger_msg);
}
