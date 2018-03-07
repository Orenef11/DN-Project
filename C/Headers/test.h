#ifndef TEST_H_
#define TEST_H_

#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../../raft_main.h"

#define MAIN_TEST 1

#define _TEST_MULTICAST_IP "224.1.1.1"
#define _TEST_MULTICAST_PORT 6060
#define _TEST_SERVER_ID 1
#define _TEST_MEMBERS_NUM 2
#define _TEST_LEADER_TIMEOUT 1000 /*milliseconds*/

#define LOGGER_FILE "./logger.log"

extern int logger_fd ;

#endif
