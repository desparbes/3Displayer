#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <libgen.h>

#define MIN(a, b) ((a)<(b)?(a):(b))
/* magic bug solver extension */

static int dir = -1;

__attribute__ ((constructor))
static void init(void)
{
    char pBuf[1024];
    
    int bytes = MIN(readlink("/proc/self/exe", pBuf, 1024), 1024 - 1);
    printf("LOL %s\n", pBuf);
    if (bytes >= 0) pBuf[bytes] = '\0';

    dir = open(dirname(pBuf), O_DIRECTORY);
}

static int char_in(char c, const char *str)
{
    for (int i = 0; str[i]; ++i)
	if (str[i] == c)
	    return 1;
    return 0;
}
    
FILE *fopen(const char *path, const char *mode)
{
    int flags = 0;
    if ( char_in('w', mode) )
	flags &= O_CREAT;
    if ( char_in('a', mode) )
	flags &= O_APPEND;
    int fd = openat(dir, path, flags);
    return fdopen(fd, mode);
}
