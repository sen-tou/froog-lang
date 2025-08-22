# Frog Lang

Frog Lang is a programming language that I designed in order to teach me how programming languages are made.
This project is purely recreational and just serves my own purposes.  

## Design

### Core
- structs
- turing complete
- functions
- everything is an expression
- no semicolons
- a small standard lib
- staically typed
- imports through namespaces
- compiler and REPL
- Result/Option typed
- manual memory managment / optional garbage collector

#### Syntax
- Types: UpperCamelCase
- function: lowerCamelCase
- constants: CAPITALIZED
- variables: lowerCamelCase

#### Data Types
- Booleans
- Option

#### Expression Evaluation
- if: evaluates to one branch with its return value
- switch: handles in if 
```
if ... then ... else ...
if ... then {} else {} -> block needs a return 
```


### Nice to have
- Generics
- Preprocessor of some kind (recurssive?)

## Ressources

- [https://craftinginterpreters.com](Craftning Interpreters)
