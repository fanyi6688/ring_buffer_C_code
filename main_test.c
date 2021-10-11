#include <stdio.h>
#include <stdlib.h>

#include "ring_buffer.h"

int main()
{
    char c;
    int readLen;
    int i = 0;
    
    unsigned char readBuffer[10];
    /* 下面的 setvbuf 函数设置标准输出为无缓冲模式. 如果 printf、putchar
        不能立即输出, 打开此注释. */
    // setvbuf(stdout,NULL,_IONBF,0);
    init_ring_buff();

    printf("Please enter a choice: ");
    do
    {
        c = getchar();
        switch(c)
        {
            case 'q':  // 结束程序.
                goto exit;
                break;

            case 'r':  // 读取环形缓冲区的数据.
                readLen = read_ring_buff(readBuffer, 10);
                printf("read_ring_buff len: %d\n", readLen);
                if(readLen > 0)
                {
                    printf("read_ring_buff data: ");
                    for(i=0; i<readLen; i++)
                    {
                        printf("%c ", (char)readBuffer[i]);
                    }
                    printf("\n");
                }
                break;

            default:  // 如果没有指定有效选项, 则向环形缓冲区写入数据.
                if(c != '\n')
                {
                    wirte_ring_buff((unsigned char*)&c, 1);
                }
                break;
        }
    }
    while(1);

exit:
    release_ring_buff();
    printf("exit.\n");
    return 0;
}

