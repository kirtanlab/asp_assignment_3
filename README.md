# W25Shell

A custom UNIX-like shell implementation with advanced piping and file operations.

## Core Functions

### Command Execution

• [Basic command execution](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L361-L429)

```
ls -l
pwd
echo hello world
```

### Built-in Commands

• `killterm` - [Kill current terminal](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L973-L982)

```
killterm
```

• `killallterms` - [Kill all shells](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L984-L1023)

```
killallterms
```

### Piping Operations

• [Forward piping](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L431-L518)

```
ls -l | grep .txt
ls -l | grep .txt | wc -l
ls | grep . | sort | head -n 3 | wc -l
```

• [Reverse piping](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L520-L607)

```
wc -l = cat sample.txt
wc -l = grep -v test = cat sample.txt
```

### File Operations

• [Append files](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L701-L768)

```
file1.txt ~ file2.txt
```

• [Count words](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L770-L806)

```
# sample.txt
```

• [Concatenate files](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L808-L840)

```
file1.txt + file2.txt
file1.txt + file2.txt + file3.txt + file4.txt + file5.txt
```

### Redirection

• [I/O Redirection](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L842-L916)

```
grep hello < input.txt
ls -l > output.txt
echo "more data" >> output.txt
grep hello < input.txt > output.txt
```

### Sequential & Conditional Execution

• [Sequential execution](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L609-L634)

```
date ; pwd
date ; pwd ; ls
echo 1 ; echo 2 ; echo 3 ; echo 4
```

• [Conditional execution](https://github.com/kirtanlab/asp_assignment_3/blob/main/kirtan_prajapati_110181626.c#L626-L699)

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
echo "Sample content" > sample.txt
echo "File 1" > file1.txt
echo "File 2" > file2.txt
echo "hello world test line" > input.txt
```

## ⚠️ WARNING

```bash
killall -u yourusername
```
