
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

#include <mdsapi/mds_api.h>

#define TRUE    1
#define FALSE   0

#define DEFAULT_CHK_PROC_CNT    4

#ifdef DEBUG
# define MDS_API_DEBUG_PRINT(x) printf x
#else
# define MDS_API_DEBUG_PRINT(x) do {} while (0)
#endif

static int zombiecnt = 0;

static int _mds_api_proc_find(const char* name);

int mds_api_proc_find_cnt(int max_cnt, const char* name)
{
    int ret = 0;
    while(max_cnt --)
    {
        ret = _mds_api_proc_find(name);
        if (ret > 0)
            break;
        sleep(1);
    }
    return ret;
}


int mds_api_proc_find(const char* name)
{
    int ret = 0;
    int max_cnt = DEFAULT_CHK_PROC_CNT;
    
    while(max_cnt --)
    {
        ret = _mds_api_proc_find(name);
        if (ret > 0)
            break;
        sleep(1);
    }
    return ret;
}

static int _mds_api_proc_find(const char* name)
{
    DIR* dir;
    struct dirent* ent;
    char buf[512];

    long  pid, ppid, pid_mon;
    char pname[100] = {0,};
    int status;
    char state;
    FILE *fp = NULL;
    int isfounded = 0;
    int num_thread = 0;

    pid_mon = getpid();

    if(!(dir = opendir("/proc"))) {
        perror("can't open /proc");
        return -1;
    }
    while((ent = readdir(dir)) != NULL) {
        long lpid = atol(ent->d_name);
        if(lpid <= 0) {
            continue;
        }
        snprintf(buf, sizeof(buf), "/proc/%ld/stat", lpid);
        fp = fopen(buf, "r");
        if(fp) {
            if((fscanf(fp, "%ld (%[^)]) %c %ld", &pid, pname, &state, &ppid)) != 4) {
                fclose(fp);
                closedir(dir);
                return -1;
            }
            if(!strcmp(pname, name)) {
                num_thread++;
                if(state == 'Z') {
                    int reskill = 0;
                    fprintf(stderr, "zombies!!!!!\n");
                    waitpid(pid, &status, WNOHANG);
                    if(zombiecnt >= 3 && pid_mon != ppid && 1 != ppid)
                    {
                        //system("echo kill_ppid >> /var/log/mon.log");
                        kill(ppid, SIGKILL);
                        sleep(1);
                        waitpid(ppid, &status, WNOHANG);
                    }
                    printf("kill %d\n", reskill);
                    fclose(fp);
                    closedir(dir);
                    return -2;
                }
                isfounded = 1;
            }
            fclose(fp);
        }
    }
    closedir(dir);
    if(isfounded == 1)
    {
        return num_thread;
    }
    return -1;
}

int mds_api_system_fork(const char *command, int deamon, char *argv[])
{
    sigset_t blockMask, origMask;
    struct sigaction saIgnore, saOrigQuit, saOrigInt, saDefault;
    pid_t childPid;
    int status, savedErrno;

    if(command == NULL) {               /* Is a shell available? */
        return system(":") == 0;
    }

    /* The parent process (the caller of system()) blocks SIGCHLD
       and ignore SIGINT and SIGQUIT while the child is executing.
       We must change the signal settings prior to forking, to avoid
       possible race conditions. This means that we must undo the
       effects of the following in the child after fork(). */

    sigemptyset(&blockMask);            /* Block SIGCHLD */
    sigaddset(&blockMask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &blockMask, &origMask);

    saIgnore.sa_handler = SIG_IGN;      /* Ignore SIGINT and SIGQUIT */
    saIgnore.sa_flags = 0;
    sigemptyset(&saIgnore.sa_mask);
    sigaction(SIGINT, &saIgnore, &saOrigInt);
    sigaction(SIGQUIT, &saIgnore, &saOrigQuit);

    MDS_API_DEBUG_PRINT(("launcher test : try fork...\r\n"));
    switch(childPid = fork()) {
        case -1: /* fork() failed */
            status = -1;
            break;          /* Carry on to reset signal attributes */

        case 0: /* Child: exec command */
            /* We ignore possible error returns because the only specified error
               is for a failed exec(), and because errors in these calls can't
               affect the caller of system() (which is a separate process) */
            MDS_API_DEBUG_PRINT(("launcher test : fork success!!\r\n"));
            saDefault.sa_handler = SIG_DFL;
            saDefault.sa_flags = 0;
            sigemptyset(&saDefault.sa_mask);

            if(saOrigInt.sa_handler != SIG_IGN) {
                sigaction(SIGINT, &saDefault, NULL);
            }
            if(saOrigQuit.sa_handler != SIG_IGN) {
                sigaction(SIGQUIT, &saDefault, NULL);
            }

            sigprocmask(SIG_SETMASK, &origMask, NULL);

            //execl("/bin/sh", "sh", "-c", command, (char *) NULL);
            execv(command, argv);
            exit(127);                     /* We could not exec the shell */

        default: /* Parent: wait for our child to terminate */
            /* We must use waitpid() for this task; using wait() could inadvertently
               collect the status of one of the caller's other children */
            if(deamon == FALSE)
            {
                MDS_API_DEBUG_PRINT(("launcher test : child Pid is [%d] try wait...\r\n", childPid));
                while(waitpid(childPid, &status, 0) == -1) {
                    if(errno != EINTR) {        /* Error other than EINTR */
                        status = -1;
                        break;                  /* So exit loop */
                    }
                }
            }
            break;
    }

    /* Unblock SIGCHLD, restore dispositions of SIGINT and SIGQUIT */
    savedErrno = errno;   /* The following may change 'errno' */

    sigprocmask(SIG_SETMASK, &origMask, NULL);
    sigaction(SIGINT, &saOrigInt, NULL);
    sigaction(SIGQUIT, &saOrigQuit, NULL);

    errno = savedErrno;

    return status;
}
