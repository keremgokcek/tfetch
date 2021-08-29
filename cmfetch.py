#!/usr/bin/env python
import re
import subprocess

computer_name = open("/sys/devices/virtual/dmi/id/product_name").read().replace("\n", "")
cpu_name = open("/proc/cpuinfo").read().splitlines()[4].split(": ")[1]
distro = re.search("PRETTY_NAME=\"(.*?)\"", open("/etc/os-release").read()).group(1)
ram_total = str(round(int(re.search("MemTotal:        (.*?) kB", open("/proc/meminfo").read()).group(1)) / 1024))
ram_used = str(round(int(re.search("MemAvailable:    (.*?) kB", open("/proc/meminfo").read()).group(1)) / 1024))
kernel = subprocess.check_output(['uname', '-r']).decode("utf-8").replace("\n", "")

print("\x1b[32mComputer Name: \x1b[34m" + computer_name)
print("\x1b[32mCPU Name:      \x1b[34m" + cpu_name)
print("\x1b[32mDistro:        \x1b[34m" + distro)
print("\x1b[32mRam:           \x1b[34m" + ram_used + "/" + ram_total + " MB")
print("\x1b[32mKernel:        \x1b[34m" + kernel)
print("\x1b[0m", end="")