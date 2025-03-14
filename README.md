# W25Shell - Custom Shell Implementation

## Overview

W25Shell is a custom shell implementation for COMP-8567 Assignment 03 (Winter 2025). This shell executes commands using fork(), exec(), and other system calls to provide functionality similar to a standard Unix shell, with additional special operations.

## Implementation and Testing Progress Checklist

### Core Functionality

- [x] Basic shell loop and prompt implementation
- [x] Command parsing and tokenization
- [x] Memory management and cleanup ([See code](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L412-L430))
- [x] Basic command execution using fork() and exec()
  - [ ] `ls` - List directory contents
  - [ ] `pwd` - Print working directory
  - [ ] `echo hello world` - Echo text
- [x] Argument count validation (1-5 arguments per command)
  - [ ] Command with 5 arguments (maximum): `ls -l -a -h -t`
  - [ ] Command with >5 arguments (should fail): `ls -l -a -h -t -r`

### Built-in Commands

- [x] `killterm` - Kill the current terminal ([See code](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L443-L447))
  - [ ] Test: `killterm` (should exit current shell)
- [x] `killallterms` - Kill all w25shell terminals ([See code](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L453-L481))
  - [ ] Test: `killallterms` (should exit all open shells)

### Piping Operations (`|`)

- [x] Basic piping implementation ([See code](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L230-L285))
  - [ ] Single pipe: `ls -l | grep .txt`
  - [ ] Two pipes: `ls -l | grep .txt | wc -l`
  - [ ] Maximum pipes (5): `ls | grep . | sort | head -n 3 | wc -l`
- [x] Argument count validation with pipes
  - [ ] Valid: `ls -l -a -h -r | wc -l`
  - [ ] Invalid: `ls -l -a -h -r -t | wc -l`

### Reverse Piping (`=`)

- [x] Basic reverse piping implementation ([See code](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L291-L345))
  - [ ] Single reverse pipe: `wc -l = cat sample.txt`
  - [ ] Multiple reverse pipes: `wc -l = grep -v test = cat sample.txt`
- [x] Argument count validation with reverse pipes
  - [ ] Valid: `wc -l = grep -a -b -c = cat`
  - [ ] Invalid: `wc -l -a = grep -a -b -c -d -e = cat`

### File Operations

- [x] Append between files (`~`) ([See code](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L353-L396))
  - [ ] Test: `file1.txt ~ file2.txt`
  - [ ] Verify: `cat file1.txt` and `cat file2.txt`
- [x] Count words in file (`#`) ([See code](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L402-L428))
  - [ ] Test: `# sample.txt`
  - [ ] Test with large file: `# large_sample.txt`
- [x] File concatenation (`+`) ([See code](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L434-L454))
  - [ ] Two-file concatenation: `file1.txt + file2.txt`
  - [ ] Maximum file concatenation: `file1.txt + file2.txt + file3.txt + file4.txt + file5.txt`

### Redirection

- [x] Input/output redirection (`<`, `>`, `>>`) ([See code](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L460-L508))
  - [ ] Test: `grep hello < input.txt`
  - [ ] Test: `ls -l > output.txt`
  - [ ] Test: `echo "more data" >> output.txt`
  - [ ] Test: `grep hello < input.txt > output.txt`

### Sequential Execution (`;`)

- [x] Basic sequential execution ([See code](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L351-L358))
  - [ ] Two commands: `date ; pwd`
  - [ ] Three commands: `date ; pwd ; ls`
  - [ ] Maximum commands (4): `echo 1 ; echo 2 ; echo 3 ; echo 4`
- [ ] Handle exceeding maximum
  - [ ] Test: `echo 1 ; echo 2 ; echo 3 ; echo 4 ; echo 5`

### Conditional Execution

- [x] AND and OR operations (`&&`, `||`) ([See code](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L364-L416))
  - [ ] Success case: `ls && echo "Command succeeded"`
  - [ ] Failure case: `ls non_existent_file && echo "This won't print"`
  - [ ] Multiple AND: `ls && pwd && echo "Both succeeded"`
  - [ ] Success after failure: `ls non_existent_file || echo "Command failed"`
  - [ ] Skip after success: `ls || echo "This won't print"`
  - [ ] Mixed conditionals: `ls && echo "Success" || echo "This won't print"`

### Edge Cases

- [x] Empty command (just press Enter)
- [ ] Non-existent command: `thiscommanddoesnotexist`
- [ ] Non-existent input file: `cat nonexistent.txt`
- [ ] Permission denied: Try writing to a read-only file
- [ ] Incorrect special character syntax: `| ls` or `# `
- [ ] Interrupt with Ctrl+C

## Compilation and Execution

### Compile the Code

```bash
gcc -o w25shell kirtan_prajapati_110181626.c
```

### Run the Shell

```bash
./w25shell
```

## Important Note

This program involves creating multiple processes which can potentially lead to a "fork bomb" that consumes system resources. It is **MANDATORY** to execute the following command periodically after working with this program:

```bash
killall -u yourusername
```

Failing to do so may result in zero marks for the assignment.

## Features Reference

### Built-in Commands

- `killterm` - Kills the current w25shell terminal
- `killallterms` - Kills all w25shell terminals that are currently open

### Argument Count Restrictions

- Each command can have 1-5 arguments (including the command name)

### Special Characters Support

- `|` - Piping (up to 5 pipe operations)

  ```
  w25shell$ ls -l | grep .txt | wc -l
  ```

- `=` - Reverse piping (up to 5 operations)

  ```
  w25shell$ wc -l = grep -v test = cat sample.txt
  ```

- `~` - Append text between two files

  ```
  w25shell$ file1.txt ~ file2.txt
  ```

- `#` - Count words in a text file

  ```
  w25shell$ # sample.txt
  ```

- `+` - Text file concatenation (up to 5 files)

  ```
  w25shell$ file1.txt + file2.txt + file3.txt
  ```

- `<`, `>`, `>>` - Input/output redirection

  ```
  w25shell$ grep hello < sample.txt
  w25shell$ ls -l > output.txt
  w25shell$ echo "more data" >> output.txt
  ```

- `;` - Sequential execution (up to 4 commands)

  ```
  w25shell$ date ; pwd ; ls -l
  ```

- `&&`, `||` - Conditional execution (up to 5 operations)
  ```
  w25shell$ ls && echo "Command succeeded"
  w25shell$ ls non_existent_file || echo "Command failed"
  ```

## Setup for Testing

To prepare for testing, create these sample files:

```bash
# Create sample text files
echo "This is sample file 1 content" > sample.txt
echo "This is file 1 content" > file1.txt
echo "This is file 2 content" > file2.txt
echo "This is file 3 content" > file3.txt
echo "This is file 4 content" > file4.txt
echo "This is file 5 content" > file5.txt

# Create a larger sample file
for i in {1..100}; do echo "Line $i of the large sample file" >> large_sample.txt; done

# Create a file for redirection tests
echo "hello world test line" > input.txt
echo "another hello test" >> input.txt
echo "third line without hello" >> input.txt
```

## Implementation Details

The shell is implemented with a modular approach:

1. **Main Shell Loop**:

   - Takes input from the user
   - Parses the input to identify commands and special characters
   - Executes the appropriate handler function

2. **Command Parsing**:

   - Tokenizes input strings
   - Identifies special character operations
   - Validates argument counts

3. **Command Execution**:

   - Uses fork() and exec() for command execution
   - Creates pipes for communication between processes
   - Handles file operations and redirection

4. **Error Handling**:
   - Validates input and argument counts
   - Provides appropriate error messages
   - Cleans up resources to prevent memory leaks

## Restrictions

1. The system() library function is not used in this implementation.
2. Special characters combinations are not required to be supported (e.g., `p1 & p2 > list.txt`).
3. The shell can only handle a maximum of 5 arguments per command (including the command name).

## Cleanup

Remember to regularly run:

```bash
killall -u yourusername
```

to clean up any stray processes, especially after testing fork-related functionality.
