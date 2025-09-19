/*
============================================================================
Name : 2.c
Author : Shikhar Mutta
Description : Write a simple program to execute in an infinite 2.c at the background. Go to /proc directory and identify all the process related information in the corresponding proc directory.
Date: 20th Aug, 2025.
============================================================================
*/

#include <stdio.h>
#include <unistd.h>

int main()
{
    while (1)
    {
        printf("Running process with PID: %d\n", getpid());
        sleep(5);
    }
    return 0;
}

/*
============================================================================
Compilation :

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P2$ cat /proc/37534/cmdline
./2     # Command that started the process

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P2$ cat /proc/37534/status
Name:	2
Umask:	0002
State:	S (sleeping)
Tgid:	37534
Ngid:	0
Pid:	37534
PPid:	37506
TracerPid:	0
Uid:	1000	1000	1000	1000
Gid:	1000	1000	1000	1000
FDSize:	256
Groups:	4 24 27 30 46 100 114 1000
NStgid:	37534
NSpid:	37534
NSpgid:	37534
NSsid:	37506
Kthread:	0
VmPeak:	    2680 kB
VmSize:	    2680 kB
VmLck:	       0 kB
VmPin:	       0 kB
VmHWM:	    1624 kB
VmRSS:	    1624 kB
RssAnon:	       0 kB
RssFile:	    1624 kB
RssShmem:	       0 kB
VmData:	     224 kB
VmStk:	     132 kB
VmExe:	       4 kB
VmLib:	    1748 kB
VmPTE:	      44 kB
VmSwap:	       0 kB
HugetlbPages:	       0 kB
CoreDumping:	0
THP_enabled:	1
untag_mask:	0xffffffffffffffff
Threads:	1
SigQ:	0/30280
SigPnd:	0000000000000000
ShdPnd:	0000000000000000
SigBlk:	0000000000000000
SigIgn:	0000000000000000
SigCgt:	0000000000000000
CapInh:	0000000000000000
CapPrm:	0000000000000000
CapEff:	0000000000000000
CapBnd:	000001ffffffffff
CapAmb:	0000000000000000
NoNewPrivs:	0
Seccomp:	0
Seccomp_filters:	0
Speculation_Store_Bypass:	thread vulnerable
SpeculationIndirectBranch:	conditional enabled
Cpus_allowed:	ff
Cpus_allowed_list:	0-7
Mems_allowed:	00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000001
Mems_allowed_list:	0
voluntary_ctxt_switches:	52
nonvoluntary_ctxt_switches:	0
x86_Thread_features:
x86_Thread_features_locked:


# List open file descriptors:
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P2$ ls -l /proc/37534/fd
total 0
lrwx------ 1 shikhar shikhar 64 Aug 20 04:44 0 -> /dev/pts/0
lrwx------ 1 shikhar shikhar 64 Aug 20 04:44 1 -> /dev/pts/0
lrwx------ 1 shikhar shikhar 64 Aug 20 04:44 2 -> /dev/pts/0


# Memory mapping:
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P2$ cat /proc/37534/maps
6271286cf000-6271286d0000 r--p 00000000 103:05 2101989                   /home/shikhar/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P2/2
6271286d0000-6271286d1000 r-xp 00001000 103:05 2101989                   /home/shikhar/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P2/2
6271286d1000-6271286d2000 r--p 00002000 103:05 2101989                   /home/shikhar/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P2/2
6271286d2000-6271286d3000 r--p 00002000 103:05 2101989                   /home/shikhar/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P2/2
6271286d3000-6271286d4000 rw-p 00003000 103:05 2101989                   /home/shikhar/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P2/2
62712af42000-62712af63000 rw-p 00000000 00:00 0                          [heap]
77a238400000-77a238428000 r--p 00000000 103:05 2235143                   /usr/lib/x86_64-linux-gnu/libc.so.6
77a238428000-77a2385b0000 r-xp 00028000 103:05 2235143                   /usr/lib/x86_64-linux-gnu/libc.so.6
77a2385b0000-77a2385ff000 r--p 001b0000 103:05 2235143                   /usr/lib/x86_64-linux-gnu/libc.so.6
77a2385ff000-77a238603000 r--p 001fe000 103:05 2235143                   /usr/lib/x86_64-linux-gnu/libc.so.6
77a238603000-77a238605000 rw-p 00202000 103:05 2235143                   /usr/lib/x86_64-linux-gnu/libc.so.6
77a238605000-77a238612000 rw-p 00000000 00:00 0
77a23872d000-77a238730000 rw-p 00000000 00:00 0
77a238744000-77a238746000 rw-p 00000000 00:00 0
77a238746000-77a238748000 r--p 00000000 00:00 0                          [vvar]
77a238748000-77a23874a000 r--p 00000000 00:00 0                          [vvar_vclock]
77a23874a000-77a23874c000 r-xp 00000000 00:00 0                          [vdso]
77a23874c000-77a23874d000 r--p 00000000 103:05 2235109                   /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
77a23874d000-77a238778000 r-xp 00001000 103:05 2235109                   /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
77a238778000-77a238782000 r--p 0002c000 103:05 2235109                   /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
77a238782000-77a238784000 r--p 00036000 103:05 2235109                   /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
77a238784000-77a238786000 rw-p 00038000 103:05 2235109                   /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7fffa346a000-7fffa348b000 rw-p 00000000 00:00 0                          [stack]
ffffffffff600000-ffffffffff601000 --xp 00000000 00:00 0                  [vsyscall]

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P2$ readlink /proc/37534/exe
/home/shikhar/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P2/2

============================================================================
*/