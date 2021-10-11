#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

struct ring_buff_s
{
    unsigned int len_used;       // 缓冲区中已使用区域的长度.
    unsigned char* p_head;       // 环形缓冲区的首地址.
    unsigned char* p_tail;       // 环形缓冲区的尾地址.
    unsigned char* p_head_used;  // 缓冲区中, 已使用区域的首地址.
    unsigned char* p_tail_used;  // 缓冲区中, 已使用区域的尾地址.
};

void init_ring_buff(void);
int wirte_ring_buff(unsigned char* buffer, unsigned int len);
int read_ring_buff(unsigned char* buffer, unsigned int len);
unsigned int get_ring_buff_used_len(void);
void release_ring_buff(void);

#endif /* RING_BUFFER_H_ end. */

