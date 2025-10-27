
# Commands and Function Learning — README

This repository documents short C programs (P_1 → P_34) that demonstrate Unix/Linux system programming concepts: signals, timers, resource limits, pipes, FIFOs, message queues, shared memory, semaphores, sockets, and concurrency primitives.  
Each `P_x` entry follows a consistent structure: **Purpose**, **Key functions / concepts**, **Program behavior**, and **Concepts illustrated**. Example compile/run notes are included where helpful.

---

## Table of contents
1. [P_1 — Timers (ITIMER_REAL, ITIMER_VIRTUAL, ITIMER_PROF)](#p_1)
2. [P_2 — getrlimit (query resource limits)](#p_2)
3. [P_3 — setrlimit (modify limits)](#p_3)
4. [P_4 — `__rdtsc()` (timestamp counter)](#p_4)
5. [P_5 — `sysconf()` and feature-checking macros](#p_5)
6. [P_6 — pthread creation & join (worker threads)](#p_6)
7. [P_7 — `pthread_self()` (thread id)](#p_7)
8. [P_8 — Signal handler + fault-trigger helpers & setitimer tests](#p_8)
9. [P_9 — `signal()` to ignore / restore SIGINT](#p_9)
10. [P_10 — Advanced sigaction() demo with `siginfo_t` & SA_SIGINFO](#p_10)
11. [P_11 — Ignore and restore SIGINT using sigaction](#p_11)
12. [P_12 — Make an orphan process (child kills parent)](#p_12)
13. [P_13 — SIGSTOP / SIGCONT: catcher & sender programs](#p_13)
14. [P_14 — Simple anonymous pipe (parent → child)](#p_14)
15. [P_15 — Parent → child message via pipe (one-way)](#p_15)
16. [P_16 — Two-way communication with two pipes (parent ↔ child)](#p_16)
17. [P_17 — `ls -l | wc` via `dup()`, `dup2()`, and `fcntl(F_DUPFD)` implementations](#p_17)
18. [P_18 — Multi-stage pipeline `ls -l | grep ^-rwx | wc` using `dup2()`](#p_18)
19. [P_19 — Create FIFO (mknod / mkfifo) and compare approaches (`strace`)](#p_19)
20. [P_20 — FIFO one-way comm: writer (`20w.c`) and reader (`20r.c`)](#p_20)
21. [P_21 — Two-way FIFO chat (server/client) with `poll()`](#p_21)
22. [P_22 — Wait on FIFO with `select()` and timeout (10s)](#p_22)
23. [P_23 — `getrlimit(RLIMIT_NOFILE)` and pipe (circular buffer) size via `F_GETPIPE_SZ`](#p_23)
24. [P_24 — Create System V message queue and print key/id](#p_24)
25. [P_25 — Inspect message queue metadata (`msqid_ds` / `ipc_perm`)](#p_25)
26. [P_26 — Send messages to System V message queue (`msgsnd`)](#p_26)
27. [P_27 — Receive messages from queue (blocking & `IPC_NOWAIT`)](#p_27)
28. [P_28 — Change message queue permissions (`msgctl IPC_SET`)](#p_28)
29. [P_29 — Remove message queue interactively (`msgctl IPC_RMID`)](#p_29)
30. [P_30 — Shared memory lifecycle: create, write, read-only attach, detach, remove](#p_30)
31. [P_31 — Create and initialize binary & counting semaphores (System V)](#p_31)
32. [P_32 — Semaphore examples: mutex for ticketing, protect shared memory, counting resources, cleanup](#p_32)
33. [P_33 — Simple two-way TCP chat (server/client) using `select()`](#p_33)
34. [P_34 — Concurrent TCP server: fork-based and pthread-based modes (with client)](#p_34)

---

### P_1
#### ITIMER_REAL, ITIMER_VIRTUAL, ITIMER_PROF
**Purpose:** Demonstrate interval timers with `setitimer()` and the signals they generate (`SIGALRM`, `SIGVTALRM`, `SIGPROF`).  
**Key funcs:** `setitimer()`, `signal()`/handler, `struct itimerval`.  
**Behavior:** `ITIMER_REAL` counts wall-clock → SIGALRM when expires; `ITIMER_VIRTUAL` counts user CPU time → SIGVTALRM; `ITIMER_PROF` counts user+system CPU time → SIGPROF.  
**Illustrates:** Differences between wall-clock vs CPU-accounted timers and using timers for periodic callbacks.

---

### P_2
#### Query resource limits with `getrlimit`
**Purpose:** Fetch process soft & hard resource limits.  
**Key funcs:** `getrlimit(resource, &limit)`, `struct rlimit`, `RLIM_INFINITY`, conditional `#ifdef` checks (e.g., `RLIMIT_NICE`).  
**Behavior:** Print `rlim_cur` (soft) and `rlim_max` (hard).  
**Illustrates:** How to check system resource limits and use `#ifdef` to remain portable.

---

### P_3
#### Modify resource limits with `setrlimit`
**Purpose:** Show changing resource limits (example: `RLIMIT_NOFILE`).  
**Key funcs:** `getrlimit()`, modify `limit.rlim_cur`, and `setrlimit()`.  
**Behavior:** Set soft limit to a new value (e.g., 2048).  
**Illustrates:** How only privileged users can raise hard limits, and how to change per-process soft limits.

---

### P_4
#### `__rdtsc()`
**Purpose:** Read CPU timestamp counter.  
**Key funcs:** `__rdtsc()` intrinsic.  
**Behavior:** Fetch high-resolution cycle counter for lightweight timing/benchmarking.  
**Illustrates:** Low-level timing via CPU TSC (platform-specific).

---

### P_5
#### `sysconf()` and feature-checking
**Purpose:** Query runtime configuration values like `_SC_AVPHYS_PAGES`.  
**Key funcs:** `sysconf(name_const)`, use `#ifdef` to compile conditionally.  
**Behavior:** Query available physical pages if supported.  
**Illustrates:** Portable checks for system features & run-time environment queries.

---

### P_6
#### pthread creation & join
**Purpose:** Create worker threads and synchronize them via `pthread_join()`.  
**Key funcs:** `pthread_create()`, `pthread_join()`, `pthread_self()`.  
**Behavior:** Main spawns threads; each prints, sleeps, and exits; main waits for them.  
**Illustrates:** Basic pthread lifecycle, passing arguments to threads, and non-deterministic interleaving.

---

### P_7
#### `pthread_self()`
**Purpose:** Retrieve the calling thread's ID.  
**Key funcs:** `pthread_self()`.  
**Illustrates:** Thread identity & optional logging.

---

### P_8
#### Signal handling demo + fault-trigger helpers
**Purpose:** Comprehensive signal demo with helpers to trigger `SIGSEGV`, `SIGFPE`, and timer tests using `alarm()` & `setitimer()`.  
**Key funcs:** `signal()`, `_exit()`, `strsignal()`, busy loops for `ITIMER_VIRTUAL`/`ITIMER_PROF`.  
**Behavior:** Install handlers, let user trigger signals via menu options.  
**Illustrates:** Async-signal-safety, creating and testing fatal signals, timers, and how to design a small interactive signal demo.

---

### P_9
#### Ignore / restore SIGINT
**Purpose:** Show `signal(SIGINT, SIG_IGN)` and `signal(SIGINT, SIG_DFL)` usage.  
**Illustrates:** Temporarily disabling Ctrl-C and restoring default terminal behavior.

---

### P_10
#### Advanced `sigaction()` sample
**Purpose:** Install handlers using `sigaction()` with `SA_SIGINFO` and print `siginfo_t` details.  
**Key funcs:** `sigaction()`, `memset()`, `sa.sa_flags = SA_SIGINFO`.  
**Behavior:** Handler prints human-readable signal name and faulting address for `SIGSEGV`.  
**Illustrates:** How to receive extra signal context and use modern `sigaction` API safely.

---

### P_11
#### Ignore and restore SIGINT using `sigaction`
**Purpose:** Temporary ignore `SIGINT` then restore default via `sigaction`.  
**Behavior:** Program ignores Ctrl-C for a short period, then restores normal termination on Ctrl-C.

---

### P_12
#### Orphan process (child kills parent)
**Purpose:** Show reparenting: child sends `SIGKILL` to parent; child remains running and becomes reparented to `init/systemd`.  
**Key funcs:** `fork()`, `kill()`, `getpid()`, `getppid()`.

---

### P_13
#### SIGSTOP cannot be caught; SIGCONT can
**Purpose:** Two-program demo: catcher cannot install handler for `SIGSTOP` (error), but handles `SIGCONT`; sender sends `SIGSTOP` then `SIGCONT`.  
**Illustrates:** Which signals are uncatchable and use of `kill()` for interprocess control.

---

### P_14
#### Anonymous pipe demo (parent → child)
**Purpose:** Parent writes a message to a pipe; child reads and displays it.  
**Key funcs:** `pipe()`, `fork()`, `write()`, `read()`.

---

### P_15
#### Parent → child one-way pipe
**Purpose:** Simple parent-to-child message transfer over a pipe; correct closing of unused ends illustrated.

---

### P_16
#### Two-way pipes (full-duplex with two pipes)
**Purpose:** Bidirectional parent-child communication using two pipes and robust `read_all`/`write_all` helpers.  
**Behavior:** Parent sends message; child replies; both print received messages.  
**Illustrates:** Proper FD management and reliable partial-I/O handling.

---

### P_17
#### `ls -l | wc` via `dup()`, `dup2()`, and `fcntl(F_DUPFD)`
**Purpose:** Implement the `ls -l | wc` pipeline using three methods of duplicating file descriptors.  
**Key funcs:** `pipe()`, `dup()`, `dup2()`, `fcntl(F_DUPFD)`, `execlp()`.  
**Behavior:** All methods produce identical `wc` output.  
**Illustrates:** Different APIs for FD redirection and how shells set up pipelines.

---

### P_18
#### Multi-stage pipeline `ls -l | grep ^-rwx | wc` (uses `dup2`)
**Purpose:** Chain three commands using two pipes and `dup2()`.  
**Behavior:** `ls -l` → `grep ^-rwx` → `wc`.  
**Illustrates:** Multi-stage pipelines and careful FD closing.

---

### P_19
#### FIFO creation: `mknod` vs `mkfifo` (and `strace` comparison)
**Purpose:** Create named pipes (FIFOs) via different methods and compare syscalls.  
**Key funcs:** `mknod()`, `mkfifo()`, `strace` for syscall tracing.  
**Behavior:** Demonstrates creation and verification via `ls -l`.  
**Illustrates:** `mkfifo` is more idiomatic/portable; `mknod` is lower-level and used for device nodes.

---

### P_20
#### FIFO one-way comm (writer `20w.c` / reader `20r.c`)
**Purpose:** One-way inter-process comm using a named FIFO.  
**Behavior:** Writer creates FIFO and writes lines; reader opens and prints them until writer closes FIFO.  
**Illustrates:** Named-pipe coordination between independent programs.

---

### P_21
#### Two-way FIFO chat (server/client) using `poll()`
**Purpose:** Full-duplex chat between server and client using two FIFOs and `poll()` for multiplexing.  
**Behavior:** Both sides can send/receive messages; typing `exit` closes the connection.  
**Illustrates:** Non-blocking I/O, `poll()`, and graceful shutdown via signal handling.

---

### P_22
#### Wait on FIFO with `select()` and timeout (10s)
**Purpose:** Use `select()` to wait for data on a FIFO with a 10-second timeout.  
**Files:** `22r.c` (reader), `22w.c` (writer).  
**Behavior:** Reader times out if no writer sends data in time.  
**Illustrates:** `select()`-based event waiting and FIFO synchronization.

---

### P_23
#### File descriptor limits & pipe buffer size
**Purpose:** Print `RLIMIT_NOFILE` soft/hard limits and a pipe's default buffer size using `F_GETPIPE_SZ`.  
**Key funcs:** `getrlimit()`, `pipe()`, `fcntl()`.  
**Illustrates:** Per-process limits and pipe internals.

---

### P_24
#### Create System V message queue and print key/id
**Purpose:** Use `ftok()` + `msgget()` to create/access a System V message queue and display its key and ID.  
**Illustrates:** Basic System V message queue creation.

---

### P_25
#### Inspect message queue metadata (`msqid_ds`)
**Purpose:** Query `msgctl(..., IPC_STAT, ...)` to print permissions, uid/gid, last send/receive times, size, message count, etc.  
**Illustrates:** Reading kernel-maintained IPC metadata.

---

### P_26
#### Send messages to System V message queue (`msgsnd`)
**Purpose:** Interactive sender that writes typed lines into a message queue; use `ipcs -q` to verify.  
**Illustrates:** Message queuing and persistence across processes.

---

### P_27
#### Receive from message queue (blocking and `IPC_NOWAIT`)
**Purpose:** Interactive receiver demonstrating blocking (`flags=0`) and non-blocking (`IPC_NOWAIT`) `msgrcv`.  
**Files:** `27s.c` (sender), `27r.c` (receiver).  
**Illustrates:** Blocking vs non-blocking IPC and safe queue removal.

---

### P_28
#### Change message queue permissions (`msgctl IPC_SET`)
**Purpose:** Modify `msg_perm.mode` via `msgctl(..., IPC_SET, ...)` to change queue permissions (example: set to `0644`).  
**Illustrates:** IPC attribute modification via `msgctl`.

---

### P_29
#### Remove message queue interactively
**Purpose:** List existing queues (`ipcs -q`) and remove selected `msqid` after user confirmation using `msgctl(IPC_RMID)`.  
**Illustrates:** Safe IPC cleanup.

---

### P_30
#### Shared memory lifecycle & read-only attach test
**Purpose:** Create shared memory, write data, have child attach read-only and attempt to write (causes SIGSEGV), detach and remove the segment.  
**Key funcs:** `shmget()`, `shmat()`, `shmdt()`, `shmctl(IPC_RMID)`.  
**Illustrates:** Memory protection with `SHM_RDONLY` and lifecycle management.

---

### P_31
#### Create binary and counting semaphores (System V)
**Purpose:** Create semaphores and initialize values (binary = 1, counting = 3).  
**Key funcs:** `semget()`, `semctl(..., SETVAL)`.  
**Illustrates:** Semaphore basics and initial values.

---

### P_32
#### Semaphore-based synchronization examples
**Purpose:** Demonstrate:
- (a) Ticket generation protected by mutex semaphore.
- (b) Protect shared memory writes with mutex.
- (c) Counting semaphore for limited resources.
- (d) Cleanup of semaphores & shared memory.
**Key funcs:** `semop()` wrappers `sem_P()`/`sem_V()`, `shmget()`, `shmat()`, `semctl(IPC_RMID)`.  
**Illustrates:** Critical section protection and resource counting with System V semaphores.

---

### P_33
#### Two-way TCP chat using sockets & `select()`
**Purpose:** Server (`33s.c`) and client (`33c.c`) that exchange messages using TCP; `select()` handles stdin and socket.  
**Illustrates:** Full-duplex TCP, `select()` multiplexing, and basic client-server model.

---

### P_34
#### Concurrent TCP server: fork and pthread modes
**Purpose:** Server (`34s.c`) supports two concurrency modes: fork-per-client and thread-per-client; client (`34c.c`) connects and receives echoes.  
**Illustrates:** Process-based vs thread-based concurrency, per-connection handling, and echo server behavior.

---

## Building & Running
- Each program is self-contained as `Px.c` (or `Px_server/client` variations).  
- Typical compile command:
```bash
gcc Px.c -o Px
```
- Some programs require multiple files or explicit flags (e.g., `-pthread` for threaded servers). Check the header comments inside each source file for precise compilation instructions and examples.

## Notes & Best Practices
- **Signal handlers:** avoid non-async-signal-safe functions (like `printf`) inside handlers — use `volatile sig_atomic_t` flags and handle I/O in the main flow.
- **Resource cleanup:** always remove IPC objects (`msgctl(..., IPC_RMID)`, `shmctl(..., IPC_RMID)`, `semctl(..., IPC_RMID)`) when done to avoid resource leaks.
- **Permission & privilege:** raising hard limits or creating certain IPC objects may require root privileges.
- **Portability:** use `#ifdef` checks for system-specific constants (e.g., `_SC_AVPHYS_PAGES`, `F_GETPIPE_SZ`) to keep code portable.
- **Concurrency:** be careful with shared memory and threads — use proper synchronization primitives (semaphores/mutexes) to prevent race conditions.

---

## License
This collection is provided for educational purposes. Feel free to reuse, adapt, and experiment with the code for learning and coursework.

---

## Contact / Author
Original examples and notes by *Shikhar Mutta* (author names are included in the source headers).  
If you want the README adjusted (shorter, more detailed per-program compile/run steps, or formatted differently), tell me which style and I will update the file.

