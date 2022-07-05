This project represents a trial to implement a RISC-V disassemble.

# Implementation:
- It depends mainly on reveiving a .bin file with the main function parameters. 
- Afterwards, the program keeps taking input and loading half words while checking whether the current instruction is a half word or not. This is done using the different in the first two bits of the two types of instructions (C, 32 bit). 
- Afterwards, the program calls the corresponding function that processes the 32-bit or the 16-bit instruction based on the output of the previous function. 
- The program begins dividing the instruction word into fields and begins to differentiate between them using fileds like (Opcode, funct2, funct3, funct4 and funct7). 
- Finally, the program outputs a certain command based on the specific case it reaches in the switch case at the end of the call tree.

# Limitations:
The program is limited only to 8KB of memory according to what is stated in the handout.

# Known Issues:
There may be some miscalculations of the immediate values in some instructions. It may be thought that this is normal since a project with this weight should occupy much more time than what was allocated for yet. Yet, there are no "crucial" or major issues with the program other than the miscalculations in some parts.

# Contributions:
- Ibrahim Gohar:
    - Developing and testing the instDecExec32() function that processes the RV32C instructions.
    - Developing Web-based UI and Deploy it online using ngrok
    - Developing the function responsible for diffrentiating between 16-bit and 32-bit instructions as well as reading them
    - Using bitmasks to load different immediate values
    - Developing function to map the register to its names
    - Developing function to Label different addresses using maps
- Moneer Maged:
    - managing the project implementation process. 
    - defining a huge map for compressed and non-compressed istruction 
    - defining tasks and assigning deadlines for them. 
    - Developing the function responsible for diffrentiating between 16-bit and 32-bit instructions.
- Abdelaaty Rehab:
    - Developing and testing the instDecExec16() function that processes the RV32C instructions.
    - Maintaining a consistent indentation style across the .cpp files.
    - Testing and fixing issues when faced during manipulating the available test cases.
    - Signing the immediate values when loaded as bytes from the input arguments and when written as output to the user. 

