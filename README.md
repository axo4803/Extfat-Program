# Extfat utils is a utility to manipulate extfat images.

### Command Options:
```
-i {inputFileName}
        To specify the input file

-o {outputFileName}
        To specify the output file

-h
        Print Help

-c
        Copy (use with -o)

-v
        Verify Checksum

-D {targetFile}
        Delete targetFile in exFAT

-x {targetFile}
        Extract targetFile in exFAT (use with -o)

-d
        Print Directory
```
{ }: *required argument*

Additional Information:
* If -o is not specified, then the program will assume the output file is the same as the input file
* If -x and -c are both active, the program will only execute -c
* -x and -D do not work on directories

### Example Invocations:
```
./extfat -h
./extfat -i test.image -o test2.image -c
./extfat -i test.image -o test2.image -v
./extfat -i test.image -o test2.image -d
./extfat -i test.image -o test2.image -x test.image
./extfat -i test.image -c -v -d -o test2.image
./extfat -i test.image -D file.txt
./extfat -i test.image -v -d -x file.txt -o output.txt -D file.txt
```
## Internals

### Build Process
```bash
% make clean
% make

To run the tests
% make clean
% make
% make tests

```

### Examples
There is a directory of examples.  They are compiled by default using the makefile
Before you can run the example programs, you need to have an file image.

```bash
% bash /examples/create_image.bash
% ./examples/fread
% ./examples/mmap
```

### References

https://pawitp.medium.com/notes-on-exfat-and-reliability-d2f194d394c2

https://learn.microsoft.com/en-gb/windows/win32/fileio/exfat-specification

https://uta.service-now.com/selfservice?id=ss_kb_article&sys_id=KB0011414

https://nemequ.github.io/munit/#download

https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html

https://www.freecodecamp.org/news/how-to-use-git-and-github-in-a-team-like-a-pro/

https://en.wikipedia.org/wiki/ExFAT

https://docs.github.com/en/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax
