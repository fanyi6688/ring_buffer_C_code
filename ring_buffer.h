#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

struct ring_buff_s
{
    unsigned int len_used;       // ����������ʹ������ĳ���.
    unsigned char* p_head;       // ���λ��������׵�ַ.
    unsigned char* p_tail;       // ���λ�������β��ַ.
    unsigned char* p_head_used;  // ��������, ��ʹ��������׵�ַ.
    unsigned char* p_tail_used;  // ��������, ��ʹ�������β��ַ.
};

void init_ring_buff(void);
int wirte_ring_buff(unsigned char* buffer, unsigned int len);
int read_ring_buff(unsigned char* buffer, unsigned int len);
unsigned int get_ring_buff_used_len(void);
void release_ring_buff(void);

#endif /* RING_BUFFER_H_ end. */

