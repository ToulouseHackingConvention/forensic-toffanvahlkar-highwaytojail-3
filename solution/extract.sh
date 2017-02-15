#!/bin/bash

OUT="out"
FILES="$OUT/files.txt"
PROCS="$OUT/psaux.txt"
MAPS="$OUT/proc_maps.txt"

mkdir -p "$OUT"

# Get the usefull files (malware and ciphered home).
volatility -f res/guest_dump --profile=LinuxDebian87x64 linux_enumerate_files > "$FILES"
inode_bin=$(grep cryptolock "$FILES" | cut -d " " -f 1)

volatility -f res/guest_dump --profile=LinuxDebian87x64 linux_find_file -i "${inode_bin}" -O "$OUT/cryptolock" &

# Dump the malware memory.
volatility -f res/guest_dump --profile=LinuxDebian87x64 linux_psaux > "$PROCS"
pid=$(grep cryptolock "$PROCS" | cut -d " " -f 1)

volatility -f res/guest_dump --profile=LinuxDebian87x64 -p "$pid" linux_proc_maps > "$MAPS"
heap_base_addr=$(grep heap "$MAPS" | awk '{ print $4 }')
volatility -f res/guest_dump --profile=LinuxDebian87x64 -p "$pid" linux_dump_map --vma "${heap_base_addr}" -D "$OUT/"

# Get a list of potentials keys from the heap of the malware.
heap=$(find "$OUT" -maxdepth 1 -type f -name "task.$pid.*.vma" -print)
python extract_key.py "$heap" "$OUT/"
