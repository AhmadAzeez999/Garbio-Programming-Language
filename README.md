# Garbio

## Syntax list:
1. var: You can store data.
``` garbio
var message = "Hello World!";

display(message);
```

2. function: To create reusable blocks of code.
``` garbio
function greet(name)
{
  display("Hello ");
  display(name);
};

greet("Alice") # Output: Hello, Alice
```
You can pass arguments to functions.

3. display: To print a message to the console.
``` garbio
display("This is a message");
```

4. displayln: To print a message with a newline character.
``` garbio
display("This message will end with a newline");
```

5. if, elseif, and else: For conditional execution.
``` garbio
var isWorking = true;

if (isWorking)
{
  display("It works!");
}
elseif (true)
{
  display("It doesn't work, but this works :)");
}
else
{
  display("Does not work");
}
```

6. while: For creating loops
``` garbio
var count = 1;

while (count < 5)
{
  display(count);
  count = count + 1;
}
```

## Data Types:
- Strings (text)
- Numbers (integers)

Basic Math Operations: You can perform addition (+), subtraction(-), multiplication(*), and division(/)
``` garbio
var a = 10;
var b = 5;

var add = a + b; # 15
var sub = a - b; # 5
var mult = a * b; # 50
var div = a / b; # 2
```
