// convert an abstract syntax tree for Hack Assembly language into machine code
#include "iobuffer.h"
#include "symbols.h"
#include "tokeniser.h"
#include "abstract-syntax-tree.h"
#include <string>
#include <algorithm>
// to simplify the code
using namespace std ;
using namespace CS_IO_Buffers ;
using namespace CS_Symbol_Tables ;
using namespace Hack_Assembler ;

//declare some gloabl variables
static symbols dest_bits;
static symbols alu_op_bits;
static symbols jump_bits;
static symbols pre_defined;
static symbols label_table;
static symbols variable_table = create_ints();
//using lineNumber to record the line number
static int lineNumber = 0;
static int variable_start_number = 16;

//declare four methods
void initialise_tables();
static void translate_a(ast instruction);
static void translate_c(ast instruction);
static void translate_name(ast instruction);

// this function provides an example of how to walk an abstract syntax tree constructed by ast_parse_xml()
//the first wlak to establish the label table
void walk_program(ast the_program)
{
    initialise_tables();
    //create the label_table
    label_table = create_ints();
    // the_program contains a vector of instructions
    int ninstructions = size_of_program(the_program) ;
    for ( int i = 0 ; i < ninstructions ; i++ )
    {
        ast instruction = get_program(the_program,i) ;
        switch(ast_node_kind(instruction))
        {
        case ast_label:
            //insert elements to label_table
            //if the inserted label is already in the label_table, call fatal_error
            if(lookup_ints(label_table, get_label_name(instruction)) != -1)
            {
                fatal_error(0,"");
            }
            //if the inserted label is not exist in the label_table, insert it.
            else
            {
                insert_ints(label_table, get_label_name(instruction), lineNumber);
            }
            break ;
        case ast_a_name:
            //increase the number of line, in order to determine the line number of inserted label
            lineNumber++;
            break ;
        case ast_a_instruction:
            lineNumber++;
            break ;
        case ast_c_instruction:
            lineNumber++;
            break ;
        default:
            fatal_error(0,"// bad node - expected ast_label,ast_a_name,ast_a_instruction or ast_c_instruction\n") ;
            break ;
        }
    }
}
//Initialise the look-up table
void initialise_tables()
{
    //the predefined symbols
    pre_defined = create_ints();
    insert_ints(pre_defined, "SP", 0);
    insert_ints(pre_defined, "LCL", 1);
    insert_ints(pre_defined, "ARG", 2);
    insert_ints(pre_defined, "THIS", 3);
    insert_ints(pre_defined, "THAT", 4);
    insert_ints(pre_defined, "SCREEN", 16384);
    insert_ints(pre_defined, "KBD", 24576);
    
    //the three part below belongs to c-instruction
    //the alu operation table(abit + alu-op)
    alu_op_bits = create_strings();
    insert_strings(alu_op_bits,"0","0101010");
    insert_strings(alu_op_bits,"1","0111111");
    insert_strings(alu_op_bits,"-1","0111010");
    insert_strings(alu_op_bits,"D","0001100");
    insert_strings(alu_op_bits,"A","0110000");
    insert_strings(alu_op_bits,"!D","0001101");
    insert_strings(alu_op_bits,"!A","0110001");
    insert_strings(alu_op_bits,"-D","0001111");
    insert_strings(alu_op_bits,"-A","0110011");
    insert_strings(alu_op_bits,"D+1","0011111");
    insert_strings(alu_op_bits,"A+1","0110111");
    insert_strings(alu_op_bits,"D-1","0001110");
    insert_strings(alu_op_bits,"A-1","0110010");
    insert_strings(alu_op_bits,"D+A","0000010");
    insert_strings(alu_op_bits,"D-A","0010011");
    insert_strings(alu_op_bits,"A-D","0000111");
    insert_strings(alu_op_bits,"D&A","0000000");
    insert_strings(alu_op_bits,"D|A","0010101");
    insert_strings(alu_op_bits,"M","1110000");
    insert_strings(alu_op_bits,"!M","1110001");
    insert_strings(alu_op_bits,"-M","1110011");
    insert_strings(alu_op_bits,"M+1","1110111");
    insert_strings(alu_op_bits,"M-1","1110010");
    insert_strings(alu_op_bits,"D+M","1000010");
    insert_strings(alu_op_bits,"D-M","1010011");
    insert_strings(alu_op_bits,"M-D","1000111");
    insert_strings(alu_op_bits,"D&M","1000000");
    insert_strings(alu_op_bits,"D|M","1010101");
    
    //the destination table
    dest_bits = create_strings();
    insert_strings(dest_bits, "NULL", "000");
    insert_strings(dest_bits, "M", "001");
    insert_strings(dest_bits, "D", "010");
    insert_strings(dest_bits, "MD", "011");
    insert_strings(dest_bits, "A", "100");
    insert_strings(dest_bits, "AM", "101");
    insert_strings(dest_bits, "AD", "110");
    insert_strings(dest_bits, "AMD", "111");
    
    //the jump table
    jump_bits = create_strings();
    insert_strings(jump_bits, "NULL", "000" );
    insert_strings(jump_bits, "JGT", "001" );
    insert_strings(jump_bits, "JEQ", "010" );
    insert_strings(jump_bits, "JGE", "011" );
    insert_strings(jump_bits, "JLT", "100" );
    insert_strings(jump_bits, "JNE", "101" );
    insert_strings(jump_bits, "JLE", "110" );
    insert_strings(jump_bits, "JMP", "111" );
    
}
// method to convert decimal number to binary number
string dectobin(string num)
{
    //convert string to int
    int number = stoi(num);
    string result;
    //specific case: 0
    if(number == 0)
    {
        string str = "0000000000000000";
        return str;
    }//normal case
    else
    {
        int mod, i;
        for(i=0; number>0; i++)
        {
            mod=number%2;
            number= number/2;
            result.push_back(mod+'0');
        }
        reverse(result.begin(), result.end());
        //add zero befor the result
        int diff = 16 - result.length();
        for (int j = 0; j < diff; ++j)
        {
            result = "0"+result;
        }
    }
    return result;
}
// the second walk to translate instructions(a_name, a_instruction, c_instruction)
void walk_program2(ast the_program)
{
    initialise_tables();
    // the_program contains a vector of instructions
    int ninstructions = size_of_program(the_program) ;
    for ( int i = 0 ; i < ninstructions ; i++ )
    {
        ast instruction = get_program(the_program,i) ;
        switch(ast_node_kind(instruction))
        {
            case ast_label:
                break ;
            case ast_a_name:
                //translate the name, which includes predefined symbols, variables, labels
                translate_name(instruction);
                break ;
            case ast_a_instruction:
                translate_a(instruction);
                break ;
            case ast_c_instruction:
                translate_c(instruction);
                break ;
            default:
                fatal_error(0,"// bad node - expected ast_label,ast_a_name,ast_a_instruction or ast_c_instruction\n") ;
                break ;
        }
    }
}

//translate c-instruction: according to the lookup table to translate the VM code to binary code
void translate_c(ast instruction)
{
    string not_used_bits = "111";
    string alu_operation = get_c_instruction_alu(instruction);
    string alu_operation_result = lookup_strings(alu_op_bits, alu_operation);
    string alu_dest = get_c_instruction_dest(instruction);
    string alu_dest_result = lookup_strings(dest_bits, alu_dest);
    string alu_jump = get_c_instruction_jump(instruction);
    string alu_jump_result = lookup_strings(jump_bits,alu_jump);
    string c_instruction_result = not_used_bits + alu_operation_result + alu_dest_result + alu_jump_result;
    //using the provided method to print the result, do not forget the '\n'
    write_to_output(c_instruction_result+'\n');
}
// translate a-instruction(number)
void translate_a(ast instruction)
{
    string str = to_string(get_a_instruction_value(instruction));
    string result = dectobin(str);
    //using the provided method to print the result, do not forget the '\n'
    write_to_output(result+'\n');

}
// translate a-instruction(name)
void translate_name(ast instruction)
{
    //case 1: not label
    if(lookup_ints(label_table,get_a_name_unresolved(instruction))==-1)//lowercase
    {
        string a_name = get_a_name_unresolved(instruction);
        int a_name_predefined = lookup_ints(pre_defined,a_name);
        //R0-R255
        if(a_name.front() == 'R')
        {
            string a_name_result = a_name.erase(0,1);//12
            string result = dectobin(a_name_result);
            //using the provided method to print the result, do not forget the '\n'
            write_to_output(result+'\n');
        }
        //predefined symbols: using pre_defined table to translate the instruction
        else if(a_name_predefined >= 0)
        {
            string res = dectobin(to_string(a_name_predefined));
            //using the provided method to print the result, do not forget the '\n'
            write_to_output(res+'\n');
        }
        //variables
        else
        {
            //check whether the instruction exist in the variable table or not
            //if do not exist, insert it
            if(lookup_ints(variable_table,a_name) == -1)
            {
                insert_ints(variable_table,a_name,variable_start_number);
                string variable_number = to_string(variable_start_number);
                string variable_result = dectobin(variable_number);
                //using the provided method to print the result, do not forget the '\n'
                write_to_output(variable_result+'\n');
                //increase the number of variable, in order to determine the variable number of inserted variable
                variable_start_number++;
            }
            //if exist, find it and translate it
            else
            {
                int old_variable_number = lookup_ints(variable_table, a_name);
                string old_variable_num = to_string(old_variable_number);
                string result = dectobin(old_variable_num);
                //using the provided method to print the result, do not forget the '\n'
                write_to_output(result+'\n');
            }
        }
    }
    //case: label
    else
    {
        //using label table to translate the instruction
        int label_result = lookup_ints(label_table, get_a_name_unresolved(instruction));
        string result = dectobin(to_string(label_result));
        //using the provided method to print the result, do not forget the '\n'
        write_to_output(result+'\n');
    }
}

// translate an abstract syntax tree representation of Hack Assembly language into Hack machine code
static void asm_translator(ast the_program)
{
    walk_program(the_program);
    walk_program2(the_program);
}

// main program
int main(int argc,char **argv)
{
    // error messages
    config_errors(iob_buffer) ;

    // parse abstract syntax tree and pass to the translator
    asm_translator(ast_parse_xml()) ;

    // flush output and errors
    print_output() ;
    print_errors() ;
}
