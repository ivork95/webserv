import sys

if sys.stdin.isatty():
    print("No input provided.")
    sys.exit(0)

for line in sys.stdin:
    if not line:
        break
    line = line.strip()
    print(line.upper())