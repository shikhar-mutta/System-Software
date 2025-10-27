Commands and Function Learning

-----------------------------------------P_1----------------------------------------- 
a. ITIMER_REAL
    --------------------     C Programming :      --------------------
    handler(int signum) :
        -Defines the signal handler function.
        -signum is the signal number.
⚠️ Note: printf() is not async-signal-safe. It works in small demos, but in real production code you should not do I/O from inside signal handlers. Instead, set a volatile sig_atomic_t flag.        

    struct itimerval timer;
        -struct itimerval defines the timer values.
    
    struct itimerval {
        struct timeval it_interval; // period between expirations
        struct timeval it_value;    // initial countdown
    };

    signal(SIGALRM, handler);
        -Registers the handler() function to handle SIGALRM.
    
    setitimer(ITIMER_REAL, &timer, NULL) 
    Calls setitimer():
        -ITIMER_REAL: means the timer decrements in real (wall-clock) time.
        -When the countdown hits zero → kernel sends SIGALRM to this process.

        📝 Program Behavior
            -Start program → waits.
            -After ~10 seconds → "ITIMER_REAL: Timer expired!" is printed.
            -After another ~10 seconds → prints again.
            -Continues indefinitely until you stop it (Ctrl+C).
    ITIMER_REAL: Counts real clock time, even while sleeping.

 ----------------------------------------------------------------------------------

 b. ITIMER_VIRTUAL
    --------------------     C Programming :      --------------------
    
    setitimer(ITIMER_VIRTUAL, &timer, NULL) 
    Calls setitimer():
            -Arms the virtual interval timer.
            -Starts counting down when the process executes on the CPU in user mode.

        📝 Program Behavior
            -Program starts → installs the virtual timer.
            -Spends CPU time looping.
            -After ~10 seconds of actual CPU usage → kernel sends SIGVTALRM.
            -Handler runs → prints:

    ITIMER_VIRTUAL: Counts only user CPU time (time the process is actively running on the CPU).
----------------------------------------------------------------------------------

 c. ITIMER_PR
    --------------------     C Programming :      --------------------
    
    setitimer(ITIMER_PROF, &timer, NULL) 
    Calls setitimer():
            -Arms the virtual interval timer.
            -Starts counting down when the process executes on the CPU in user mode.

        📝 Program Behavior
            -Program runs → installs a profiling timer.
            -Executes a busy loop.
            -After ~10 seconds of total CPU time (user + kernel) → kernel sends SIGPROF.

    ITIMER_PROF: counts user time + system time. So it advances during system calls as well.

ITIMER_REAL - measures wall-clock time. Even if the process sleeps, the timer still runs.
ITIMER_PROF - ignores wall-clock. If the process is sleeping and not using CPU, the timer does not advance.
ITIMER_VIRTUAL - counts only user-mode CPU time. If your process makes a system call, that kernel execution time does not count.

-----------------------------------------P_2----------------------------------------- 

    --------------------     C Programming :      --------------------

    struct rlimit limit; 
        -to hold the result.

    getrlimit(resource, &limit)
        -to fetch the current soft and hard limits for the resource.

    limit.rlim_cur == RLIM_INFINITY
        -rlim_cur = soft limit (current usable limit).
    limit.rlim_max == RLIM_INFINITY
        -rlim_max = hard limit (ceiling; only root can raise it).
    
    #ifdef RLIMIT_NICE
        print_limit(RLIMIT_NICE, "Nice value");
    #endif
    #ifdef blocks ensure the code only calls print_limit() if the constant is defined:
        -RLIMIT_MSGQUEUE: size of POSIX message queues.
        -RLIMIT_NICE: ceiling on the nice value.
        -RLIMIT_RTPRIO: ceiling on real-time priority.

-----------------------------------------P_3----------------------------------------- 

    --------------------     C Programming :      --------------------

    struct rlimit limit; 
        -to hold the result.

    getrlimit(resource, &limit)
        -to fetch the current soft and hard limits for the resource.

    limit.rlim_cur == RLIM_INFINITY
        -rlim_cur = soft limit (current usable limit).
    limit.rlim_max == RLIM_INFINITY
        -rlim_max = hard limit (ceiling; only root can raise it).
    
    limit.rlim_cur = 2048;       
    limit.rlim_max = limit.rlim_max;
    setrlimit(RLIMIT_NOFILE, &limit)
        -modify resource limit

-----------------------------------------P_4----------------------------------------- 

    --------------------     C Programming :      --------------------
    __rdtsc() 
        -read time stamp counter
    
-----------------------------------------P_5----------------------------------------- 

    --------------------     C Programming :      --------------------
    sysconf(name_const) 
        -It is a POSIX system call used in C/C++ to query system configuration values at runtime.
    

    #ifdef _SC_AVPHYS_PAGES
        av_pages = sysconf(_SC_AVPHYS_PAGES);
    #endif
    Explanation : 
        #ifdef _SC_AVPHYS_PAGES
        -This is a preprocessor conditional.
        -It literally means: “If the symbol _SC_AVPHYS_PAGES is defined in the system headers, then include the next block of code when compiling.”
        -Why? Because not all Unix-like systems provide _SC_AVPHYS_PAGES.
            -On Linux: it exists, gives number of currently available physical memory pages.
            -On some BSD or older POSIX systems: it may not exist at all.
        -Without #ifdef, trying to compile on a system where _SC_AVPHYS_PAGES is missing would cause a compilation error.

-----------------------------------------P_6----------------------------------------- 

    --------------------     C Programming :      --------------------
    void *worker(void *arg) 
        -This defines the thread function (worker) which will be executed by each thread.
        -int id = *(int *)arg;
            -The arg pointer actually points to an integer (ids[i]).
            -Cast it to int * and dereference → get the thread’s ID.
        -pthread_self() → returns the pthread ID of the current thread.
    
    pthread_create(&threads[i], NULL, worker, &ids[i])
        -&threads[i] → where the system stores the created thread’s handle.
        -NULL → default thread attributes.
        -worker → function each thread should run.
        -&ids[i] → argument passed to the thread function (pointer to an integer ID).
        -If pthread_create returns non-zero → error occurred, print message and exit.

    pthread_join(threads[i], NULL) → blocks until thread threads[i] terminates.
        -The second argument could capture the return value of the thread, but here we pass NULL since we don’t need it.
        -If pthread_join fails, print error and exit.

    🔑 Summary of Flow
            Main thread creates 3 worker threads.
            Each worker thread prints a start message, sleeps for 1 second, and then prints a finish message.
            Main thread waits (pthread_join) until all workers finish.
            Once all workers are done, main prints a message and exits.
    🔑 Key Behavior Notes
            Concurrency: All 3 threads run in parallel with the main thread.
            Interleaving: Print statements can appear in different orders across runs.
            Synchronization: pthread_join() ensures the main thread waits until all child threads are done before exiting.
            Determinism: The exact order of "started" and "finished" messages is not deterministic.

-----------------------------------------P_7----------------------------------------- 

    --------------------     C Programming :      --------------------
    
    -pthread_self() → returns the pthread ID of the current thread.

-----------------------------------------P_8----------------------------------------- 

    --------------------     C Programming :      --------------------

    handler() — the signal handler
        -strsignal(signum) -> returns a human-readable name for the signal 
        -For fatal faults (SIGSEGV, SIGFPE) the handler calls _exit(128+signum) to terminate the program immediately. 
            _exit() is chosen because it is async-signal-safe

Fault-triggering helpers
    trigger_sigsegv() 
        -Dereferences a NULL pointer to produce SIGSEGV (segmentation fault).
        -volatile helps prevent some compilers from optimizing the crash away — still compile with -O0 to be safe.

    trigger_sigfpe()
        -Intentionally divides by zero to raise SIGFPE (floating-point/arith exception).
        -Again, use -O0 (no optimization) because aggressive optimizers may detect undefined behavior and remove it.

    test_alarm_syscall()
        -alarm(3) requests a SIGALRM after ~3 seconds of real (wall-clock) time.
        -pause() sleeps until any signal is caught (and handler returns), then pause() returns.
        -After handler runs, execution resumes and prints the return message.

    test_setitimer_vitual()
        -ITIMER_VIRTUAL counts user CPU time consumed by the process. When the process has used ~1 second of user CPU, the kernel delivers SIGVTALRM.
        -Because it counts CPU time, sleeping or waiting doesn’t move the timer; the program must actually consume CPU (hence the busy loop).
        -The infinite busy loop is used to ensure user CPU accumulates. The handler must interrupt this loop for the program to change behavior.
        
    test_setitimer_prof()
        -ITIMER_PROF counts user + system CPU time used by the process. When the combined CPU time reaches the interval, the kernel delivers SIGPROF.
        -The loop calls getpid() frequently to consume some kernel time (system time) in addition to user CPU.

    main() — installation of handlers & menu
        signal(SIGSEGV, handler);
        signal(SIGINT, handler);
        signal(SIGFPE, handler);
        signal(SIGALRM, handler);
        signal(SIGVTALRM, handler);
        signal(SIGPROF, handler);
    The menu reads user input and calls appropriate test functions:
        a — trigger SIGSEGV
        b — sleep for 10s and allow user to press Ctrl-C to send SIGINT
        c — trigger SIGFPE
        d — alarm(3) SIGALRM
        e — setitimer ITIMER_REAL → SIGALRM
        f — ITIMER_VIRTUAL → SIGVTALRM (busy loop)
        g — ITIMER_PROF → SIGPROF (busy loop + syscall)
        q — quit

-----------------------------------------P_9----------------------------------------- 

    --------------------     C Programming :      --------------------
    signal(SIGINT, SIG_IGN);
        -This tells the kernel: if the program receives SIGINT (the interrupt signal sent by pressing Ctrl-C in the terminal), ignore it.
        -Normally, pressing Ctrl-C would terminate the program, but now it’s disabled.
    SIG_IGN = IGN ignore

    signal(SIGINT, SIG_DFL);
        -Restores the default action for SIGINT (which is program termination).
    SIG_DFL = DFL default


-----------------------------------------P_10----------------------------------------- 

    --------------------     C Programming :      --------------------

    Purpose of the program
        -This is a signal-handling demo written in C. It installs handlers for:
        -SIGSEGV (segmentation fault),
        -SIGINT (Ctrl-C),
        -SIGFPE (arithmetic error like divide-by-zero),
        -using the modern sigaction() API.
        -Then it lets you trigger those signals and see how the handlers respond.

    Helper: print_siginfo(int sig, siginfo_t *info)
        -Runs inside the signal handler.
        -siginfo_t provides extra information:
            -si_signo: the signal number,
            -si_errno: error number (rarely set),
            -si_code: cause (e.g., divide-by-zero, invalid memory).
        -For SIGSEGV, prints the faulting memory address (si_addr).
        -For SIGFPE, prints the exact trap code (e.g., integer divide by zero).

    Handler: handler(int signum, siginfo_t *info, void *ucontext)
        -strsignal() to print a human-readable signal name.

    Installing handlers:
        memset
            -A function from <string.h> used to set a block of memory to a particular value (byte by byte).
            -Signature:
                -void *memset(void *ptr, int value, size_t num);
            -ptr → starting address of memory to fill
            -value → byte value to set (converted to unsigned char)
            -num → number of bytes to fill
        sa.sa_flags = SA_SIGINFO;
            By default, sigaction expects you to use sa_handler (a simpler function pointer taking only int signo).
            By setting SA_SIGINFO, you tell the kernel to use sa_sigaction instead.
        Installing handlers with sigaction() : 
            sigaction(signal, &sa, NULL)
                -signal → which signal to catch (SIGSEGV, SIGFPE, SIGINT).
                -&sa → new action to set (your handler).
                -NULL → don’t save the old action (we don’t care about restoring it later).

Here’s a **brief summary (P_11 to P_15)** in the same structured format you provided earlier:


-----------------------------------------P_11-----------------------------------------

     --------------------     Functions & Concepts :      --------------------

    -Ignore a `SIGINT` signal (Ctrl+C) temporarily, then reset its default behavior using `sigaction()`.

    -Key Functions and Concepts:

        -> `struct sigaction sa;` — used to define signal actions.
        -> `sigaction(SIGINT, &sa, NULL);` — installs signal behavior.
        -> `sa.sa_handler = SIG_IGN;` → Ignore `SIGINT`.
        -> `sa.sa_handler = SIG_DFL;` → Restore default (terminate on `Ctrl+C`).
        -> `memset()` — initializes structure with zeros.

    📝 Program Behavior:    
        -> 1. First, ignores `SIGINT` (Ctrl+C has no effect for 5 seconds).
        -> 2. Then resets `SIGINT` to default.
        -> 3. Pressing `Ctrl+C` again now terminates the program.

    Concepts Illustrated: 
    Signal ignoring and restoring default using `sigaction`.


-----------------------------------------P_12-----------------------------------------

     --------------------     Functions & Concepts :      --------------------

    Purpose:  
    Create an   orphan process   — child kills its parent using `kill()` system call.

    Key Functions and Concepts:   

        - `fork()` → create child and parent processes.
        - `kill(getppid(), SIGKILL)` → child sends `SIGKILL` to terminate parent.
        - `getpid()` / `getppid()` → print process and parent IDs.

    📝 Program Behavior:  

        1. Parent and child processes start.
        2. Child sends `SIGKILL` to parent → parent dies.
        3. Child’s parent becomes `init/systemd` (PID 1).
        4. Child continues running, showing it’s now an orphan.

    Concepts Illustrated: 
        -> Process hierarchy and re-parenting.
        -> Using `kill()` to send signals between processes.


-----------------------------------------P_13-----------------------------------------

     --------------------     Functions & Concepts :      --------------------

    Purpose:  
    Two programs demonstrating signal handling for `SIGSTOP` and `SIGCONT`.
    `13c.c` → Catcher, `13s.c` → Sender.

    Catcher (13c.c):  
        -> Attempts `sigaction(SIGSTOP)` → fails (cannot catch or ignore `SIGSTOP`).
        -> Installs handler for `SIGCONT` → prints message when continued.
        -> Waits indefinitely with `pause()`.

    Sender (13s.c):   
        -> Takes target PID as argument.
        -> Sends `SIGSTOP` → stops the catcher.
        -> After delay, sends `SIGCONT` → resumes catcher, triggering its handler.

    📝 Program Behavior:  
        `SIGSTOP` cannot be caught or handled.
        `SIGCONT` can be caught; handler runs on resume.

    Concepts Illustrated:
    Uncatchable vs. catchable signals.
    Interprocess signaling using `kill()`.

-----------------------------------------P_14-----------------------------------------

     --------------------     Functions & Concepts :      --------------------

    Purpose:  
    Demonstrate use of a pipe for interprocess communication (IPC).

    Key Functions and Concepts:   

        - `pipe(fd)` → creates a unidirectional data channel.
        - `fork()` → creates parent/child.
        - Parent → `write()` to pipe.
        - Child → `read()` from pipe.

    📝 Program Behavior:  

    Parent writes `"Hello from parent via pipe!"` into pipe.
    Child reads and prints the message.

    Concepts Illustrated: 

        -> Basic IPC using pipes.
        -> Synchronization via process blocking on `read()`/`write()`.

-----------------------------------------P_15-----------------------------------------

     --------------------     Functions & Concepts :      --------------------

    Purpose:    
        -> Send data from **parent to child** process using a pipe.

    Key Functions and Concepts:   
        - Similar to P_14.
        - Parent closes read end and writes message.
        - Child closes write end and reads message.

    📝 Program Behavior:  
        - Parent sends: `"Hello Child, this is Parent!"`.
        - Child receives and displays the same message.

    Concepts Illustrated:   

    One-way communication (Parent → Child).
    Proper pipe end management and interprocess data transfer.


-----------------------------------------P_16-----------------------------------------

     --------------------     Functions & Concepts :      --------------------

    Purpose:
        - Two-way communication between parent and child using two pipes.

    Key Functions and Concepts:
        - `pipe(p2c)` → for parent → child communication.
        - `pipe(c2p)` → for child → parent communication.
        - `fork()` → creates child process.
        - `read_all()` / `write_all()` → helper functions to handle partial I/O safely.
        - `close()` → closes unused pipe ends in both processes.

    📝 Program Behavior:   
        1. Parent writes `"Hello Child, this is Parent!"` into pipe.
        2. Child reads it and prints.
        3. Child replies `"Hello Parent — message received loud and clear!"`.
        4. Parent reads and prints child’s reply.

    Concepts Illustrated: 
        - Bidirectional IPC using two pipes.
        - Proper handling of pipe ends and reliable I/O.

-----------------------------------------P_17-----------------------------------------

     --------------------     Functions & Concepts :      --------------------

    Purpose:
        Execute command pipeline:
        `ls -l | wc`
        Implemented using three methods: `dup()`, `dup2()`, and `fcntl()`.

    Key Functions and Concepts:
        - `pipe()` → creates communication channel.
        - `fork()` → creates two child processes.
        -   Method 1:    `dup()` → duplicates file descriptors manually.
        -   Method 2:    `dup2()` → directly duplicates to specific FD.
        -   Method 3:    `fcntl(F_DUPFD)` → duplicates using file control.
        - `execlp()` → executes external commands (`ls`, `wc`).

    📝 Program Behavior:
        - For each method, output of `ls -l` is piped into `wc`.
        - The output line count, word count, and byte count are displayed.
        - All three methods give identical results.

    Concepts Illustrated: 
        - File descriptor redirection and duplication.
        - Process pipeline implementation (like shell behavior).

-----------------------------------------P_18-----------------------------------------

     --------------------     Functions & Concepts :      --------------------

    Purpose:
        Count total number of executable files in the current directory using:
        `ls -l | grep ^-rwx | wc`
        (uses **two pipes** and **dup2()**)

    Key Functions and Concepts:

    Two pipes:
        -> `p1` for `ls → grep`
        -> `p2` for `grep → wc`
        Three processes created via `fork()`.
        `dup2()` redirects standard I/O between them.
        `execlp()` executes `ls`, `grep`, and `wc`.

    📝 Program Behavior:
        First process runs `ls -l`.
        Second process filters output with `grep ^-rwx`.
        Third process counts lines/words/bytes using `wc`.

    Concepts Illustrated:
        Multi-stage pipeline (three commands).
        Command chaining using `dup2` and multiple pipes.

-----------------------------------------P_19-----------------------------------------

     --------------------     Functions & Concepts :      --------------------

    Purpose:
        Create a **FIFO (named pipe)** using various methods and compare.

    Key Functions and Concepts:
        a. `mknod` command
        b. `mkfifo` command
        c. `strace` used to compare syscalls
        d. `mknod()` system call
        e. `mkfifo()` library function
        `S_IFIFO` → type flag for FIFO.
        `0666` → read/write permissions.

    📝 Program Behavior:
        User chooses creation method.
        FIFO file is created in current directory.
        Verified using `ls -l`.
        Can test with `strace` to see internal syscall behavior.

    Concepts Illustrated:
        FIFO (named pipe) creation via multiple methods.
        `mkfifo` is the **preferred** and **portable** method.
        Difference between **system call (mknod)** and **library call (mkfifo)**.

-----------------------------------------P_20-----------------------------------------

     --------------------     Functions & Concepts :      --------------------

    Purpose:
        Demonstrate **one-way communication** between two independent programs using **FIFO**.
    Programs:
        `20w.c` → Writer
        `20r.c` → Reader

    Writer (20w.c):
        Creates FIFO (`mkfifo()` with `0666`).
        Opens FIFO in write-only mode.
        Takes user input and writes it into FIFO.
        Stops on EOF (Ctrl+D).

    Reader (20r.c):
        Opens the same FIFO in **read-only** mode.
        Reads and displays data received from writer.
        Exits when writer closes the FIFO.      

    📝 Program Behavior:
        1. Start reader → waits for data.
        2. Start writer → sends text messages.
        3. Reader displays them live.
        4. Writer exits → reader detects EOF and terminates.

    Concepts Illustrated:
        -One-way IPC via named FIFO.
        -Coordination between independent processes.
        -Practical use of `open`, `read`, `write` on FIFOs.




-----------------------------------------P_21-----------------------------------------

     --------------------     Functions & Concepts :      --------------------

    Purpose:
        Implement **two-way communication** between two independent programs using **FIFOs** (named pipes).
        Programs:

    `21P1.c` → **Server**
    `21P2.c` → **Client**

    Key Functions and Concepts:
    Two FIFOs created:
            `/tmp/fifo_server_to_client`
            `/tmp/fifo_client_to_server`
        `mkfifo()` → creates named pipes (with 0666 permissions).
        `poll()` → used for non-blocking I/O monitoring on both standard input and FIFO.
        `signal(SIGINT, handler)` → clean shutdown on Ctrl+C.
        Each side reads from one FIFO and writes to the other.

    📝 Program Behavior:
        Server and client both can send/receive messages simultaneously.
        Typing `"exit"` on either side ends communication gracefully.
        Uses non-blocking read/write and full-duplex FIFO communication.        

    Concepts Illustrated:
        Bidirectional inter-process communication using **two FIFOs**.
        `poll()`-based multiplexed I/O handling.
        Signal-safe shutdown design.

-----------------------------------------P_22-----------------------------------------

     --------------------     Functions & Concepts :      --------------------

    Purpose:
        Demonstrate **waiting for data on a FIFO** using the `select()` system call with a timeout (10 seconds).
    Programs:
        `22r.c` → **Reader (uses select)**
        `22w.c` → **Writer**

    Key Functions and Concepts:
        `mkfifo()` → creates FIFO `"myfifo"`.
        `select(fd+1, &readfds, NULL, NULL, &tv)` → waits for data with 10-second timeout.
        If data available → `read()` and display message.
        Writer simply opens FIFO in **write-only** mode and sends a message string.

    📝 Program Behavior:
        1. Reader waits for data for 10 seconds.
        2. If writer sends data within that time, reader prints it.
        3. If no data arrives → prints timeout message.

    Concepts Illustrated:
        Using `select()` for event-driven I/O.
        FIFO synchronization between two processes.
        Handling blocking vs. non-blocking reads.

-----------------------------------------P_23-----------------------------------------

     --------------------     Functions & Concepts :      --------------------

    Purpose:
        Print the **maximum number of open files** a process can have and the **default size of a pipe** (circular buffer).

    Key Functions and Concepts:
        `getrlimit(RLIMIT_NOFILE, &rl)` → retrieves soft and hard limits on open files.
        `pipe(pipefd)` → creates unnamed pipe.
        `fcntl(pipefd[0], F_GETPIPE_SZ)` → retrieves system’s default pipe size.

    📝 Program Behavior:
    Displays:
        * Soft and hard file descriptor limits.
        * Default pipe buffer size (in bytes).

    Concepts Illustrated:
        * Process resource limits (`rlimit`).
        * Pipe buffer internals in Linux (`F_GETPIPE_SZ`).
        * System call introspection.

-----------------------------------------P_24-----------------------------------------

     --------------------     Functions & Concepts :      --------------------

    Purpose:
        Create a **System V message queue** and print its **key** and **message queue ID**.

    Key Functions and Concepts:
        * `ftok(".", 'A')` → generates a unique key using project ID `'A'`.
        * `msgget(key, IPC_CREAT | 0666)` → creates or accesses message queue.
        * `printf()` outputs queue key and ID.

    📝 Program Behavior:
        * Successfully creates a message queue and displays:
  * Key value (integer).
  * Message queue ID assigned by kernel.

    Concepts Illustrated:
        System V IPC initialization (message queues).
        Using `ftok()` for key generation.
        Basics of queue creation permissions.

-----------------------------------------P_25-----------------------------------------

     --------------------     Functions & Concepts :      --------------------

    Purpose:
        Display **detailed metadata** about a message queue using `msqid_ds` and `ipc_perm` structures.

    Key Functions and Concepts:
    `msgctl(msqid, IPC_STAT, &buf)` → fetches queue info.
    Displays:
        * Access permissions
        * UID & GID
        * Time of last `msgsnd` / `msgrcv`
        * Queue size and message count
        * Max allowed bytes
        * PIDs of last sender and receiver

    📝 Program Behavior:
        Prints complete message queue attributes retrieved from the kernel.

    Concepts Illustrated:
        * Inspecting kernel-maintained message queue statistics.
        * Understanding System V IPC data structures (`msqid_ds`, `ipc_perm`).

-----------------------------------------P_26-----------------------------------------

     --------------------     Functions & Concepts :      --------------------

    Purpose:
        Send messages to a **System V message queue** and verify using `$ipcs -q`.

    Key Functions and Concepts:
        * `msgget()` → create/get existing queue.
        * `msgsnd()` → send messages with specific type (`mtype`).
        * User inputs text messages; “exit” ends sending.
        * `ipcs -q` used externally to view queue stats.

    📝 Program Behavior:
        * Prompts user for input messages.
        * Each message is sent into the queue.
        * Messages remain until received or queue removed.

    Concepts Illustrated:
        * Sending data to message queues.
        * Persistent IPC mechanism across processes.
        * Verifying IPC objects with Linux utilities.

-----------------------------------------P_27-----------------------------------------

     --------------------     Functions & Concepts :      --------------------

    Purpose:
        Send and receive messages from a message queue using two programs:
    PRogram:
        * `27s.c` → **Sender**
        * `27r.c` → **Receiver**

    Key Functions and Concepts:
        * `msgsnd()` → sender writes messages.
        * `msgrcv()` → receiver reads messages.
        * Demonstrates two modes:

    * Blocking (`flag = 0`)
    * Non-blocking (`flag = IPC_NOWAIT`)
    * `msgctl(IPC_RMID)` → removes message queue.

    📝 Program Behavior:

        * Sender sends messages to queue with key `1234`.
        * Receiver:

    1. In blocking mode → waits for messages.
    2. In non-blocking mode → checks queue without waiting.
    3. Option to remove queue.

    Concepts Illustrated:
        -Two-way IPC using message queues.
        -Blocking vs. non-blocking message retrieval.
        -Cleaning up IPC resources safely.


-----------------------------------------P_28-----------------------------------------

     --------------------     Functions & Concepts :      --------------------

    Purpose:
        Change the **permissions** of an existing message queue using the `msqid_ds` structure and `msgctl()` system call.

    Key Functions and Concepts:
        * `ftok("/tmp", 65)` → generate unique key.
        * `msgget(key, 0666 | IPC_CREAT)` → get or create message queue.
        * `msgctl(msqid, IPC_STAT, &buf)` → fetch current queue info.
        * Modify: `buf.msg_perm.mode = (buf.msg_perm.mode & ~0777) | 0644;`
        * `msgctl(msqid, IPC_SET, &buf)` → update new permissions.

    📝 Program Behavior:
        1. Displays message queue ID and current permissions.
        2. Changes permissions to `0644`.
        3. Verifies the change by reading the queue attributes again.

    Concepts Illustrated:
        * Using `msgctl()` for **metadata modification**.
        * Accessing and updating **IPC permission fields**.
        * Demonstrates safe IPC attribute manipulation.

-----------------------------------------P_29-----------------------------------------

     --------------------     Functions & Concepts :      --------------------

    Purpose:
        Remove an existing **System V message queue** interactively.

    Key Functions and Concepts:
        * Displays existing queues via `system("ipcs -q")`.
        * Reads user input (queue ID).
        * Confirms deletion (`Y/N`).
        * `msgctl(msqid, IPC_RMID, NULL)` → removes queue.

    📝 Program Behavior:
        * Lists current message queues.
        * Prompts user for an `msqid` to delete.
        * On confirmation → deletes the queue and confirms success.
        * Cancels safely if invalid input or user declines.

    Concepts Illustrated:
        * Safe and user-interactive **IPC cleanup**.
        * `msgctl()` with `IPC_RMID`.
        * Error checking and input validation.

-----------------------------------------P_30-----------------------------------------

     --------------------     Functions & Concepts :      --------------------

    Purpose:
        Demonstrate creation and lifecycle management of **shared memory**:
    Steps: 
        1. Create shared memory.
        2. Write data.
        3. Attach read-only and verify write protection.
        4. Detach and remove shared memory.

    Key Functions and Concepts:
        * `ftok("/tmp", 77)` → generate key.
        * `shmget(key, size, IPC_CREAT | 0666)` → create shared memory.
        * `shmat(shmid, NULL, 0)` → attach read-write.
        * `shmat(shmid, NULL, SHM_RDONLY)` → attach read-only.
        * `shmdt(ptr)` → detach.
        * `shmctl(shmid, IPC_RMID, NULL)` → remove.

    📝 Program Behavior:
        * Child attaches with `SHM_RDONLY` and attempts to overwrite (triggers SIGSEGV).
        * Parent writes data to shared memory.
        * Parent detaches and removes the segment safely.

    Concepts Illustrated:
        * Shared memory protection (`SHM_RDONLY`).
        * Safe attach/detach and cleanup.
        * Handling segmentation fault in memory protection tests.

-----------------------------------------P_31-----------------------------------------

     --------------------     Functions & Concepts :      --------------------

    Purpose:
        Create and initialize **binary** and **counting semaphores** using System V IPC.

    Key Functions and Concepts:
        * `ftok("/tmp", 11)` and `ftok("/tmp", 22)` → unique keys.
        * `semget(key, 1, IPC_CREAT | 0666)` → create semaphore set.
        * `semctl(semid, 0, SETVAL, arg)` → set initial value.

    * Binary semaphore = 1.
    * Counting semaphore = 3.

    📝 Program Behavior:
        * Displays created semaphore IDs and their initialized values.

    Concepts Illustrated:
        * Binary vs Counting semaphore difference.
        * Using `semctl()` with `union semun`.
        * IPC semaphore initialization.

-----------------------------------------P_32-----------------------------------------

     --------------------     Functions & Concepts :      --------------------

    Purpose:
        Implement semaphore-based **synchronization** for multiple use cases:

        (a) Ticket number generation.
        (b) Protect shared memory from concurrent writes.
        (c) Manage multiple pseudo-resources with counting semaphores.
        (d) Cleanup of semaphores and shared memory.

    Key Functions and Concepts:
        * `sem_P()` / `sem_V()` → custom wait/signal operations using `semop()`.
        * `shmget()` + `shmat()` → shared memory for shared counter.
        * `semget()` → create mutex (binary) and counting semaphores.
        * `semctl(IPC_RMID)` / `shmctl(IPC_RMID)` → cleanup.

    📝 Program Behavior:
        * Multiple child processes safely increment a shared ticket counter.
        * Shared memory write access protected by mutex.
        * Counting semaphore allows only limited concurrent access.
        * Cleans up all resources after execution.

    Concepts Illustrated:
        **Critical section protection** with semaphores.
        **Counting semaphore** for resource management.
        IPC synchronization across multiple processes.

-----------------------------------------P_33-----------------------------------------

     --------------------     Functions & Concepts :      --------------------

    Purpose:
        Implement a **two-way TCP chat** between **server** and **client** using sockets and `select()` for multiplexed I/O.

    `33s.c` → Server
    `33c.c` → Client

    Key Functions and Concepts:
        `socket()`, `bind()`, `listen()`, `accept()` for server setup.
        `connect()` for client connection.
        `select()` monitors both stdin and socket for activity.
        `send()` / `recv()` for communication.

    📝 Program Behavior:
        * Server waits for a client.
        * Client connects, both can chat interactively.
        * Either side types `"exit"` to terminate connection.
        * Concurrent input/output handled through `select()`.

    Concepts Illustrated:
        * Full-duplex TCP communication.
        * Non-blocking I/O via `select()`.
        * Client-server communication model.

-----------------------------------------P_34-----------------------------------------

     --------------------     Functions & Concepts :      --------------------

    Purpose:
        Implement a **concurrent TCP server** in two modes:

    * (a) Using `fork()`
    * (b) Using `pthread_create()`
    
    Files:
        `34s.c` → Server
        `34c.c` → Client

    Key Functions and Concepts:
        * `socket()`, `bind()`, `listen()`, `accept()` for server setup.
        * Fork-based concurrency → new process for each client.
        * Thread-based concurrency → new thread per client.
        * `recv()` and `send()` handle message exchange.
        * Echo server behavior — replies back same message.

    📝 Program Behavior:
        * User chooses between fork-based or thread-based server.
        * Each client connection handled independently (process/thread).
        * Client (`34c.c`) connects, sends messages, and receives echoed responses.
        * “exit” command closes connection gracefully.

    Concepts Illustrated:
        * Process-based vs thread-based concurrency.
        * Connection handling in parallel.
        * TCP echo communication model.




