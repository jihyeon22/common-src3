
#include <stdio.h>
#include <stdlib.h>

#include <mdsapi/mds_api.h>

// test for cl server system..
//  - c# encrypt api test 

#define ENCRYPT_KEY "Ijkl67vc5AETBfFO"

// c# api result...
#define ORIGINAL_STR "[MPT@@0122487746407201170608224709A37.42334127.1853704411200000E10E101z]"
#define ENCRYPT_STR "XCI1F8BiMs3rZVDS0QVC55LNUIUP9WpwfuOzVOfoCnDv/77obGnaeBTgWNZMcKljdDfc74P3o3VKDIvtzmC+2XFkZyrfShW7S8V+ZUwvdOc="

static void test_decrypt_cbc(void)
{
    char test_encrypt_base64_str[] = ENCRYPT_STR;

    int ret_base64_size = 0;
    
    char in_key[16] ={0,};

    unsigned char encrypt_data[512] = { 0, };
    unsigned char decrypt_data[512] = { 0, };

    int i = 0;

    strcpy(in_key, ENCRYPT_KEY);

    // 1. decrypt base 64 string..
    ret_base64_size = mds_api_b64_decode(ENCRYPT_STR, strlen(ENCRYPT_STR), encrypt_data, sizeof(encrypt_data));
    printf("base64 decode size : [%d]\r\n", ret_base64_size);

    // 2. decrypt binary data..
    mds_api_AES_CBC_decrypt_buffer(decrypt_data, encrypt_data, ret_base64_size, in_key, in_key);

   
    for (i = 0; i < ret_base64_size; i++)
    {
        // printf("[%d] : %d => [%c]\r\n", i, decrypt_data[i], decrypt_data[i]);
        if ( decrypt_data[ret_base64_size - i] == 0x08 )
        {
            decrypt_data[ret_base64_size - i] = '\0';
        }
        else
            break;
    }

    printf("CBC decrypt: [%s]\r\n", decrypt_data);
}


static void test_decrypt_cbc2(void)
{
    char test_encrypt_base64_str[] = ENCRYPT_STR;

    int ret_base64_size = 0;
    
    char in_key[16] ={0,};

    unsigned char encrypt_data[512] = { 0, };
    unsigned char decrypt_data[512] = { 0, };

    int i = 0;
    int decrypt_size = 0;

    strcpy(in_key, ENCRYPT_KEY);

    // 1. decrypt base 64 string..
    ret_base64_size = mds_api_b64_decode(ENCRYPT_STR, strlen(ENCRYPT_STR), encrypt_data, sizeof(encrypt_data));
    printf("base64 decode size : [%d]\r\n", ret_base64_size);

    // 2. decrypt binary data..
    decrypt_size = mds_api_AES_CBC_decrypt_buffer2(decrypt_data, encrypt_data, ret_base64_size, in_key, in_key, 0x08);


    printf("CBC decrypt: [%s] [%d]\r\n", decrypt_data, decrypt_size);
}

static void test_encrypt_cbc(void)
{
    char target_str[] = ORIGINAL_STR;

    int i = 0;
    
    char in_key[16] ={0,};

    unsigned char encrypt_data[512] = { 0, };
    unsigned char input_data_buff[512] = { 0, };
    int encrypt_target_len = 0;
    
    char base64_str_buff[512] ={ 0 ,};
    int ret_base64_size = 0;
    
    strcpy(in_key, ENCRYPT_KEY);

    encrypt_target_len = mds_api_AES_input_buff_init(input_data_buff, sizeof(input_data_buff), ORIGINAL_STR, strlen(ORIGINAL_STR), 0x08);
    printf("encrypt_target_len is [%d]\r\n", encrypt_target_len);
    mds_api_AES_CBC_encrypt_buffer(encrypt_data, input_data_buff, encrypt_target_len, in_key, in_key);

    printf("CBC encrypt: ");
    ret_base64_size = mds_api_b64_encode(encrypt_data, encrypt_target_len, base64_str_buff, sizeof(base64_str_buff));


    printf(">> %d <<\r\n", ret_base64_size);
    printf(">> %s <<\r\n", base64_str_buff);
}


static void test_encrypt_cbc2(void)
{
    char target_str[] = ORIGINAL_STR;

    int i = 0;
    
    char in_key[16] ={0,};

    unsigned char encrypt_data[512] = { 0, };
    unsigned char input_data_buff[512] = { 0, };
    int encrypt_target_len = 0;
    
    char base64_str_buff[512] ={ 0 ,};
    int ret_base64_size = 0;
    
    strcpy(in_key, ENCRYPT_KEY);

    encrypt_target_len = mds_api_AES_CBC_encrypt_buffer2(encrypt_data, ORIGINAL_STR, strlen(ORIGINAL_STR), in_key, in_key, 0x08);

    printf("CBC encrypt2: ");
    ret_base64_size = mds_api_b64_encode(encrypt_data, encrypt_target_len, base64_str_buff, sizeof(base64_str_buff));

    printf(">> %d <<\r\n", ret_base64_size);
    printf(">> %s <<\r\n", base64_str_buff);
}


static void test_encrypt_cbc2_malloc(void)
{
    char target_str[] = ORIGINAL_STR;

    int i = 0;
    
    char in_key[16] ={0,};

    unsigned char* encrypt_data_p = NULL;
    int encrypt_target_len = 0;
    
    char base64_str_buff[512] ={ 0 ,};
    int ret_base64_size = 0;
    
    strcpy(in_key, ENCRYPT_KEY);

    encrypt_target_len = mds_api_AES_CBC_encrypt_buffer2_malloc(&encrypt_data_p, ORIGINAL_STR, strlen(ORIGINAL_STR), in_key, in_key, 0x08);

    printf("CBC encrypt2: ");
    ret_base64_size = mds_api_b64_encode(encrypt_data_p, encrypt_target_len, base64_str_buff, sizeof(base64_str_buff));

    printf(">> %d <<\r\n", ret_base64_size);
    printf(">> %s <<\r\n", base64_str_buff);

    free(encrypt_data_p);
}



void main()
{
    // api test 
    //mds_api_write_data_maxsize("/data/mds/log/log.test.txt","testmsg\r\n",strlen("testmsg\r\n"),128);
    printf("%s start\r\n",__TIME__);
//    mds_api_poweroff_and_log("testapp", "bye");
    test_encrypt_cbc2_malloc();
    test_decrypt_cbc2();
}
