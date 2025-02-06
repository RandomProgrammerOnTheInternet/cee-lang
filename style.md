# Coding Style
Follow this style when writing code for the compiler
## Naming
Use `snake_case` for EVERYTHING.
Add a `_t` to structs and unions, like this `struct_t`
However, use `SCREAMING_SNAKE_CASE` for macros.
Use descriptive names. Don't call anything `foo` or whatever.
You can use abbreviations *only* if the meaning is widely known, like `temp`.
## Comments
```C
// single line comments look like this (note the space)

/*
 * Multiline comments look like this
 * 
 * text.
 */
```
Put a comment before a function to describe what it does.
Example:
```C
/*
 * increment: increments num
 *
 * num: the number to be incremented
 *
 * returns: the incremented number
 */
int increment(int num) {
	num++;
	return num;
}
```
## Character Limit
The character limit is 80 chars. Period.
If you find yourself writing a line of code that is more than 80 chars, break it up.
Example:
```C
int function(int a, int b, int c, char d, float e,
			 long *f, data_type_t g, data_type **h) {
	do_something();
}
```
Only exception is comments.
## Indentation
Indentations are **tabs**, not spaces.
This doesnt apply to comments. You can also break this rule if you need a few extra spaces to align stuff.
The tab size is 4.
The indentation limit is 3 indents. Any more than that, and your code most likely sucks.
Use extraction and inversion to make your code more readable.
## Curly Braces
Curly braces are **ALWAYS** on the same line.
Example:
```C
if(condition) {
	statement;
}
```
This also applies to functions.
## Functions
Functions should do one thing, and one thing only.
You should break up your function into smaller functions.
##
