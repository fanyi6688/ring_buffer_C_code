/****************************************************************
此代码来源于如下网址:
http://www.360doc.com/content/19/1205/18/2690044_877670510.shtml
****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "ring_buffer.h"

#define RING_BUFF_SIZE  10        // 环形缓冲区总长度, 可以按需修改.

static struct ring_buff_s RING_STRU;

/****************************************************************
Description: 初始化环形缓冲区.
Others     : 环形缓冲区可以是 malloc 申请的内存, 也可以是 Flash 
             存储介质.
****************************************************************/
void init_ring_buff(void)
{
    // 初始化全局变量 RING_STRU.
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
Description: 向环形缓冲区中写入数据.
Parameters : buffer  : 待写入数据的首地址.
           : data_len: 待写入数据的长度.
Returns    : -1: 待写入数据的长度超过环形缓冲区的总长度.
             -2: 环形缓冲区没有初始化.
              0: 操作成功.
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

    assert(buffer);  // 如果 buffer 为 NULL, 则结束程序.

    /* 将要写入的数据 memcpy 到 RING_STRU.p_tail_used 处. */
    /* 缓冲区剩余空间不够, 需要分成两段写入. */
    if(RING_STRU.p_tail_used + data_len > RING_STRU.p_tail)
    {
        len1 = RING_STRU.p_tail - RING_STRU.p_tail_used;  // 缓冲区剩余可用空间的长度.
        /* 写到缓冲区末尾之后, 剩余待写入数据的长度. 这部分数据需要
            从缓冲区首地址写入.*/
        len2 = data_len - len1;
        memcpy(RING_STRU.p_tail_used, buffer, len1);
        memcpy(RING_STRU.p_head, buffer + len1, len2);
        RING_STRU.p_tail_used = RING_STRU.p_head + len2;  // 更新已使用缓冲区的尾地址.
    }
    else // 缓冲区剩余空间够用.
    {
        memcpy(RING_STRU.p_tail_used, buffer, data_len);
        RING_STRU.p_tail_used += data_len;  // 更新已使用缓冲区的尾地址.
    }

    /* RING_STRU.len_used + data_len: 新的已使用缓冲区的长度. */
    /* 如果新的已使用缓冲区的长度大于缓冲区总长度, 则需要重新计算已使用缓冲区
       的首地址 RING_STRU.p_head_used. 否则, RING_STRU.p_head_used 不变. */
    if(RING_STRU.len_used + data_len > RING_BUFF_SIZE)
    {
        /* moveLen: 已使用缓冲区的首地址 RING_STRU.p_head_used 需要移动的距离. */
        moveLen = RING_STRU.len_used + data_len - RING_BUFF_SIZE;

        /* 如果 RING_STRU.p_head_used 移动之后, 超过缓冲区首地址, 则需要
            从缓冲区首地址开始移动一段合适的距离. */
        if(RING_STRU.p_head_used + moveLen > RING_STRU.p_tail)
        {
            len3 = RING_STRU.p_tail - RING_STRU.p_head_used;  // 计算 RING_STRU.p_tail 到缓冲区尾地址的距离.
            len4 = moveLen - len3;  // 计算 RING_STRU.p_head_used 从缓冲区首地址开始, 还需要移动多少距离.
            RING_STRU.p_head_used = RING_STRU.p_head + len4;
        }
        else
        {
            RING_STRU.p_head_used = RING_STRU.p_head_used + moveLen;
        }
        RING_STRU.len_used = RING_BUFF_SIZE;  // 整个缓冲区都被写满了.
    }
    else
    {
        RING_STRU.len_used += data_len;      // 缓冲区没有被写满.
    }

    return 0;
}

/****************************************************************
Description: 从缓冲区内读出数据.
Parameters : buffer: 保存读出的数据.
             len   : 指定读取数据的长度.
Returns    : -1: 没有初始化.
              0: 缓冲区中没有数据.
             >0: 实际读取的长度.
****************************************************************/
int read_ring_buff(unsigned char* buffer, unsigned int len)
{
    int len1 = 0, len2 = 0;

    if(RING_STRU.p_head == NULL)
    {
        return -1;
    }
    assert(buffer);  // 如果 buffer 为 NULL, 则结束程序.

    if(RING_STRU.len_used == 0)
    {
        return 0;
    }

    /* 如果要求读取数据的长度大于缓冲区中有效数据的长度, 则能够读取的长度
       最大只能为有效数据的长度. */
    if(len > RING_STRU.len_used)
    {
        len = RING_STRU.len_used;
    }

    /* 如果已使用区域的首地址加上待读取数据的长度之和, 超出缓冲区尾地址,
       则需要分成两段进行读取. */
    if(RING_STRU.p_head_used + len > RING_STRU.p_tail)
    {
        len1 = RING_STRU.p_tail - RING_STRU.p_head_used; // 从 RING_STRU.p_tail 开始到末尾的所有数据的长度.
        /* 读取完从 RING_STRU.p_tail 开始到末尾的所有数据之后, 还剩多少数据需要从缓冲区首地址开始读取. */
        len2 = len - len1;
        memcpy(buffer, RING_STRU.p_head_used, len1); // 读取从 RING_STRU.p_tail 开始到末尾的所有数据.
        memcpy(buffer+len1, RING_STRU.p_head, len2); // 从缓冲区首地址开始, 读取剩余长度的数据.
        RING_STRU.p_head_used = RING_STRU.p_head + len2;  // 更新已使用缓冲区的首地址.
    }
    else
    {
        memcpy(buffer, RING_STRU.p_head_used, len);
        RING_STRU.p_head_used = RING_STRU.p_head_used + len;  // 更新已使用缓冲区的首地址.
    }

    RING_STRU.len_used -= len;            // 更新已使用缓冲区的长度.
    return len;
}

/****************************************************************
Description: 获取缓冲区中已使用区域的长度.
Returns    : RING_STRU.len_used: 缓冲区中已使用区域的长度.
****************************************************************/
unsigned int get_ring_buff_used_len(void)
{
    return RING_STRU.len_used;
}

/****************************************************************
Description: 释放环形缓冲区.
****************************************************************/
void release_ring_buff(void)
{
    if(RING_STRU.p_head != NULL)
    {
        free(RING_STRU.p_head);
    }
    RING_STRU.p_head = NULL;
}

