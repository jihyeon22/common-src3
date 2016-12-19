/**
 *       @file  logd_rpc.h
 *      @brief  logd rpc program header
 *
 * logd 프로그램의 프로그램 번호와 프로시저 번호 및 프로토콜을 정의한다.
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

#ifndef _LOGD_RPC_H
#define _LOGD_RPC_H

#include <pthread.h>

#define LOGDPROG	5000
#define LOGDVERS	1
/* LOGD PROCEDURE 1 */
#define LOGDPROC 1

#define SOCKNAME	"/var/log/LOGD"

struct log_message {
	int type;
	int service;
	int level;
	char pname[32];
	char message[1024];
};

extern void logd(int, int, const char *, ...);

enum {
	eSVC_COMMON=0,
	eSVC_BASE,
	eSVC_AT,
	eSVC_MODEL,
	eSVC_NETWORK,
	eSVC_REGI,
	eSVC_MILEAGE,
	eSVC_GPS,
	eSVC_UPDATE,
	eSVC_MAX,
};

enum {
	eDebug = 0,
	eWarning,
	eInfo,
	eError,
	eTrace,
	eMAX,
};


//#define LOGD(svc,msg...)	logd(svc, eDebug, msg)	// debug 	: gray
// workaround!! 
#define LOGD(svc,msg...)	logd(svc,eDebug, msg)	
#define LOGW(svc,msg...)	logd(svc,eWarning, msg)	// warning  : brown
#define LOGI(svc,msg...)	logd(svc,eInfo, msg)	// info  	: green
#define LOGE(svc,msg...)	logd(svc,eError, msg)	// error 	: red
#define LOGT(svc,msg...)	logd(svc,eTrace, msg)	// trace 	: brightmagenta

/*
#define LOGD(msg...)	logd(eSVC_MODEL, eDebug, msg)	// debug 	: gray
#define LOGW(msg...)	logd(eSVC_MODEL,eWarning, msg)	// warning  : brown
#define LOGI(msg...)	logd(eSVC_MODEL,eInfo, msg)	// info  	: green
#define LOGE(msg...)	logd(eSVC_MODEL,eError, msg)	// error 	: red
#define LOGT(msg...)	logd(eSVC_MODEL,eTrace, msg)	// trace 	: brightmagenta
*/
void logd_dump(void);
void logd_init(void);
#endif

