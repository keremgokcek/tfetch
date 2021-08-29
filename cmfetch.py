#!/usr/bin/env python
import re
import subprocess

computer_name = open("/sys/devices/virtual/dmi/id/product_name").read().replace("\n", "")
cpu_name = open("/proc/cpuinfo").read().splitlines()[4].split(": ")[1]
distro = re.search(r"PRETTY_NAME=\"(.*?)\"", open("/etc/os-release").read()).group(1)
ram_total = str(round(int(re.search(r"MemTotal:        (.*?) kB", open("/proc/meminfo").read()).group(1)) / 1024))
ram_used = str(round(int(re.search(r"MemAvailable:    (.*?) kB", open("/proc/meminfo").read()).group(1)) / 1024))
kernel = subprocess.check_output(['uname', '-r']).decode("utf-8").replace("\n", "")
arch = subprocess.check_output(['uname', '-m']).decode("utf-8").replace("\n", "")
terminal = subprocess.check_output(['ps -o comm= -p "$(($(ps -o ppid= -p "$(($(ps -o sid= -p "$$")))")))"'], shell=True).decode("utf-8").replace("\n", "")
uptime_c = subprocess.check_output(['uptime']).decode("utf-8").replace("\n", "")
uptime = [re.findall(r"(\d+):(\d+)", uptime_c)[1][0], re.sub(r"0(\d)", r"\1", re.findall(r"(\d+):(\d+)", uptime_c)[1][1])]

print("\x1b[32mComputer Name: \x1b[34m" + computer_name)
print("\x1b[32mCPU Name:      \x1b[34m" + cpu_name)
print("\x1b[32mDistro:        \x1b[34m" + distro)
print("\x1b[32mRam:           \x1b[34m" + ram_used + "/" + ram_total + " MB")
print("\x1b[32mKernel:        \x1b[34m" + kernel)
print("\x1b[32mArchitecture:  \x1b[34m" + arch)
print("\x1b[32mTerminal:      \x1b[34m" + terminal)
print("\x1b[32mUptime:        \x1b[34m" + uptime[0] + " hours " + uptime[1] + " minutes")
print("\x1b[0m", end="")