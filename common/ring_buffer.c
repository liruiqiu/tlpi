/**
 * Title 	: 
 * Author	: 
 * Date		: 
 * Description 	: implement an ring buffer just like kfifo. Please note that
 * the data in the buffer will keep there until them be read out. If the buffer
 * is full, the data could not be put into any more, until the read action free
 * some space. 
 *
 * NOTE: This version used for user space.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "ring_buffer.h"
#include "printk.h"

struct ring_buffer *ring_buffer_create(unsigned int size)
{
	struct ring_buffer *ring_buf;

	ring_buf = malloc(sizeof(*ring_buf));
	if (!ring_buf) {
		perror("malloc()");
		return NULL;
	}

	ring_buf->data = malloc(size+1);
	if (!ring_buf->data) {
		perror("malloc()");
		free(ring_buf);
		return NULL;
	}

	ring_buf->size = size+1;
	ring_buf->front = 0;
	ring_buf->rear = 0;

	return ring_buf;
}

void ring_buffer_destroy(struct ring_buffer *ring_buf)
{
	if (ring_buf) {
		if (ring_buf->data) {
			free(ring_buf->data);
			ring_buf->data = NULL;
		}
		free(ring_buf);
		ring_buf = NULL;
	}
}

/**
 * ring_buffer_put - puts some data into the ring_buffer, no locking version
 * @ring_buf: the ring_buffer to be used.
 * @buffer: the data to be added.
 * @len: the length of the data to be added.
 *
 * This function copies at most @len bytes from the @buffer into
 * the ring_buffer depending on the free space, and returns the number of
 * bytes copied.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these functions.
 */

int ring_buffer_put(struct ring_buffer *ring_buf,   
		const char *buffer, int len)   
{   
	unsigned int l;   

	//len = min(len, ring_buf->size - ring_buf->rear + ring_buf->front);   /* first put the data starting from rear to buffer end */   
    /* l = min(len, ring_buf->size - (ring_buf->rear & (ring_buf->size - 1)));   memcpy(ring_buf->data + (ring_buf->rear & (ring_buf->size - 1)), buffer, l); */  

    len = min(len, ring_buffer_space_len(ring_buf));

	/* first put the data starting from rear to buffer end */   
	l = min(len, ring_buf->size - (ring_buf->rear %ring_buf->size));   
	memcpy(ring_buf->data + (ring_buf->rear % ring_buf->size), buffer, l);   

	/* then put the rest (if any) at the beginning of the buffer */   
	if(len > l) memcpy(ring_buf->data, buffer + l, len - l);

	ring_buf->rear += len;   
    //pr_info("len=%d,l=%d,r_pos=%d,w_pos=%d", len, l, ring_buf->front,ring_buf->rear);

	return len;   
}  

/**
 *  ring_buffer_get - gets some data from the ring_buffer, no locking version
 *  @ring_buf: the ring_buffer to be used.
 *  @buffer: where the data must be copied.
 *  @len: the size of the destination buffer.
 * 
 *  This function copies at most @len bytes from the ring_buffer into the
 *  @buffer and returns the number of copied bytes.
 * 
 *  Note that with only one concurrent reader and one concurrent
 *  writer, you don't need extra locking to use these functions.
 */
int ring_buffer_get(struct ring_buffer *ring_buf,
		char *buffer, int len)
{
	unsigned int l;
    
    len = min(len, ring_buffer_len(ring_buf));

	/* first get the data from ring_buf->front until the end of the buffer */
	l = min(len, ring_buf->size - (ring_buf->front % ring_buf->size));
	memcpy(buffer, ring_buf->data + (ring_buf->front % ring_buf->size), l);

	/* then get the rest (if any) from the beginning of the buffer */
    if (len > l) memcpy(buffer + l, ring_buf->data, len - l);

	ring_buf->front += len;

    //pr_info("len=%d,l=%d,r_pos=%d,w_pos=%d", len, l, ring_buf->front,ring_buf->rear);
	return len;
}

int ring_buffer_send_socket(int fd, struct ring_buffer *ring_buf, unsigned int len)
{
	unsigned int l;
	unsigned int send_num;
	unsigned int send_total_num;
	int i;
	send_num = 0;
	send_total_num = 0;
	for (i=0; i<2; i++)
	{
		len =  ring_buffer_len(ring_buf);

		l = min(len, ring_buf->size - (ring_buf->front % ring_buf->size));

		send_num = send(fd, ring_buf->data + (ring_buf->front % ring_buf->size), l, 0); 
		pr_info("send_num=%d,len=%d,l=%d,front=%d,rear=%d",
			send_num, len, l, ring_buf->front, ring_buf->rear);

		if (send_num == 0) 
			return send_num;
		else if (send_num == -1)
		{
			if(errno == EWOULDBLOCK || errno == EAGAIN) send_num = -EWOULDBLOCK;
			return send_num;
		}
		else
			ring_buf->front += send_num;
		
		send_total_num += send_num;

		if(len <= l) break;
	}
/*
	send_num += send(fd, ring_buf->data, len - l -send_num, 0); 
	pr_info("send_len=%d,len=%d,l=%d,",send_num,len,l);

	if (send_num == 0) 
		return send_num;
	else if (send_num == -1)
	{
		if(errno == EWOULDBLOCK || errno == EAGAIN) send_num = -EWOULDBLOCK;
		return send_num;
	}
	else
		ring_buf->front += send_num;
*/

	return send_total_num;
}

int ring_buffer_recv_socket(int fd, struct ring_buffer *ring_buf, unsigned int len)
{
	unsigned int l;
	unsigned int recv_num;
	unsigned int recv_total_num;
	int i;
	recv_num = 0;
	recv_total_num = 0;
	for (i=0; i<2; i++)
	{
		len = ring_buffer_space_len(ring_buf);

		l = min(len, ring_buf->size - (ring_buf->rear % ring_buf->size));

		recv_num = recv(fd, ring_buf->data + (ring_buf->rear % ring_buf->size), l, 0); 
		pr_info("recv_num=%d,len=%d,l=%d,front=%d,rear=%d",
			recv_num, len, l, ring_buf->front, ring_buf->rear);
		if (recv_num == 0) 
			return recv_num;
		else if (recv_num == -1)
		{
			if(errno == EWOULDBLOCK || errno == EAGAIN) 
				recv_num = -EWOULDBLOCK;
			return recv_num;
		}
		else
			ring_buf->rear += recv_num;

		recv_total_num += recv_num;

		if(len <= l) break;
	}

	return recv_total_num;
}
/**
 *  ring_buffer_print - print ringbuf data
 *  @ring_buf: the ring_buffer to be used.
 *  @cnt : the number byte to be print.
 */
void ring_buffer_print(struct ring_buffer *ring_buf, unsigned int cnt)
{
	return;
}

/**
 *  ring_buffer_from_dev - read data from device, and put them into ring_buffer
 *  @fd: file descriptor of device
 *  @ring_buf: the ring_buffer to be used.
 *  @len: the length of the data to be read.
 */
ssize_t ring_buffer_from_dev(int fd, struct ring_buffer *ring_buf, unsigned int len)
{
	unsigned int l;   
	ssize_t ret, total;

	total = 0;

	len = min(len, ring_buf->size - ring_buf->rear + ring_buf->front); 

	/* first put the data starting from rear to buffer end */   
	l = min(len, ring_buf->size - (ring_buf->rear & (ring_buf->size - 1)));   

	ret = read(fd, ring_buf->data + (ring_buf->rear & (ring_buf->size - 1)), l);
	if (ret < 0) {
		return ret;
	}

	total = ret;

	if (total < l) {
		ring_buf->rear += total;   

		return total;
	}

	if (len -l) {
		ret = read(fd, ring_buf->data, len - l);
		if (ret < 0) {
			return ret;
		}

		total += ret;
	}

	ring_buf->rear += total;   

	return total;   
}




/* FIXME: just for connection, as use "send()", for details, man 2 send */
/*
   unsigned int ring_buffer_to_socket(int socket_fd, struct ring_buffer *ring_buf, unsigned int len)
   {
   unsigned int l;
   unsigned int send_num;

   len = min(len, ring_buf->rear - ring_buf->front);

   l = min(len, ring_buf->size - (ring_buf->front & (ring_buf->size - 1)));

   send_num = send(socket_fd, ring_buf->data + (ring_buf->front & (ring_buf->size - 1)), l, 0); 
   if (send_num < 0){ 
   perror("send()");
   exit(1);
   }

   if (len - l) {
   send_num = send(socket_fd, ring_buf->data, len - l, 0); 
   if (send_num < 0){ 
   perror("send()");
   exit(1);
   }
   }
   ring_buf->front += len;

   return len;
   }
   */



/**
 * ring_buffer_reset - removes the entire ring_buffer content
 * @ring_buf: address of the ring_buffer to be used
 *
 * Note: usage of ring_buffer_reset() is dangerous. It should be only called when the
 * ring_buffer is exclusived locked or when it is secured that no other thread is
 * accessing the fifo.
 */
void ring_buffer_reset(struct ring_buffer *ring_buf)
{
	ring_buf->rear = ring_buf->front = 0;
}

unsigned int ring_buffer_is_empty(struct ring_buffer *ring_buf)
{
	return 0 == ring_buffer_len(ring_buf);
}

unsigned int ring_buffer_is_full(struct ring_buffer *ring_buf)
{
	return ring_buf->size <= ring_buffer_len(ring_buf);
}

unsigned int ring_buffer_len(struct ring_buffer *ring_buf)
{
	return (ring_buf->rear -  ring_buf->front + ring_buf->size) % ring_buf->size;
}

unsigned int ring_buffer_space_len(struct ring_buffer *ring_buf)
{
	return ring_buf->size - ring_buffer_len(ring_buf);
}


