// convert Hack Machine Code into Assembly Language
#include "iobuffer.h"
#include "symbols.h"
#include "tokeniser-d.h"
#include <vector>
#include <algorithm>
#include <set>
// to make out programs a bit neater
using namespace std ;

using namespace CS_IO_Buffers ;
using namespace CS_Symbol_Tables ;
using namespace Hack_Disassembler ;

// This program must disassemble Hack Machine Code and output the equivalent Hack Assembly Language.
//
// A-instructions must be translated into symbolic form using the following rules:
//
// 1. IF symbols is false
//    THEN
//     - the A-instruction is output in numeric form.
//
// 2. IF an A-instruction is followed by a C-instruction and the jump bits of the C-instruction are not "000"
//    THEN
//     - IF the value of the A-instruction is the address an instruction in the program
//       THEN
//        - the instruction must be allocated a label and the A-instruction must use this label
//        - the first labelled location in memory must be labelled L0, the next L1 and so on
//
// 3. IF an A-instruction is followed by a C-instruction that reads or writes memory
//    THEN
//     - IF the address has a predefined name
//       THEN
//        - the A-instruction must use the predefined name
//        - where there is a choice, use SP, LCL, ARG, THIS and THAT in preference to R0, R1, R2 R3 and R4
//    - IF the address is the address of an existing variable location
//      THEN
//       - the A-instructon must use the variable's name
//    - IF the address is the address of the next free variable location
//      THEN
//       - the A-instructon must use the next free variable's name and
//       - the address of the next free variable must be incremented by 1
//       - the first variable name allocated will be for address 16 and will be called v_0
//       - the second variable name allocated will be for address 17 and will be called v_1
//       - and so on
//
// 4. ELSE the A-instruction is output in numeric form.
//
// C-instructions must be translated using the following rules:
//
// 1. IF the destination bits are "000"
//    THEN
//     - do not output a destination component for the C-instruction
//
// 2. IF the ALU operation is not recognised
//   THEN
//    - replace the ALU operation with: "< ** UNDEFINED ALU OPERATION ** >"
//
// Do not output a jump component for a C-instruction if it the jump bits are "000"
// 3. IF the jump bits are "000"
//    THEN
//     - do not output a jump componentfor the C-instruction
//

// Hack Machine Code:
// program  ::= (instr tk_eol?)* tk_eoi
// instr    ::= tk_a_instr | tk_c_instr

// parse Hack Machine Code and return a vector of tokens, one per instruction
vector<Token> parse_program()
{
    vector<Token> instructions ;

    while ( have(tk_instruction) )
    {
        instructions.push_back(mustbe(tk_instruction)) ;
        if ( have(tk_eol) ) next_token() ;
    }
    mustbe(tk_eoi) ;
    return instructions ;
}

/*****************   REPLACE THE FOLLOWING CODE  ******************/

//declare some gloabl variables (table-name)
static symbols dest_bits;
static symbols alu_op_bits;
static symbols jump_bits;
static symbols pre_defined;
static symbols label_table = create_strings();

//the vector to store the label value(e.g: @12), store 12 in vector
vector <int> label_value;

//declare three methods
void initialise_tables();

static void disassembler_c(Token instructions);

static void creat_label_table(Token instruction, vector<int> label_value);

//Initialise the look-up table
void initialise_tables()
{
    //the predefined symbols
    pre_defined = create_strings();
    insert_strings(pre_defined, "0", "SP");
    insert_strings(pre_defined, "1", "LCL");
    insert_strings(pre_defined, "2", "ARG");
    insert_strings(pre_defined, "3", "THIS");
    insert_strings(pre_defined, "4", "THAT");
    insert_strings(pre_defined, "16384", "SCREEN");
    insert_strings(pre_defined, "24576", "KBD");
    
    //the three part below belongs to c-instruction
    //the alu operation table(abit + alu-op)
    alu_op_bits = create_strings();
    insert_strings(alu_op_bits,"0101010","0");
    insert_strings(alu_op_bits,"0111111","1");
    insert_strings(alu_op_bits,"0111010","-1");
    insert_strings(alu_op_bits,"0001100","D");
    insert_strings(alu_op_bits,"0110000","A");
    insert_strings(alu_op_bits,"0001101","!D");
    insert_strings(alu_op_bits,"0110001","!A");
    insert_strings(alu_op_bits,"0001111","-D");
    insert_strings(alu_op_bits,"0110011","-A");
    insert_strings(alu_op_bits,"0011111","D+1");
    insert_strings(alu_op_bits,"0110111","A+1");
    insert_strings(alu_op_bits,"0001110","D-1");
    insert_strings(alu_op_bits,"0110010","A-1");
    insert_strings(alu_op_bits,"0000010","D+A");
    insert_strings(alu_op_bits,"0010011","D-A");
    insert_strings(alu_op_bits,"0000111","A-D");
    insert_strings(alu_op_bits,"0000000","D&A");
    insert_strings(alu_op_bits,"0010101","D|A");
    insert_strings(alu_op_bits,"1110000","M");
    insert_strings(alu_op_bits,"1110001","!M");
    insert_strings(alu_op_bits,"1110011","-M");
    insert_strings(alu_op_bits,"1110111","M+1");
    insert_strings(alu_op_bits,"1110010","M-1");
    insert_strings(alu_op_bits,"1000010","D+M");
    insert_strings(alu_op_bits,"1010011","D-M");
    insert_strings(alu_op_bits,"1000111","M-D");
    insert_strings(alu_op_bits,"1000000","D&M");
    insert_strings(alu_op_bits,"1010101","D|M");
    //the destination table
    dest_bits = create_strings();
    insert_strings(dest_bits, "000", "NULL");
    insert_strings(dest_bits, "001", "M");
    insert_strings(dest_bits, "010", "D");
    insert_strings(dest_bits, "011", "MD");
    insert_strings(dest_bits, "100", "A");
    insert_strings(dest_bits, "101", "AM");
    insert_strings(dest_bits, "110", "AD");
    insert_strings(dest_bits, "111", "AMD");
    //the jump table
    jump_bits = create_strings();
    insert_strings(jump_bits, "000", "NULL" );
    insert_strings(jump_bits, "001", "JGT" );
    insert_strings(jump_bits, "010", "JEQ" );
    insert_strings(jump_bits, "011", "JGE" );
    insert_strings(jump_bits, "100", "JLT" );
    insert_strings(jump_bits, "101", "JNE" );
    insert_strings(jump_bits, "110", "JLE" );
    insert_strings(jump_bits, "111", "JMP" );
    
}

/*================================================================================================================*/
/*                                                 DISASSEMBLE NO SYMBOLS                                         */
/*================================================================================================================*/

// disassemble a Hack Machine Code program without using any symbols
static void disassemble_no_symbols(vector<Token> instructions)
{
    //create the look-up table
    initialise_tables();
    //using for loop to traverse the instructions stored in vector
    for (int i = 0; i < instructions.size(); ++i)
    {
        // the current instruction is a-instruction
        if(have(instructions[i], tk_a_instr))
        {
            //obtain the value of this instruction
            int a_value = token_ivalue(instructions[i]);
            //print the instruction in the standard way
            string wspace = "        ";//8 whitespace
            string a = "@";
            string result = wspace+a+to_string(a_value);
            //using the required method to print result, this method is in iob_buffer.h
            write_to_output(result+'\n');
        }
        // the current instruction is c-instruction
        else if (have(instructions[i], tk_c_instr))
        {
            //alu-operation
            string abit = token_a_bit(instructions[i]);
            string aluop = token_alu_op(instructions[i]);
            string aluopbits = abit+aluop;
            string aluopbits_result;
            //specific case: the ALU operation is not recognised, print "< ** UNDEFINED ALU OPERATION ** >"
            if(aluopbits=="1111111")
            {
                aluopbits_result = "< ** UNDEFINED ALU OPERATION ** >";
            }
            // normal case: using the alu_op_bits table to disassemble the 16-bit code
            else
            {
                aluopbits_result = lookup_strings(alu_op_bits,aluopbits);
            }
            
            //dest: using the dest_bits table to disassemble the 16-bit code
            string dest = token_dest(instructions[i]);
            string dest_result = lookup_strings(dest_bits,dest);
            
            //jump: using the jump_bits table to disassemble the 16-bit code
            string jump = token_jump(instructions[i]);
            string jump_result = lookup_strings(jump_bits, jump);
            //specific case: jump is null, ignore jump part
            if(jump_result=="NULL")
            {
                string wspace = "        ";//8 whitespace
                string result = wspace + dest_result + "=" + aluopbits_result;
                write_to_output(result+'\n');
            }
            //normal case:  jump is not null, print it.
            else
            {
                string wspace = "        ";//8 whitespace
                string result = wspace + aluopbits_result + ";" + jump_result;
                write_to_output(result+'\n');
            }
        }
    }
}

/*================================================================================================================*/
/*                                                 DISASSEMBLE SYMBOLS                                            */
/*================================================================================================================*/

//the same disassembler_c method in disassemble_no_symbols
static void disassembler_c(Token instruction)
{
    //alu-operation
    string abit = token_a_bit(instruction);
    string aluop = token_alu_op(instruction);
    string aluopbits = abit+aluop;
    string aluopbits_result;
    //specific case: the ALU operation is not recognised, print "< ** UNDEFINED ALU OPERATION ** >"
    if(aluopbits=="1111111")
    {
        aluopbits_result = "< ** UNDEFINED ALU OPERATION ** >";
    }
    // normal case: using the alu_op_bits table to disassemble the 16-bit code
    else
    {
        aluopbits_result = lookup_strings(alu_op_bits,aluopbits);
    }
    //dest: using the dest_bits table to disassemble the 16-bit code
    string dest = token_dest(instruction);
    string dest_result = lookup_strings(dest_bits,dest);
    //jump: using the jump_bits table to disassemble the 16-bit code
    string jump = token_jump(instruction);
    string jump_result = lookup_strings(jump_bits, jump);
    //specific case: jump is null, ignore jump part
    if(jump_result=="NULL")
    {
        string wspace = "        ";//8 whitespace
        string result = wspace + dest_result + "=" + aluopbits_result;
        write_to_output(result+'\n');
    }
    //normal case:  jump is not null, print it.
    else
    {
        string wspace = "        ";//8 whitespace
        string result = wspace + aluopbits_result + ";" + jump_result;
        write_to_output(result+'\n');
    }
    
}

// convert Hack Machine Code into Assembly Language
// use predefined names, generic label names and generic variable names
static void disassemble_symbols(vector<Token> instructions)
{
    //create the look-up table
    initialise_tables();
    
    //1. find the label value and store it in a vector
    for (int i = 0; i < instructions.size()-1; ++i)
    {
        if(have(instructions[i], tk_a_instr))
        {
            string abit = token_a_bit(instructions[i+1]);
            //dest: write memory
            string dest = token_dest(instructions[i+1]);
            //jump: not 000
            string jump = token_jump(instructions[i+1]);
            //label
            if((have(instructions[i], tk_a_instr)||abit!="1"||dest.back()!='1') && jump != "000")
            {
                string spelling = token_spelling(instructions[i]);
                int a_value = token_ivalue(instructions[i]);
                label_value.push_back(a_value);
            }
        }
    }
    
    //2. creat the label table and sort it
    for (int i = 0; i < instructions.size(); ++i)
    {
        creat_label_table(instructions[i],label_value);
    }
    
    //3.disassemble all kind of instructions
    for (int i = 0; i < instructions.size(); ++i)
    {
        //using set to just store the unduplicated elements
        set<int> s(label_value.begin(), label_value.end());
        //update the elements in vector, then there is no duplicated elements in vector
        label_value.assign(s.begin(), s.end());
        int a = instructions.size()-1;
        // disassemble a-instruction
        if(i<a)
        {
            // print the label(e.g: (L0))
            for (int j = 0; j < label_value.size(); ++j)
            {
                if(i==label_value[j])
                {
                    string str_label = lookup_strings(label_table, to_string(label_value[j]));
                    string a = "(";
                    string b = ")";
                    string result = a+str_label+b;
                    write_to_output(result+'\n');
                }
            }
            //get the below three values in the next instruction
            //abit: read memory
            string abit = token_a_bit(instructions[i+1]);
            //dest: write memory
            string dest = token_dest(instructions[i+1]);
            //jump: not 000
            string jump = token_jump(instructions[i+1]);
    
            //an A-instruction is followed by a C-instruction that reads or writes memory
            if (have(instructions[i], tk_a_instr)&&(abit=="1"||dest.back()=='1'))
            {
                int a_value = token_ivalue(instructions[i]);
                //predefined symbols
                if(a_value == 16384 || a_value == 24576 || a_value <= 4)
                {
                    string a_val = to_string(a_value);
                    //using the pre_defined table to disassemble the 16-bit code
                    string result = lookup_strings(pre_defined,a_val);
                    string wspace = "        ";//8 whitespace
                    string a = "@";
                    string res = wspace+a+result;
                    write_to_output(res+'\n');
                }
                //variables: different print out form
                else
                {
                    //R5 - R15:
                    if(a_value < 16 && a_value >4)//R14: 14-16=-2
                    {
                        string wspace = "        ";
                        string a = "@";
                        string b = "R";
                        string result = wspace+a+b+to_string(a_value);
                        write_to_output(result+'\n');
                    }
                    //R16 ==> v_0
                    else if(a_value >= 16)
                    {
                        int v_result = a_value - 16;
                        string wspace = "        ";
                        string a = "@";
                        string b = "v";
                        string c = "_";
                        string result = wspace+a+b+c+to_string(v_result);
                        write_to_output(result+'\n');
                    }
                }
            }
            
            //an A-instruction is followed by a C-instruction and the jump bits of the C-instruction are not "000"
            else if(have(instructions[i], tk_a_instr)&&abit!="1"&&dest.back()!='1' && jump != "000")
            {
                //using the label_table to disassemble the instruction
                int a_value = token_ivalue(instructions[i]);
                string str_label = lookup_strings(label_table,to_string(a_value));//L0...
                string wspace = "        ";
                string a = "@";
                string b = str_label;
                string result = wspace+a+b;
                write_to_output(result+'\n' );
            }
            
            //the A-instruction is output in numeric form
            else if(have(instructions[i], tk_a_instr))
            {
                int a_value = token_ivalue(instructions[i]);
                string wspace = "        ";//8 whitespace
                string a = "@";
                string result = wspace+a+to_string(a_value);
                write_to_output(result+'\n');
            }
        }
        
        //disassemble c-instruction
        if(i<=a)
        {
            if(have(instructions[i], tk_c_instr))
            {
                disassembler_c(instructions[i]);
            }
        }
    }
}

// create the label table method and sort the label value in vector
static void creat_label_table(Token instruction,vector<int> label_value)
{
    //use set to store the unduplicated elements
    set<int> s(label_value.begin(), label_value.end());
    //let the label_value vector to store the unduplicated elements stored in set
    label_value.assign(s.begin(), s.end());
    //sort the elements in vector(from small to big)
    sort(label_value.begin(),label_value.end());
    //create the label_table
    for (int j = 0; j < label_value.size(); ++j)
    {
        if (label_value[j] == token_ivalue(instruction))
        {
            //checking whether the instruction have been inserted in label_table or not
            //if not, insert it; else, do nothing
            if(lookup_strings(label_table, to_string(token_ivalue(instruction)))=="")
            {
                string label = "L"+to_string(j);
                insert_strings(label_table, to_string(token_ivalue(instruction)), label);
            }
        }
    }
}

/*****************        DOWN TO HERE         ******************/


// main program
int main(int argc,char **argv)
{
    // initialise tokeniser
    next_token() ;

    // is the first command line argument "N", ie Numeric Output ?
    if ( argc > 1 && string(argv[1]) == "N" )
    {
        // YES, output all A-instructions in numeric form
        disassemble_no_symbols(parse_program()) ;
    }
    else
    {
        // NO, output A-instructions in symbolic form where possible
        disassemble_symbols(parse_program()) ;
    }

    // flush the output and any errors
    print_output() ;
    print_errors() ;
}

