/****************************************************************
�˴�����Դ��������ַ:
http://www.360doc.com/content/19/1205/18/2690044_877670510.shtml
****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "ring_buffer.h"

#define RING_BUFF_SIZE  10        // ���λ������ܳ���, ���԰����޸�.

static struct ring_buff_s RING_STRU;

/****************************************************************
Description: ��ʼ�����λ�����.
Others     : ���λ����������� malloc ������ڴ�, Ҳ������ Flash 
             �洢����.
****************************************************************/
void init_ring_buff(void)
{
    // ��ʼ��ȫ�ֱ��� RING_STRU.
    RING_STRU.len_used = 0;
    RING_STRU.p_head = NULL;
    RING_STRU.p_tail = NULL;
    RING_STRU.p_head_used = NULL;
    RING_STRU.p_tail_used = NULL;

    if(RING_STRU.p_head == NULL)
    {
        RING_STRU.p_head = (unsigned char*)malloc(RING_BUFF_SIZE);
    }
    
    RING_STRU.p_head_used = RING_STRU.p_tail_used = RING_STRU.p_head;
    RING_STRU.p_tail = RING_STRU.p_head + RING_BUFF_SIZE;
}

/****************************************************************
Description: ���λ�������д������.
Parameters : buffer  : ��д�����ݵ��׵�ַ.
           : data_len: ��д�����ݵĳ���.
Returns    : -1: ��д�����ݵĳ��ȳ������λ��������ܳ���.
             -2: ���λ�����û�г�ʼ��.
              0: �����ɹ�.
****************************************************************/
int wirte_ring_buff(unsigned char* buffer, unsigned int data_len)
{
    int len1 = 0, len2 = 0, moveLen = 0, len3 = 0, len4 = 0;

    if(data_len > RING_BUFF_SIZE)
    {
        return -1;
    }
    
    if(RING_STRU.p_head == NULL)
    {
        return -2;
    }

    assert(buffer);  // ��� buffer Ϊ NULL, ���������.

    /* ��Ҫд������� memcpy �� RING_STRU.p_tail_used ��. */
    /* ������ʣ��ռ䲻��, ��Ҫ�ֳ�����д��. */
    if(RING_STRU.p_tail_used + data_len > RING_STRU.p_tail)
    {
        len1 = RING_STRU.p_tail - RING_STRU.p_tail_used;  // ������ʣ����ÿռ�ĳ���.
        /* д��������ĩβ֮��, ʣ���д�����ݵĳ���. �ⲿ��������Ҫ
            �ӻ������׵�ַд��.*/
        len2 = data_len - len1;
        memcpy(RING_STRU.p_tail_used, buffer, len1);
        memcpy(RING_STRU.p_head, buffer + len1, len2);
        RING_STRU.p_tail_used = RING_STRU.p_head + len2;  // ������ʹ�û�������β��ַ.
    }
    else // ������ʣ��ռ乻��.
    {
        memcpy(RING_STRU.p_tail_used, buffer, data_len);
        RING_STRU.p_tail_used += data_len;  // ������ʹ�û�������β��ַ.
    }

    /* RING_STRU.len_used + data_len: �µ���ʹ�û������ĳ���. */
    /* ����µ���ʹ�û������ĳ��ȴ��ڻ������ܳ���, ����Ҫ���¼�����ʹ�û�����
       ���׵�ַ RING_STRU.p_head_used. ����, RING_STRU.p_head_used ����. */
    if(RING_STRU.len_used + data_len > RING_BUFF_SIZE)
    {
        /* moveLen: ��ʹ�û��������׵�ַ RING_STRU.p_head_used ��Ҫ�ƶ��ľ���. */
        moveLen = RING_STRU.len_used + data_len - RING_BUFF_SIZE;

        /* ��� RING_STRU.p_head_used �ƶ�֮��, �����������׵�ַ, ����Ҫ
            �ӻ������׵�ַ��ʼ�ƶ�һ�κ��ʵľ���. */
        if(RING_STRU.p_head_used + moveLen > RING_STRU.p_tail)
        {
            len3 = RING_STRU.p_tail - RING_STRU.p_head_used;  // ���� RING_STRU.p_tail ��������β��ַ�ľ���.
            len4 = moveLen - len3;  // ���� RING_STRU.p_head_used �ӻ������׵�ַ��ʼ, ����Ҫ�ƶ����پ���.
            RING_STRU.p_head_used = RING_STRU.p_head + len4;
        }
        else
        {
            RING_STRU.p_head_used = RING_STRU.p_head_used + moveLen;
        }
        RING_STRU.len_used = RING_BUFF_SIZE;  // ��������������д����.
    }
    else
    {
        RING_STRU.len_used += data_len;      // ������û�б�д��.
    }

    return 0;
}

/****************************************************************
Description: �ӻ������ڶ�������.
Parameters : buffer: �������������.
             len   : ָ����ȡ���ݵĳ���.
Returns    : -1: û�г�ʼ��.
              0: ��������û������.
             >0: ʵ�ʶ�ȡ�ĳ���.
****************************************************************/
int read_ring_buff(unsigned char* buffer, unsigned int len)
{
    int len1 = 0, len2 = 0;

    if(RING_STRU.p_head == NULL)
    {
        return -1;
    }
    assert(buffer);  // ��� buffer Ϊ NULL, ���������.

    if(RING_STRU.len_used == 0)
    {
        return 0;
    }

    /* ���Ҫ���ȡ���ݵĳ��ȴ��ڻ���������Ч���ݵĳ���, ���ܹ���ȡ�ĳ���
       ���ֻ��Ϊ��Ч���ݵĳ���. */
    if(len > RING_STRU.len_used)
    {
        len = RING_STRU.len_used;
    }

    /* �����ʹ��������׵�ַ���ϴ���ȡ���ݵĳ���֮��, ����������β��ַ,
       ����Ҫ�ֳ����ν��ж�ȡ. */
    if(RING_STRU.p_head_used + len > RING_STRU.p_tail)
    {
        len1 = RING_STRU.p_tail - RING_STRU.p_head_used; // �� RING_STRU.p_tail ��ʼ��ĩβ���������ݵĳ���.
        /* ��ȡ��� RING_STRU.p_tail ��ʼ��ĩβ����������֮��, ��ʣ����������Ҫ�ӻ������׵�ַ��ʼ��ȡ. */
        len2 = len - len1;
        memcpy(buffer, RING_STRU.p_head_used, len1); // ��ȡ�� RING_STRU.p_tail ��ʼ��ĩβ����������.
        memcpy(buffer+len1, RING_STRU.p_head, len2); // �ӻ������׵�ַ��ʼ, ��ȡʣ�೤�ȵ�����.
        RING_STRU.p_head_used = RING_STRU.p_head + len2;  // ������ʹ�û��������׵�ַ.
    }
    else
    {
        memcpy(buffer, RING_STRU.p_head_used, len);
        RING_STRU.p_head_used = RING_STRU.p_head_used + len;  // ������ʹ�û��������׵�ַ.
    }

    RING_STRU.len_used -= len;            // ������ʹ�û������ĳ���.
    return len;
}

/****************************************************************
Description: ��ȡ����������ʹ������ĳ���.
Returns    : RING_STRU.len_used: ����������ʹ������ĳ���.
****************************************************************/
unsigned int get_ring_buff_used_len(void)
{
    return RING_STRU.len_used;
}

/****************************************************************
Description: �ͷŻ��λ�����.
****************************************************************/
void release_ring_buff(void)
{
    if(RING_STRU.p_head != NULL)
    {
        free(RING_STRU.p_head);
    }
    RING_STRU.p_head = NULL;
}

