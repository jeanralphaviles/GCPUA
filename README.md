GCPUA
=====
This is an assembler I wrote for the Gator-CPU that is used in EEL 3701 at the University of Florida. This program takes a text file of G-CPU assembly instructions and translates it into a .mif rom file filled with all the correct opcodes. I wrote this to simplify the creation of programs for the G-CPU.

NOTE_1: I would NEVER trust this assembler with my Lab/Homework problems. With all the hacks I mentioned earlier. I guarantee there's some scary bug hiding somewhere waiting to pounce, like a spider sitting in it's web. I would triple check the output of this program every time I used it.

NOTE_2: I reckon most of the normal G-CPU instructions will work 99% of the time, but it's the assembler directives (ORG, EQU, DS.B, ..) that will probably end up causing the most problems. Only ORG is really working as it should, EQU will work but it can cause labels to appear in in the comments of the .mif file in the wrong spots, and the other directives aren't supported at all.

TL;DR - There's bugs and ORG and EQU are the only directives that work.

FORMAT GUIDE:
=============
Instructions are written one per line, with an optional label at the start of the line followed by a colon. Tokens should be separated by spaces.
Ex. LDAB 1, X and END: BEQ END are valid lines, but
COMA SUM_AB and LDAB#00 are not.

Labels can also be written on their own line.
SUM: LDAB #00
is equivalent to
SUM:
LDAB #00

All numbers you write in the assembly code are interpreted as hexadecimal, all
'$'s you write in front of numbers should be ignored. But I wouldn't count on it,
I would just not write them to be safe. Writing any other symbol, such as a '@'
for binary numbers will break the code and cause a problems.

The directive ORG can be used, but be careful not to cause two instructions to
write to the same place in memory. The program should inform you of the error
when it occurs, but just be careful.

Comments can only be put after an instruction. Semicolons delineate inline-comments
and an * denotes a comment line.

COMPILATION:
============
  On *nix machines, you can run "make" to compile the source code. Your
  compiler will need to support the C++11 standard for it to work.

  On Windows, you'll need to compile all the source files yourself. The code
  should compile & run on Windows, but I haven't tested it. Make sure your
  compile supports the C++11 standard.

RUNNING:
========
  To run the compiled Gator-CPU assembler you'll need to supply 3 things.
  A file with your assembly code, a filename for the generated rom contents, and
  the size of your rom (base 10). You can then run the program ...
  "./gcpua exampleAssembly.txt out.mif 1024"

TEST:
=====
  You can also run "make test" to run a test assemble.

