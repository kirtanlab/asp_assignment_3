# W25Shell

A custom UNIX-like shell implementation with advanced piping and file operations.

## Core Functions

### Command Execution

• [Basic command execution](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L174-L226)

```
ls -l
pwd
echo hello world
```

### Built-in Commands

• `killterm` - [Kill current terminal](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L443-L447)

```
killterm
```

• `killallterms` - [Kill all shells](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L453-L481)

```
killallterms
```

### Piping Operations

• [Forward piping](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L230-L285)

```
ls -l | grep .txt
ls -l | grep .txt | wc -l
ls | grep . | sort | head -n 3 | wc -l
```

• [Reverse piping](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L291-L345)

```
wc -l = cat sample.txt
wc -l = grep -v test = cat sample.txt
```

### File Operations

• [Append files](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L353-L396)

```
file1.txt ~ file2.txt
```

• [Count words](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L402-L428)

```
# sample.txt
```

• [Concatenate files](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L434-L454)

```
file1.txt + file2.txt
file1.txt + file2.txt + file3.txt + file4.txt + file5.txt
```

### Redirection

• [I/O Redirection](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L460-L508)

```
grep hello < input.txt
ls -l > output.txt
echo "more data" >> output.txt
grep hello < input.txt > output.txt
```

### Sequential & Conditional Execution

• [Sequential execution](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L351-L358)

```
date ; pwd
date ; pwd ; ls
echo 1 ; echo 2 ; echo 3 ; echo 4
```

• [Conditional execution](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L364-L416)

```
ls && echo "Command succeeded"
ls non_existent_file && echo "This won't print"
ls && pwd && echo "Both succeeded"
ls non_existent_file || echo "Command failed"
ls || echo "This won't print"
```

## Edge Cases

```
# Empty command (just press Enter)
<Enter>

# Non-existent command
thiscommanddoesnotexist

# Non-existent file
cat nonexistent.txt

# Interrupt
Ctrl+C
```

## Build & Run

```bash
gcc -o w25shell kirtan_prajapati_110181626.c
./w25shell
```

## Test Setup

```bash
# Create test files
echo "Sample content" > sample.txt
echo "File 1" > file1.txt
echo "File 2" > file2.txt
echo "hello world test line" > input.txt
```

## ⚠️ WARNING

```bash
# MANDATORY: Run periodically to clean up processes
killall -u yourusername
```

_Failure to clean up processes may result in zero marks for the assignment._
