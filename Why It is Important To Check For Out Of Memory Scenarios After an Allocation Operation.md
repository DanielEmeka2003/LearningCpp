# Why It is Important To Check For Out Of Memory Scenarios After an Allocation Operation

This is a great question — and it cuts right into the gap between **defined language behaviour** and **typical OS behaviour in practice**. Let’s break it down clearly:

------

## 🧠 **What `malloc` Actually Does**

In C, the `malloc(size_t size)` function attempts to allocate memory on the heap. According to the **C standard**, if the allocation fails:

> It returns `NULL`.

This gives programmers the **option** to check for failure:

```c
void* p = malloc(1024);
if (p == NULL) {
    // handle failure
}
```

------

## 🧱 **But Why Does `malloc` Ever Fail?**

`malloc` can fail in several cases:

1. **System is genuinely out of memory** (RAM + swap exhausted).
2. **User process has reached its memory limits** (ulimit or cgroup quotas).
3. **The allocator itself can't fulfil the request** (fragmentation, large contiguous block unavailable).
4. **Artificial denial** due to `malloc`-level thresholds (custom allocators, safety caps).

------

## 🖥️ **What the Operating System Actually Does**

In *modern desktop/server OSes* like Linux, Windows, or macOS, the behaviour is nuanced:

### 1. **Linux and Over commit**

On Linux, by default:

- The kernel **over commits** memory.
- This means `malloc()` often **succeeds**, even when there isn’t enough physical memory.
- Only **when the memory is actually used** (e.g., via `memset`, or writing to the pointer) does the kernel decide whether to **kill the process (OOM killer)**.

> In short: **`malloc` succeeds → write to memory → OOM killer strikes**.

You can change this via `/proc/sys/vm/overcommit_memory`.

### 2. **Windows**

On Windows:

- `VirtualAlloc` or `HeapAlloc` checks system memory availability more conservatively.
- You’re less likely to get overcommit behavior.
- Still, when you run out, the allocation fails, and `malloc` returns `NULL`.

### 3. **macOS**

- Similar to Linux in that it may over commit, but OOM behaviour is often more graceful — memory pressure notification APIs may get triggered before killing.

------

## 😵 **So Why Does `malloc` Return NULL, Then?**

Despite the OOM killer and over commitment tricks, `malloc` returning `NULL` is still:

- **Defined behaviour by the language spec.**
- Useful for **custom allocators**, embedded systems, real-time systems.
- Essential in **low-level systems programming**, where robustness is a must (e.g., file systems, kernels, runtimes).

### Example:

In an **embedded system** or constrained environment:

- There’s no swap.
- There’s no OOM killer.
- If `malloc` fails, it’s your job to recover, log, or reboot cleanly.

------

## 🧩 So to your question:

> *If the OS might just kill the process, what's the point of checking `malloc` for NULL?*

### The answer is:

- **In environments where allocation can fail gracefully (e.g., embedded, controlled systems), it matters**.
- **In systems with over commit/OOM killers, checking `NULL` doesn't help if the process is killed before the return** — but:
  - `malloc` might still return `NULL` if the kernel *predicts* failure.
  - Or you’re using custom allocators or limits.

> So the language gives you the **tool** (`NULL` check), but in practice, it may or may not be effective — especially under over commit/virtual memory systems.

------

## ✅ TL;DR

- `malloc` returning `NULL` is **defined and portable behaviour** in C.
- **Modern OSes often over commit memory**, so actual allocation failure may **not occur until memory is used**.
- **If the process is killed due to OOM**, checking `NULL` won’t help.
- Still, **checking for `NULL` is good practice** — especially in environments where memory failure can be recovered from or logged.

Let me know if you want to explore how other languages like Rust, Go, or Java handle this under the hood — their strategies are very different.