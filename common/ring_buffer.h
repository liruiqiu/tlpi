#ifndef __ring_buffer_H
#define	__ring_buffer_H
/*
#define min(x,y) ({ 			\
		typeof(x) _x = (x);	\
		typeof(y) _y = (y);	\
		(void) (&_x == &_y);	\
		_x < _y ? _x : _y; })
*/

struct ring_buffer {
	void *data;
	unsigned int size;
	//unsigned int front;
	//unsigned int rear;
	unsigned char front;
	unsigned char rear;
} __packed;
typedef struct ring_buffer ring_buffer_t;

struct ring_buffer *ring_buffer_create(unsigned int bytes);
void ring_buffer_destroy(struct ring_buffer *ring_buf);

void ring_buffer_reset(struct ring_buffer *ring_buf);

int ring_buffer_put(struct ring_buffer *ring_buf, 
 	const char *buf, int len);

int ring_buffer_get(struct ring_buffer *ring_buf,
 		char *buf, int len);

void ring_buffer_print(struct ring_buffer *ring_buf, unsigned int cnt);

ssize_t ring_buffer_from_dev(int fd, struct ring_buffer *ring_buf, unsigned int len);

unsigned int ring_buffer_is_empty(struct ring_buffer *ring_buf);
unsigned int ring_buffer_is_full(struct ring_buffer *ring_buf);

unsigned int ring_buffer_len(struct ring_buffer *ring_buf);
unsigned int ring_buffer_space_len(struct ring_buffer *ring_buf);

//unsigned int ring_buffer_to_socket(int socket_fd, struct ring_buffer *ring_buf, unsigned int len);
int ring_buffer_send_socket(int fd, struct ring_buffer *ring_buf, unsigned int len);
int ring_buffer_recv_socket(int fd, struct ring_buffer *ring_buf, unsigned int len);

#endif
