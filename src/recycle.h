#define MAX_BUF		32768 - 128
#define MAX_BUF_LIST 	11
#define BASE_BUF 	1024

/* valid states */
#define BUFFER_SAFE	0
#define BUFFER_OVERFLOW	1
#define BUFFER_FREED 	2

BUFFER	*new_buf args( (void) );
BUFFER  *new_buf_size args( (int size) );
void	free_buf args( (BUFFER *buffer) );
bool	add_buf args( (BUFFER *buffer, char *string) );
void	clear_buf args( (BUFFER *buffer) );
char	*buf_string args( (BUFFER *buffer) );
