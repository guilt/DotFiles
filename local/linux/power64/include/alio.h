/* Generic IO header. */
#ifndef ALIO_H
#define ALIO_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdarg.h>

#ifndef NULL
#define NULL (void *)(0)
#endif /* NULL */

struct PACKFILE;
typedef struct PACKFILE PACKFILE;

#define FILE PACKFILE
#define fpos_t int
#define off_t int

#ifndef EOF
#define EOF -1
#endif /* EOF */

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

/* To be called before all initialization */
void init_alio();

PACKFILE* multi_pack_fopen(const char *filename, const char *mode);

#define fopen multi_pack_fopen
#define fdopen(...) NULL
#define freopen(name,mode,stream) (fclose(stream),stream=fopen(name,mode))
#define fflush(...) 0
#define fpurge(...) 0
#define fclose pack_fclose

#define fseek(stream,pos,whence) (type==SEEK_CUR?pack_fseek(stream,*pos):-1)
#define ftell(...) 0
#define fseeko fseek
#define ftello ftell
#define fgetpos(stream,pos) (*pos=ftell(stream))
#define fsetpos(stream,pos) fseek(stream,*pos,SEEK_CUR)

#define feof pack_feof
#define ferror pack_ferror

#define fgetc pack_getc
#define getc fgetc
#define getc_unlocked getc
#define getchar stdio_getchar
#define getchar_unlocked getchar
#define ungetc(c,stream) 0
#define getw(stream) (int)pack_igetl(stream)
#define gets stdio_gets
#define fgets pack_fgets

#define fputc pack_putc
#define putc fputc
#define putc_unlocked putc
#define putchar(c) stdio_putchar(c)
#define putchar_unlocked putchar
#define putw(w,stream) (int)pack_iputl((long)w,stream)
#define puts stdio_puts
#define fputs pack_fputs

#define fread(s,size,items,stream) (unsigned int)pack_fread(s,size*items,stream)
#define fwrite(s,size,items,stream) (unsigned int)pack_fwrite(s,size*items,stream)

#define sscanf    stdio_sscanf
#define vsscanf   stdio_vsscanf
#define scanf     stdio_scanf
#define vscanf    stdio_vscanf

int pack_vfscanf(PACKFILE *stream, const char *format, va_list ap);
int pack_fscanf(PACKFILE *stream, const char *format, ...);

#define vfscanf   pack_vfscanf
#define fscanf    pack_fscanf

#define vsprintf  stdio_vsprintf
#define sprintf   stdio_sprintf
#define vprintf   stdio_vprintf
#define printf    stdio_printf

int pack_vfprintf(FILE *stream, const char *format, va_list ap);
int pack_fprintf(PACKFILE *f, const char *format, ...);

#define vfprintf  pack_vfprintf
#define fprintf   pack_fprintf

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ALIO_H */