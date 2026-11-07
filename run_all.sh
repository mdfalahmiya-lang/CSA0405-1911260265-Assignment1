#!/usr/bin/env bash
# Compiles and runs every program of CSA0405 Assignment 1 and saves the evidence
# (outputs, strace traces, time results) in the task folders.
# Usage:  ./run_all.sh        (needs gcc; strace is needed for the strace files)
set -u
ROOT="$(cd "$(dirname "$0")" && pwd)"
cd "$ROOT" || exit 1
CF="-std=c11 -Wall -Wextra -pthread"
HAVE_STRACE=1; command -v strace >/dev/null 2>&1 || HAVE_STRACE=0
[ "$HAVE_STRACE" = 0 ] && echo "WARNING: strace not found. Install it:  sudo apt install strace   (strace files will be skipped)"

# ---- environment (used by README and the report) ----
{
  echo "GCC=$(gcc --version | head -1)"
  echo "GCC_VER=$(gcc -dumpfullversion)"
  echo "DISTRO=$(. /etc/os-release; echo "$PRETTY_NAME")"
  echo "KERNEL=$(uname -r)"
  echo "VCPUS=$(nproc)"
} > env.txt
cat env.txt

echo "== Task 5 =="
( cd Task05 || exit 1
  gcc $CF task05_demo.c -o task05_demo && ./task05_demo > output.txt
  gcc -std=gnu11 -Wall -Wextra syscall_cost.c -o syscall_cost && ./syscall_cost >> output.txt
  if [ "$HAVE_STRACE" = 1 ]; then
    strace -o trace_full.txt ./task05_demo > /dev/null
    sed -n '/record.txt/,$p' trace_full.txt > trace_demo.txt
    strace -c -o strace_c.txt ./task05_demo > /dev/null
  fi
  cat output.txt; rm -f record.txt )

echo "== Task 6 =="
( cd Task06 || exit 1
  gcc $CF task06_copy.c -o task06 && ./task06 | tee output.txt
  if [ "$HAVE_STRACE" = 1 ]; then
    strace -c -o strace_c.txt ./task06 > /dev/null
    strace -e trace=read,write -o strace_rw.txt ./task06 > /dev/null
    echo "--- strace -c ---"; cat strace_c.txt
  fi )

echo "== Task 7 =="
( cd Task07 || exit 1
  gcc $CF task07_shell.c -o task07 && ./task07 | tee output.txt
  gcc $CF task07_dup.c -o task07_dup && ./task07_dup > output_redirected_dup.txt
  echo "--- redirected, second fflush removed, exit(3) ---"; cat output_redirected_dup.txt )

echo "== Task 10 =="
( cd Task10 || exit 1
  gcc -std=gnu11 -Wall -Wextra -pthread task10_bench.c -o task10 || exit 1
  { time ./task10 > output.txt; } 2> time.txt
  cat output.txt; echo "--- time ---"; cat time.txt )

echo "Done. Now run ./commit_per_task.sh"
