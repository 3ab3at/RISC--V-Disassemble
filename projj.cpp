#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <iomanip>
#include <bits/stdc++.h>
using namespace std;

unsigned int pc = 0x0;

string result = "No Results";

map<int, string> label;
int l_count = 0;

unsigned char memory[8*1024] = {NULL};	// only 8KB of memory located at address 0

string ABI[32] = {"zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

//ofstream printResult("result.txt");
bool done = false;
void emitError(char *s) {
	ofstream printResult;
	printResult.open("result.txt", ios::app);
	printResult << s;
	exit(0);
}

void printPrefix(unsigned int instA, unsigned int instW) {
	ofstream printResult;
	printResult.open("result.txt", ios::app);
	if (!done) {
		printResult << "InstAdd\t\t" << "InstHEX\t\t" << "InstAss\t" << "Parameters\n";
		done = true;
	}
	printResult << "0x" << hex << std::setfill('0') << std::setw(8) << instA << "\t0x" << std::setw(8) << instW;
}

string labelize(unsigned int imm) {
    if (label[imm] == ""){
        label[imm] = "L" + to_string(l_count);
        l_count++;
    }
    return label[imm];
}

void instDecExec32(unsigned int instWord) {
	unsigned int rd, rs1, rs2, funct3, funct7, opcode;
	unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
	unsigned int address;
	ofstream printResult;
	printResult.open("result.txt", ios::app);	
	
	unsigned int instPC = pc - 4;

	opcode = instWord & 0x0000007F;
	rd = (instWord >> 7) & 0x0000001F;
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	rs2 = (instWord >> 20) & 0x0000001F;
	funct7 = (instWord >> 25) & 0x0000007F;

	printPrefix(instPC, instWord);

	if(opcode == 0x0033) {	// R Instructions
		switch(funct3) {
			case 0: {
				if (funct7 == 32)
					printResult << "\tSUB\t\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";
				else 
					printResult << "\tADD\t\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";
				break;
			}
			case 1: { printResult << "\tSLL\t\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n"; break; }
			case 2: { printResult << "\tSLT\t\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";   break; }
		    case 3: { printResult << "\tSLTU\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";  break; }
			case 4: { printResult << "\tXOR\t\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";   break; }
			case 5: {
				if (funct7 == 32)
					printResult << "\tSRA\t\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";
				else
					printResult << "\tSRL\t\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";
				break;
			}
			case 6: { printResult << "\tOR\t\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n"; break; }
			case 7: { printResult << "\tAND\t\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n"; break; }
			default: { printResult << "\tUnknown R Instruction \n"; }
		}
	} 
	else if (opcode == 0x003A) {	// R w-based instructions
	    switch (funct3) {
	        case 0: {
				if (funct7 == 32)
					printResult << "\tSUBW\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";
				else
					printResult << "\tADDW\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";
				break;
			}
			case 1: { printResult << "\tSLLW\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n"; break;}
			case 5: {
				if (funct7 == 32)
					printResult << "\tSRAW\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";
				else
					printResult << "\tSRLW\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";
				break;
			}
			default: printResult << "\tUnknown R w-based Instruction \n";
	    }
	} 
	else if (opcode == 0x0023) {	// S instructions
	    unsigned int imm = (funct7<<5) | rd;
		string xx = to_string(imm);
		int x = stoi(xx);
		if (((instWord >> 31) & 0x0001) == 1) x -= (1 << 12);		
	    switch (funct3) {
	        case 0: { printResult << "\tSB\t\t" << ABI[rs2] << x << "(" << ABI[rs1] << ")\n"; break; }
	        case 1: { printResult << "\tSH\t\t" << ABI[rs2] << x << "(" << ABI[rs1] << ")\n"; break; }
	        case 2: { printResult << "\tSW\t\t" << ABI[rs2] << x << "(" << ABI[rs1] << ")\n"; break; }
	        case 3: { printResult << "\tSD\t\t" << ABI[rs2] << x << "(" << ABI[rs1] << ")\n"; break; }
			default: { printResult << "\tUnknown S Instruction \n"; }
	    }
	} 
	else if (opcode == 0x0063) {	// SB instructions
	    unsigned int immm = ((funct7 & 0x40) << 5) | ((rd & 0x1) << 11) | ((rd & 0x1E) << 1) | ((funct7 & 0x3F) << 5);
		string xx = to_string(immm);
		int x = stoi(xx);
		if (((instWord >> 31) & 0x0001) == 1) x -= (1 << 13);
        string imm = labelize(immm);
	    switch (funct3){
	        case 0: { printResult << "\tBEQ\t\t" << ABI[rs1] << ", " << ABI[rs2] << x << "\n"; break; }
	        case 1: { printResult << "\tBNE\t\t" << ABI[rs1] << ", " << ABI[rs2] << x << "\n"; break; }
	        case 4: { printResult << "\tBLT\t\t" << ABI[rs1] << ", " << ABI[rs2] << x << "\n"; break; }
	        case 5: { printResult << "\tBGE\t\t" << ABI[rs1] << ", " << ABI[rs2] << x << "\n"; break; }
	        case 6: { printResult << "\tBLTU\t" << ABI[rs1] << ", " << ABI[rs2] << x << "\n"; break; }
	        case 7: { printResult << "\tBGEU\t" << ABI[rs1] << ", " << ABI[rs2] << x << "\n"; break; }
			default: { printResult << "\tUnknown SB Instruction \n"; }
	    }
	} 
	else if (opcode == 0x0017) { // U instruction (auipc)
	    unsigned int imm = (funct7<<25) | (rs2<<20) | (rs1<<15) | (funct3<<12);
		string xx = to_string(imm);
		int x = stoi(xx);
		if (((instWord >> 31) & 0x0001) == 1) x -= (1 << 32);
	    printResult << "\tAUIPC\t" << ABI[rd] << x << "\n";
	} 
	else if (opcode == 0x0037) { // U instruction (lui)
	    unsigned int imm = (funct7<<25) | (rs2<<20) | (rs1<<15) | (funct3<<12);
		string xx = to_string(imm);
		int x = stoi(xx);
		if (((instWord >> 31) & 0x0001) == 1) x -= (1 << 32);
	    printResult << "\tAUIPC\t" << ABI[rd] << x << "\n";
	} 
	else if (opcode == 0x006F) {	// UJ instructions
	    unsigned int tmp = (((funct7<<25) | (rs2<<20) | (rs1<<15) | (funct3<<12))>>11);
	    unsigned int immm = (tmp & 0x100000) | ((tmp & 0x1FE)<<11) | ((tmp & 0x200)<<2) | ((tmp & 0xFFC00)>>9);
        string imm = labelize(immm);
		string xx = to_string(immm);
		int x = stoi(xx);
		if (((instWord >> 31) & 0x0001) == 1) x -= (1 << 21);
	    printResult << "\tJAL\t\t" << ABI[rd] << "\t" << x << "\n"; 
	} 
	else {	// I instructions
	    unsigned int imm = (funct7 << 5) | rs2;
		string xx = to_string(imm);
		int x = stoi(xx);
		if (((instWord >> 31) & 0x0001) == 1) x -= (1 << 12);
	    if (opcode == 0x0003) { // load instructions
	        switch (funct3) {
    	        case 0: { printResult << "\tLB\t\t" << ABI[rd] << x << "(" << ABI[rs1] << ")\n"; break; } 
    	        case 1: { printResult << "\tLH\t\t" << ABI[rd] << x << "(" << ABI[rs1] << ")\n"; break; }
    	        case 2: { printResult << "\tLW\t\t" << ABI[rd] << x << "(" << ABI[rs1] << ")\n"; break; }
    	        case 3: { printResult << "\tLD\t\t" << ABI[rd] << x << "(" << ABI[rs1] << ")\n"; break; }
    	        case 4: { printResult << "\tLBU\t\t" << ABI[rd] << x << "(" << ABI[rs1] << ")\n"; break; }
    	        case 5: { printResult << "\tLHU\t\t" << ABI[rd] << x << "(" << ABI[rs1] << ")\n"; break; }
    	        case 6: { printResult << "\tLWU\t\t" << ABI[rd] << x << "(" << ABI[rs1] << ")\n"; break; }
    			default: { printResult << "\tUnknown I Instruction \n"; }
	        }
	    } 
		else if (opcode == 0x000F) { // fence instructions
	        switch (funct3) {
    	        case 0: printResult << "\tFENCE\n";   break;
    	        case 1: printResult << "\tFENCE.I\n";   break;
    			default: printResult << "\tUnknown I Instruction \n";
	        }
	    } 
		else if (opcode == 0x0013) { // imm instructions
	        switch (funct3) {
    	        case 0: { printResult << "\tADDI\t" << ABI[rd] << ", " << ABI[rs1] << x << "\n"; break; }
    	        case 1: { printResult << "\tSLLI\t" << ABI[rd] << ", " << ABI[rs1] << x << "\n"; break; }
    	        case 2: { printResult << "\tSLTI\t" << ABI[rd] << ", " << ABI[rs1] << x << "\n"; break; }
    	        case 3: { printResult << "\tSLTIU\t" << ABI[rd] << ", " << ABI[rs1] << x << "\n"; break; }
    	        case 4: { printResult << "\tXORI\t" << ABI[rd] << ", " << ABI[rs1] << x << "\n"; break; }
    	        case 5: {
					if(funct7 == 32)
						printResult << "\tSRAI\t" << ABI[rd] << ", " << ABI[rs1] << x << "\n";
					else
						printResult << "\tSRLI\t" << ABI[rd] << ", " << ABI[rs1] << x << "\n";
    					break;
				}
    	        case 6: { printResult << "\tORI\t\t" << ABI[rd] << ", " << ABI[rs1] << x << "\n"; break; }
    	        case 7: { printResult << "\tANDI\t" << ABI[rd] << ", " << ABI[rs1] << x << "\n"; break; }
    			default: { printResult << "\tUnknown I Instruction \n"; }
	        }
        } 
		else if (opcode == 0x001B) { // w-based instructions
	    	unsigned int imm = (funct7 << 5) | rs2;
			string xx = to_string(imm);
			int x = stoi(xx);
			if (((instWord >> 31) & 0x0001) == 1) x -= (1 << 18);
            switch (funct3) {
    	        case 0: { printResult << "\tADDIW\t" << ABI[rd] << ", " << ABI[rs1] << x << "\n"; break; }
    	        case 1: { printResult << "\tSLLIW\t" << ABI[rd] << ", " << ABI[rs1] << x << "\n"; break; }
    	        case 5: {
					if(funct7 == 32)
						printResult << "\tSRAIW\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";
					else
						printResult << "\tSRLIW\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";
    					break;
				}
    			default: { printResult << "\tUnknown I Instruction \n"; }
	        }
        } 
		else if (opcode == 0x0063) { // jalr instruction
            printResult << "\tJALR\t" << ABI[rd] << ", " << ABI[rs1] << ", 0x" << imm << "\n";
        } 
		else if (opcode == 0x0073) { // enviromental instructions
			unsigned int imm = (funct7 << 5) | rs2;
			string xx = to_string(imm);
			int x = stoi(xx);
			if (((instWord >> 31) & 0x0001) == 1) x -= (1 << 12);
            switch (funct3) {
    	        case 1: { printResult << "\tCSRRW\t" << ABI[rd] << ", " << ABI[rs1] << x << "\n"; break; }
    	        case 2: { printResult << "\tCSRRS\t" << ABI[rd] << ", " << ABI[rs1] << x << "\n"; break; }
    	        case 3: { printResult << "\tCSRRC\t" << ABI[rd] << ", " << ABI[rs1] << x << "\n"; break; }
    	        case 5: { printResult << "\tCSRRWI\t" << ABI[rd] << ", 0x" << rs1 << x << "\n"; break; }
    	        case 6: { printResult << "\tCSRRSI\t" << ABI[rd] << ", 0x" << rs1 << x << "\n"; break; }
    	        case 7: { printResult << "\tCSRRCI\t" << ABI[rd] << ", 0x" << rs1 << x << "\n"; break; }
    	        case 0: {
					if(imm == 0)
						printResult << "\tECALL\n";
    				else
						printResult << "\tEBREAK\n";
					break;
				}
    			default: printResult << "\tUnknown I Instruction \n";
            }
        } 
		else {
            printResult << "\tUnknown Instruction \n";
        }
	}
}

void instDecExec16(unsigned int instWord)
{
	unsigned int rd, rs1, rs2, funct3, funct7, opcode;
	unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
	unsigned int address;

	ofstream printResult;
	printResult.open("result.txt", ios::app);

	unsigned int instPC = pc - 2;

	opcode = instWord & 0x0003;
	funct3 = (instWord >> 13) & 0x0007;
	rs1 = (instWord >> 7) & 0x0007;
	rs2 = (instWord >> 2) & 0x0007;
    rd = rs2;
	
	printPrefix(instPC, instWord);

	if(opcode == 0) {	// Q0 Instructions
		switch(funct3) {
			case 0: {
				unsigned int immedi = (instWord >> 5);
                if (immedi == 0) {printResult << "\tIllegal Instruction \n"; break;}
                unsigned int immedi2 = ((immedi & 0x0001) << 3) | ((immedi & 0x0002) << 1) | ((immedi & 0x003C) << 4) | ((immedi & 0x00C0) >> 2);
                printResult << "\tC.ADDI4SPN\t" << ABI[rd] << ", " << ABI[rs1] << ", 0x" << immedi2 << "\n";
				break;
            }
			case 2: {
				unsigned int immedi = (instWord >> 5);
				unsigned int uimm1 = ((immedi & 0x0002) << 1) | (((immedi >> 5) & 0x0007) << 3) | ((immedi & 0x0001) << 6);
				printResult << "\tC.LW\t" << ABI[rd] << ", " << ABI[rs1] << ", 0x" << uimm1 << "\n";   
				break;
			}
			case 4: {
				printResult << "Reserved memory location \n";
				break;
			}
			case 6: {
				unsigned int immedi = (instWord >> 5);
				unsigned int uimm1 = ((immedi & 0x0002) << 1) | (((immedi >> 5) & 0x0007) << 3) | ((immedi & 0x0001) << 6);
				printResult << "\tC.SW\t" << ABI[rd] << ", " << ABI[rs1] << ", 0x" << uimm1 << "\n";   
				break;
			}
			default: { printResult << "\tUnknown C.Quadrant0 Instruction \n"; }
		}
	} 
	else if (opcode == 0x0001) {	// Q1 instructions
        rd = (instWord >> 7) & 0x001F;
	    switch (funct3) {
	        case 0: {
				unsigned int immedi = (instWord >> 2);
				if (immedi == 0) 
					printResult << "C.NOP\n";
				else {
					unsigned int immedi2 = (immedi & 0x001F) | (((immedi >> 10) & 0x0001) << 5);
					string xx = to_string(immedi2);
					int x = stoi(xx);
					if (((immedi >> 10) & 0x0001) == 1) x -= (1 << 5);
					printResult << "\tC.ADDI\t" << ABI[rd] << ", " << ABI[rd] << ", 0x" << x << "\n";
				}
				break;
			}
			case 1: {
				unsigned int immediate = (((instWord >> 2) & 0x0001) << 5) | (((instWord >> 3) & 0x0007) << 1) | (((instWord >> 6) & 0x0001) << 7) | (((instWord >> 7) & 0x0001) << 6) | (((instWord >> 8) & 0x0001) << 10) | (((instWord >> 9) & 0x0003) << 8) | (((instWord >> 11) & 0x0001) << 4) | (((instWord >> 12) & 0x0001) << 11);
				printResult << "\tC.JAL\t" << immediate << "\n";
				break;
			}
            case 2: {
                unsigned int immedi = (instWord >> 2);
                unsigned int immedi2 = (immedi & 0x001F) | (((immedi >> 10) & 0x0001) << 5);
				string xx = to_string(immedi2);
				int x = stoi(xx);
				if (((immedi >> 10) & 0x0001) == 1) x -= (1 << 5);
                printResult << "\tC.LI\t" << ABI[rd] << ", 0x" << x << "\n";
                break;       
			}
            case 3: {
                unsigned int immedi = (instWord >> 2);
                if (rd == 0x0002) {
                    unsigned int immedi2 = ((immedi & 0x0001) << 5) | ((immedi & 0x0006) << 6) | ((immedi & 0x0008) << 3) | (immedi & 0x0010) | (((immedi >> 10) & 0x0001) << 9);
                    printResult << "\tC.ADDI16SP\t" << ABI[rd] << ", " << ABI[rd] << ", 0x" << immedi2 << "\n";
                } 
                else {
                    unsigned int immedi2 = ((immedi & 0x001F) << 12) | (((immedi >> 10) & 0x0001) << 17);
                    printResult << "\tC.LUI\t" << ABI[rd] << ", 0x" << immedi2 << "\n";
                }     
				break;
			}
			case 4: {
                unsigned char funct2 = (instWord >> 10) & 0x0003; 
                rd = rd & 0x000F;
                if (funct2 == 0) {
                    unsigned int immedi = (instWord >> 2);
                    unsigned int immedi2 = (immedi & 0x001F) | (((immedi >> 10) & 0x0001) << 5);
                    printResult << "\tC.SRLI\t" << ABI[rd] << ", " << ABI[rd] << ", 0x" << immedi << "\n";  
                }
                else if (funct2 == 0x1) {
                    int immedi = (instWord >> 2);
                    int immedi2 = (immedi & 0x001F) | (((immedi >> 10) & 0x0001) << 5);
                    printResult << "\tC.SRAI\t" << ABI[rd] << ", " << ABI[rd] << ", 0x" << immedi << "\n"; 
                    printResult << (int) funct2 << endl;
                }
                else if (funct2 == 0b10) {
                    unsigned int immedi = (instWord >> 2);
                    unsigned int immedi2 = (immedi & 0x001F) | (((immedi >> 10) & 0x0001) << 5);
					string xx = to_string(immedi2);
					int x = stoi(xx);
					if (((immedi >> 10) & 0x0001) == 1) x -= (1 << 5);
                    printResult << "\tC.ANDI\t" << ABI[rd] << ", " << ABI[rd] << ", 0x" << x << "\n";
                }
                else {
                    unsigned int funct6 = (instWord >> 10);
                    switch (funct6) {
                        case 0x0023: {
                            unsigned funct = (instWord >> 5) & 0x0003;
                            rd = rs1;
                            switch (funct) {
                                case 0: {
                                    printResult << "\tC.SUB\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";   
									break;
								}
                                case 1: {
                                    printResult << "\tC.XOR\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";   
									break;
								}
                                case 2: {
                                    printResult << "\tC.OR\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";   
									break;
								}
                                case 3: {
                                    printResult << "\tC.AND\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";   
									break;
								}
                            }
                            break;
                        }    
                    	default: printResult << "\t Unknown Integer Register-Register Operation \n";
                    }
                }
				break;
			}
            case 5: {
				unsigned int immediate = (((instWord >> 2) & 0x0001) << 5) | (((instWord >> 3) & 0x0007) << 1) | (((instWord >> 6) & 0x0001) << 7) | (((instWord >> 7) & 0x0001) << 6) | (((instWord >> 8) & 0x0001) << 10) | (((instWord >> 9) & 0x0003) << 8) | (((instWord >> 11) & 0x0001) << 4) | (((instWord >> 12) & 0x0001) << 11);
				printResult << "\tC.J\tx" << immediate << "\n";
                break;
			}
            case 8: {
                unsigned int immedi = (instWord >> 2);
                unsigned int immedii2 = ((immedi & 0x0001) << 5) | (immedi & 0x0006) | ((immedi & 0x0018) << 3) | (((immedi >> 8) & 0x0003) << 3) | (((immedi >> 10) & 0x0001) << 8);
                string immedi2 = labelize(immedii2);
                printResult << "\tC.BEQZ\t" << ABI[rd] << ", "  << immedi2 << "\n";   
				break; 
			}
            case 9: {
                unsigned int immedi = (instWord >> 2);
                unsigned int immedii2 = ((immedi & 0x0001) << 5) | (immedi & 0x0006) | ((immedi & 0x0018) << 3) | (((immedi >> 8) & 0x0003) << 3) | (((immedi >> 10) & 0x0001) << 8);
                string immedi2 = labelize(immedii2);
                printResult << "\tC.BNEZ\t" << ABI[rd] << ", "  << immedi2 << "\n";   
				break; 
			}
			default: { printResult << "\tUnknown R w-based Instruction \n"; }
	    }
	} 
    else if (opcode == 2) {
        unsigned int funct4 = (instWord >> 12);
        rs1 = (instWord >> 7) & 0x001F;
        rs2 = (instWord >> 2) & 0x001F;
        unsigned int immedi = instWord >> 2;
        if ((funct4 >> 1) == 0) {
            unsigned int immedi2 = (rs2) | (((immedi >> 12) & 0x0001) << 5);
			printResult << "\tC.SLLI\t" << ABI[rd] << ", " << ABI[rd] << ", 0x" << immedi2 << "\n";
        }
		else if ((funct4 >> 1) == 2) {
			unsigned int immedi2 = ((immedi & 0x0003) << 6) | (immedi & 0x001C) | (((immedi >> 10) & 0x0001) << 5);
			printResult << "\tC.LWSP\t" << ABI[rd] << ", " << ABI[rd] << ", 0x" << immedi2 << "\n";
		}
		else if ((funct4 >> 1) == 6) {
			unsigned int immedi2 = (((immedi >> 5) & 0x0003) << 6) | ((immedi >> 5) & 0x003C);
			printResult << "\tC.SWSP\t" << ABI[rs2] << ", " << ABI[rs2] << ", 0x" << immedi2 << "\n";
		}
		else { 
			switch (funct4) {
				case 8: {
					if (rs2 == 0)
						printResult << "\tC.JR\t" << ABI[rd] << ", " << ABI[rs1] << "\n";
					else
						printResult << "\tC.MV\t" << ABI[rd] << ", " << ABI[rs1] << "\n";
					break;
				}
				case 9: {
					if (rs1 == 0 && rs2 == 0)
						printResult << "\tC.EBREAK\n";
					else if (rs2 == 0)
						printResult << "\tC.JALR\t" << ABI[rd] << ", " << ABI[rs1] << "\n";
					else 
						printResult << "\tC.ADD\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";
					break;
				}
				default: { printResult << "\tUnknown Control Transfer Instruction \n"; }
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

void get_code(char *filename) {

	unsigned int instWord = 0;
	ifstream inFile;
	ofstream outFile;

	inFile.open(filename, ios::in | ios::binary | ios::ate);

	if(inFile.is_open()) {
		int fsize = inFile.tellg();

		inFile.seekg (0, inFile.beg);
		if (!inFile.read((char *)memory, fsize)) emitError("Cannot read from input file\n");
		while(true) { 
			int bytes = read_inst(instWord);			
			if(bytes == 4) instDecExec32(instWord);
			else if (bytes == 2) instDecExec16(instWord);
            if(pc == fsize) break;
		}
	} 
	else emitError("Cannot access input file\n");
}   

int main() {
	ofstream printResult("result.txt");
	string filename = "test1.bin";
	cin >> filename;
	char *path = new char [filename.length()+1];
    strcpy (path, filename.c_str());
	get_code(path);
	return 0;
}