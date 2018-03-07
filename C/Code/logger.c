#include "../Headers/logger.h"


//uses

//add this lines to a global H file

///////////////////////////////////////////////////////////////////
//#define WRITE_TO_LOGGER(logger_level,problem_desc,msg_type,args_num,...) ( \
				__WRITE_TO_LOGGER__(sharedRaftData.python_functions.write_to_logger,logger_level,problem_desc,msg_type,args_num,...) )
////////////////////////////////////////////////////////////////


//change the sharedRaftData.python_functions.write_to_logger to yours function
//write_to_logger type : void (*write_to_logger)(int debug_level,char * logger_msg)


/*debug level option
 * 
 * 
#define DEBUG_LEVEL 10
#define INFO_LEVEL 20
#define WARN_LEVEL 30
#define ERROR_LEVEL 40
#define FATAL_LEVEL 50
 * 
 * */
//to log int's values 
//int a = 0;
//int b = 0;
//int arguments_num = 2; 
//WRITE_TO_LOGGER(DEBUG_LEVEL,"some msg",INT_VALUES,arguments_num,LOG(a),LOG(b));

//char * cmd ;
//char * key;
//char * value;
//int arguments_num = 3;
//WRITE_TO_LOGGER(DEBUG_LEVEL,"some msg",CHARS_VALUES,arguments_num,LOG(cmd),LOG(key),LOG(value));
//
//logger.c
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
//logger.c
//do not call this function directly - call WRITE_TO_LOGGER
void add_logger_msg(void (*write_to_logger)(int logger_level,char* logger_info),int logger_level,const char * file,const char *func_name,const int line,
				const char* problem_desc, int msg_type,int args_num,...){
	char logger_msg[MAX_LOGGER_MSG];
	int loc=0,is_valid = MAX_LOGGER_MSG;
	va_list ap;
	va_start(ap, args_num);
	loc=sprintf(logger_msg,"FILE:%s, FUNCTION:%s, LINE:%d\nMESSAGE:%s",
					file,func_name,line,problem_desc);
	if(args_num>0){
		loc+=sprintf(logger_msg+loc, "\nVARIABLES: ");
	}
	for(int i=0;i<args_num && (is_valid>1);i++){
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
	if(is_valid<35){
		loc=MAX_LOGGER_MSG-35;
	}
	logger_msg[loc]='\n';
	//30 bytes
	sprintf(logger_msg+loc+1,"-----------------------------\n");
	logger_msg[loc+31]=0;
	//puts(logger_msg);
	write_to_logger(logger_level,logger_msg);
}
