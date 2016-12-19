#ifndef  _LAUNCHER_H_
#define  _LAUNCHER_H_

#undef DEBUG

#ifdef DEBUG
# define DEBUG_PRINT(x) printf x
#else
# define DEBUG_PRINT(x) do {} while (0)
#endif

typedef struct
{
	char name[128];
	char cmd_line[256];
	int deamon;
	int chk_process;
	char *argv[64];
	int num_thread;
	int cur_num_thread;
} PROG_INFO_T;

int read_ini(void);
void prog_start();
void check_prog(void);

#endif /* _LAUNCHER_H_*/
