GCPUA
=====
This is an assembler I wrote for the Gator-CPU that is used in EEL 3701 at the University of Florida. This program takes a text file of G-CPU assembly instructions and translates it into a .mif rom file filled with all the correct opcodes. I wrote this to simplify the creation of programs for the G-CPU.

NOTE: I would not trust this assembler with my Lab/Homework problems. Always be sure to check the output of the assembler.

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
for binary numbers will cause an error in assembling.

The directive ORG can be used, but be careful not to cause two instructions to
write to the same place in memory. The program should inform you of the error
when it occurs. Directives EQU, DS.B, and DC.B are also available.

Comments can only be written. Semicolons delineate inline-comments
and an * denotes a comment line. Note that if you create a line comment that
isn't immediately followed by an instruction it will not appear in the output mif file.

COMPILATION:
============
  On *nix machines, you can run "make" to compile the source code. Your
  compiler will need to support the C++11 standard for it to compile.

  On Windows, you'll need to compile all the source files yourself. The code
  should compile & run on Windows, but I haven't tested it. Make sure your
  compiler supports the C++11 standard. I've good luck with the MSVC compiler.

RUNNING:
========
  To run the compiled Gator-CPU assembler you'll need to supply 3 things.
  A file with your assembly code, a filename for the generated rom contents, and
  the size of your rom (base 10). You can then run the program ...
  "./gcpua exampleAssembly.txt out.mif 1024"

TEST:
=====
  You can also run "make test" to run a test assemble.

