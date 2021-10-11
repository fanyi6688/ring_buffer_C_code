#include <stdio.h>
#include <stdlib.h>

#include "ring_buffer.h"

int main()
{
    char c;
    int readLen;
    int i = 0;
    
    unsigned char readBuffer[10];
    /* ����� setvbuf �������ñ�׼���Ϊ�޻���ģʽ. ��� printf��putchar
        �����������, �򿪴�ע��. */
    // setvbuf(stdout,NULL,_IONBF,0);
    init_ring_buff();

    printf("Please enter a choice: ");
    do
    {
        c = getchar();
        switch(c)
        {
            case 'q':  // ��������.
                goto exit;
                break;

            case 'r':  // ��ȡ���λ�����������.
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

            default:  // ���û��ָ����Чѡ��, �����λ�����д������.
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

