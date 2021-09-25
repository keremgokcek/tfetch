#!/usr/bin/env python3
import os
import re
import subprocess

def escape_ansi(line):
    ansi_escape =re.compile(r'(\x9B|\x1B\[)[0-?]*[ -\/]*[@-~]')
    return ansi_escape.sub('', line)

cm = {
  'rown': 0,
  'coln': 0,
  'content': ""
}

with open("/etc/cmfetch/cm", encoding='unicode_escape') as file:
  cm['content'] = file.read()
  file.seek(0)
  for i in file.read().splitlines():
    if len(escape_ansi(i)) + 1 > cm['rown']:
      cm['rown'] = len(escape_ansi(i)) + 1
  file.seek(0)
  cm['coln'] = len(file.read().splitlines())

print(cm['content'])

with open("/sys/devices/virtual/dmi/id/product_name") as file:
  computer_name = file.read().replace("\n", "")
with open("/proc/cpuinfo") as file:
  cpu_name = file.read().splitlines()[4].split(": ")[1]
with open("/etc/os-release") as file:
  distro = re.search(r"PRETTY_NAME=\"(.*?)\"", file.read()).group(1)
with open("/proc/meminfo") as file:
  ram_total = str(round(int(re.search(r"MemTotal: +(\d+) kB", file.read()).group(1)) / 1024))
  file.seek(0)
  ram_used = str(round(int(re.search(r"MemAvailable: +(\d+) kB", file.read()).group(1)) / 1024))
kernel = subprocess.check_output(['uname', '-r']).decode("utf-8").replace("\n", "")
arch = subprocess.check_output(['uname', '-m']).decode("utf-8").replace("\n", "")
terminal = subprocess.check_output(['ps -o comm= -p "$(($(ps -o ppid= -p "$(($(ps -o sid= -p "$$")))")))"'], shell=True).decode("utf-8").replace("\n", "")
def get_uptime():
  uptime_c = subprocess.check_output(['uptime']).decode("utf-8").replace("\n", "")
  try:
    uptime = [re.findall(r"(\d+):(\d+)", uptime_c)[1][0], re.sub(r"0(\d)", r"\1", re.findall(r"(\d+):(\d+)", uptime_c)[1][1])]
  except:
    uptime = ["0", re.findall(r"(\d+) min", uptime_c)[0]]
  return uptime

def get_resolution():
  resolution_a = subprocess.check_output(['xrandr -q | grep \' connected\''], shell=True).decode("utf-8").splitlines()
  resolution = []
  for i in resolution_a:
    resolution.append(re.search(r"\d+x\d+", i).group(0))
  return resolution

shell = os.path.basename(os.getenv("SHELL")) 
de = os.getenv("XDG_CURRENT_DESKTOP")
wm = subprocess.check_output("ps -e | grep -m 1 -o -F -e arcan -e asc -e clayland -e dwc -e fireplace -e gnome-shell -e greenfield -e grefsen -e hikari -e kwin -e lipstick -e maynard -e mazecompositor -e motorcar -e orbital -e orbment -e perceptia -e river -e rustland -e sway -e ulubis -e velox -e wavy -e way-cooler -e wayfire -e wayhouse -e westeros -e westford -e weston -e i3 -e dwm -e openbox", shell=True).decode("utf-8").replace("\n", "")

print(f"\033[{cm['coln']}A", end="")
print(f"\033[{cm['rown']}C", end="")
print("\x1b[32mComputer Name: \x1b[34m" + computer_name)
print(f"\033[{cm['rown']}C", end="")
print("\x1b[32mCPU Name:      \x1b[34m" + cpu_name)
print(f"\033[{cm['rown']}C", end="")
print("\x1b[32mDistro:        \x1b[34m" + distro)
print(f"\033[{cm['rown']}C", end="")
print("\x1b[32mRam:           \x1b[34m" + ram_used + "/" + ram_total + " MB")
print(f"\033[{cm['rown']}C", end="")
print("\x1b[32mKernel:        \x1b[34m" + kernel)
print(f"\033[{cm['rown']}C", end="")
print("\x1b[32mArchitecture:  \x1b[34m" + arch)
print(f"\033[{cm['rown']}C", end="")
print("\x1b[32mTerminal:      \x1b[34m" + terminal)
print(f"\033[{cm['rown']}C", end="")
uptime = get_uptime()
if uptime[0] == "0":
  print("\x1b[32mUptime:        \x1b[34m" + uptime[1] + " minutes")
else:
  print("\x1b[32mUptime:        \x1b[34m" + uptime[0] + " hours " + uptime[1] + " minutes")
i=1
for resolution in get_resolution():
  print(f"\033[{cm['rown']}C", end="")
  print("\x1b[32mResolution " + str(i) + ":  \x1b[34m" + resolution)
  i += 1
print(f"\033[{cm['rown']}C", end="")
print("\x1b[32mShell:         \x1b[34m" + shell)
print(f"\033[{cm['rown']}C", end="")
if de != None:
  print("\x1b[32mDE:            \x1b[34m" + de)
  print(f"\033[{cm['rown']}C", end="")
print("\x1b[32mWM:            \x1b[34m" + wm)
print("\x1b[0m", end="")
