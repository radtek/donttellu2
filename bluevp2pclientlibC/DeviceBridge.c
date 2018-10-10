#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include "DeviceBridge.h"
#include "NetSocket.h"
#include "DataPool.h"
//*********************
uint8_t u8CRC8Table[256] =
{
    0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15,
    0x38, 0x3f, 0x36, 0x31, 0x24, 0x23, 0x2a, 0x2d,
    0x70, 0x77, 0x7e, 0x79, 0x6c, 0x6b, 0x62, 0x65,
    0x48, 0x4f, 0x46, 0x41, 0x54, 0x53, 0x5a, 0x5d,
    0xe0, 0xe7, 0xee, 0xe9, 0xfc, 0xfb, 0xf2, 0xf5,
    0xd8, 0xdf, 0xd6, 0xd1, 0xc4, 0xc3, 0xca, 0xcd,
    0x90, 0x97, 0x9e, 0x99, 0x8c, 0x8b, 0x82, 0x85,
    0xa8, 0xaf, 0xa6, 0xa1, 0xb4, 0xb3, 0xba, 0xbd,
    0xc7, 0xc0, 0xc9, 0xce, 0xdb, 0xdc, 0xd5, 0xd2,
    0xff, 0xf8, 0xf1, 0xf6, 0xe3, 0xe4, 0xed, 0xea,
    0xb7, 0xb0, 0xb9, 0xbe, 0xab, 0xac, 0xa5, 0xa2,
    0x8f, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9d, 0x9a,
    0x27, 0x20, 0x29, 0x2e, 0x3b, 0x3c, 0x35, 0x32,
    0x1f, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0d, 0x0a,
    0x57, 0x50, 0x59, 0x5e, 0x4b, 0x4c, 0x45, 0x42,
    0x6f, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7d, 0x7a,
    0x89, 0x8e, 0x87, 0x80, 0x95, 0x92, 0x9b, 0x9c,
    0xb1, 0xb6, 0xbf, 0xb8, 0xad, 0xaa, 0xa3, 0xa4,
    0xf9, 0xfe, 0xf7, 0xf0, 0xe5, 0xe2, 0xeb, 0xec,
    0xc1, 0xc6, 0xcf, 0xc8, 0xdd, 0xda, 0xd3, 0xd4,
    0x69, 0x6e, 0x67, 0x60, 0x75, 0x72, 0x7b, 0x7c,
    0x51, 0x56, 0x5f, 0x58, 0x4d, 0x4a, 0x43, 0x44,
    0x19, 0x1e, 0x17, 0x10, 0x05, 0x02, 0x0b, 0x0c,
    0x21, 0x26, 0x2f, 0x28, 0x3d, 0x3a, 0x33, 0x34,
    0x4e, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5c, 0x5b,
    0x76, 0x71, 0x78, 0x7f, 0x6A, 0x6d, 0x64, 0x63,
    0x3e, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2c, 0x2b,
    0x06, 0x01, 0x08, 0x0f, 0x1a, 0x1d, 0x14, 0x13,
    0xae, 0xa9, 0xa0, 0xa7, 0xb2, 0xb5, 0xbc, 0xbb,
    0x96, 0x91, 0x98, 0x9f, 0x8a, 0x8D, 0x84, 0x83,
    0xde, 0xd9, 0xd0, 0xd7, 0xc2, 0xc5, 0xcc, 0xcb,
    0xe6, 0xe1, 0xe8, 0xef, 0xfa, 0xfd, 0xf4, 0xf3
};
#define proccrc8(u8CRC, u8Data) (u8CRC8Table[u8CRC ^ u8Data])

//*********************
static int g_nCom = -1;
static struct termios g_termios;
static int g_nRun = 0;
//*********************
static int GetCRC(uint8_t data[],const int nLen)
{
    uint8_t u8CRC = 0;
    int i = 0;
    for(i = 0; i < nLen; ++i)
    {
        u8CRC = proccrc8(u8CRC, data[i]);
    }
    printf("<%s:%d>CRC=[%02X]\n",__FILE__,__LINE__,u8CRC);
    return u8CRC;
}
static int DataReader(char buf[],int nLen)
{
    if(0 > g_nCom)
    {
        return -1;
    }
    return read(g_nCom,buf,nLen);
}
static int DataWriter(const char buf[],const int nLen)
{
    if(0 > g_nCom)
    {
        return -1;
    }
    return write(g_nCom,buf,nLen);
}
//*********************
int OpenDevice(const char* szName)
{
    int nRet = 1;
    do
    {
        CloseDevice();
        if(0 > (g_nCom = open(szName,O_RDWR)))
        {
            printf("<%s:%d>open [%s] error=[%d]\n",__FILE__,__LINE__,szName,g_nCom);
            break;
        }
        nRet = fcntl(g_nCom,F_SETFL,0);
        if(nRet)
        {
            printf("<%s:%d>fcntl error=[%d]\n",__FILE__,__LINE__,nRet);
            break;
        }
        memset(&g_termios,0x00,sizeof(g_termios));
        //tcflush(g_nCom,TCIOFLUSH);
        //Set Baud :19200
        cfsetispeed(&g_termios,B19200);
        cfsetospeed(&g_termios,B19200);
        g_termios.c_cflag |=CLOCAL |CREAD;
        //Data bit :8bits
        g_termios.c_cflag &= ~CSIZE;
        g_termios.c_cflag |= CS8;
        //even-odd :No
        g_termios.c_cflag &= ~PARENB;
        //g_termios.c_iflag &= ~INPCK;
        //Stop Bits :1
        g_termios.c_cflag &= ~CSTOPB;
        //set Timeouts:10s
//        g_termios.c_cc[VTIME] = 100;
//        g_termios.c_cc[VMIN] = 0;
        //
        nRet = tcsetattr(g_nCom,TCSANOW,&g_termios);
        if(0 != nRet)
        {
            printf("<%s:%d>tcsetattr error=[%d]\n",__FILE__,__LINE__,nRet);
            break;
        }
        tcflush(g_nCom,TCIOFLUSH);
        g_nRun = 1;
        nRet = 0;
    }
    while(0);
    if(nRet)
    {
        CloseDevice();
    }
    return nRet;
}
int CloseDevice(void)
{
    int nRet = 1;
    g_nRun = 0;
    if(0 <= g_nCom)
    {
        close(g_nCom);
        g_nCom = -1;
    }
    return nRet;
}

int ShutOnLight(void)
{
    char buf[1024];
//    int nLen = sizeof(buf);
    memset(buf,0x00,sizeof(buf));
    //同步码 + 数据长度 + 数据类型 + 包头CRC校验 + 数据 + 数据CRC校验
    //电灯状态
    //0x66  + 0x02   + 0x02   +    Head CRC + 0x01 + Data CRC
    buf[0] = 0x66;
    buf[1] = 0x02;
    buf[2] = 0x01;
    buf[3] = GetCRC((uint8_t*)&(buf[1]),2);
    buf[4] = 0x01;
    buf[5] = 0x01;
    buf[6] = GetCRC((uint8_t*)&(buf[4]),2);

    printf("<%s(%d):%s>buf=[0x%02X",__FILE__,__LINE__,__FUNCTION__,buf[0]);
    int i = 1;
    for(; i < 7; ++i)
    {
        printf(",0x%02X ",buf[i]);
    }
    printf("]\n");

    int nRet = DataWriter(buf,strlen(buf));
    if(nRet != strlen(buf))
    {
        return -1;
    }

    return 0;
}
int ShutOffLight(void)
{
    char buf[1024];
//    int nLen = sizeof(buf);
    memset(buf,0x00,sizeof(buf));
    //同步码 + 数据长度 + 数据类型 + 包头CRC校验 + 数据 + 数据CRC校验
    //电灯状态
    //0x66  + 0x02   + 0x02   +    Head CRC + 0x01 + Data CRC
    buf[0] = 0x66;
    buf[1] = 0x02;
    buf[2] = 0x01;
    buf[3] = GetCRC((uint8_t*)&(buf[1]),2);
    buf[4] = 0x01;
    buf[5] = 0x00;
    buf[6] = GetCRC((uint8_t*)&(buf[4]),2);

    printf("<%s(%d):%s>buf=[0x%02X",__FILE__,__LINE__,__FUNCTION__,buf[0]);
    int i = 1;
    for(; i < 7; ++i)
    {
        printf(",0x%02X ",buf[i]);
    }
    printf("]\n");

    int nRet = DataWriter(buf,strlen(buf));
    if(nRet != strlen(buf))
    {
        return -1;
    }

    return 0;
}
int GetLightState(void)
{
    char buf[1024];
//    int nLen = sizeof(buf);
    memset(buf,0x00,sizeof(buf));
    //同步码 + 数据长度 + 数据类型 + 包头CRC校验 + 数据 + 数据CRC校验
    //电灯状态
    //0x66  + 0x01   + 0x02   +    Head CRC + 0x01 + Data CRC
    buf[0] = 0x66;
    buf[1] = 0x01;
    buf[2] = 0x02;
    buf[3] = GetCRC((uint8_t*)&(buf[1]),2);
    buf[4] = 0x01;
    buf[5] = GetCRC((uint8_t*)&(buf[4]),1);

    printf("<%s(%d):%s>buf=[0x%02X",__FILE__,__LINE__,__FUNCTION__,buf[0]);
    int i = 1;
    for(; i < 6; ++i)
    {
        printf(",0x%02X ",buf[i]);
    }
    printf("]\n");
    int nRet = DataWriter(buf,strlen(buf));
    if(nRet != strlen(buf))
    {
        return -1;
    }

    return 0;
}

void WatchComm(int* nRun)
{
    fd_set rd;
    struct timeval tmval;
    int nRet = 0;
    char buf[1024];
    int nLen = sizeof(buf);
    int i = 0;
    uint8_t crclen = 0x00;
    uint8_t crcdata = 0x00;
    while(nRun && *nRun)
    {
        tmval.tv_sec = 10;
        tmval.tv_usec = 0;
        FD_ZERO(&rd);
        FD_SET(g_nCom,&rd);
        nRet = select(g_nCom + 1,&rd,NULL,NULL,&tmval);
        if(0 < nRet && FD_ISSET(g_nCom,&rd))
        {
            memset(buf,0x00,sizeof(buf));
            nLen = sizeof(buf);
            nRet = DataReader(buf,nLen);
            printf("<%s(%d):%s>DataReader=[%d]\n",__FILE__,__LINE__,__FUNCTION__,nRet);
            if(7 > nRet)
            {
                continue;
            }
            printf("<%s(%d):%s>data=[0x%02X",__FILE__,__LINE__,__FUNCTION__,buf[0]);
            for(i = 1; i < nRet; ++i)
            {
                printf(",0x%02X",buf[i]);
            }
            printf("]\n");
            crclen = GetCRC((uint8_t*)&(buf[1]),2);
            crcdata = GetCRC((uint8_t*)&(buf[4]),(int)buf[1]);
            //printf("<%s(%d):%s>crclen=[0x%02X],crcdata=[0x%02X]\n",__FILE__,__LINE__,__FUNCTION__,crclen,crcdata);
            if(7 <= nRet && 0x66 == buf[0] && 0x02 == buf[2] && crclen == buf[3]
                    && 0x01 == buf[4] && crcdata == buf[nRet - 1])
            {
                if(0x00 == buf[5])
                {
                    g_TD[0].eState = TDSTATE_SHUTOFF;
                    AddTaskData();
                }
                else if(0x01 == buf[5])
                {
                    g_TD[0].eState = TDSTATE_SHUTON;
                    AddTaskData();
                }
            }
        }
        else if(0 == nRet)
        {
            printf("<%s(%d):%s>Timeout\n",__FILE__,__LINE__,__FUNCTION__);
        }
        else
        {
            printf("<%s(%d):%s>Error=[%d]\n",__FILE__,__LINE__,__FUNCTION__,nRet);
        }
    }
}

int AutoRecall(struct timeval* ptmval)
{
    fd_set rd;
    FD_ZERO(&rd);
    FD_SET(g_nCom,&rd);
    printf("<%s(%d):%s>select Start\n",__FILE__,__LINE__,__FUNCTION__);
    int nRet = select(g_nCom + 1,&rd,NULL,NULL,ptmval);
    if(1 == nRet)
    {
        printf("<%s:%d>select=\n",__FILE__,__LINE__);
        if(FD_ISSET(g_nCom,&rd))
        {
            char buf[1024];
            int nLen = sizeof(buf);
            memset(buf,0x00,sizeof(buf));
            nLen = sizeof(buf);
            nRet = DataReader(buf,nLen);
            printf("<%s(%d):%s>DataReader=[%d],data=[0x%02X",__FILE__,__LINE__,__FUNCTION__,nRet,buf[0]);
            int i = 1;
            for(; i < nRet; ++i)
            {
                printf(",0x%02X",buf[i]);
            }
            printf("]\n");

            uint8_t crclen = GetCRC((uint8_t*)&(buf[1]),2);
            uint8_t crcdata = GetCRC((uint8_t*)&(buf[4]),(int)buf[1]);
            printf("<%s(%d):%s>crclen=[0x%02X],crcdata=[0x%02X]\n",__FILE__,__LINE__,__FUNCTION__,crclen,crcdata);
            if(7 <= nRet && 0x66 == buf[0] && 0x02 == buf[2] && crclen == buf[3]
                    && 0x01 == buf[4] && crcdata == buf[nRet - 1])
            {
                if(0x00 == buf[5])
                {
                    g_TD[0].eState = TDSTATE_SHUTOFF;
                    nRet = 0;
                }
                else if(0x01 == buf[5])
                {
                    g_TD[0].eState = TDSTATE_SHUTON;
                    nRet = 0;
                }
                else
                {
                    nRet = 1;
                }
            }
            else
            {
                nRet = 2;
            }
        }
        else
        {
            nRet = 3;
        }
    }
    else if(0 == nRet)
    {
        printf("<%s:%d>Timeout\n",__FILE__,__LINE__);
        nRet = -1;
    }
    else
    {
        printf("<%s:%d>Error=[%d]\n",__FILE__,__LINE__,nRet);
        nRet = -2;
    }
    return nRet;
}
