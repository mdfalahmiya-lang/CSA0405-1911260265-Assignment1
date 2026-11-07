#!/usr/bin/env bash
# One commit per task (real timestamps - do not back-date commits).
# Usage: run inside your cloned repository, after ./run_all.sh
set -e
git rev-parse --is-inside-work-tree >/dev/null
printf 'bin/\n*.o\ntask05_demo\nsyscall_cost\ntask06\ntask07\ntask07_dup\ntask10\n' > .gitignore
git add README.md run_all.sh commit_per_task.sh fill_evidence.py env.txt .gitignore 2>/dev/null || true
git commit -m "Add README, scripts and environment details" || true
declare -A MSG=(
 [Task01]="Task 1: ranked requirements and single-OS alternative"
 [Task02]="Task 2: OS types, SMP vs AMP, cluster fail-over"
 [Task03]="Task 3: OS structures and microkernel vs monolithic"
 [Task04]="Task 4: OS services table"
 [Task05]="Task 5: system calls, strace evidence, call cost"
 [Task06]="Task 6: copy program (TODO 1) and buffer-size measurements"
 [Task07]="Task 7: fork/exec/wait shell program and fflush experiment"
 [Task08]="Task 8: CPU modes, interrupts, protection"
 [Task09]="Task 9: memory hierarchy and I/O methods"
 [Task10]="Task 10: measurements (strace -c, time, fork vs threads)"
 [Task11]="Task 11: final OS plan"
 [Task12]="Task 12: design challenge - remote monitoring and cloud"
)
for t in Task01 Task02 Task03 Task04 Task05 Task06 Task07 Task08 Task09 Task10 Task11 Task12; do
  git add "$t"
  git commit -m "${MSG[$t]}" || echo "(nothing new to commit for $t)"
done
git log --oneline
echo "Now:  git push origin main"
