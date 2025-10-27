/*
============================================================================
Name : 5.c
Author : Shikhar Mutta
Description : Write a program to print the system limitation of
                a. maximum length of the arguments to the exec family of functions.
                b. maximum number of simultaneous process per user id.
                c. number of clock ticks (jiffy) per second.
                d. maximum number of open files
                e. size of a page
                f. total number of pages in the physical memory
                g. number of currently available pages in the physical memory.
Date: 17th Sep 2025.
============================================================================
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/resource.h>
#include <stdint.h>
#include <inttypes.h>

static void print_sysconf_long(const char *name, int name_const)
{
    errno = 0;
    long val = sysconf(name_const);
    if (val == -1)
    {
        if (errno == 0)
        {
            printf("%-50s : (indeterminate / not available)\n", name);
        }
        else
        {
            printf("%-50s : (error) %s\n", name, strerror(errno));
        }
    }
    else
    {
        printf("%-50s : %ld\n", name, val);
    }
}

int main(void)
{
    printf("\nSystem limits & info\n");
    printf("=====================\n\n");

    print_sysconf_long("a) _SC_ARG_MAX (max length of arguments for exec)", _SC_ARG_MAX);

#ifdef RLIMIT_NPROC
    {
        struct rlimit rl;
        if (getrlimit(RLIMIT_NPROC, &rl) == 0)
        {
            if (rl.rlim_cur == RLIM_INFINITY)
                printf("%-50s : RLIMIT_NPROC (soft) = unlimited\n", "b) RLIMIT_NPROC (soft) (max processes per user)");
            else
                printf("%-50s : RLIMIT_NPROC (soft) = %" PRIuMAX "\n", "b) RLIMIT_NPROC (soft) (max processes per user)", (uintmax_t)rl.rlim_cur);

            if (rl.rlim_max == RLIM_INFINITY)
                printf("%-50s : RLIMIT_NPROC (hard) = unlimited\n", "   RLIMIT_NPROC (hard)");
            else
                printf("%-50s : RLIMIT_NPROC (hard) = %" PRIuMAX "\n", "   RLIMIT_NPROC (hard)", (uintmax_t)rl.rlim_max);
        }
        else
        {
            printf("%-50s : getrlimit(RLIMIT_NPROC) error: %s\n", "b) RLIMIT_NPROC", strerror(errno));
        }
    }
#else
    printf("%-50s : (RLIMIT_NPROC not available on this platform)\n", "b) RLIMIT_NPROC");
#endif
#ifdef _SC_CHILD_MAX
    print_sysconf_long("   _SC_CHILD_MAX (sysconf)", _SC_CHILD_MAX);
#else
    printf("%-50s : ( _SC_CHILD_MAX not defined )\n", "   _SC_CHILD_MAX (sysconf)");
#endif

#ifdef _SC_CLK_TCK
    print_sysconf_long("c) _SC_CLK_TCK (clock ticks per second)", _SC_CLK_TCK);
#else
    printf("%-50s : ( _SC_CLK_TCK not defined )\n", "c) _SC_CLK_TCK (clock ticks per second)");
#endif

#ifdef RLIMIT_NOFILE
    {
        struct rlimit rl;
        if (getrlimit(RLIMIT_NOFILE, &rl) == 0)
        {
            if (rl.rlim_cur == RLIM_INFINITY)
                printf("%-50s : RLIMIT_NOFILE (soft) = unlimited\n", "d) RLIMIT_NOFILE (soft) (max open files)");
            else
                printf("%-50s : RLIMIT_NOFILE (soft) = %" PRIuMAX "\n", "d) RLIMIT_NOFILE (soft) (max open files)", (uintmax_t)rl.rlim_cur);

            if (rl.rlim_max == RLIM_INFINITY)
                printf("%-50s : RLIMIT_NOFILE (hard) = unlimited\n", "   RLIMIT_NOFILE (hard)");
            else
                printf("%-50s : RLIMIT_NOFILE (hard) = %" PRIuMAX "\n", "   RLIMIT_NOFILE (hard)", (uintmax_t)rl.rlim_max);
        }
        else
        {
            printf("%-50s : getrlimit(RLIMIT_NOFILE) error: %s\n", "d) RLIMIT_NOFILE", strerror(errno));
        }
    }
#else
    printf("%-50s : (RLIMIT_NOFILE not available on this platform)\n", "d) RLIMIT_NOFILE");
#endif
#ifdef _SC_OPEN_MAX
    print_sysconf_long("   _SC_OPEN_MAX (sysconf) (per-process limit)", _SC_OPEN_MAX);
#else
    printf("%-50s : ( _SC_OPEN_MAX not defined )\n", "   _SC_OPEN_MAX (sysconf)");
#endif

#if defined(_SC_PAGESIZE)
    print_sysconf_long("e) _SC_PAGESIZE (page size in bytes)", _SC_PAGESIZE);
#elif defined(_SC_PAGE_SIZE)
    print_sysconf_long("e) _SC_PAGE_SIZE (page size in bytes)", _SC_PAGE_SIZE);
#else
    printf("%-50s : (page size sysconf not available)\n", "e) page size");
#endif

#ifdef _SC_PHYS_PAGES
    {
        errno = 0;
        long phys_pages = sysconf(_SC_PHYS_PAGES);
        if (phys_pages == -1)
        {
            if (errno == 0)
                printf("%-50s : (indeterminate)\n", "f) _SC_PHYS_PAGES (total physical pages)");
            else
                printf("%-50s : (error) %s\n", "f) _SC_PHYS_PAGES", strerror(errno));
        }
        else
        {
            printf("%-50s : %ld pages\n", "f) _SC_PHYS_PAGES (total physical pages)", phys_pages);
        }
    }
#else
    printf("%-50s : ( _SC_PHYS_PAGES not defined )\n", "f) total physical pages");
#endif

#ifdef _SC_AVPHYS_PAGES
    {
        errno = 0;
        long av_pages = sysconf(_SC_AVPHYS_PAGES);
        if (av_pages == -1)
        {
            if (errno == 0)
                printf("%-50s : (indeterminate)\n", "g) _SC_AVPHYS_PAGES (available physical pages)");
            else
                printf("%-50s : (error) %s\n", "g) _SC_AVPHYS_PAGES", strerror(errno));
        }
        else
        {
            printf("%-50s : %ld pages\n", "g) _SC_AVPHYS_PAGES (available physical pages)", av_pages);
        }
    }
#else
    printf("%-50s : ( _SC_AVPHYS_PAGES not defined )\n", "g) available physical pages");
#endif

    long page_size = -1;
#if defined(_SC_PAGESIZE)
    page_size = sysconf(_SC_PAGESIZE);
#elif defined(_SC_PAGE_SIZE)
    page_size = sysconf(_SC_PAGE_SIZE);
#endif

    long phys_pages = -1, av_pages = -1;
#ifdef _SC_PHYS_PAGES
    phys_pages = sysconf(_SC_PHYS_PAGES);
#endif

#ifdef _SC_AVPHYS_PAGES
    av_pages = sysconf(_SC_AVPHYS_PAGES);
#endif

    if (page_size > 0)
    {
        printf("\nDerived memory sizes using page size = %ld bytes:\n", page_size);
        if (phys_pages > 0)
        {
            unsigned long long total_bytes = (unsigned long long)phys_pages * (unsigned long long)page_size;
            printf("%-50s : %llu bytes (%.2f MB)\n", "   Total physical memory", (unsigned long long)total_bytes, total_bytes / (1024.0 * 1024.0));
        }
        if (av_pages > 0)
        {
            unsigned long long avail_bytes = (unsigned long long)av_pages * (unsigned long long)page_size;
            printf("%-50s : %llu bytes (%.2f MB)\n", "   Available physical memory", (unsigned long long)avail_bytes, avail_bytes / (1024.0 * 1024.0));
        }
    }
    else
    {
        printf("\n(Unable to compute derived memory sizes: page size unknown)\n");
    }

    printf("\n");
    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P5$ gcc 5.c -o 5

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P5$ ./5

System limits & info
=====================

a) _SC_ARG_MAX (max length of arguments for exec)  : 2097152
b) RLIMIT_NPROC (soft) (max processes per user)    : RLIMIT_NPROC (soft) = 30280
   RLIMIT_NPROC (hard)                             : RLIMIT_NPROC (hard) = 30280
   _SC_CHILD_MAX (sysconf)                         : 30280
c) _SC_CLK_TCK (clock ticks per second)            : 100
d) RLIMIT_NOFILE (soft) (max open files)           : RLIMIT_NOFILE (soft) = 1024
   RLIMIT_NOFILE (hard)                            : RLIMIT_NOFILE (hard) = 1048576
   _SC_OPEN_MAX (sysconf) (per-process limit)      : 1024
e) _SC_PAGESIZE (page size in bytes)               : 4096
f) _SC_PHYS_PAGES (total physical pages)           : 1976374 pages
g) _SC_AVPHYS_PAGES (available physical pages)     : 190638 pages

Derived memory sizes using page size = 4096 bytes:
   Total physical memory                           : 8095227904 bytes (7720.21 MB)
   Available physical memory                       : 780853248 bytes (744.68 MB)

============================================================================
*/
