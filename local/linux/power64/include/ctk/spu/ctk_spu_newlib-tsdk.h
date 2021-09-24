/*
 * CTK - The Cell Tool Kit Library
 * http://ctk-dev.sourceforge.net/
 * http://cell.fixstars.com/ctk/
 *
 * Copyright (C) 2006-2008 Fixstars Corp.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License,
 * or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef _CTK_NEWLIB_WRAPPER_H
#define _CTK_NEWLIB_WRAPPER_H

#ifdef __STDC__
# define __EXFUN(name, proto)             ctk_## name proto
#else
# define __EXFUN(name, proto)             ctk_## name()
#endif

#ifdef __cplusplus
extern "C" {
#endif

FILE *	__EXFUN(tmpfile, (void));
char *	__EXFUN(tmpnam, (char *));
int	__EXFUN(fclose, (FILE *));
int	__EXFUN(fflush, (FILE *));
FILE *	__EXFUN(freopen, (const char *, const char *, FILE *));
void	__EXFUN(setbuf, (FILE *, char *));
int	__EXFUN(setvbuf, (FILE *, char *, int, size_t));
int	__EXFUN(fprintf, (FILE *, const char *, ...));
int	__EXFUN(fscanf, (FILE *, const char *, ...));
int	__EXFUN(printf, (const char *, ...));
int	__EXFUN(scanf, (const char *, ...));
int	__EXFUN(sscanf, (const char *, const char *, ...));
int	__EXFUN(vfprintf, (FILE *, const char *, __VALIST));
int	__EXFUN(vprintf, (const char *, __VALIST));
int	__EXFUN(vsprintf, (char *, const char *, __VALIST));
int	__EXFUN(fgetc, (FILE *));
char *  __EXFUN(fgets, (char *, int, FILE *));
int	__EXFUN(fputc, (int, FILE *));
int	__EXFUN(fputs, (const char *, FILE *));
int	__EXFUN(getc, (FILE *));
int	__EXFUN(getchar, (void));
char *  __EXFUN(gets, (char *));
int	__EXFUN(putc, (int, FILE *));
int	__EXFUN(putchar, (int));
int	__EXFUN(puts, (const char *));
int	__EXFUN(ungetc, (int, FILE *));
size_t	__EXFUN(fread, (_PTR, size_t _size, size_t _n, FILE *));
size_t	__EXFUN(fwrite, (const _PTR , size_t _size, size_t _n, FILE *));
int	__EXFUN(fgetpos, (FILE *, _fpos_t *));
int	__EXFUN(fseek, (FILE *, long, int));
int	__EXFUN(fsetpos, (FILE *, const _fpos_t *));
long	__EXFUN(ftell, ( FILE *));
void	__EXFUN(rewind, (FILE *));
void	__EXFUN(clearerr, (FILE *));
int	__EXFUN(feof, (FILE *));
int	__EXFUN(ferror, (FILE *));
void    __EXFUN(perror, (const char *));
#ifndef _REENT_ONLY
FILE *	__EXFUN(fopen, (const char *_name, const char *_type));
int	__EXFUN(sprintf, (char *, const char *, ...));
int	__EXFUN(remove, (const char *));
int	__EXFUN(rename, (const char *, const char *));
#endif
#ifndef __STRICT_ANSI__
int	__EXFUN(asprintf, (char **, const char *, ...));
#ifdef _COMPILING_NEWLIB
int	__EXFUN(fseeko, (FILE *, _off_t, int));
_off_t	__EXFUN(ftello, ( FILE *));
#else
int	__EXFUN(fseeko, (FILE *, off_t, int));
off_t	__EXFUN(ftello, ( FILE *));
#endif
int	__EXFUN(vfiprintf, (FILE *, const char *, __VALIST));
int	__EXFUN(iprintf, (const char *, ...));
int	__EXFUN(fiprintf, (FILE *, const char *, ...));
int	__EXFUN(siprintf, (char *, const char *, ...));
char *	__EXFUN(tempnam, (const char *, const char *));
int	__EXFUN(vasprintf, (char **, const char *, __VALIST));
int	__EXFUN(vsnprintf, (char *, size_t, const char *, __VALIST));
int	__EXFUN(vfscanf, (FILE *, const char *, __VALIST));
int	__EXFUN(vscanf, (const char *, __VALIST));
int	__EXFUN(vsscanf, (const char *, const char *, __VALIST));
#ifndef _REENT_ONLY
int	__EXFUN(fcloseall, (_VOID));
int	__EXFUN(snprintf, (char *, size_t, const char *, ...));
#endif

_PTR 	 __EXFUN(memchr,(const _PTR, int, size_t));
int 	 __EXFUN(memcmp,(const _PTR, const _PTR, size_t));
_PTR 	 __EXFUN(memcpy,(_PTR, const _PTR, size_t));
_PTR	 __EXFUN(memmove,(_PTR, const _PTR, size_t));
_PTR	 __EXFUN(memset,(_PTR, int, size_t));
char 	*__EXFUN(strcat,(char *, const char *));
char 	*__EXFUN(strchr,(const char *, int));
int	 __EXFUN(strcmp,(const char *, const char *));
int	 __EXFUN(strcoll,(const char *, const char *));
char 	*__EXFUN(strcpy,(char *, const char *));
size_t	 __EXFUN(strcspn,(const char *, const char *));
char 	*__EXFUN(strerror,(int));
size_t	 __EXFUN(strlen,(const char *));
char 	*__EXFUN(strncat,(char *, const char *, size_t));
int	 __EXFUN(strncmp,(const char *, const char *, size_t));
char 	*__EXFUN(strncpy,(char *, const char *, size_t));
char 	*__EXFUN(strpbrk,(const char *, const char *));
char 	*__EXFUN(strrchr,(const char *, int));
size_t	 __EXFUN(strspn,(const char *, const char *));
char 	*__EXFUN(strstr,(const char *, const char *));

#ifndef _REENT_ONLY
char 	*__EXFUN(strtok,(char *, const char *));
#endif /* _REENT_ONLY */
#endif /* __STRICT_ANSI__ */

size_t	 __EXFUN(strxfrm,(char *, const char *, size_t));

#ifndef __STRICT_ANSI__
char 	*__EXFUN(strtok_r,(char *, const char *, char **));

int	 __EXFUN(bcmp,(const void *, const void *, size_t));
void	 __EXFUN(bcopy,(const void *, void *, size_t));
void	 __EXFUN(bzero,(void *, size_t));
int	 __EXFUN(ffs,(int));
char 	*__EXFUN(index,(const char *, int));
_PTR	 __EXFUN(memccpy,(_PTR, const _PTR, int, size_t));
_PTR	 __EXFUN(mempcpy,(_PTR, const _PTR, size_t));
char 	*__EXFUN(rindex,(const char *, int));
int	 __EXFUN(strcasecmp,(const char *, const char *));
char 	*__EXFUN(strdup,(const char *));
char 	*__EXFUN(_strdup_r,(struct _reent *, const char *));
char 	*__EXFUN(strndup,(const char *, size_t));
char 	*__EXFUN(_strndup_r,(struct _reent *, const char *, size_t));
char 	*__EXFUN(strerror_r,(int, char *, size_t));
size_t	 __EXFUN(strlcat,(char *, const char *, size_t));
size_t	 __EXFUN(strlcpy,(char *, const char *, size_t));
int	 __EXFUN(strncasecmp,(const char *, const char *, size_t));
size_t	 __EXFUN(strnlen,(const char *, size_t));
char 	*__EXFUN(strsep,(char **, const char *));
char	*__EXFUN(strlwr,(char *));
char	*__EXFUN(strupr,(char *));
#endif /* __STRICT_ANSI__ */

#ifdef __cplusplus
};
#endif

#undef ___EXFUN

#endif /* _CTK_NEWLIB_WRAPPER_H */

#if 0
#undef adjtimex	
#undef clearerr	
#undef close	
#undef creat	
#undef fclose	
#undef feof	
#undef ferror	
#undef fflush	
#undef fgetc	
#undef fgetpos	
#undef fgets	
#undef fopen	
#undef fprintf	
#undef fputc	
#undef fputs	
#undef fread	
#undef freopen	
#undef fscanf	
#undef fseek	
#undef fsetpos	
#undef fstat	
#undef ftell	
#undef ftok	
#undef fwrite	
#undef getc	
#undef getchar	
#undef getpagesize	
#undef gets	
#undef gettimeofday	
#undef kill	
#undef lseek	
#undef lstat	
#undef mmap_ea	
#undef mremap_ea	
#undef msync_ea	
#undef munmap_ea	
#undef open	
#undef perror	
#undef printf	
#undef putc	
#undef putchar	
#undef puts	
#undef read	
#undef remove	
#undef rename	
#undef rewind	
#undef scanf	
#undef setbuf	
#undef setvbuf	
#undef shm_open	
#undef shm_unlink	
#undef shmat_ea	
#undef shmctl_ea	
#undef shmdt_ea	
#undef shmget	
#undef snprintf	
#undef sprintf	
#undef spu_syscall	
#undef spu_syscall_va	
#undef sscanf	
#undef stat	
#undef system	
#undef tmpfile	
#undef tmpnam	
#undef ungetc	
#undef unlink	
#undef vfprintf	
#undef vfscanf	
#undef vprintf	
#undef vscanf	
#undef vsnprintf	
#undef vsprintf	
#undef vsscanf	
#undef wait	
#undef waitpid	
#undef write	

#define adjtimex	ctk_adjtimex
#define clearerr	ctk_clearerr
#define close	ctk_close
#define creat	ctk_creat
#define fclose	ctk_fclose
#define feof	ctk_feof
#define ferror	ctk_ferror
#define fflush	ctk_fflush
#define fgetc	ctk_fgetc
#define fgetpos	ctk_fgetpos
#define fgets	ctk_fgets
#define fopen	ctk_fopen
#define fprintf	ctk_fprintf
#define fputc	ctk_fputc
#define fputs	ctk_fputs
#define fread	ctk_fread
#define freopen	ctk_freopen
#define fscanf	ctk_fscanf
#define fseek	ctk_fseek
#define fsetpos	ctk_fsetpos
#define fstat	ctk_fstat
#define ftell	ctk_ftell
#define ftok	ctk_ftok
#define fwrite	ctk_fwrite
#define getc	ctk_getc
#define getchar	ctk_getchar
#define getpagesize	ctk_getpagesize
#define gets	ctk_gets
#define gettimeofday	ctk_gettimeofday
#define kill	ctk_kill
#define lseek	ctk_lseek
#define lstat	ctk_lstat
#define mmap_ea	ctk_mmap_ea
#define mremap_ea	ctk_mremap_ea
#define msync_ea	ctk_msync_ea
#define munmap_ea	ctk_munmap_ea
#define open	ctk_open
#define perror	ctk_perror
#define printf	ctk_printf
#define putc	ctk_putc
#define putchar	ctk_putchar
#define puts	ctk_puts
#define read	ctk_read
#define remove	ctk_remove
#define rename	ctk_rename
#define rewind	ctk_rewind
#define scanf	ctk_scanf
#define setbuf	ctk_setbuf
#define setvbuf	ctk_setvbuf
#define shm_open	ctk_shm_open
#define shm_unlink	ctk_shm_unlink
#define shmat_ea	ctk_shmat_ea
#define shmctl_ea	ctk_shmctl_ea
#define shmdt_ea	ctk_shmdt_ea
#define shmget	ctk_shmget
#define snprintf	ctk_snprintf
#define sprintf	ctk_sprintf
#define spu_syscall	ctk_spu_syscall
#define spu_syscall_va	ctk_spu_syscall_va
#define sscanf	ctk_sscanf
#define stat	ctk_stat
#define system	ctk_system
#define tmpfile	ctk_tmpfile
#define tmpnam	ctk_tmpnam
#define ungetc	ctk_ungetc
#define unlink	ctk_unlink
#define vfprintf	ctk_vfprintf
#define vfscanf	ctk_vfscanf
#define vprintf	ctk_vprintf
#define vscanf	ctk_vscanf
#define vsnprintf	ctk_vsnprintf
#define vsprintf	ctk_vsprintf
#define vsscanf	ctk_vsscanf
#define wait	ctk_wait
#define waitpid	ctk_waitpid
#define write	ctk_write
#endif
