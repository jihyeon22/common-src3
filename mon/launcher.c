#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>

#include "iniparser.h"

#include "board_system.h"
#include "launcher.h"

#include <mdsapi/mds_api.h>


#define TRUE    1
#define FALSE   0

PROG_INFO_T prog_info[32];

int prg_cnt = 0;
int fork_retry_cnt = 0;
int check_process_interval = 0;
int zombiecnt = 0;

int _check_nomon_file(void);

#define INI_INVALID_KEY_STRING     ((char*)-1)
#define INI_INVALID_KEY_INT        -1

int read_ini(void)
{
    dictionary * ini ;
    char*  s;
    int i, j;
    char temp[256];
    int cmd_argc;

    //jwrho ++
    ini = iniparser_load(INI_PATH);
    //jwrho --
    if(ini == NULL) {
        goto return_fail;
    }

    // Get Program Setting
    i = iniparser_getint(ini, "main_info:prg_cnt", -1);
    if(i != INI_INVALID_KEY_INT) {
        prg_cnt = i;
    }
    else {
        goto return_fail;
    }

    i = iniparser_getint(ini, "main_info:run_retry", -1);
    if(i != INI_INVALID_KEY_INT) {
        fork_retry_cnt = i;
    }
    else {
        goto return_fail;
    }

    i = iniparser_getint(ini, "main_info:check_process_interval", -1);
    if(i != INI_INVALID_KEY_INT) {
        check_process_interval = i;
    }
    else {
        goto return_fail;
    }

    // Get Program info...
    memset(prog_info, 0, sizeof(PROG_INFO_T) * 32);
    for(i = 0 ; i < prg_cnt; i++)
    {
        memset(temp, 0, 256);

        sprintf(temp, "program_%d:path", i);
        s = iniparser_getstring(ini, temp, INI_INVALID_KEY_STRING);
        if(s != INI_INVALID_KEY_STRING) {
            strncpy(prog_info[i].cmd_line, s, strlen(s));
            strncat(prog_info[i].cmd_line, "/", 1);
        }
        else {
            goto return_fail;
        }

        sprintf(temp, "program_%d:name", i);
        s = iniparser_getstring(ini, temp, INI_INVALID_KEY_STRING);
        if(s != INI_INVALID_KEY_STRING) {
            strncpy(prog_info[i].name, s, strlen(s));
            strncat(prog_info[i].cmd_line, s, strlen(s));
        }
        else {
            goto return_fail;
        }

        sprintf(temp, "program_%d:command_argc", i);
        cmd_argc = iniparser_getint(ini, temp, -1);
        if(cmd_argc == INI_INVALID_KEY_INT) {
            goto return_fail;
        }

        prog_info[i].argv[0] = malloc(strlen(prog_info[i].cmd_line) + 1);
        memset(prog_info[i].argv[0], 0, strlen(prog_info[i].cmd_line) + 1);
        strncpy(prog_info[i].argv[0], prog_info[i].cmd_line,
                strlen(prog_info[i].cmd_line) + 1);

        for(j = 1; j <= cmd_argc; j++) {
            sprintf(temp, "program_%d:command_arg_%d", i , j);
            s = iniparser_getstring(ini, temp, INI_INVALID_KEY_STRING);
            if(s != INI_INVALID_KEY_STRING) {
                prog_info[i].argv[j] = malloc(strlen(s) + 1);
                memset(prog_info[i].argv[j], 0, strlen(s) + 1);
                strncpy(prog_info[i].argv[j], s, strlen(s));
            } else {
                goto return_fail;
            }
        }

        sprintf(temp, "program_%d:make_deamon", i);
        s = iniparser_getstring(ini, temp, INI_INVALID_KEY_STRING);
        if(s != INI_INVALID_KEY_STRING)
        {
            if(!strncmp(s, "yes", strlen(s))) {
                prog_info[i].deamon = TRUE;
            }
            else {
                prog_info[i].deamon = FALSE;
            }
        }
        else {
            goto return_fail;
        }

        sprintf(temp, "program_%d:check_process", i);
        s = iniparser_getstring(ini, temp, INI_INVALID_KEY_STRING);
        if(s != INI_INVALID_KEY_STRING)
        {
            if(!strncmp(s, "yes", strlen(s)))
            {
                prog_info[i].chk_process = TRUE;
            }
            else
            {
                prog_info[i].chk_process = FALSE;
            }
        }
        else {
            goto return_fail;
        }

        sprintf(temp, "program_%d:num_thread", i);
        prog_info[i].num_thread = iniparser_getint(ini, temp, 0);

    }

    iniparser_freedict(ini);
    return TRUE;

return_fail:
    fprintf(stderr, "Cannot read INI file.\n");
    return FALSE;
}


void prog_start()
{
    int i, ret;

    //system("ls -la /system/bin/"); //sometimes, hang system.
    for(i = 0; i < prg_cnt; i++) {
        int f_tmp = fork_retry_cnt;
        do {
            if ( mds_api_check_exist_file(prog_info[i].cmd_line,3) == DEFINES_MDS_API_OK )
            {
                ret = mds_api_system_fork(prog_info[i].cmd_line, prog_info[i].deamon,
                              prog_info[i].argv);
            }
            else
            {
                printf("[mon:error] : err1 [%s] file is not exist! skip check routine..\r\n",prog_info[i].cmd_line);
                ret = 0;
            }
            sleep(1);

        } while((ret == -1) && (f_tmp--));
        if(!f_tmp) {
            fprintf(stderr, "[MOND] Run recovery service.\n");
            exit(1);
        }
    }
    // Send Linux Alive Check..
    system(CMD_ALIVE_END);
}

void check_prog(void)
{
    int i;
    int chkcnt = 10;
    int found_fail = 0;
    int found_fail_apps = 0;
    DEBUG_PRINT(("start check process!!!\r\n"));
    do {
        found_fail_apps = 0;
        sleep(check_process_interval);
        for(i = 0; i < prg_cnt; i++)
            if((prog_info[i].chk_process == TRUE))
            {
                found_fail = 1;
                zombiecnt = 0;
                chkcnt = 10;
                while(chkcnt--)
                {
                    int res = 0;
                    prog_info[i].cur_num_thread = 0;
                    res = mds_api_proc_find((prog_info[i].name));
                    if(res == -1)
                    {
                        found_fail_apps = 1;
                        found_fail = 1;
                    }
                    else if(res == -2)
                    {
                        found_fail_apps = 1;
                        zombiecnt++;
                        if(zombiecnt >= 5)
                        {
                            mds_api_poweroff_and_log("mon", "check_prog case 1");
                        }
                        chkcnt = 10;
                        continue;
                    }
                    else
                    {
                        found_fail = 0;
                        prog_info[i].cur_num_thread = res;
                        break;
                    }
                    sleep(1);
                }
#if 1
                if(found_fail) {
                    if ( mds_api_check_exist_file(prog_info[i].cmd_line,3) == DEFINES_MDS_API_OK )
                        mds_api_system_fork(prog_info[i].cmd_line, prog_info[i].deamon, prog_info[i].argv);
                    else
                        printf("[mon:error] : err2 [%s] file is not exist!!!\r\n",prog_info[i].cmd_line);
                }
#endif
            }
        
        if(found_fail_apps != 0)
        {
            continue;
        }

        for(i = 0; i < prg_cnt; i++)
        {
            if(prog_info[i].chk_process == FALSE)
            {
                continue;
            }
            
            if(prog_info[i].num_thread == 0)
            {
                continue;
            }
            
            if(prog_info[i].cur_num_thread >= prog_info[i].num_thread)
            {
                continue;
            }
                    
            if(_check_nomon_file() < 0)
            {
                mds_api_poweroff_and_log("mon", "check_prog case 2");
            }
        }
    } while(1);
}

int _check_nomon_file(void)
{
    if(access(FLG_ERR_PATH, F_OK) == 0)
    {
        return 0;
    }
    
    return -1;
}
