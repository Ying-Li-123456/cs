//==========================
// Computer System
// Student: Ying Li
// id: your a1776903
// Semester:s1
// Year:2020
// Assignment 3
//==========================
#include "iobuffer.h"
#include "symbols.h"
#include "abstract-syntax-tree.h"

// to shorten our code:
using namespace std ;
using namespace CS_IO_Buffers ;
using namespace Jack_Compiler ;

// ***** WHAT TO DO *****
//
// MODIFY the skeleton code below to walk an abstract syntax tree, ast, of a Jack class
//        and pretty print the equivalent Jack source code.
//
// The skeleton code has one function per node in the abstract tree
//  - they each extract all fields into local variables
//  - they each call the appropriate walk_* function to walk any sub-trees
//
// The structure of the skeleton code is just a suggestion
//  - you may want to change the parameters / results to suit your own logic
//  - you can change it as much as you like

// forward declarations of one function per node in the abstract syntax tree
void walk_class(ast t) ;
void walk_class_var_decs(ast t) ;
void walk_var_dec(ast t) ;
void walk_subr_decs(ast t) ;
void walk_subr(ast t) ;
void walk_constructor(ast t) ;
void walk_function(ast t) ;
void walk_method(ast t) ;
void walk_param_list(ast t) ;
void walk_subr_body(ast t) ;
void walk_var_decs(ast t) ;
void walk_statements(ast t) ;
void walk_statement(ast t) ;
void walk_let(ast t) ;
void walk_let_array(ast t) ;
void walk_if(ast t) ;
void walk_if_else(ast t) ;
void walk_while(ast t) ;
void walk_do(ast t) ;
void walk_return(ast t) ;
void walk_return_expr(ast t) ;
void walk_expr(ast t) ;
void walk_term(ast t) ;
void walk_int(ast t) ;
void walk_string(ast t) ;
void walk_bool(ast t) ;
void walk_null(ast t) ;
void walk_this(ast t) ;
void walk_unary_op(ast t) ;
void walk_var(ast t) ;
void walk_array_index(ast t) ;
void walk_call_as_function(ast t) ;
void walk_call_as_method(ast t) ;
void walk_subr_call(ast t) ;
void walk_expr_list(ast t) ;
void walk_infix_op(ast t) ;

// vector to store static variables and field variables
vector<string> static_variable ;
vector<string> field_variable ;

// declare and initialise the global variables
string indentation = "    " ;
int indentation_level = 0 ;

// method to calculate the indentation
string indentation_calcualte(int indentation_level)
{
    string inden = "" ;
    
    // indentation_level is 1
    if(indentation_level==1)
    {
        inden = indentation ;
    }
    // indentation_level is larger than 1
    else
    {
        for (int i = 0; i < indentation_level; ++i)
        {
            inden = inden + indentation ;
        }
    }
    
    return inden ;
}

void walk_class(ast t)
{
    string myclassname = get_class_class_name(t) ;
    ast var_decs = get_class_var_decs(t) ;
    ast subr_decs = get_class_subr_decs(t) ;
    
    // print out the class name and left curly bracket, and then increase the indentation_level
    // (when i meet a left curly bracket, i will increase the indentation_level;
    // when i meet a right curly bracket, i will decrease the indentation_level.)
    write_to_output("class " + myclassname + "\n") ;
    write_to_output( "{\n") ;
    indentation_level++ ;
    
    walk_class_var_decs(var_decs) ;
    
    walk_subr_decs(subr_decs) ;
    
    write_to_output("}\n") ;
    indentation_level-- ;
}

void walk_class_var_decs(ast t)
{
    int ndecs = size_of_class_var_decs(t) ;
    
    for ( int i = 0 ; i < ndecs ; i++ )
    {
        walk_var_dec(get_class_var_decs(t,i)) ;
    }
    
    // according to the rule described in myUni, all static variable
    // declarations must be displayed before any field variable declarations.
    // print out the static variables first
    for (int j = 0; j < static_variable.size(); ++j)
    {
        string inden = indentation_calcualte(indentation_level) ;
        write_to_output(inden+static_variable[j]+"\n") ;
    }
    
    // there is a new line between static variables and field variables
    if(!static_variable.empty())
    {
        write_to_output("\n") ;
    }
    
    // print out the field variables after printing the static variables
    for (int j = 0; j < field_variable.size(); ++j)
    {
        string inden = indentation_calcualte(indentation_level) ;
        write_to_output(inden+field_variable[j]+"\n") ;
    }
    
    // there is a new line after printing out the field variables
    if(!field_variable.empty())
    {
        write_to_output("\n") ;
    }
}

void walk_var_dec(ast t)
{
    string name = get_var_dec_name(t) ;
    string type = get_var_dec_type(t) ;
    string segment = get_var_dec_segment(t) ;
    int offset = get_var_dec_offset(t) ;
    string str ;
    
    // casr 1: local variables
    if(segment == "local")
    {
        write_to_output("var " + type +" "+ name +" ;\n") ;
    }
    // case 2: store static variable declarations in a vector, which is easy to print them out
    else if(segment == "static")
    {
        str = "static " + type + " " + name + " ;" ;
        static_variable.push_back(str) ;
    }
    // case 3: store field variable declarations in a vector, we have to print out these after static variable declarations
    else if(segment == "this")
    {
        str = "field " + type + " " + name + " ;" ;
        field_variable.push_back(str) ;
    }
    //case 4: print out the variable declarations which belong to argument
    else if(segment == "argument")
    {
        write_to_output(type +" " + name) ;
    }
}

void walk_subr_decs(ast t)
{
    int size = size_of_subr_decs(t) ;
    
    for ( int i = 0 ; i < size ; i++ )
    {
        // calculate the indentation and print out it
        string inden = indentation_calcualte(indentation_level) ;
        write_to_output(inden) ;
        
        walk_subr(get_subr_decs(t,i)) ;
        
        // according to the description in myUni, all subroutine declarations in a class,
        // except the last, must be followed by an empty line.
        if( i != size-1 )
        {
            write_to_output("\n") ;
        }
    }
}

void walk_subr(ast t)
{
    ast subr = get_subr_subr(t) ;

    // different subroutine kinds: constructor, function, method
    switch(ast_node_kind(subr))
    {
    case ast_constructor:
        walk_constructor(subr) ;
        break ;
    case ast_function:
        walk_function(subr) ;
        break ;
    case ast_method:
        walk_method(subr) ;
        break ;
    default:
        fatal_error(0,"Unexpected subroutine kind") ;
        break ;
    }
}

void walk_constructor(ast t)
{
    string vtype = get_constructor_vtype(t) ;
    string name = get_constructor_name(t) ;
    ast param_list = get_constructor_param_list(t) ;
    ast subr_body = get_constructor_subr_body(t) ;
    
    // print out the constructor, type, name and brackets
    write_to_output("constructor " + vtype + " " + name + "(") ;
    walk_param_list(param_list) ;
    write_to_output(")\n") ;
    
    // calculate the indentation and print out it and the left curly bracket, increase the indentation level
    string inden = indentation_calcualte(indentation_level) ;
    write_to_output(inden+"{\n" ) ;
    indentation_level++ ;
    
    walk_subr_body(subr_body) ;
    
    // print out the right curly brackets and decrease the indentation level
    write_to_output(inden+"}\n") ;
    indentation_level-- ;
}

void walk_function(ast t)
{
    string vtype = get_function_vtype(t) ;
    string name = get_function_name(t) ;
    ast param_list = get_function_param_list(t) ;
    ast subr_body = get_function_subr_body(t) ;
    
    // print out the function, type, name and brackets
    write_to_output("function " + vtype + " " + name + "(") ;
    walk_param_list(param_list) ;
    write_to_output(")\n") ;
    
    // calculate the indentation and print out it and the left curly bracket, increase the indentation level
    string inden = indentation_calcualte(indentation_level) ;
    write_to_output(inden+"{\n") ;
    indentation_level++ ;
    
    walk_subr_body(subr_body) ;
    
    // print out the right curly brackets and decrease the indentation level
    write_to_output(inden+"}\n") ;
    indentation_level-- ;
}

void walk_method(ast t)
{
    string vtype = get_method_vtype(t) ;
    string name = get_method_name(t) ;
    ast param_list = get_method_param_list(t) ;
    ast subr_body = get_method_subr_body(t) ;
    
    // print out the method, type, name and brackets
    write_to_output("method " + vtype + " " + name + "(") ;
    walk_param_list(param_list) ;
    write_to_output(")\n") ;
    
    // calculate the indentation and print out it and the left curly bracket, increase the indentation level
    string inden = indentation_calcualte(indentation_level) ;
    write_to_output(inden+"{\n") ;
    indentation_level++ ;
    walk_subr_body(subr_body) ;
    
    // print out the right curly brackets and decrease the indentation level
    write_to_output(inden+"}\n") ;
    indentation_level-- ;
}

void walk_param_list(ast t)
{
    int ndecs = size_of_param_list(t) ;
    
    for ( int i = 0 ; i < ndecs ; i++ )
    {
        walk_var_dec(get_param_list(t,i)) ;
        
        // when we print out the parameters, we have to print out the comma except the last parameter
        if(i != ndecs -1)
        {
            write_to_output(",") ;
        }
    }
}

void walk_subr_body(ast t)
{
    ast decs = get_subr_body_decs(t) ;
    ast body = get_subr_body_body(t) ;

    // go to variable declaration part
    walk_var_decs(decs) ;
    
    // goto statements part
    walk_statements(body) ;
}

void walk_var_decs(ast t)
{
    int ndecs = size_of_var_decs(t) ;
    
    // use for loop to print out variable declarations
    for ( int i = 0 ; i < ndecs ; i++ )
    {
        // calculate the indentation and print out the indentation, let keyword eq operation and semi
        string inden = indentation_calcualte(indentation_level) ;
        write_to_output(inden) ;
        
        walk_var_dec(get_var_decs(t,i)) ;
        
        // after the last variable declaration, we need to print out a new line
        if(i == ndecs -1)
        {
            write_to_output("\n") ;
        }
    }
}

void walk_statements(ast t)
{
    int nstatements = size_of_statements(t) ;
    
    // use for loop to  walk all statements
    for ( int i = 0 ; i < nstatements ; i++ )
    {
        walk_statement(get_statements(t,i)) ;
        ast_kind kind = ast_node_kind(get_statement_statement(get_statements(t,i)));
        
        // if statement, if_else statement and while statement must be
        // followed by a blank line unless they are immediately followed by "}".
        if(i != nstatements-1)
        {
            if(kind == ast_if||kind == ast_if_else|| kind == ast_while)
            {
                write_to_output("\n") ;
            }
        }
    }
}

void walk_statement(ast t)
{
    ast statement = get_statement_statement(t) ;

    //different kinds of statements
    switch(ast_node_kind(statement))
    {
    case ast_let:
        walk_let(statement) ;
        break ;
    case ast_let_array:
        walk_let_array(statement) ;
        break ;
    case ast_if:
        walk_if(statement) ;
        break ;
    case ast_if_else:
        walk_if_else(statement) ;
        break ;
    case ast_while:
        walk_while(statement) ;
        break ;
    case ast_do:
        walk_do(statement) ;
        break ;
    case ast_return:
        walk_return(statement) ;
        break ;
    case ast_return_expr:
        walk_return_expr(statement) ;
        break ;
    case ast_statements:
        walk_statements(statement) ;
        break ;
    default:
        fatal_error(0,"Unexpected statement kind") ;
        break ;
    }
}

void walk_let(ast t)
{
    ast var = get_let_var(t) ;
    ast expr = get_let_expr(t) ;
    
    // calculate the indentation and print out the indentation, let keyword eq operation and semi
    string inden = indentation_calcualte(indentation_level) ;
    write_to_output(inden + "let ") ;
    walk_var(var) ;
    write_to_output(" = ") ;
    
    walk_expr(expr) ;
    write_to_output(" ;\n") ;
}

void walk_let_array(ast t)
{
    ast var = get_let_array_var(t) ;
    ast index = get_let_array_index(t) ;
    ast expr = get_let_array_expr(t) ;
    string inden = indentation_calcualte(indentation_level);
    
    // calculate the indentation and print out the indentation, let keyword eq operation and semi
    write_to_output(inden + "let ") ;
    walk_var(var) ;
    
    // print out the brackets, eq and semi
    write_to_output("[") ;
    walk_expr(index) ;
    write_to_output("]") ;
    write_to_output(" = ") ;
    walk_expr(expr) ;
    write_to_output(" ;\n") ;
}

void walk_if(ast t)
{
    ast condition = get_if_condition(t) ;
    ast if_true = get_if_if_true(t) ;
    
    // calculate the indentation and print out the indentation,
    // if keyword and parentheses; goto walk_expr to print out the condition
    string inden = indentation_calcualte(indentation_level) ;
    write_to_output(inden+"if (") ;
    walk_expr(condition) ;
    write_to_output(")\n") ;
    
    // walk to the body part, print out the left curly bracket and increase the indentation level
    write_to_output(inden+"{\n") ;
    indentation_level++ ;
    walk_statements(if_true) ;
    
    // when i finished the walk _statements, print out the right curly bracket and decrease the indentation level
    write_to_output(inden+"}\n") ;
    indentation_level-- ;
}

void walk_if_else(ast t)
{
    ast condition = get_if_else_condition(t) ;
    ast if_true = get_if_else_if_true(t) ;
    ast if_false = get_if_else_if_false(t) ;
    
    // calculate the indentation and print out the indentation, if keyword and parentheses
    string inden = indentation_calcualte(indentation_level) ;
    write_to_output(inden+"if (") ;
    walk_expr(condition) ;
    write_to_output(")\n") ;
    
    // walk to the body part, print out the left curly bracket and increase the indentation level
    write_to_output(inden+"{\n") ;
    indentation_level++ ;
    walk_statements(if_true) ;
    
    // when i finished the walk _statements, print out the right curly bracket and decrease the indentation level
    write_to_output(inden+"}\n") ;
    indentation_level-- ;
    
    // walk to the body part, print out else keyword, the left curly bracket and increase the indentation level
    write_to_output(inden+"else\n") ;
    write_to_output(inden+"{\n") ;
    indentation_level++ ;
    walk_statements(if_false) ;
    
    // when i finished the walk _statements, print out the right curly bracket and decrease the indentation level
    write_to_output(inden+"}\n") ;
    indentation_level-- ;
}

void walk_while(ast t)
{
    ast condition = get_while_condition(t) ;
    ast body = get_while_body(t) ;
    
    // calculate the indentation and print out the indentation, while keyword and parentheses
    string inden = indentation_calcualte(indentation_level) ;
    
    write_to_output(inden+"while (") ;
    walk_expr(condition) ;
    write_to_output(")\n") ;
    
    // walk to the body part, print out the left curly bracket and increase the indentation level
    write_to_output(inden+"{\n") ;
    indentation_level++ ;
    walk_statements(body) ;
    
    // when i finished the walk _statements, print out the right curly bracket and decrease the indentation level
    write_to_output(inden+"}\n") ;
    indentation_level-- ;
}

void walk_do(ast t)
{
    ast call = get_do_call(t) ;
    
    // calculate the indentation
    string inden = indentation_calcualte(indentation_level) ;
    
    // print out the indentation and do keyword
    write_to_output(inden+"do ") ;
    
    switch(ast_node_kind(call))
    {
    case ast_call_as_function:
        walk_call_as_function(call) ;
        break ;
    case ast_call_as_method:
        walk_call_as_method(call) ;
        break ;
    default:
        fatal_error(0,"Unexpected call kind") ;
        break ;
    }
    
    write_to_output(" ;\n") ;
}

void walk_return(ast t)
{
    // calculate the indentation
    string inden = indentation_calcualte(indentation_level) ;
    
    // print out the indentation, return and semi
    write_to_output(inden + "return ;\n") ;
}

void walk_return_expr(ast t)
{
    ast expr = get_return_expr(t) ;
    
    // calculate the indentation
    string inden = indentation_calcualte(indentation_level) ;
    
    // print out the indentation and return
    write_to_output(inden + "return ") ;
    
    walk_expr(expr) ;
    
    //print out the semi
    write_to_output(" ;\n") ;
}

void walk_expr(ast t)
{
    int term_ops = size_of_expr(t) ;
    
    for ( int i = 0 ; i < term_ops ; i++ )
    {
        ast term_op = get_expr(t,i) ;
        
        if ( i % 2 == 0 )
        {
            walk_term(term_op) ;
        }
        else
        {
            walk_infix_op(term_op) ;
        }
    }
}

void walk_term(ast t)
{
    ast term = get_term_term(t) ;

    // different kinds of term
    switch(ast_node_kind(term))
    {
    case ast_int:
        walk_int(term) ;
        break ;
    case ast_string:
        walk_string(term) ;
        break ;
    case ast_bool:
        walk_bool(term) ;
        break ;
    case ast_null:
        walk_null(term) ;
        break ;
    case ast_this:
        walk_this(term) ;
        break ;
    case ast_expr:
        //print out the parentheses
        write_to_output("(") ;
        walk_expr(term) ;
        write_to_output(")") ;
        break ;
    case ast_unary_op:
        walk_unary_op(term) ;
        break ;
    case ast_var:
        walk_var(term) ;
        break ;
    case ast_array_index:
        walk_array_index(term) ;
        break ;
    case ast_call_as_function:
        walk_call_as_function(term) ;
        break ;
    case ast_call_as_method:
        walk_call_as_method(term) ;
        break ;
    default:
        fatal_error(0,"Unexpected term kind") ;
        break ;
    }
}

void walk_int(ast t)
{
    int _constant = get_int_constant(t) ;
    
    // print out the integer, using to_string method to convert int to string
    write_to_output(to_string(_constant)) ;
}

void walk_string(ast t)
{
    string _constant = get_string_constant(t) ;
    
    // print out the string including the ""
    write_to_output("\"" + _constant + "\"") ;
}

void walk_bool(ast t)
{
    bool _constant = get_bool_t_or_f(t) ;
    
    // case 1: true, print out the result
    if(_constant)
    {
        write_to_output("true") ;
    }
    // case 2: false, print out the result
    else
    {
        write_to_output("false") ;
    }
}

void walk_null(ast t)
{
    // print out null
    write_to_output("null") ;
}

void walk_this(ast t)
{
    // print out this
    write_to_output("this") ;
}

void walk_unary_op(ast t)
{
    string uop = get_unary_op_op(t) ;
    ast term = get_unary_op_term(t) ;
    
    //print out the unary operation
    write_to_output(uop) ;
    
    walk_term(term) ;
}

void walk_var(ast t)
{
    string name = get_var_name(t) ;
    string type = get_var_type(t) ;
    string segment = get_var_segment(t) ;
    int offset = get_var_offset(t) ;
    
    // print out the variables's name
    write_to_output(name) ;
}

void walk_array_index(ast t)
{
    ast var = get_array_index_var(t) ;
    ast index = get_array_index_index(t) ;

    walk_var(var) ;
    
    // print out the square brackets
    write_to_output("[") ;
    walk_expr(index) ;
    write_to_output("]") ;
}

void walk_call_as_function(ast t)
{
    string class_name = get_call_as_function_class_name(t) ;
    ast subr_call = get_call_as_function_subr_call(t) ;
    
    // print out the class name if it is not this
    if(class_name != "this")
    {
        write_to_output(class_name+".") ;
    }
    
    walk_subr_call(subr_call) ;
}


void walk_call_as_method(ast t)
{
    string class_name = get_call_as_method_class_name(t) ;
    ast var = get_call_as_method_var(t) ;
    ast subr_call = get_call_as_method_subr_call(t) ;
    
    switch(ast_node_kind(var))
    {
    case ast_this:
        // we do not need to print out this, when we use call_as_method
//        walk_this(var) ;
        break ;
    case ast_var:
        
        walk_var(var) ;
        write_to_output(".") ;
        break ;
    default:
        fatal_error(0,"Expected var or this") ;
        break ;
    }
    walk_subr_call(subr_call) ;
}

void walk_subr_call(ast t)
{
    string subr_name = get_subr_call_subr_name(t) ;
    ast expr_list = get_subr_call_expr_list(t) ;
    
    // print out the subroutine name and brackets
    write_to_output(subr_name+"(") ;
    
    walk_expr_list(expr_list) ;
    write_to_output( ")") ;
}

void walk_expr_list(ast t)
{
    int nexpressions = size_of_expr_list(t) ;
    
    for ( int i = 0 ; i < nexpressions ; i++ )
    {
        walk_expr(get_expr_list(t,i)) ;
        
        // except for the last element in expr_list, there should be a comma and print out it
        if(i != nexpressions-1 )
        {
            write_to_output(",") ;
        }
    }
}

void walk_infix_op(ast t)
{
    string op = get_infix_op_op(t) ;
    
    // print out the operation
    write_to_output(" "+op+" ") ;
}

// main program
int main(int argc,char **argv)
{
    // walk an AST parsed from XML and pretty print equivalent Jack code
    walk_class(ast_parse_xml()) ;

    // flush the output and any errors
    print_output() ;
    print_errors() ;
}

