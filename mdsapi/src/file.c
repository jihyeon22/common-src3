
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

#include "board_system.h"

//--------------------


static int _isExistFile(const char *file, int timeout)
{
    while(timeout--) {
        if(access(file, F_OK) == 0) {
            return 0;
        }
        printf(".\n");
        sleep(1);
    }
    printf("\n=============================== \n");
    printf("%s> %s can not open : err[%d]\n", __func__, file, errno);
    printf("=============================== \n\n");
    return -1;
}
static int _file_recovery(char *file)
{
    char recovery[128] = {0};

    sprintf(recovery, "%s.bak", file);
    if(mds_api_cp(recovery, file, 1) < 0)
    {
        return -ERR_FILE_NOT_EXIST;
    }

    return ERR_NONE;
}

static void _file_backup(const char *file)
{
    char backup[128] = {0};
    
    if(_isExistFile(file, 1) >= 0) {
        sprintf(backup, "%s.bak", file);
        mds_api_cp(file, backup, 1);
    }
}

static int _load_file(char *file, void *pdata, int length)
{
        int err = ERR_NONE;
    unsigned short fcrc16;
    unsigned short crc16;
    int fd;
    int nreadbytes;
    int retry;
    unsigned char *pbuf;
    memset(pdata, 0x00, length);

    pbuf = (unsigned char *)malloc(length + 2);
    if(pbuf == NULL)
        return -ERR_MEM_ALLOC;

    retry = 0;
    while(retry++ < 3) {
        fd = open(file, O_RDONLY);
        if(fd <= 0) {
            err = -ERR_FILE_OPEN;
            continue;
        }

        nreadbytes = read(fd, pbuf, (length+2));
        close(fd);

        if(nreadbytes == (length+2))
            break;

        err = -ERR_FILE_READ;
    }

    //read fail
    if(retry >= 3) {
        printf("read file length error!!!\n");
        free(pbuf);
        close(fd);
        return err;
    }
    
    //read ok
    memcpy(&fcrc16, &pbuf[length], 2);
    mds_api_crc16_get(NULL, 0);     
    crc16 = mds_api_crc16_get(pbuf, length);

    if(fcrc16 != crc16) { //crc error
        printf("geo fence crc error, but will recovery.\n");
        free(pbuf);
        close(fd);
        return -ERR_DATA_CRC;
    }
        
    memcpy(pdata, pbuf, length);
    close(fd);
    free(pbuf);
    return ERR_NONE;
}
//--------------------------------------------

mdsapiRet_t mds_api_write_data(const char *filename, unsigned char *buff, const int data_len, int append)
{
    FILE *fp;

    if(append == 0)
    {
        fp = fopen(filename, "w+");
    }
    else
    {
        fp = fopen(filename, "a");
    }
    if(fp != NULL)
    {
        fwrite(buff, data_len, 1, fp);
        fflush(fp);
        fclose(fp);
        return DEFINES_MDS_API_OK;
    }
    return DEFINES_MDS_API_NOK;
}

#define MAX_READ_BUFF 1024
mdsapiRet_t mds_api_write_data_maxsize(const char *filename, unsigned char *buff, const int data_len, int max_size)
{
    
    FILE *read_fp;
    FILE *write_fp;
    
    unsigned char tmp_file_name[128] ={0,};
    int file_size = 0;
    int start_offset = 0;
    int read_size = 0;
    
    unsigned char read_buff[MAX_READ_BUFF] = {0,};
    
    // file exist check..
    if ( mds_api_check_exist_file(filename, 3) == DEFINES_MDS_API_NOK )
        return mds_api_write_data(filename, buff, data_len, 0);
    
    // tem file name..
    sprintf((char *)tmp_file_name, "%s.bak", filename);
    mds_api_cp((const char *)filename, tmp_file_name, 1);
    
    // read fd
    read_fp = fopen((char *)tmp_file_name, "r");
    if (read_fp == NULL)
    {
        fclose(read_fp);
        return DEFINES_MDS_API_NOK;
    }
    
    // write fd : temp file
    write_fp = fopen(filename, "w+");
    if (write_fp == NULL)
    {
        fclose(write_fp);
        return DEFINES_MDS_API_NOK;
    }
    
    // check max size and offset
    fseek(read_fp, 0L, SEEK_END);
    file_size = ftell(read_fp);
    
    if ( (data_len + file_size) > max_size )
    {
        start_offset = data_len + file_size - max_size;
    }
    
    fseek(read_fp, start_offset, SEEK_SET);
    
    while (1)
    {
        memset(&read_buff, 0x00, MAX_READ_BUFF);
        read_size = fread(read_buff, 1, MAX_READ_BUFF, read_fp);
        if( read_size <= 0 )
            break;
        fwrite(read_buff, read_size, 1, write_fp);
        fflush(write_fp);        
    }
    
    fwrite(buff, data_len, 1, write_fp);
    fflush(write_fp);   
    
    fclose(write_fp);
    fclose(read_fp);
    
    unlink((char *)tmp_file_name);
    return DEFINES_MDS_API_OK;
}

int mds_api_read_data(const char *filename, unsigned char *buff, const int buff_len)
{
    FILE *fp;
    int res = 0;
    fp = fopen(filename, "r");
    if(fp != NULL)
    {
        res = fread(buff, 1, buff_len, fp);
        fclose(fp);
        return res;
    }
    return -1;
}

unsigned char mds_api_checksum_xor(const unsigned char *buff, const int data_len)
{
    int i;
    unsigned char ret = buff[0];
    for(i = 1 ; i < data_len ; i++)
    {
        ret ^= buff[i];
    }
    return ret;
}

int mds_api_check_exist_file(const char *filename, int timeout)
{
    if ( timeout == 0 )
    {
        if(access(filename, F_OK) == 0)
             return DEFINES_MDS_API_OK;
    }

    while(timeout-- > 0) {
            if(access(filename, F_OK) == 0)
                    return DEFINES_MDS_API_OK;
            sleep(1);
    }
    
    return DEFINES_MDS_API_NOK;
}

int mds_api_cp(const char *from, const char *to,  int overwrite)
{
    int fd_to = -1, fd_from = -1;
    char buf[4096] = {0};
    ssize_t nread = 0;
    int saved_errno = 0;
    struct stat stat_buf;

    fd_from = open(from, O_RDONLY | O_EXCL);
    if(fd_from < 0)
    {
        return DEFINES_MDS_API_NOK;
    }

    fstat(fd_from, &stat_buf);

    if(overwrite)
    {
        fd_to = open(to, O_WRONLY | O_CREAT | O_TRUNC, stat_buf.st_mode);
    }
    else
    {
        fd_to = open(to, O_WRONLY | O_CREAT, stat_buf.st_mode);
    }

    if(fd_to < 0)
    {
        goto out_error;
    }

    while(nread = read(fd_from, buf, sizeof buf), nread > 0)
    {
        char *out_ptr = buf;
        ssize_t nwritten;

        do {
            nwritten = write(fd_to, out_ptr, nread);

            if(nwritten >= 0)
            {
                nread -= nwritten;
                out_ptr += nwritten;
            }
            else if(errno != EINTR)
            {
                goto out_error;
            }
        } while(nread > 0);
    }

    if(nread == 0)
    {
        if(close(fd_to) < 0)
        {
            fd_to = -1;
            goto out_error;
        }
        close(fd_from);

        /* Success! */
        return DEFINES_MDS_API_OK;
    }

out_error:
    saved_errno = errno;

    if(fd_from >= 0)
    {
        close(fd_from);
    }
    
    if(fd_to >= 0)
    {
        close(fd_to);
    }

    errno = saved_errno;

    return DEFINES_MDS_API_NOK;
}



void mds_api_rm_all(char *dir)
{
        DIR *theFolder;
        struct dirent *next_file;
        char filepath[256];

        if(dir == NULL)
        {
                return;
        }

        theFolder = opendir(dir);
        if(theFolder == NULL)
        {
                return;
        }

        while ( (next_file = readdir(theFolder)) != NULL )
        {
                if (0==strcmp(next_file->d_name, ".") || 0==strcmp(next_file->d_name, "..")) { continue; }

                // build the path for each file in the folder
                snprintf(filepath, sizeof(filepath)-1, "%s/%s", dir, next_file->d_name);
                remove(filepath);
        }
        
        closedir(theFolder);
}



int mds_api_get_file_size(const char* file_path)
{
    FILE *fp;
    unsigned int sz = 0;
    
    if(_isExistFile(file_path, 3) < 0) {
        return -1;
    }
    
    fp = fopen(file_path, "r");
    if(fp == NULL)
    {
        return -1;
    }
    
    fseek(fp, 0L, SEEK_END);
    sz = ftell(fp);
    
    printf("log file size : %u\n", sz);
    
    fclose(fp);
    
    return sz;
}



// -----------------------------





int mds_api_storage_load_file(char *file, void *pdata, int length)
{
    int ret;

    if(_isExistFile(file, 3) < 0) {
        if(_file_recovery(file) != ERR_NONE)
            return -ERR_FILE_NOT_EXIST;
    }

    ret = _load_file(file, pdata, length);

    if(ret == -ERR_DATA_CRC) {
        if(_file_recovery(file) == ERR_NONE)
            ret = _load_file(file, pdata, length);
    }

    return ret;
}

int mds_api_storage_save_file(const char *file, void *pdata, int length)
{
    int fd;
    unsigned short crc16;

    _file_backup(file);

    fd = open( file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd < 0) {
        printf("%s> %s file open fail : err[%d]\n", __func__, file, errno);
        return -ERR_FILE_OPEN;
    }

    mds_api_crc16_get(NULL, 0);     
    crc16 = mds_api_crc16_get((unsigned char *)pdata, length);

    write(fd, pdata, length);
    write(fd, &crc16, 2);
    close(fd);

    return 0;
}

