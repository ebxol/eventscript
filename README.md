# Event Script
Basic procedural interpreted language with simple loops and variables (similar to Batch scripting). I am sure it's turing-complete but probably impractical for anything sophisticated. This project was made in September 2015.

There are a few example scripts provided (sorting and summation)

# Example

```
event main:
//this script calculates the summation of numbers from 1 to N and verifies it with gauss's method

var int max = 0
var int sum
var int total
print("Type in a positive number N to sum from 0 to N")
print("\n")
USERINPUT(max)

	loop (sum = 0; <= max; 1)
	total = sum + total
	endloop

var int gauss = max + 1
gauss = max * gauss / 2

print("The result is ")
printval(total)
print("\n")

print("The gaussian method result is ")
printval(gauss)
print("\n")

endevent
```

# Installation

The VS2013 solution is provided. Simply rebuild the project.
