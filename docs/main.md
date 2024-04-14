# kern.

Kern is a 32-bit x86 based operating system.

What does that mean? 
Well, 32-bit means that it can access $2^{32}$ different memory addresses.
x86 is the processor architecture it is built upon.

## Command Line
Kern uses a shell (for now), so you'll have to type commands to do stuff.
It is currently quite simple. It supports these commands:

- clear: Clears the screen
- disk \<r/w> \<sector> [data]: Reads/Writes data from/to sector on disk
- echo \<msg>: Prints msg to the screen
- exit: Exits the shell
- help: Prints all available commands
- ls: Prints all files
- reboot: Reboots the system
- test: Tests the newest feature _Debug_

You can also print/run files by typing their filename (More in **Filesystem**)

## Filesystem
Kern has a custom filesystem. This filesystem works as a simple filetable.
It doesn't support directories at the moment.
Here's a list of supported filetypes:

| Type | Description | CLI Behaivour |
| ---- | ----------- | ------------- |
| .bin | Executable binary | Executes file |
| .txt | Raw ASCII text file | Prints out file content |
| .tab | Filetable format | Prints files in filetable |
| .dat | Data Binary | Prints HEX content of file |
| .asm | Assembly source code | Prints out file content |

### Filetable format 
The filetable format (.tab) is a binary file containing

| Byte(s) | Content  |
| ------- | -------- |
| 0-9     | Filename |
| 10-12   | Filetype |
| 13      | Number of filetable entries |
| 14      | Starting sector |
| 15      | File Length (in sectors) |

