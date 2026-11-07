# CSA0405-1911260265-Assignment1

Operating Systems (CSA0405) - Assignment 1 (CO1): Evaluate and Justify the Operating-System Design of a Smart Hospital

| Field | Entry |
|-------|-------|
| Name | Mohammed Falah Miya |
| Register number | 1911260265 |
| Course | CSA0405 Operating Systems |
| Submission date | 05/11/2026 |
| GitHub repository | https://github.com/mdfalahmiya-lang/OS/tree/main |
| Compiler and version | see `env.txt` (written by `run_all.sh`; e.g. gcc 13.3.0) |
| Linux distribution and version | see `env.txt` (e.g. Ubuntu 24.04.5 LTS) |
| Compile command | `gcc -std=c11 -Wall -Wextra -pthread <file>.c -o <name>` (Task10 uses `-std=gnu11`) |

## Repository layout
One folder per task (`Task01` ... `Task12`). Each holds the source files, command outputs, screenshots and a `NOTES.md`.
Tasks 1-4, 8, 9, 11 and 12 are written analyses (see the report); the programs are in Tasks 5, 6, 7 and 10.

## How to run (everything at once)
```bash
sudo apt install -y gcc strace      # strace is needed for Tasks 5, 6 and 10
./run_all.sh                        # compiles, runs, and saves every output next to its source
```

## How to run (task by task)
| Task | Folder | Commands |
|------|--------|----------|
| 5 | Task05 | `gcc -std=c11 -Wall -Wextra task05_demo.c -o task05_demo && strace -o trace_full.txt ./task05_demo` ; `gcc -std=gnu11 -Wall -Wextra syscall_cost.c -o syscall_cost && ./syscall_cost` |
| 6 | Task06 | `gcc -std=c11 -Wall -Wextra -pthread task06_copy.c -o task06 && ./task06` ; `strace -c ./task06` |
| 7 | Task07 | `gcc -std=c11 -Wall -Wextra task07_shell.c -o task07 && ./task07` ; `gcc -std=c11 -Wall -Wextra task07_dup.c -o task07_dup && ./task07_dup > out.txt` |
| 10 | Task10 | `gcc -std=gnu11 -Wall -Wextra -pthread task10_bench.c -o task10 && time ./task10` |

## Results of each task
- Task 1 - ranked requirement tables for six environments; single-OS alternative evaluated (report).
- Task 2 - OS type chosen per environment with rejected alternatives; SMP for imaging, cluster with fail-over for the database (report).
- Task 3 - structure comparison; microkernel (QNX/FreeRTOS style) for monitors, modular monolithic (Linux) for the database server (report).
- Task 4 - service inclusion/exclusion table for each environment (report).
- Task 5 - `strace` of `task05_demo`: raw `read()` asks for 63 bytes, `fread()` asks for 4096; system-call versus function-call cost ratio in `Task05/output.txt`.
- Task 6 - completed `copy_file`: 10001 / 21 / 4 `read()` calls for buffers of 1 / 512 / 4096 bytes (matches the expected output); `strace -c` counts in `Task06/strace_c.txt`.
- Task 7 - completed shell program: job 1 exit status 0, job 2 exit status 3 (matches the expected output); with the second `fflush` removed and `exit(3)`, redirected output repeats "shell: running job 2" (`Task07/`).
- Task 8 - user/kernel mode, interrupts, memory protection; polling vs interrupts for the monitors (report).
- Task 9 - memory hierarchy, data placement, DMA vs interrupt-per-word for a 500 MB image (report).
- Task 10 - buffer-size, `time`, process vs thread measurements (`Task10/output.txt`, `Task10/time.txt`).
- Task 11 - final plan, limits and risks, virtual-machine alternative (report).
- Task 12 - design challenge (home monitoring and cloud records): modify the original design (report).

## Commit history
One commit per task, made progressively with `./commit_per_task.sh` (or by hand, e.g. `git add Task06 && git commit -m "Task 6: copy program and measurements"`).
