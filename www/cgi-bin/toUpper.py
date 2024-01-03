import sys

if sys.stdin.isatty():
    print("No input provided.")
else:
    for line in sys.stdin:
        if not line:
            break
        line = line.strip()
        print(line.upper())