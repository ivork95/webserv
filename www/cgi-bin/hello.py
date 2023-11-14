import os
import sys

if sys.stdin.isatty():
    print("No input provided.")
    sys.exit(0)
# print("helllo")
for line in sys.stdin:
    if not line:
        break
    line = line.strip()
    print(line)