#include <stdio.h>
#include <stdlib.h>


#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

#define TL500_BATT_ADC_ROOT_DIR     "/sys/devices"
#define TL500_BATT_ADC_DIR_PREFIX   "qpnp-vadc-"

// /sys/devices/qpnp-vadc-

#include <mdsapi/mds_api.h>

mdsapiRet_t mds_api_get_internal_batt_tl500(int* voltage)
{
    static char batt_adc_path[128] ={0,};

    char read_buff[128] = {0,};
    char vol_num_buff[8] = {0,};

    if ( strlen(batt_adc_path) == 0 )
    {
        DIR *dp;
        struct dirent *ep;

        dp = opendir (TL500_BATT_ADC_ROOT_DIR);
        if (dp != NULL)
        {
            while (ep = readdir (dp))
            {
                if ( strncmp(ep->d_name, TL500_BATT_ADC_DIR_PREFIX, strlen(TL500_BATT_ADC_DIR_PREFIX)) == 0)
                {
                    sprintf(batt_adc_path, "%s/%s/vph_pwr", TL500_BATT_ADC_ROOT_DIR,ep->d_name);
                    break;
                }
            }
            (void) closedir (dp);
        }
        else
            perror ("Couldn't open the directory");
    }

    printf("batt_adc_dir is [%s]\r\n", batt_adc_path);
    if (mds_api_check_exist_file(batt_adc_path,1) == DEFINES_MDS_API_NOK)
        return DEFINES_MDS_API_NOK;

    mds_api_read_data(batt_adc_path, read_buff, 128);

    printf("adc read string is [%s] \r\n", read_buff);
    // Result:4046562 Raw:967d
    {
        char *tr;
        char token_0[ ] = ": ";
        char token_1[ ] = "\r\n";
        char *temp_bp = NULL;
        
        char *p_cmd = NULL;
        
        p_cmd = read_buff;
        
        tr = strtok_r(p_cmd, token_0, &temp_bp);
        if(tr == NULL) return DEFINES_MDS_API_NOK;
        //printf("tok [%s]\r\n", tr);

        tr = strtok_r(NULL, token_0, &temp_bp);
        if(tr == NULL) return DEFINES_MDS_API_NOK;
        //voltage_val = strtol(tr,p_tmp,10);
        //printf("tok [%s]\r\n", tr);
        // 왼쪽에서 3자리만 출력
        sprintf(vol_num_buff, "%.*s\n", 3, tr);
        *voltage = atoi(vol_num_buff);
        return DEFINES_MDS_API_OK;
        //printf("long int [%lu]\r\n",voltage_val);
        /*
        tr = strtok_r(NULL, token_0, &temp_bp);
        if(tr == NULL) return DEFINES_MDS_API_NOK;
        //printf("tok [%s]\r\n", tr);
        
        tr = strtok_r(NULL, token_1, &temp_bp);
        if(tr == NULL) return DEFINES_MDS_API_NOK;
        //printf("tok [%s]\r\n", tr);
        */
    }

}






