import sys

def multiply(num1, num2):
    return num1 * num2

if __name__ == "__main__":
    # Check if the correct number of arguments is provided
    if len(sys.argv) != 3:
        print("Usage: python multiplication.py <num1> <num2>")
        sys.exit(1)

    # Get the input numbers from command-line arguments
    try:
        num1 = int(sys.argv[1])
        num2 = int(sys.argv[2])
    except ValueError:
        print("Invalid input. Please enter valid numbers.")
        sys.exit(1)

    # Calculate the multiplication
    result = multiply(num1, num2)

    # Print the result
    print(result)