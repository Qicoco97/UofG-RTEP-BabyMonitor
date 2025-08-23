#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

//串口初始化
void init_tty(int fd)
{
        struct termios newtio;
        struct termios oldtio;
        bzero(&newtio,sizeof(struct termios));

        tcgetattr(fd,&oldtio);
        newtio.c_cflag |= CLOCAL | CREAD;
        cfsetispeed(&newtio,B2400);
        cfsetospeed(&newtio,B2400);
        printf("c_cflag=%x\n",newtio.c_cflag);
        newtio.c_cflag &= ~CSIZE;
        printf("c_cflag=%x\n",newtio.c_cflag);
        newtio.c_cflag |= CS8;
        printf("c_cflag=%x\n",newtio.c_cflag);
        newtio.c_cflag &= ~PARENB;
        newtio.c_iflag &= ~INPCK;
        newtio.c_cflag &= ~CSTOPB;
        newtio.c_cc[VTIME] = 0;
        newtio.c_cc[VMIN] = 0;
        //设置原始模式
        newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /*Input*/
        newtio.c_oflag &= ~OPOST; /*Output*/
        //设置终端模式
        //newtio.c_lflag |= (ICANON | ECHO | ECHOE | ISIG); /*Input*/
        //newtio.c_oflag |= OPOST; /*Output*/
        tcflush(fd,TCIOFLUSH);
        tcsetattr(fd,TCSANOW,&newtio);
}

//PM2.5模块数据处理
int da_pro(char* da_buf)
{
        unsigned char crr;
        double vout;
        int ud,i;
        if(da_buf==NULL)
                return -1;
        for(i=0;i<10;i++){
                if(da_buf[i]==0xAA)
                        break;
        }
       if(da_buf[i+0] != 0xAA || da_buf[i+6]!= 0xFF)
                return -1;
        crr = da_buf[i+1] + da_buf[i+2] + da_buf[i+3] + da_buf[i+4];
        if(da_buf[i+5] != crr)
                return -1;
        vout = (da_buf[i+1]*256+da_buf[i+2])/1024.0*8.0;
        ud = 800 * vout;
        return ud;
}


int main()
{
        int i;
        int fd = -1;
        int ret = -1;
        int pm;
        char buf[16];
        fd = open("/dev/ttyAMA0",O_RDWR);
        if(fd < 0){
                printf("没有串口设备/dev/ttyAMA0\n");
                exit(1);
        }

        printf("fd=%d\n",fd);
        init_tty(fd);

        while(1){
                bzero(buf,16);
                if(read(fd,buf,15)>0){
                        for(i=0;i<15;i++){
//                              printf(" %X ",buf[i]);
                        }
                        pm = 0;
                        pm=da_pro(buf);
                        if(pm==-1)
                                printf("recv data err...\n");
                        else
                                printf("PM2.5 = %d\n",pm);
                }
                usleep(300*1000);
        }
        close(fd);
}
