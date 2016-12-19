
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>

#include <mdsapi/mds_api.h>


//return -1 : error
//       other : available memory In KB
int mds_api_get_available_memory(void)
{
    FILE *meminfo = fopen("/proc/meminfo", "r");
    if(meminfo == NULL)
        return -1;

    char line[256];
    while(fgets(line, sizeof(line), meminfo))
    {
        int ram;
        if(sscanf(line, "MemFree: %d kB", &ram) == 1)
        {
            fclose(meminfo);
            return ram;
        }
    }

    fclose(meminfo);
    return -1;
}

int mds_api_get_module_list(const char* module_name)
{
    FILE *moduleinfo = fopen("/proc/modules", "r");
    if(moduleinfo == NULL)
        return -1;

    char line[256];
    while(fgets(line, sizeof(line), moduleinfo))
    {
        if( strstr(line, module_name) != NULL)
        {
            fclose(moduleinfo);
            return DEFINES_MDS_API_OK;
        }
    }

    fclose(moduleinfo);
    return DEFINES_MDS_API_NOK;
}



void mds_api_write_procfs(char* value, char* procfs_path)
{
	int fd;
	fd = open(procfs_path, O_RDWR);
	if (fd > 0)
	{
		write(fd, value, strlen(value));
		close(fd);
	}
}
