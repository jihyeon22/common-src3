/**
 *       @file  mond.c
 *      @brief  NeoM2M by Taco Monitor
 *
 * Detailed description starts here.
 *
 *     @author  Yoonki (IoT), yoonki@mdstec.com
 *
 *   @internal
 *     Created  2013년 02월 27일
 *    Revision  $Id: doxygen.templates,v 1.3 2010/07/06 09:20:12 mehner Exp $
 *    Compiler  gcc/g++
 *     Company  MDS Technology, R.Korea
 *   Copyright  Copyright (c) 2013, Yoonki
 *
 * This source code is released for free distribution under the terms of the
 * GNU General Public License as published by the Free Software Foundation.
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory.h>
#include <netinet/in.h>
#include <signal.h>
#include <errno.h>
#include <semaphore.h>

#include <mdsapi/mds_api.h>

#include "launcher.h"

int g_restart_flag = 0;

void *start_service(void *data)
{
    int ntry_ini = 0;
    
    while(++ntry_ini <= 3)
    {
        if(read_ini()) {
            break;
        }
        sleep(1);
    }
    
    if(ntry_ini > 3)
    {
        exit(EXIT_FAILURE);
    }

    if ( g_restart_flag == 0)
        prog_start();


    check_prog();
    /* NORETURN */
    return NULL;
}

int
main(int argc, char *argv[])
{
    int pid, sid;
    int count = 0;
    int i = 0;

    mds_api_stackdump_init();

    // make deamon Process...
    pid = fork();
    while(pid < 0)
    {
        perror("fork error : ");
        pid = fork();
        if(count == 10) {
            exit(0);
        }
        sleep(10);
        count++;
    }

    if(pid > 0) {
        exit(EXIT_SUCCESS);
    }

    sid = setsid();
    if(sid < 0) {
        exit(EXIT_FAILURE);
    }

    g_restart_flag = 0;

    // check argument
    if (argc >= 2)
    {
        for (i = 0; i<argc ; i++)
        {
            if( strcmp(argv[i],"-restart") == 0 )
            {
                printf("restart flag is set\n");
                g_restart_flag = 1;

            }
        }
    }

    chdir("/");
    start_service(NULL);
    /* NORETURN */
    return 0;
}
