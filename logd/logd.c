/**
 *       @file  logd.c
 *      @brief  logd (log & debug ) deamon
 *
 * Detailed description starts here.
 *
 *     @author  Yoonki (IoT), yoonki@mdstec.com
 *
 *   @internal
 *     Created  2013년 02월 27일
 *    Revision  $Id: doxygen.templates,v 1.3 2010/07/06 09:20:12 mehner Exp $
 *    Compiler  gcc/g++
 *     Company  MDS Technologt, R.Korea
 *   Copyright  Copyright (c) 2013, Yoonki
 *
 * This source code is released for free distribution under the terms of the
 * GNU General Public License as published by the Free Software Foundation.
 * =====================================================================================
 */

#include "logd_rpc.h"
#include "rev_debug.h"
/*******************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <memory.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <sys/time.h>

#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif


int prgcfg_level = 0;
int prgcfg_service = 0;

int sockfd = 0;

typedef struct
{
	int index;
    char * cmd;
	char * print;
}LOGD_CMD_SET_T;

static LOGD_CMD_SET_T svc_cmd_set[]=
{
	{eSVC_COMMON,   "SVC_COMMON",   "COMMON"},
	{eSVC_BASE,     "SVC_BASE",     "BASE"},
	{eSVC_AT,       "SVC_AT",       "AT"},
	{eSVC_MODEL,    "SVC_MODEL",    "MODEL"},
	{eSVC_NETWORK,  "SVC_NETWORK",  "NETWORK"},
	{eSVC_REGI,     "SVC_REGI",     "REGI"},
	{eSVC_MILEAGE,  "SVC_MILEAGE",  "MILEAGE"},
	{eSVC_GPS,      "SVC_GPS",      "GPS"},	
	{eSVC_UPDATE,   "SVC_UPDATE",   "UPDATE"},
	{0,				NULL,			NULL}
};

static LOGD_CMD_SET_T level_cmd_set[]=
{
	{eDebug,    "L_DBUG",  NULL},
	{eWarning,  "L_WARN",  NULL},
	{eInfo,     "L_INFO",  NULL},
	{eError,    "L_ERR",   NULL},
	{eTrace,    "L_TRACE", NULL},
	{0,         NULL,      NULL},
};

char *g_application_name = NULL;

void
termination_handler(int signum)
{
	fprintf(stderr, "Received signal %d\n", signum);
	switch(signum) {
		case SIGINT:
			close(sockfd);
			fprintf(stderr, "SIGINT\n");
			break;
		default:
			break;
	}
	fprintf(stderr, "Termination\n");
	exit(1);
}

void print_log(struct log_message l_msg)
{
	int enable_level = ((1 << l_msg.level) & prgcfg_level ) ;
	int enable_svc = ((1 << l_msg.service) & prgcfg_service );

	struct timeval tv;
	struct tm *ptm;

	//printf("print_log is [%s]\r\n",l_msg.message);
	/*
	printf("prgcfg_level is [0x%08x]\r\n",prgcfg_level);
	printf("prgcfg_service is [0x%08x]\r\n",prgcfg_service);
	
	printf("(1 << l_msg.level) is [0x%08x]\r\n",(1 << l_msg.level));
	printf("(1 << l_msg.service) is [0x%08x]\r\n",(1 << l_msg.service));
	*/
	/*
	printf("enable_level is [%d]\r\n", enable_level);
	printf("enable_svc is [%d]\r\n", enable_svc);
	*/
	
	gettimeofday(&tv, NULL);
	ptm = localtime(&tv.tv_sec);

	if (enable_level && enable_svc)
	{
		switch(l_msg.level)
		{
			case eDebug:	// debug
				fprintf(stderr, GRAY_COLOR "[%02d:%02d:%02d] %s D %s %s" NORMAL_COLOR, ptm->tm_hour,
						ptm->tm_min, ptm->tm_sec, l_msg.pname, svc_cmd_set[l_msg.service].print, l_msg.message);
				break;
			case eWarning:	// warning
				fprintf(stderr, BROWN_COLOR "[%02d:%02d:%02d] %s W %s %s" NORMAL_COLOR, ptm->tm_hour,
						ptm->tm_min, ptm->tm_sec,  l_msg.pname, svc_cmd_set[l_msg.service].print, l_msg.message);
				break;
			case eInfo:	// info
				fprintf(stderr, GREEN_COLOR "[%02d:%02d:%02d] %s I %s %s" NORMAL_COLOR, ptm->tm_hour,
						ptm->tm_min, ptm->tm_sec, l_msg.pname, svc_cmd_set[l_msg.service].print, l_msg.message);
				break;
			case eError:	// error
				fprintf(stderr, RED_COLOR "[%02d:%02d:%02d] %s E %s %s" NORMAL_COLOR, ptm->tm_hour,
						ptm->tm_min, ptm->tm_sec, l_msg.pname, svc_cmd_set[l_msg.service].print, l_msg.message);
				break;
			case eTrace:	// Trace
				fprintf(stderr, CYAN_COLOR "[%02d:%02d:%02d] %s T %s %s" NORMAL_COLOR, ptm->tm_hour,
						ptm->tm_min, ptm->tm_sec, l_msg.pname, svc_cmd_set[l_msg.service].print, l_msg.message);
				break;
			default:
				break;

		}
	}
}
/*
enum {
	eSVC_COMMON=1
	eSVC_BASE,
	eSVC_MODEL,
	eSVC_MAX,
};

enum {
	eDebug = 1,
	eWarning,
	eInfo,
	eError,
	eTrace,
	eMAX;
};
*/





int
main(int argc, char *argv[])
{
	int option;
	struct sockaddr_un logd_addr;
	struct log_message l_msg;
	int i = 0;
	int j = 0;
	
	prgcfg_level = 0;
	prgcfg_service = 0;
	
	if (argc > 1)
	{
		if ( strncmp(argv[1], "--help", strlen("--help")) == 0)
		{
			printf("1. application name filter available\n");
			printf("\t./logd -app gtrack\n");

			printf("2. debug level can filter to print debug message\n");
			printf("\tavailable level list : ");
			j = 0;
			while(1) {
				if(level_cmd_set[j].cmd == NULL)
					break;
				printf("[%s]", level_cmd_set[j].cmd);
				j += 1;

			}
			printf("\r\n");
		
			printf("3. service tag can filter to print debug message\n");
			printf("\tavailable service list : ");
			j = 0;
			while(1)
			{
				if(svc_cmd_set[j].cmd == NULL)
					break;
				printf("[%s]", svc_cmd_set[j].cmd);
				j += 1;
			}
			printf("\r\n");
			return 0;
		}
		
	}
	
	// get level argument
	for (i = 0 ; i < argc ; i++)
	{
		//application filter check
		if(!strcmp(argv[i], "-app")) {
			if(i+1 >= argc) {
				printf("error appcation name dont exist\n");
				exit(0);
			}
			g_application_name = argv[i+1];
				

		}

		j = 0;
		while(1)
		{
			if(level_cmd_set[j].cmd == NULL)
				break;

			if ( strncmp(argv[i], level_cmd_set[j].cmd, strlen(level_cmd_set[j].cmd)) == 0 ) 
			{
				printf("enable level [%s] \r\n", level_cmd_set[j].cmd);
				prgcfg_level |= ( 1 << level_cmd_set[j].index );
			}

			j += 1;
		}
	}
	
	
	// get service argument
	for (i = 0 ; i < argc ; i++)
	{
		j = 0;
		while(1)
		{
			if(svc_cmd_set[j].cmd == NULL)
				break;

			if ( strncmp (argv[i], svc_cmd_set[j].cmd, strlen(svc_cmd_set[j].cmd)) == 0 ) 
			{
				printf("enable service [%s] \r\n", svc_cmd_set[j].cmd);
				prgcfg_service |= ( 1 << svc_cmd_set[j].index );
			}
			j += 1;
		}
	}
	
	// if argument is not setting then print all
	
	if (prgcfg_level == 0)
		prgcfg_level = 0xffffffff;
	
	if (prgcfg_service == 0)
		prgcfg_service = 0xffffffff;
	
	//fprintf(stderr, "LOGD Application v%s. %s %d. Build Date:%s\n", VERSION_STRING, REPO_NAME, REPO_REV, BUILD_DATE);
	//fprintf(stderr, "prgcfg_level = [0x%08x], prgcfg_service = [0x%08x]\n", prgcfg_level, prgcfg_service);

	signal(SIGINT, termination_handler);
	signal(SIGPIPE, SIG_IGN); // To avoid SIGPIPE

	/*
	if(mkdir(TEMP_PATH, 0755) != 0)
		if (errno != EEXIST){
	    	perror("[LOGD] cannot make directory TEMP_PATH");
	    	exit(0);
	}*/

	/*
	if(mkdir("/system/log", 0755) != 0)
		if (errno != EEXIST){
	    	perror("[LOGD] cannot make directory /system/log");
	    	exit(0);
	}*/

	unlink(SOCKNAME);

	sockfd = socket(PF_FILE, SOCK_DGRAM, 0);
	if(sockfd == -1) {
		perror("socket() err");
		assert(0);
	}

	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

	memset(&logd_addr, 0, sizeof(logd_addr));
	logd_addr.sun_family = AF_UNIX;
	strcpy(logd_addr.sun_path, SOCKNAME);

	if(bind(sockfd, (struct sockaddr*) &logd_addr, sizeof(logd_addr)) < 0) {
		perror("bind() err");
		assert(0);
	}

	while(1)
	{
		memset(&l_msg, 0, sizeof(struct log_message));
		recvfrom(sockfd, &l_msg, sizeof(struct log_message), 0 , NULL, 0);

		if(g_application_name != NULL) {
			if(strncmp(l_msg.pname, g_application_name, sizeof(l_msg.pname))) {
				//printf("application filterd...\n");
				continue;
			}
		}

		if(l_msg.type == 0) {
			/* ADD Insert Queue */
			print_log(l_msg);
		} else if(l_msg.type == 1) {
			/* FILE DUMP */

		}
	}
	/* NORETURN */
}
