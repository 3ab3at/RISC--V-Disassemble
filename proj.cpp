#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <iomanip>
#include <bits/stdc++.h>
using namespace std;

unsigned int pc = 0x0;

unsigned char memory[8*1024] = {NULL};	// only 8KB of memory located at address 0

void emitError(char *s) {
	cout << s;
	exit(0);
}

int f(unsigned x)
{
    if (x <= INT_MAX)
        return static_cast<int>(x);

    if (x >= INT_MIN)
        return static_cast<int>(x - INT_MIN) + INT_MIN;

    throw x; // Or whatever else you like
}

void printPrefix(unsigned int instA, unsigned int instW) {
	cout << "0x" << hex << std::setfill('0') << std::setw(8) << instA << "\t0x" << std::setw(8) << instW;
}

// Function to handle the output of the 32bit instructions
void instDecExec32(unsigned int instWord) {
	unsigned int rd, rs1, rs2, funct3, funct7, opcode;
	unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
	unsigned int address;

	unsigned int instPC = pc - 4;

	//Generating the required fields for the instruction
	opcode = instWord & 0x0000007F;
	rd = (instWord >> 7) & 0x0000001F;
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	rs2 = (instWord >> 20) & 0x0000001F;
	funct7 = (instWord >> 25) & 0x0000007F;

	printPrefix(instPC, instWord);

	if(opcode == 0x0033) {	// R Instructions
		switch(funct3) {
			case 0: {		//Compare using Func3
				if (funct7 == 32)
					cout << "\tSUB\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
				else 
					cout << "\tADD\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
				break;
			}
			case 1: { cout << "\tSLL\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n"; break; }
			case 2: { cout << "\tSLT\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";   break; }
			case 3: { cout << "\tSLTU\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";  break; }
			case 4: { cout << "\tXOR\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";   break; }
			case 5: {
				if (funct7 == 32)
					cout << "\tSRA\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
				else
					cout << "\tSRL\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
				break;
			}
			case 6: { cout << "\tOR\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n"; break; }
			case 7: { cout << "\tAND\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n"; break; }
			default: { cout << "\tUnkown R Instruction \n"; }
		}
	} 
	else if (opcode == 0x003A) {	// R w-based instructions
	    switch (funct3) {		//Compare using Funct3
	        case 0: {
				if (funct7 == 32)
					cout << "\tSUBW\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
				else
					cout << "\tADDW\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
				break;
			}
			case 1: { cout << "\tSLLW\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n"; break;}
			case 5: {
				if (funct7 == 32)
					cout << "\tSRAW\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
				else
					cout << "\tSRLW\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
				break;
			}
			default: cout << "\tUnkown R w-based Instruction \n";
	    }
	} 
	else if (opcode == 0x0023) {	// S instructions
	    unsigned int imm = (funct7<<5) | rd;
	    switch (funct3) {		//Compare using Funct3
	        case 0: { cout << "\tSB\tx" << rs2 << ", " << imm << "(x" << rs1 << ")\n"; break; }
	        case 1: { cout << "\tSH\tx" << rs2 << ", " << imm << "(x" << rs1 << ")\n"; break; }
	        case 2: { cout << "\tSW\tx" << rs2 << ", " << imm << "(x" << rs1 << ")\n"; break; }
	        case 3: { cout << "\tSD\tx" << rs2 << ", " << imm << "(x" << rs1 << ")\n"; break; }
			default: { cout << "\tUnkown S Instruction \n"; }
	    }
	} 
	else if (opcode == 0x0063) {	// SB instructions
	    unsigned int imm = ((funct7 & 0x40) << 5) | ((rd & 0x1) << 11) | ((rd & 0x1E) << 1) | ((funct7 & 0x3F) << 5);
	    switch (funct3){			//Compare using Funct3
	        case 0: { cout << "\tBEQ\tx" << rs1 << ", x" << rs2 << ", 0x" << imm << "\n"; break; }
	        case 1: { cout << "\tBNE\tx" << rs1 << ", x" << rs2 << ", 0x" << imm << "\n"; break; }
	        case 4: { cout << "\tBLT\tx" << rs1 << ", x" << rs2 << ", 0x" << imm << "\n"; break; }
	        case 5: { cout << "\tBGE\tx" << rs1 << ", x" << rs2 << ", 0x" << imm << "\n"; break; }
	        case 6: { cout << "\tBLTU\tx" << rs1 << ", x" << rs2 << ", 0x" << imm << "\n"; break; }
	        case 7: { cout << "\tBGEU\tx" << rs1 << ", x" << rs2 << ", 0x" << imm << "\n"; break; }
			default: { cout << "\tUnkown SB Instruction \n"; }
	    }
	} 
	else if (opcode == 0x0017) { 	// U instruction (auipc)
	    unsigned int imm = (funct7<<25) | (rs2<<20) | (rs1<<15) | (funct3<<12);
	    cout << "\tAUIPC\tx" << rd << ", 0x" << imm << "\n";
	} 
	else if (opcode == 0x0037) { 	// U instruction (lui)
	    unsigned int imm = (funct7<<25) | (rs2<<20) | (rs1<<15) | (funct3<<12);
	    cout << "\tAUIPC\tx" << rd << ", 0x" << imm << "\n";
	} 
	else if (opcode == 0x006F) {	// UJ instructions
	    unsigned int tmp = (((funct7<<25) | (rs2<<20) | (rs1<<15) | (funct3<<12))>>11);
	    unsigned int imm = (tmp & 0x100000) | ((tmp & 0x1FE)<<11) | ((tmp & 0x200)<<2) | ((tmp & 0xFFC00)>>9);
	    cout << "\tJAL\tx" << rd << ", 0x" << imm << "\n"; 
	} 
	else {	// I instructions
	    unsigned int imm = (funct7 << 5) | rs2;
	    if (opcode == 0x0003) { 	// load instructions
	        switch (funct3) {		//Compare using funct3
    	        case 0: { cout << "\tLB\tx" << rd << ", " << imm << "(x" << rs1 << ")\n"; break; } 
    	        case 1: { cout << "\tLH\tx" << rd << ", " << imm << "(x" << rs1 << ")\n"; break; }
    	        case 2: { cout << "\tLW\tx" << rd << ", " << imm << "(x" << rs1 << ")\n"; break; }
    	        case 3: { cout << "\tLD\tx" << rd << ", " << imm << "(x" << rs1 << ")\n"; break; }
    	        case 4: { cout << "\tLBU\tx" << rd << ", " << imm << "(x" << rs1 << ")\n"; break; }
    	        case 5: { cout << "\tLHU\tx" << rd << ", " << imm << "(x" << rs1 << ")\n"; break; }
    	        case 6: { cout << "\tLWU\tx" << rd << ", " << imm << "(x" << rs1 << ")\n"; break; }
    			default: { cout << "\tUnkown I Instruction \n"; }
	        }
	    } 
		else if (opcode == 0x000F) { // fence instructions
	        switch (funct3) {
    	        //case 0: cout << "\tFENCE\tx" << rs1 << ", x" << rs2 << ", 0x" << imm << "\n";   break;
    	        case 1: cout << "\tFENCE.I\n";   break;
    			default: cout << "\tUnkown I Instruction \n";
	        }
	    } 
		else if (opcode == 0x0013) { // imm instructions
	        switch (funct3) { //Compare using funct3
    	        case 0: { cout << "\tADDI\tx" << rd << ", x" << rs1 << ", 0x" << imm << "\n"; break; }
    	        case 1: { cout << "\tSLLI\tx" << rd << ", x" << rs1 << ", 0x" << imm << "\n"; break; }
    	        case 2: { cout << "\tSLTI\tx" << rd << ", x" << rs1 << ", 0x" << imm << "\n"; break; }
    	        case 3: { cout << "\tSLTIU\tx" << rd << ", x" << rs1 << ", 0x" << imm << "\n"; break; }
    	        case 4: { cout << "\tXORI\tx" << rd << ", x" << rs1 << ", 0x" << imm << "\n"; break; }
    	        case 5: {
					if(funct7 == 32)
						cout << "\tSRAI\tx" << rd << ", x" << rs1 << ", 0x" << imm << "\n";
					else
						cout << "\tSRLI\tx" << rd << ", x" << rs1 << ", 0x" << imm << "\n";
    					break;
				}
    	        case 6: { cout << "\tORI\tx" << rd << ", x" << rs1 << ", 0x" << imm << "\n"; break; }
    	        case 7: { cout << "\tANDI\tx" << rd << ", x" << rs1 << ", 0x" << imm << "\n"; break; }
    			default: { cout << "\tUnkown I Instruction \n"; }
	        }
        } 
		else if (opcode == 0x001B) { // w-based instructions
            switch (funct3) {
    	        case 0: { cout << "\tADDIW\tx" << rd << ", x" << rs1 << ", 0x" << imm << "\n"; break; }
    	        case 1: { cout << "\tSLLIW\tx" << rd << ", x" << rs1 << ", 0x" << imm << "\n"; break; }
    	        case 5: {
					if(funct7 == 32)
						cout << "\tSRAIW\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
					else
						cout << "\tSRLIW\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
    					break;
				}
    			default: { cout << "\tUnkown I Instruction \n"; }
	        }
        } 
		else if (opcode == 0x0063) { // jalr instruction
            cout << "\tJALR\tx" << rd << ", x" << rs1 << ", 0x" << imm << "\n";
        } 
		else if (opcode == 0x0073) { // enviromental instructions
            switch (funct3) {
    	        case 1: { cout << "\tCSRRW\tx" << rd << ", x" << rs1 << ", 0x" << imm << "\n"; break; }
    	        case 2: { cout << "\tCSRRS\tx" << rd << ", x" << rs1 << ", 0x" << imm << "\n"; break; }
    	        case 3: { cout << "\tCSRRC\tx" << rd << ", x" << rs1 << ", 0x" << imm << "\n"; break; }
    	        case 5: { cout << "\tCSRRWI\tx" << rd << ", 0x" << rs1 << ", 0x" << imm << "\n"; break; }
    	        case 6: { cout << "\tCSRRSI\tx" << rd << ", 0x" << rs1 << ", 0x" << imm << "\n"; break; }
    	        case 7: { cout << "\tCSRRCI\tx" << rd << ", 0x" << rs1 << ", 0x" << imm << "\n"; break; }
    	        case 0: {
					if(imm == 0)
						cout << "\tECALL\n";
    				else
						cout << "\tEBREAK\n";
					break;
				}
    			default: cout << "\tUnkown I Instruction \n";
            }
        } 
		else {
            cout << "\tUnkown Instruction \n";
        }
	}
}

//Function to handle the 16bit instructions
void instDecExec16(unsigned int instWord)
{
	unsigned int rd, rs1, rs2, funct3, funct7, opcode;
	unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
	unsigned int address;

	unsigned int instPC = pc - 2;

	//Generatig the required fields for the instruction
	opcode = instWord & 0x0003;
	funct3 = (instWord >> 13) & 0x0007;
	rs1 = (instWord >> 7) & 0x0007;
	rs2 = (instWord >> 2) & 0x0007;
    rd = rs2;
	
	printPrefix(instPC, instWord);

	if(opcode == 0) {	// Q0 Instructions
		switch(funct3) {  //Compare using Funct3
			case 0: {
				unsigned int immedi = (instWord >> 5);
                if (immedi == 0) {cout << "\tIllegal Instruction \n"; break;}
                unsigned int immedi2 = ((immedi & 0x0001) << 3) | ((immedi & 0x0002) << 1) | ((immedi & 0x003C) << 4) | ((immedi & 0x00C0) >> 2);
                cout << "\tC.ADDI4SPN\tx" << rd << ", x" << rs1 << ", " << immedi2 << "\n";
				break;
            }
			case 2: {
				unsigned int immedi = (instWord >> 5);
				unsigned int uimm1 = ((immedi & 0x0002) << 1) | (((immedi >> 5) & 0x0007) << 3) | ((immedi & 0x0001) << 6);
				cout << "\tC.LW\tx" << rd << ", x" << rs1 << ", " << uimm1 << "\n";   
				break;
			}
			case 6: {
				unsigned int immedi = (instWord >> 5);
				unsigned int uimm1 = ((immedi & 0x0002) << 1) | (((immedi >> 5) & 0x0007) << 3) | ((immedi & 0x0001) << 6);
				cout << "\tC.SW\tx" << rd << ", x" << rs1 << ", " << uimm1 << "\n";   
				break;
			}
			default: { cout << "\tUnkown C.Quadrant0 Instruction \n"; }
		}
	} 
	else if (opcode == 0x0001) {	// Q1 instructions
        rd = (instWord >> 7) & 0x001F;
	    switch (funct3) {		//Compare using Funct3
	        case 0: {
				unsigned int immedi = (instWord >> 2);
				if (immedi == 0) 
					cout << "C.NOP\n";
				else {
					int immedi2 = (immedi & 0x001F) | (((immedi >> 10) & 0x0001) << 5);
					cout << "\tC.ADDI\tx" << rd << ", x" << rd << ", " << immedi2 << "\n";
				}
				break;
			}
			case 1: {
				// COPY HERE TO THE NEW CODE
				unsigned int immediate = (((instWord >> 2) & 0x0001) << 5) | (((instWord >> 3) & 0x0007) << 1) | (((instWord >> 6) & 0x0001) << 7) | (((instWord >> 7) & 0x0001) << 6) | (((instWord >> 8) & 0x0001) << 10) | (((instWord >> 9) & 0x0003) << 8) | (((instWord >> 11) & 0x0001) << 4) | (((instWord >> 12) & 0x0001) << 11);
				cout << "\tC.JAL\tx" << immediate << "\n";
			}
            case 2: {
                unsigned int immedi = (instWord >> 2);
                int immedi2 = (immedi & 0x001F) | (((immedi >> 10) & 0x0001) << 5);
                cout << "\tC.LI\tx" << rd << ", " << immedi2 << "\n";
                break;       
			}
            case 3: {
                unsigned int immedi = (instWord >> 2);
                if (rd == 0x0002) {
                    unsigned int immedi2 = ((immedi & 0x0001) << 5) | ((immedi & 0x0006) << 6) | ((immedi & 0x0008) << 3) | (immedi & 0x0010) | (((immedi >> 10) & 0x0001) << 9);
                    cout << "\tC.ADDI16SP\tx" << rd << ", x" << rd << ", " << immedi2 << "\n";
                } 
                else {
                    unsigned int immedi2 = ((immedi & 0x001F) << 12) | (((immedi >> 10) & 0x0001) << 17);
					// HERE COPY TO THE NEW CODE
					string xx = to_string(immedi2);
					int x = stoi(xx);
					if (((immedi >> 10) & 0x0001) == 1) x -= (1 << 18);
                    cout << "\tC.LUI\tx" << rd << ", " << to_string(x) << "\n";
                }     
				break;
			}
			case 4: {
                unsigned char funct2 = (instWord >> 10) & 0x0003; 
                rd = rd & 0x000F;
                if (funct2 == 0) {
                    unsigned int immedi = (instWord >> 2);
                    unsigned int immedi2 = (immedi & 0x001F) | (((immedi >> 10) & 0x0001) << 5);
                    cout << "\tC.SRLI\tx" << rd << ", x" << rd << ", " << immedi << "\n";  
                }
                else if (funct2 == 0x1) {
                    int immedi = (instWord >> 2);
                    int immedi2 = (immedi & 0x001F) | (((immedi >> 10) & 0x0001) << 5);
                    cout << "\tC.SRAI\tx" << rd << ", x" << rd << ", " << immedi << "\n"; 
                    cout << (int) funct2 << endl;
                }
                else if (funct2 == 0b10) {
                    unsigned int immedi = (instWord >> 2);
                    unsigned int immedi2 = (immedi & 0x001F) | (((immedi >> 10) & 0x0001) << 5);
                    cout << "\tC.ANDI\tx" << rd << ", x" << rd << ", " << immedi << "\n";
                }
                else {
                    unsigned int funct6 = (instWord >> 10);
                    switch (funct6) {    // More info needed using Funct6
                        case 0x0023: {
                            unsigned funct = (instWord >> 5) & 0x0003;
                            rd = rs1;
                            switch (funct) {
                                case 0: {
                                    cout << "\tC.SUB\tx" << rd << ", x" << rs1 << ", " << rs2 << "\n";   
									break;
								}
                                case 1: {
                                    cout << "\tC.XOR\tx" << rd << ", x" << rs1 << ", " << rs2 << "\n";   
									break;
								}
                                case 2: {
                                    cout << "\tC.OR\tx" << rd << ", x" << rs1 << ", " << rs2 << "\n";   
									break;
								}
                                case 3: {
                                    cout << "\tC.AND\tx" << rd << ", x" << rs1 << ", " << rs2 << "\n";   
									break;
								}
                            }
                            break;
                        }    
                    	default: cout << "\t Unknown Integer Register-Register Operation \n";
                    }
                }
				break;
			}
            case 5: {
				unsigned int immediate = (((instWord >> 2) & 0x0001) << 5) | (((instWord >> 3) & 0x0007) << 1) | (((instWord >> 6) & 0x0001) << 7) | (((instWord >> 7) & 0x0001) << 6) | (((instWord >> 8) & 0x0001) << 10) | (((instWord >> 9) & 0x0003) << 8) | (((instWord >> 11) & 0x0001) << 4) | (((instWord >> 12) & 0x0001) << 11);
				cout << "\tC.J\tx" << immediate << "\n";
                break;
			}
            case 8: {
                unsigned int immedi = (instWord >> 2);
                unsigned int immedi2 = ((immedi & 0x0001) << 5) | (immedi & 0x0006) | ((immedi & 0x0018) << 3) | (((immedi >> 8) & 0x0003) << 3) | (((immedi >> 10) & 0x0001) << 8);
                cout << "\tC.BEQZ\tx" << rd << ", "  << immedi2 << "\n";   
				break; 
			}
            case 9: {
                unsigned int immedi = (instWord >> 2);
                unsigned int immedi2 = ((immedi & 0x0001) << 5) | (immedi & 0x0006) | ((immedi & 0x0018) << 3) | (((immedi >> 8) & 0x0003) << 3) | (((immedi >> 10) & 0x0001) << 8);
                cout << "\tC.BNEZ\tx" << rd << ", "  << immedi2 << "\n";   
				break; 
			}
			default: { cout << "\tUnkown R w-based Instruction \n"; }
	    }
	} 
    else if (opcode == 2) {    //Q2 instructions
        unsigned int funct4 = (instWord >> 12);
        rs1 = (instWord >> 7) & 0x001F;
        rs2 = (instWord >> 2) & 0x001F;
        unsigned int immedi = instWord >> 2;    //Compare using Funct3 at first
        if ((funct4 >> 1) == 0) {
            unsigned int immedi2 = (rs2) | (((immedi >> 12) & 0x0001) << 5);
			cout << "\tC.SLLI\tx" << rd << ", x" << rd << ", " << immedi2 << "\n";
        }
		else if ((funct4 >> 1) == 2) {
			unsigned int immedi2 = ((immedi & 0x0003) << 6) | (immedi & 0x001C) | (((immedi >> 10) & 0x0001) << 5);
			cout << "\tC.LWSP\tx" << rd << ", x" << rd << ", " << immedi2 << "\n";
		}
		else if ((funct4 >> 1) == 6) {
			unsigned int immedi2 = (((immedi >> 5) & 0x0003) << 6) | ((immedi >> 5) & 0x003C);
			cout << "\tC.SWSP\tx" << rs2 << ", x" << rs2 << ", " << immedi2 << "\n";
		}
		else { 						//Use funct4 if the above cases are not met
			switch (funct4) {
				case 8: {
					if (rs2 == 0)
						cout << "\tC.JR\tx" << ", x" << rs1 << "\n";
					else
						cout << "\tC.MV\tx" << ", x" << rd << ", x" << rs1 << "\n";
					break;
				}
				case 9: {
					if (rs1 == 0 && rs2 == 0)
						cout << "\tC.EBREAK\n";
					else if (rs2 == 0)
						cout << "\tC.JALR\tx" << ", x" << rs1 << "\n";
					else 
						cout << "\tC.ADD\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
					break;
				}
				default: { cout << "\tUnknown Control Transfer Instruction \n"; }
			}
		}
    }
}

int read_inst(unsigned int &instWord) {
    // check whether the instruction line is compressed or not
    if ((memory[pc] & 0x0003) == 3) {   
        instWord = 	(unsigned char)memory[pc] | (((unsigned char)memory[pc + 1]) << 8) | (((unsigned char) memory[pc + 2]) << 16) | (((unsigned char) memory[pc + 3]) << 24);
    	pc += 4;
    	return 4;
    } 
	else {
        instWord = 	(unsigned char) memory[pc] | (((unsigned char) memory[pc + 1]) << 8);
    	pc += 2;
    	return 2;
    }
    return 0;
}

int main(int argc, char *argv[]) {

	unsigned int instWord = 0;
	ifstream inFile;
	ofstream outFile;

	if (argc < 2) emitError("use: rvcdiss <machine_code_file_name>\n");

	inFile.open(argv[1], ios::in | ios::binary | ios::ate);

	if(inFile.is_open()) {
		int fsize = inFile.tellg();

		inFile.seekg (0, inFile.beg);
		if (!inFile.read((char *)memory, fsize)) emitError("Cannot read from input file\n");
		while(true) { 
			int bytes = read_inst(instWord);				//load the next instruction and find its size in the memory
			if(bytes == 4) instDecExec32(instWord);			//handle the non compressed instructions
			else if (bytes == 2) instDecExec16(instWord);	//handle the compressed instructions
            if(pc == fsize) break;							// stop when PC reached address 32
		}
	} 
	else emitError("Cannot access input file\n");	
}   