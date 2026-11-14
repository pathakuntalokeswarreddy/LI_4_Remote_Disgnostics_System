
#ifndef HEADER_H
#define HEADER_H

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include <limits.h>

#include <signal.h>
#include<pthread.h>
#include <errno.h>
#include <sys/socket.h>

#include <arpa/inet.h>


#include <termios.h>  //for Up and Down arrows


#define SUCCESS            0
#define FAILURE           -1


/* ANSI Color Codes */
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"
#define BOLD    "\x1b[1m"

#define REQ_PROC_ALL  "get-processes"
#define REQ_MEM_USG   "get-mem"
#define REQ_CPU_USG   "get-cpu-usage"
#define REQ_PORTS_USG "get-ports-used"   
#define REQ_OPEN_FD   "get-open-fd"
#define REQ_KILL      "kill"
 

#define RESP_PROC_TERM      "SERVER:_Processe_terminated"
#define RESP_EXEC_FAIL      "SERVER:_Command_Execution_failed"
#define RESP_INVALID_ARG    "SERVER:_Invalid_Argument/PID"

#endif