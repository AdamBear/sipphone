#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <getopt.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ippaging.h"

#define CONFIG_PROGRAM "ippaging"
#define CONFIG_VERSION "1.0.0.0"


int do_start(PAGING_PARAM * param)
{
    printf("do_start.\n");

    strcpy(param->local_ip_address, "224.0.15.68");
    param->local_udp_port = 7308;
    printf("param->local_ip_address = %s\n", param->local_ip_address);
    printf("param->local_udp_port = %d\n", param->local_udp_port);
    printf("param->remote_ip_address = %s\n", param->remote_ip_address);
    printf("param->remote_udp_port = %d\n", param->remote_udp_port);
    printf("param->request_ip_address = %s\n", param->request_ip_address);
    printf("param->payload_type = %d\n", param->payload_type);
    printf("param->codec_name = %s\n", param->codec_name);

    param->paging_in = 1;

    /*local 为 要paging到哪个IP,哪个端口*/
    //strcpy(param->local_ip_address, "10.2.2.51");
    //param->local_udp_port = 3000;

    paging_start(param);
}

int do_stop(PAGING_PARAM * param)
{
    printf("do_stop.\n");
    strcpy(param->local_ip_address, "10.2.2.140");
    param->local_udp_port = 3005;

    printf("param->local_ip_address = %s\n", param->local_ip_address);
    printf("param->local_udp_port = %d\n", param->local_udp_port);
    printf("param->remote_ip_address = %s\n", param->remote_ip_address);
    printf("param->remote_udp_port = %d\n", param->remote_udp_port);
    printf("param->request_ip_address = %s\n", param->request_ip_address);

    paging_stop(param, SP_STOP);
    printf("%d\n", __LINE__);
}

void input_info(PAGING_PARAM * param)
{
    char input_line[1024] = {0};
    printf("\nPlease input local ip address:");
    scanf("%s", param->local_ip_address);
    //strcpy(param->local_ip_address, input_line);

    printf("\nPlease input local udp port:");
    scanf("%d", &param->local_udp_port);

    printf("\nPlease input remote ip address:");
    scanf("%s", param->remote_ip_address);
    //strcpy(param->remote_ip_address, input_line);

    printf("\nPlease input remote udp port:");
    scanf("%d", &param->remote_udp_port);

    printf("param->local_ip_address = %s\n", param->local_ip_address);
    printf("param->local_udp_port = %d\n", param->local_udp_port);
    printf("param->remote_ip_address = %s\n", param->remote_ip_address);
    printf("param->remote_udp_port = %d\n", param->remote_udp_port);
}

int main(int argc, int argv[])
{
    int debug_level;
    int temp;
    //FD_ZERO(&g_fdr);

    //INIT_LIST_HEAD(&listen_line);
    paging_init();

    paging_set_event_callback(PAGING_EVENT_ID_START, do_start);
    paging_set_event_callback(PAGING_EVENT_ID_STOP, do_stop);

    sleep(1);

    PAGING_PARAM param;



#if 1
    memset(&param, 0, sizeof(PAGING_PARAM));
    strcpy(param.local_ip_address, "127.0.0.1");
    param.remote_udp_port = 7308;

    strcpy(param.remote_ip_address, "224.0.15.66");
    paging_add_listen(&param);


#if 0
    memset(&param, 0, sizeof(PAGING_PARAM));
    param.paging_in = 0;
    strcpy(param.local_ip_address, "10.2.2.135");
    strcpy(param.remote_ip_address, "224.0.15.65");
    param.local_udp_port = 3000;
    param.remote_udp_port = 7308;

    while (1)
    {
        sleep(1);
        paging_start(&param);
        sleep(1);
        paging_stop(&param);
    }

#endif
#endif

    //memset(&param, 0, sizeof(PAGING_PARAM));
    //param.paging_in = 1;
    //strcpy(param.local_ip_address, "10.2.2.139");
    //strcpy(param.remote_ip_address, "224.0.0.1");
    //param.local_udp_port = 3000;
    //paging_start(&param);


    //memset(&param, 0, sizeof(PAGING_PARAM));
    //strcpy(param.local_ip_address, "127.0.0.1");
//  param.local_udp_port = 3001;
    //strcpy(param.remote_ip_address, "10.2.2.51");
    //strcpy(param.remote_ip_address, "224.0.0.1");
    //paging_remove_listen(&param);


    char input_char[32] = {0};
    while (1)
    {
        memset(input_char, 0, sizeof(input_char));

        printf("\ninput :a/A=add line; r/R=remove line; s/S=start paging; p/P=stop paging, q/Q:quit\nPlease input:");

        scanf("%s", input_char);

        switch (input_char[0])
        {
        case 'q':
        case 'Q':
            paging_exit();
            //exit(0);
            break;
        case 'a':
        case 'A':
            input_info(&param);
            paging_add_listen(&param);
            break;
        case 'r':
        case 'R':
            input_info(&param);
            paging_remove_listen(&param);
            break;
        case 's':
        case 'S':
            printf("\ninput paging direct(0=paging out/1=paging in):");
            scanf("%d", &param.paging_in);
            input_info(&param);

            paging_start(&param);
            break;
        case 'p':
        case 'P':
            printf("\ninput paging direct(0=paging out/1=paging in):");
            scanf("%d", &param.paging_in);
            input_info(&param);
            paging_stop(&param, SP_STOP);
            break;
        case 't':
        case 'T':
            memset(&param, 0, sizeof(PAGING_PARAM));
            strcpy(param.local_ip_address, "127.0.0.1");
            param.remote_udp_port = 7309;

            strcpy(param.remote_ip_address, "224.0.15.66");
            paging_add_listen(&param);
            break;
        case 'd':
        case 'D':
            paging_display_info();
            break;
        case 'o':
        case 'O':
            memset(&param, 0, sizeof(PAGING_PARAM));
            param.paging_in = 0;
            strcpy(param.local_ip_address, "10.2.2.139");
            strcpy(param.remote_ip_address, "224.0.1.2");
            param.local_udp_port = 3000;
            param.remote_udp_port = 5000;
            paging_start(&param);
            break;
        case 'g':
        case 'G':

            scanf("%d", &debug_level);
            paging_set_debug_level(debug_level);
            break;
        default:
            break;
        }
        printf("%d\n", __LINE__);
        //sleep(1);
        printf("%d\n", __LINE__);
    }

    return 0;
}
