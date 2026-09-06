# LittleShell

![LittleShell is a toy shell in C](/docs/Presentation.png)

LittleShell is a toy shell in C based on the lessons from shipthatcode.com (completed on a separate branch). It features builtin commands,program execution, history, IO redirection, pipelines and logical operators and a small test suite.

## Features

### 1. Builtin Commands
  - echo
  - cd
  - pwd
  - set 
  - unset
### 2.Tokenization based on three states
  - Default: tokens end on white spaces
  - Single quotes: everything is a literal until closing quote
  - Double quotes: same as single quotes but allows variable expansion with $
### 3.History
  - print a list of last executed command through history command
  - !n executes and entry from the list if n is valid
  - !! prints last executed command
### 4. IO redirection
  - support for '>' '>>' '<'
  - support for redirecting output to other file descriptors
### 5.Program execution
  - fork() + exec() call with IO redirection if the case
### 6.Pipelines
  - implemented using pipe system call and dup2 for IPC
### 7.Logical operators
  - support for && || ;

## Testing
  The program was tested using a python script in the tests folder (to run test python test_shell.py)