
# simple_compiler

Experimentally testing compiler building.

## About

This is a compiler for my new language, and it is the first compiler I have made.

I call this project a simple compiler, a compiler from scratch, naive compiler or an improvised compiler.

The idea of this project is to reinvent a compiler. I know a few things about what compilers are, how they work, and what parts they have, but I have generally never written a compiler, read other compilers, or studied compiler design closely.

From what I know, a lexer tokenizes text, a parser understands the logic, and a code generator generates assembly. I want to use these ideas to create a working compiler.

I chose MIPS for this compiler because, in parallel with this project, I am studying assembly at university, and we are working with MIPS.

## WHAT IS ALREADY DONE

* Text buffer: done
* Lexer
* Only a few types still need to be implemented
* Parser and code generation are tightly connected
* Variable declarations: done
* Creating arrays and assigning variables: done
* Declaring functions and passing parameters: done
* Functions do not return anything yet. Maybe I will change this later.
* Functions are not recursive yet. Maybe I will change this later.
* Embedded assembly: done
* Branches (`if` / `else`): not done yet, but will be done
* Loops: not done yet. I want to implement them, but maybe I will have only recursion, or only loops, or both.
* Standard library: currently only null-terminated character array printing

## PHILOSOPHY

The philosophy of the language is simple.

When creating a compiler, you want to invent your own language because you can control a lot of things. From a technical fight, it also becomes a design fight, and you get a better understanding of why some programming languages are designed similarly. I will show some examples later.

The philosophy of my language is that I want to make a Snake game in my language. To make a Snake game, I do not need a lot of features. If I do not need a lot of features, the language also does not need a lot of features, so I can create a simple compiler with a simple language.

Whenever I want to add something, I think about whether it is worth it and whether I will actually use it to create Snake. For example, the early version of the compiler had type checking, but I removed it. Function returns are another feature that I do not really need. They are also more complicated to implement in assembly. Variables are register-sized, and every variable has a fixed size.

Another part of the philosophy is “no magic.”

In a lot of languages, there are magic functions: functions that cannot be written in the language itself. They are default, “God-given” functions, and you do not know where they come from. They may come from the compiler or somewhere else.

I decided to eliminate that. I will not have a `print` function in my language that is secretly defined inside the compiler. All functions should be possible to write in the language itself.

This is why I need embedded assembly. It is something that C has, and it allows the standard library to be written in the same language, meaning there is no magic.

It may be seen as cheating to use embedded assembly so early, especially because it gives the language a lot of power, but I would argue that it is even better. The language's functions and features are written in the language itself, meaning that you could even argue that part of the compiler is written in `.yu`.

The reason for the `.yu` extension is that, while creating this compiler, I was playing *Prey*, and the main character was Dr. Yu.

This language does not have an AST, which may not be the usual or recommended way of creating a language. I know that ASTs exist, but I have never actually tried to implement one.

My thinking was that I could write a simple compiler with direct code generation, so I do not need an AST for this simple compiler.

ASTs, sorting algorithms, code-generation algorithms, and optimizations are interesting topics that I will pursue later when I make a serious compiler project. In that project, I want to create a language that I would actually like to use, something more complex, faster, and more in line with modern compiler-writing techniques and established algorithms and optimizations.

That is a later project. Before doing it, I want to write some code in OCaml, Zig, and Rust to get a better understanding of what other languages have and what I can take from them. Right now, my knowledge of compiled languages is mainly based on C and C++, and I do not want to just write a simpler version of C.

## IMPLEMENTATION

### TEXT BUFFER

For a lot of reasons, C++ strings and vectors are slow and ideologically against the C-style data-oriented design I prefer.

However, I almost never replaced them with better data structures when I was programming in C++, because in a lot of cases, performance is not necessarily important, and implementing your own dynamic text array is time-consuming.

But when I started programming this compiler, I chose C, which left me without a premade dynamic character array system.

The idea is that I want a character array that is predictable, easy to fit into the cache, and does not need to reallocate every time. This led me to a chunk-based data structure.

I need to put data into the cache, process it, and then get the next data. If you have fixed-size arrays that are the size of a cache-friendly chunk, it is like pre-planning how the data will go into the cache and be processed.

This led to the idea of having a struct containing an array and a pointer to the next chunk. I am essentially writing the file into it and then reading it word by word for the lexer.

It is much simpler than a C++-style vector, and at the same time, I do not need anything more complicated. It is simpler and faster in the places where I actually care about .

### LEXER

I am lazy, and the lexer is a great representation of my laziness.

The lexer completely changed the way I designed the programming language. A lot of programming languages have a concept of reserved words, where those reserved words represent tokens.

If I had written this in C++, maybe I would have just used a hash map to do the same thing as other languages and tokenize keywords. But because I did not have one, I just used the simplest possible solution.

I call it FLT, or First Letter Tokenizing.

I look at the first letter, and it tells me the token type of the word. It is as simple as, for example, `$[name]` for a variable or `#[name]` for a function call. A normal `[word]` is simply a variable name.

It is simple, but it works.

There are also types that are a little different. For example, a character is `'c'`. I look for the concept not only at the start but also at the end. If I do not see the ending character, it is a lexer error.

Another type is `"something"`, which represents embedded assembly. It works similarly to the character system, but at the end it removes the `"`, similar to how `$name` removes the `$`.

The tokens are stored in an enum.

There are also other types, such as `{` and `}`, where I tokenize them in a more standard way as left bracket and right bracket. If a bracket is not closed, it is not a lexer error; it is a parser error.

This may seem inconsistent, and it is inconsistent, but it was the simplest way I could figure out how to implement it.

If I did it in a more standard way, I would probably treat every `"`, `[`, and `'` as separate things and handle all of them in the parser. Although, I do not know if that would cause problems later.

### PARSER

When I receive the tokenized array, I want to immediately generate code.

As I mentioned earlier, I decided to use direct code generation because I thought it was the simplest way to do things.

This makes syntax such as `a+b+c*(2+3)` difficult to figure out how to parse, so I skipped it. Maybe I will come back later and figure it out, but maybe not, because it is unnecessary for the Snake compiler.

The way I parse things is what I call FWP, or First Word Parsing.

From the first token, I determine the grammar. This is similar to something that many languages figure out.

For example, C is not an FWP-friendly language because `int` can mean a variable declaration or be part of a function declaration, so you cannot determine the grammar from the first token alone.

Some other languages have something like `fn` before a function declaration, so they can determine the grammar rules more easily from the first token. My grammar works similarly.

Also, parsing a function is a masterpiece of engineering.

I call the parser function with a simple flag to determine whether it is parsing a function declaration or a normal declaration. It parses like a normal program, except that when it sees `}`, if it was parsing the main function, it produces an error. Otherwise, it simply exits back to the previous function.

This means I can declare a function inside a function inside a function, similar to some other programming languages.

There is also an interesting fact about how this works with jumping, which I describe in the code generation section.

Also, the function passing variables to a function by value and by address was an interesting design and technical challenge, which I kind of figured out technically, but I will not list the details of how because most likely I will change it design-wise for a simpler and more powerful way to do it.

Okay, I will tell that interesting challenge. Right now, when declaring a function, you only mention that it is a name, and you can pass both values and addresses when calling the function, which is kind of an inconsistent mess right now. But I will most likely think of a better design, like only passing addresses, which would be more consistent, or I can do synchronized shape checking. I don't know. There is a design choice here. It is not technical, but generally in programming, both are technical.

### CODEGEN

I have direct code generation, meaning that the parser calls something and I generate the code immediately.

There is no list of code that is generated first and then analyzed to make a more thoughtful design.

This method requires the parser to know what is in the code generator and the code generator to know what is in the parser.

For example, I have two buffers: a declaration buffer and a main buffer.

The declaration buffer contains all declarations, while the main buffer contains all code, such as assignments.

Some things require both. For example, declaring and assigning a value requires code in both buffers.

I know I could do the same thing using only the declaration buffer, and it might even be more optimized, but I do not care about optimization here. I care about simplicity, so when doing both is simpler, I do both.

However, when I am doing something called a function definition, I cannot use the same system.

I do not have a third buffer for function definitions. It would not be hard to make one, but I decided not to. Instead, I use some insane code that feels like hacking.

I call it “jump-over hacking.”

When I am writing the main function and there is a function definition in the middle of it, I simply jump over the function definition while declaring the function in the middle of the main buffer.

I think this is unnecessarily complicated. It requires the parser to be aware of the start and end of a function definition.

Although it may seem like a lot of unnecessary complexity, this approach also saves some programming complexity.

If I had a third buffer, then when I was parsing things inside the function, I would have to target that third buffer. Because I am doing everything in one buffer, I do not need to add a flag to every code-generation operation to specify which buffer should be targeted.

So while it might seem like unnecessary complexity, it actually makes the code simpler in some ways. Instead of having a flag for every code-generation operation, I only need a flag for the parser.

### TESTING

I have not done any testing yet.

It seems stupid. Why? It is a compiler. Testing should be one of the cheapest things to do. You do not even need to think much about testing: you have an input and an output, and for the same input, you should get the same output every time.

I could have separate tests for the lexer, parser, and so on, as well as global tests.

The reason I have not done this is that I am also designing the language. Every time I go back to the tests, I have to change them and figure out what the new output should be.

The syntax is changing so rapidly from what I originally planned that testing it right now is basically meaningless.

I will add testing at the point where I am reasonably sure about the grammar of the language.

It will be really useful to just run the code and not have to check the output manually.

