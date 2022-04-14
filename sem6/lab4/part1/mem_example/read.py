#!/usr/bin/python3
import sys
import re
def print_usage():
    print("Usage: read.py pid")
    exit(1)
def read_heap(pid, only_writable=True):
    mem_perm = 'rw' if only_writable else 'r-'
    maps_filename = "/proc/{}/maps".format(pid)
    mem_filename = "/proc/{}/mem".format(pid)
    try:
        with open(maps_filename, 'r') as maps_file:
            with open(mem_filename, 'rb+', 0) as mem_file:
                for line in maps_file.readlines():
                    addr_perm = r'([0-9A-Fa-f]+)-([0-9A-Fa-f]+) ([-r][-w])'
                    m = re.search(addr_perm, line)
                    h = re.search(r'(\[heap\])', line)
                    if m.group(3) == mem_perm and h and h.group(0) == "[heap]":
                        start_addr = int(m.group(1), 16)
                        end_addr = int(m.group(2), 16)
                        mem_file.seek(start_addr)
                        heap = mem_file.read(end_addr - start_addr)
                        print(''.join(chr(i) for i in heap))
    except IOError as e:
        print("[ERROR] Can not open file {}:".format(maps_filename))
        print("        I/O error({}): {}".format(e.errno, e.strerror))
        exit(1)
try:
    if len(sys.argv) != 2:
        print_usage()
    pid = int(sys.argv[1])
    read_heap(pid)
except Exception as e:
    print_usage()
