//==========================
// Computer System
// Student: Ying Li
// id: your a1776903
// Semester:s1
// Year:2020
// Assignment 3
//==========================
#include <string>
#include <vector>
#include "iobuffer.h"
#include "symbols.h"
#include "abstract-syntax-tree.h"

// to shorten our code:
using namespace std ;
using namespace CS_IO_Buffers ;
using namespace CS_Symbol_Tables ;
using namespace Jack_Compiler ;

// ***** WHAT TO DO *****
//
// MODIFY the skeleton code below to parse a Jack class and construct the equivalent abstract syntax tree, ast.
//        The main function will print the ast formatted as XML
//
// NOTE: the correct segment and offset is recorded with every variable in the ast
//       so the code generator does not need to use any symbol tables
//
// NOTE: use the fatal function below to print fatal error messages with the current
//       input file position marked.
//
// The skeleton code has one function per non-terminal in the Jack grammar
//  - comments describing how to create relevant ast nodes prefix most functions
//  - ast nodes are immutable - you must create all the sub-trees before creating a new node
//
// The structure of the skeleton code is just a suggestion
//  - it matches the grammar but does not quite match the ast structure
//  - you will need to change the parameters / results / functions to suit your own logic
//  - you can change it as much as you like
//
// NOTE: the best place to start is to write code to parse the input without attempting to create
//       any ast nodes. If correct this parser will report errors when it finds syntax errors but
//       correct programs will fail because the tree printer will not be provided with a valid ast.
//       Once the parsing works modify your code to create the ast nodes.
//
// NOTE: the return create_empty() ; statements are only present so that this skeleton code will compile.
//       these statements need to be replaced by code that creates the correct ast nodes.
//

// fatal error handling
static void fatal(string message)
{
    fatal_error(0,"***** Error:\n" + token_context() + message) ;
}

// The Jack grammar to be recognised:
// program          ::= One or more classes, each class in a separate file named <class_name>'.Jack'
// class            ::= 'class' identifier '{' class_var_decs subr_decs '}'
// class_var_decs   ::= (static_var_dec | field_var_dec)*
// static_var_dec   ::= 'static' type identifier (',' identifier)* ';'
// field_var_dec    ::= 'field' type identifier (',' identifier)* ';'
// type             ::= 'int' | 'char' | 'boolean' | identifier
// vtype            ::= 'void' | type
// subr_decs        ::= (constructor | function | method)*
// constructor      ::= 'constructor' identifier identifier '(' param_list ')' subr_body
// function         ::= 'function' vtype identifier '(' param_list ')' subr_body
// method           ::= 'method' vtype identifier '(' param_list ')' subr_body
// param_list       ::= ((type identifier) (',' type identifier)*)?
// subr_body        ::= '{' var_decs statements '}'
// var_decs         ::= var_dec*
// var_dec          ::= 'var' type identifier (',' identifier)* ';'

// statements       ::= statement*
// statement        ::= let | if | while | do | return
// let              ::= 'let' identifier index? '=' expr ';'
// if               ::= 'if' '(' expr ')' '{' statements '}' ('else' '{' statements '}')?
// while            ::= 'while' '(' expr ')' '{' statements '}'
// do               ::= 'do' identifier (id_call | call) ';'
// return           ::= 'return' expr? ';'

// expr             ::= term (infix_op term)*
// term             ::= integer_constant | string_constant | 'true' | 'false' | 'null' | 'this' | '(' expr ')' | unary_op term | var_term
// var_term         ::= identifier (index | id_call | call)?
// index            ::= '[' expr ']'
// id_call          ::= '.' identifier call
// call             ::= '(' expr_list ')'
// expr_list        ::= (expr (',' expr)*)?
// infix_op         ::= '+' | '-' | '*' | '/' | '&' | '|' | '<' | '>' | '='
// unary_op         ::= '-' | '~'


// forward declarations of one function per non-terminal rule in the Jack grammar - except for program
// NOTE: parse_type(), parse_vtype(), parse_unary_op() all return a Token not an ast
// NOTE: parse_static_var_dec(), parse_field_var_dec(), parse_var_dec() all return vector<ast> not an ast
//
ast parse_class() ;
ast parse_class_var_decs() ;
vector<ast> parse_static_var_dec() ;
vector<ast> parse_field_var_dec() ;
Token parse_type() ;
Token parse_vtype() ;
ast parse_subr_decs() ;
ast parse_constructor() ;
ast parse_function() ;
ast parse_method() ;
ast parse_param_list(int num) ;
ast parse_subr_body() ;
ast parse_var_decs() ;
vector<ast> parse_var_dec(int &local_offset_count) ;

ast parse_statements() ;
ast parse_statement() ;
ast parse_let() ;
ast parse_if() ;
ast parse_while() ;
ast parse_do() ;
ast parse_return() ;

ast parse_expr() ;
ast parse_term() ;
ast parse_var_term() ;
ast parse_index() ;
ast parse_id_call() ;
ast parse_call() ;
ast parse_expr_list() ;
ast parse_infix_op() ;
Token parse_unary_op() ;

// symbol table management so we can lookup declared variables
static symbols class_variable_table = 0 ;
static symbols subroutine_variable_table = 0 ;

// a global to record the next free position in the static/this segment
static int static_offset_count = 0 ;
static int field_offset_count = 0 ;
static string class_name ;

// class ::= 'class' identifier '{' class_var_decs subr_decs '}'
// create_class(myclassname,class_var_`decs,class_subrs)
//
ast parse_class()
{
    /*
    * initialise the class variable table, which is the class scope
    * according to the grammar above to parse the command:
    * 1. 'A': mustbe(tk_A)
    * 2. A: parse_A（）
    */
    push_error_context("parse_class()") ;
    
    class_variable_table = create_variables() ;
    mustbe(tk_class) ;
    class_name = token_spelling(mustbe(tk_identifier)) ;
    mustbe(tk_lcb) ;
    ast class_var_decs = parse_class_var_decs() ;
    ast subr_decs = parse_subr_decs() ;
    mustbe(tk_rcb) ;
    
    pop_error_context() ;
    
    return create_class(class_name, class_var_decs, subr_decs) ;
}

// class_var_decs ::= (static_var_dec | field_var_dec)*
// returns ast_class_var_decs: create_class_var_decs(vector<ast> decs)
// create_class_var_decs must be passed a vector of ast_var_dec
//
ast parse_class_var_decs()
{
    push_error_context("parse_class_var_decs()") ;
    
    vector<ast> ast_var_dec ;
    vector<ast> myvector ;
    
    while(have(tk_class_var))
    {
        if(have(tk_static))
        {
            myvector = parse_static_var_dec() ;
            
            // store all static variable declarations in ast_var_dec vector
            ast_var_dec.insert(ast_var_dec.end(),myvector.begin(),myvector.end()) ;
        }
        else if(have(tk_field))
        {
            myvector = parse_field_var_dec() ;
            
            // store all field variable declarations in ast_var_dec vector
            ast_var_dec.insert(ast_var_dec.end(),myvector.begin(),myvector.end()) ;
        }
    }
    
    pop_error_context() ;
    
    return create_class_var_decs(ast_var_dec) ;
}

// static_var_dec ::= 'static' type identifier (',' identifier)* ';'
// returns vector of ast_var_dec
//
// ast_var_dec: create_var_dec(string name,string segment,int offset,string type)
// create_var_dec must be passed:
// . name: the variable's name
// . segment: the variable's segment
// . offset: the variable's position in it's segment
// . type: the variable's type
//
vector<ast> parse_static_var_dec()
{
    push_error_context("parse_class()") ;
    
    vector<ast> static_var_dec ;
    
    // according to the rule above to parse static variable declarations
    while(have(tk_static))
    {
        mustbe(tk_static) ;
        string type = token_spelling(parse_type()) ;
        Token token = mustbe(tk_identifier) ;
        string name = token_spelling(token) ;
        string segment = "static" ;
        ast var_dec = create_var_dec(name,segment,static_offset_count,type) ;
        static_var_dec.push_back(var_dec) ;
        
        // create a variable object named myvar
        st_variable myvar(name, type, segment, static_offset_count) ;
        
        // it is an error to declare something twice
        if(!insert_variables(class_variable_table, name, myvar))
        {
            fatal("Error!!!") ;
        }
        
        // if insert, increase the static_offset_count number
        static_offset_count++ ;
    
        // deal with the case: static aa, bb, cc;
        while(have(tk_comma))
        {
            mustbe(tk_comma) ;
            token = mustbe(tk_identifier) ;
            name = token_spelling(token) ;
            var_dec = create_var_dec(name,segment,static_offset_count,type) ;
            static_var_dec.push_back(var_dec);
            
            // create a variable object named myvar1
            st_variable myvar1(name, type, segment, static_offset_count) ;
    
            if(!insert_variables(class_variable_table, name, myvar1))
            {
                fatal("Error!!!") ;
            }
            
            static_offset_count++ ;
        }
    }
    
    mustbe(tk_semi) ;
    
    pop_error_context() ;
    
    return static_var_dec ;
}

// field_var_dec ::= 'field' type identifier (',' identifier)* ';'
// returns vector of ast_var_dec
//
// ast_var_dec: create_var_dec(string name,string segment,int offset,string type)
// create_var_dec must be passed:
// . name: the variable's name
// . segment: the variable's segment
// . offset: the variable's position in it's segment
// . type: the variable's type
//
vector<ast> parse_field_var_dec()
{
    push_error_context("parse_class()") ;
    
    vector<ast> field_var_dec ;
    
    while(have(tk_field))
    {
        mustbe(tk_field) ;
        string type = token_spelling(parse_type()) ;
        Token token = mustbe(tk_identifier) ;
        string name = token_spelling(token) ;
        string segment = "this" ;
        ast var_dec = create_var_dec(name,segment,field_offset_count,type) ;
        field_var_dec.push_back(var_dec) ;
        
        // create a variable object named myvar
        st_variable myvar(name, type, segment, field_offset_count) ;
        
        if(!insert_variables(class_variable_table, name, myvar))
        {
            fatal("Error!!!") ;
        }
        
        // if insert, increase the field offset number
        field_offset_count++ ;
        
        // deal with the case: field aa, bb, cc;
        while(have(tk_comma))
        {
            mustbe(tk_comma) ;
            token = mustbe(tk_identifier) ;
            name = token_spelling(token) ;
            var_dec = create_var_dec(name,segment,field_offset_count,type) ;
            field_var_dec.push_back(var_dec) ;
            st_variable myvar1(name, type, segment, field_offset_count) ;

            if(!insert_variables(class_variable_table, name, myvar1))
            {
                fatal("Error!!!") ;
            }
            
            field_offset_count++ ;
        }
    }
    
    mustbe(tk_semi) ;
    
    pop_error_context() ;
    
    return field_var_dec ;
}

// type ::= 'int' | 'char' | 'boolean' | identifier
// returns the Token for the type
Token parse_type()
{
    push_error_context("parse_type()") ;
    
    Token token ;
    
    //different kinds of type
    if(have(tk_int))
    {
        token = mustbe(tk_int) ;
        
        return token ;
    }
    else if(have(tk_char))
    {
        token = mustbe(tk_char) ;
        
        return token ;
    }
    else if(have(tk_boolean))
    {
        token = mustbe(tk_boolean) ;
        
        return token ;
    }
    else
    {
        return mustbe(tk_identifier) ;
    }
}

// vtype ::= 'void' | type
// returns the Token for the type
Token parse_vtype()
{
    push_error_context("parse_vtype()") ;
    
    Token vtype ;
    
    // different kind of vtype
    if(have(tk_void))
    {
        vtype = mustbe(tk_void) ;
    }
    else
    {
        vtype = parse_type() ;
    }
    
    pop_error_context() ;
    
    return vtype ;
}

// subr_decs ::= (constructor | function | method)*
// returns ast_subr_decs: create_subr_decs(vector<ast> subrs)
// create_subr_decs must be passed an vector of ast_subr
//
// ast_subr: create_subr(ast subr)
// create_subr must be passed one of: ast_constructor, ast_function or ast_method
//
ast parse_subr_decs()
{
    push_error_context("parse_subr_decs()") ;
    
    vector<ast> my_own_ast_subr_decs ;
    
    // different kinds of subroutines
    while(have(tk_subroutine))
    {
        if(have(tk_constructor))
        {
            ast c = parse_constructor() ;
            ast s1 = create_subr(c) ;
            my_own_ast_subr_decs.push_back(s1) ;
        }
        else if(have(tk_function))
        {
            ast c = parse_function() ;
            ast s1 = create_subr(c) ;
            my_own_ast_subr_decs.push_back(s1) ;
        }
        else if(have(tk_method))
        {
            ast c = parse_method() ;
            ast s1 = create_subr(c) ;
            my_own_ast_subr_decs.push_back(s1) ;
        }
    }
    
    pop_error_context() ;
    
    return create_subr_decs(my_own_ast_subr_decs) ;
}

// constructor ::= 'constructor' identifier identifier '(' param_list ')' subr_body
// returns ast_constructor: create_constructor(string vtype,string name,ast params,ast body)
// . vtype: the constructor's return type, this must be it's class name
// . name: the constructor's name within its class
// . params: ast_param_list - the constructor's parameters
// . body: ast_subr_body - the constructor's body
//
ast parse_constructor()
{
    push_error_context("parse_constructor()") ;
    
    // initialise the subroutine variable table including variables declared in constructor
    subroutine_variable_table = create_variables() ;
    
    mustbe(tk_constructor) ;
    string vtype = token_spelling(mustbe(tk_identifier)) ;
    
    // the constructor's return type, this must be it's class name
    if(vtype != class_name)
    {
        fatal("Error!!!") ;
    }
    
    string name = token_spelling(mustbe(tk_identifier)) ;
    mustbe(tk_lrb) ;
    
    //in a constructor the first visible parameter is argument 0
    ast params = parse_param_list(0) ;
    mustbe(tk_rrb) ;
    ast body = parse_subr_body() ;
    
    // at the end of this subroutine, delete the table
    // 1. all variables declared in subroutines are useful in this subroutine scope
    // 2. to avoid mess up when we have the same variable name in different subroutines.
    delete_variables(subroutine_variable_table) ;
    
    pop_error_context() ;
    
    return create_constructor(vtype, name, params, body) ;
}

// function ::= 'function' vtype identifier '(' param_list ')' subr_body
// returns ast_function: create_function(string vtype,string name,ast params,ast body)
// . vtype: the function's return type
// . name: the function's name within its class
// . params: ast_param_list - the function's parameters
// . body: ast_subr_body - the function's body
//
ast parse_function()
{
    push_error_context("parse_function()") ;
    
    // initialise the subroutine variable table including variables declared in function
    subroutine_variable_table = create_variables() ;
    
    mustbe(tk_function) ;
    string vtype = token_spelling(parse_vtype()) ;
    string name = token_spelling(mustbe(tk_identifier)) ;
    mustbe(tk_lrb) ;
    
    //in a function the first visible parameter is argument 0
    ast params = parse_param_list(0) ;
    mustbe(tk_rrb) ;
    ast body = parse_subr_body() ;
    delete_variables(subroutine_variable_table) ;
    
    pop_error_context() ;
    
    return create_function(vtype, name, params, body) ;
}

// method ::= 'method' vtype identifier '(' param_list ')' subr_body
// returns ast_method: create_method(string vtype,string name,ast params,ast body)
// . vtype: the method's return type
// . name: the method;s name within its class
// . params: ast_param_list - the method's explicit parameters
// . body: ast_subr_body - the method's body
//
ast parse_method()
{
    push_error_context("parse_method()") ;
    
    // initialise the subroutine variable table including variables declared in method
    subroutine_variable_table = create_variables() ;
    
    mustbe(tk_method) ;
    string vtype = token_spelling(parse_vtype()) ;
    string name = token_spelling(mustbe(tk_identifier)) ;
    mustbe(tk_lrb) ;
    
    //in a method the first visible parameter is argument 1
    ast params = parse_param_list(1) ;
    mustbe(tk_rrb) ;
    ast body = parse_subr_body() ;
    
    delete_variables(subroutine_variable_table) ;
    
    pop_error_context() ;
    
    return create_method(vtype, name, params, body) ;
}

// param_list ::= ((type identifier) (',' type identifier)*)?
// returns ast_param_list: create_param_list(vector<ast> params)
// create_param_list must be passed a vector of ast_var_dec
//
// ast_var_dec: create_var_dec(string name,string segment,int offset,string type)
// create_var_dec must be passed:
// . name: the variable's name
// . segment: the variable's segment
// . offset: the variable's position in it's segment
// . type: the variable's type
//
ast parse_param_list(int num)
{
    push_error_context("parse_param_list()") ;
    
    int argument_offset_count;
    //using a vector to store all parameters
    vector<ast> params ;
    
    // in a function or constructor the first visible parameter is argument 0
    if(num == 0)
    {
        argument_offset_count = 0 ;
    }
    //the first visible parameter in method is argument 1
    else
    {
        argument_offset_count = 1 ;
    }

    if(have(tk_type))
    {
        string type = token_spelling(mustbe(tk_type)) ;
        Token token = mustbe(tk_identifier) ;
        string name = token_spelling(token) ;
        string segment = "argument" ;
        ast para = create_var_dec(name,segment,argument_offset_count,type) ;
        params.push_back(para) ;
        st_variable myvar(name, type, segment, argument_offset_count) ;
        
        // same one, call fatal error
        if(!insert_variables(subroutine_variable_table,name,myvar))
        {
            fatal("Error!!!") ;
        }
        
        // if insert, increase the argument offset number
        argument_offset_count++ ;
        
        while(have(tk_comma))
        {
            mustbe(tk_comma) ;
            type = token_spelling(mustbe(tk_type)) ;
            token = mustbe(tk_identifier) ;
            name = token_spelling(token) ;
            ast para = create_var_dec(name,segment,argument_offset_count,type) ;
            params.push_back(para) ;
            st_variable myvar1(name, type, segment, argument_offset_count) ;
            
            if(!insert_variables(subroutine_variable_table,name,myvar1))
            {
                fatal("Error!!!") ;
            }
            
            argument_offset_count++ ;
        }
    }
    
    pop_error_context() ;
    
    return create_param_list(params) ;
}

// subr_body ::= '{' var_decs statements '}'
// returns ast_subr_body: create_subr_body(ast decs,ast body)
// create_subr_body must be passed:
// . decs: ast_var_decs - the subroutine's local variable declarations
// . body: ast_statements - the statements within the body of the subroutinue
//
ast parse_subr_body()
{
    push_error_context("parse_subr_body()") ;
    
    ast decs ;
    ast body ;
    
    mustbe(tk_lcb) ;
    decs = parse_var_decs() ;
    body = parse_statements() ;
    mustbe(tk_rcb) ;
    
    pop_error_context() ;
    
    return create_subr_body(decs, body) ;
}

// var_decs ::= var_dec*
// returns ast_var_decs: create_var_decs(vector<ast> decs)
// create_var_decs must be passed a vector of ast_var_dec
//
ast parse_var_decs()
{
    push_error_context("parse_var_decs()") ;
    
    vector<ast> decs ;
    vector<ast> all_decs ;
    int local_offset_count = 0 ;
    
    while(have(tk_var))
    {
        // using a vector to store all variable declarations
        decs = parse_var_dec(local_offset_count) ;
        all_decs.insert(all_decs.end(),decs.begin(),decs.end()) ;
    }
    
    pop_error_context() ;
    
    return create_var_decs(all_decs) ;
}

// var_dec ::= 'var' type identifier (',' identifier)* ';'
// returns vector of ast_var_dec
//
// ast_var_dec: create_var_dec(string name,string segment,int offset,string type)
// create_var_dec must be passed:
// . name: the variable's name
// . segment: the variable's segment
// . offset: the variable's position in it's segment
// . type: the variable's type
//
vector<ast> parse_var_dec(int &local_offset_count)
{
    push_error_context("parse_var_dec()") ;
    
    vector<ast> my_ast_var_dec ;

    mustbe(tk_var) ;
    string type = token_spelling(parse_type()) ;
    Token token = mustbe(tk_identifier) ;
    string name = token_spelling(token) ;
    string segment = "local" ;
    ast var = create_var_dec(name,segment,local_offset_count,type) ;
    my_ast_var_dec.push_back(var) ;
    st_variable myvar(name, type, segment, local_offset_count) ;
    
    // same one, call fatal error
    if(!insert_variables(subroutine_variable_table,name,myvar))
    {
        fatal("Error!!!") ;
    }
    
    local_offset_count++ ;

    while(have(tk_comma))
    {
        mustbe(tk_comma) ;
        
        token = mustbe(tk_identifier) ;
        name = token_spelling(token) ;
        ast var = create_var_dec(name,segment,local_offset_count,type) ;
        my_ast_var_dec.push_back(var) ;
        st_variable myvar(name, type, segment, local_offset_count) ;
        
        if(!insert_variables(subroutine_variable_table,name,myvar))
        {
            fatal("Error!!!") ;
        }
        
        local_offset_count++ ;
    }
    
    mustbe(tk_semi) ;
    
    pop_error_context() ;
    
    return my_ast_var_dec ;
}

// statements ::= statement*
// create_statements(vector<ast> statements)
// create_statements must be passed a vector of ast_statement
//
ast parse_statements()
{
    push_error_context("parse_statements()") ;
    
    vector<ast> statements ;
    
    // parse one or more statements
    while(have(tk_statement))
    {
        statements.push_back(parse_statement()) ;
    }
    
    pop_error_context() ;
    
    return create_statements(statements) ;
}

// statement ::= let | if | while | do | return
// create_statement(ast statement)
// create_statement initialiser must be one of: ast_let, ast_let_array, ast_if, ast_if_else, ast_while, ast_do, ast_return or ast_return_expr
//
ast parse_statement()
{
    push_error_context("parse_statement()") ;
    
    ast statement ;
    
    // different kind of statements
    if(have(tk_let))
    {
        statement = parse_let() ;
        return create_statement(statement) ;
    }
    else if(have(tk_if))
    {
        statement = parse_if() ;
        return create_statement(statement) ;
    }
    else if(have(tk_while))
    {
        statement = parse_while() ;
        return create_statement(statement) ;
    }
    else if(have(tk_do))
    {
        statement = parse_do() ;
        return create_statement(statement) ;
    }
    else if(have(tk_return))
    {
        statement = parse_return() ;
        return create_statement(statement) ;
    }
    
    //if there is another statement, call fatal error
    fatal_error(0,"here") ;
    
    pop_error_context() ;
    
    return -1 ;
}

// let ::= 'let' identifier index? '=' expr ';'
// return one of: ast_let or ast_let_array
//
// create_let(ast var,ast expr)
// . var: ast_var - the variable being assigned to
// . expr: ast_expr - the variable's new value
//
// create_let_array(ast var,ast index,ast expr)
// . var: ast_var - the variable for the array being assigned to
// . index: ast_expr - the array index
// . expr: ast_expr - the array element's new value
//
ast parse_let()
{
    push_error_context("parse_let()") ;
    
    ast var ;
    ast expr ;
    
    mustbe(tk_let) ;
    Token token = mustbe(tk_identifier) ;
    string name = token_spelling(token) ;
    
    // find variable in two tables and stored in an object
    st_variable subr_myvar = lookup_variables(subroutine_variable_table,name) ;
    st_variable class_myvar = lookup_variables(class_variable_table,name) ;
    
    // the variable exists in subroutine_variable_table, get the properties of this object
    if(subr_myvar.offset != -1)
    {
        var = create_var(subr_myvar.name,subr_myvar.segment,subr_myvar.offset,subr_myvar.type) ;
    }
    // the variable exists in class_variable_table, get the properties of this object
    else if(class_myvar.offset != -1)
    {
        var = create_var(class_myvar.name,class_myvar.segment,class_myvar.offset,class_myvar.type) ;
    }
    // if not in the two tables, call fatal error
    else
    {
        fatal("Error!!!") ;
    }
    
    // if have lsb, it is an array, create_let_array
    if(have(tk_lsb))
    {
        ast index = parse_index() ;
        mustbe(tk_eq) ;
        expr = parse_expr() ;
        mustbe(tk_semi);
        return create_let_array(var,index,expr) ;
    }
    
    mustbe(tk_eq) ;
    expr = parse_expr();
    mustbe(tk_semi) ;
    
    pop_error_context() ;
    
    return create_let(var, expr) ;
}

// if ::= 'if' '(' expr ')' '{' statements '}' ('else' '{' statements '}')?
// return one of: ast_if or ast_if_else
//
// create_if(ast condition,ast if_true)
// . condition: ast_expr - the if condition
// . if_true: ast_statements - the if true branch
//
// create_if_else(ast condition,ast if_true,ast if_false)
// . condition: ast_expr - the if condition
// . if_true: ast_statements - the if true branch
// . if_false: ast_statements - the if false branch
//
ast parse_if()
{
    push_error_context("parse_if()") ;
    
    ast condition ;
    ast if_true ;
    ast if_false ;
    
    mustbe(tk_if) ;
    mustbe(tk_lrb) ;
    condition = parse_expr() ;
    mustbe(tk_rrb) ;
    mustbe(tk_lcb) ;
    if_true = parse_statements() ;
    mustbe(tk_rcb) ;
    
    // if there is an else, it is if_else statement
    if(have(tk_else))
    {
        mustbe(tk_else) ;
        mustbe(tk_lcb) ;
        if_false = parse_statements() ;
        mustbe(tk_rcb) ;
        return create_if_else(condition, if_true, if_false) ;
    }
    
    pop_error_context() ;
    
    // create if statement
    return create_if(condition, if_true) ;
}

// while ::= 'while' '(' expr ')' '{' statements '}'
// returns ast_while: create_while(ast condition,ast body)
// . condition: ast_expr - the loop condition
// . body: ast_statements - the loop body
//
ast parse_while()
{
    push_error_context("parse_while()") ;
    
    ast condition ;
    ast body ;
    
    mustbe(tk_while) ;
    mustbe(tk_lrb) ;
    condition = parse_expr() ;
    mustbe(tk_rrb) ;
    mustbe(tk_lcb) ;
    body = parse_statements() ;
    mustbe(tk_rcb) ;
    
    pop_error_context() ;
    
    return create_while(condition,body) ;
}

// do ::= 'do' identifier (call | id_call) ';'
// returns ast_do: create_do(ast call)
// create_do must be passed one of: ast_call_as_function or ast_call_as_method
//
// ast_call_as_function: create_call_as_function(string class_name,ast subr_call)
// create_ast_call_as_function must be passed:
// . class_name: name of the function's class
// . subr_call: ast_subr_call - the function's name within it's class and it's explicit arguments
//
// ast_call_as_method: create_call_as_method(string class_name,ast object,ast subr_call)
// create_ast_call_as_method must be passed:
// . class_name: name of the method's class
// . object: ast_expr - the object the method is applied to
// . subr_call: ast_subr_call - the method's name within it's class and it's explicit arguments
//

//call: (    id_call: .
ast parse_do()
{
    push_error_context("parse_do()") ;
    
    mustbe(tk_do) ;
    Token token = mustbe(tk_identifier) ;
    string name = token_spelling(token) ;
    st_variable subr_myvar = lookup_variables(subroutine_variable_table,name) ;
    st_variable class_myvar = lookup_variables(class_variable_table,name) ;
    
    // have(tk_lrb): parse_call()
    if(have(tk_lrb))
    {
        ast expr_list = parse_call() ;
        mustbe(tk_semi) ;
        ast subr_call = create_subr_call(name,expr_list) ;
        ast call_as_method = create_call_as_method(class_name,create_this(),subr_call) ;
        pop_error_context() ;
        
        return create_do(call_as_method) ;
    }
    
    // have(tk_stop): parse_id_call()
    if(have(tk_stop))
    {
        // the variable is in subroutine_variable_table, it is a method call(object.method())
        if(subr_myvar.offset != -1)
        {
            ast subr_call = parse_id_call() ;
            ast object = create_var(subr_myvar.name,subr_myvar.segment,subr_myvar.offset,subr_myvar.type) ;
            ast call_as_method = create_call_as_method(subr_myvar.type,object,subr_call) ;
            mustbe(tk_semi) ;
            
            return create_do(call_as_method) ;
            
        }
        // the variable is in class_variable_table, it is a method call(object.method())
        else if(class_myvar.offset != -1)
        {
            ast subr_call = parse_id_call() ;
            ast object = create_var(class_myvar.name,class_myvar.segment,class_myvar.offset,class_myvar.type) ;
            ast call_as_method = create_call_as_method(class_myvar.type,object,subr_call) ;
            mustbe(tk_semi) ;
            
            return create_do(call_as_method) ;
            
        }
        // do not exist in the two tables, it is a function call(class.function())
        else
        {
            mustbe(tk_stop) ;
            Token to = mustbe(tk_identifier) ;
            string sname = token_spelling(to) ;
            mustbe(tk_lrb) ;
            ast expr_list = parse_expr_list() ;
            mustbe(tk_rrb) ;
            ast subr_call = create_subr_call(sname,expr_list) ;
            ast call_as_function = create_call_as_function(name,subr_call) ;
            mustbe(tk_semi) ;
            
            return create_do(call_as_function) ;
        }
    }
    
    fatal_error(0,"here") ;
    
    pop_error_context() ;
    
    return -1 ;
}

// return ::= 'return' expr? ';'
// returns one of: ast_return or ast_return_expr
//
// ast_return: create_return()
//
// ast_return_expr: create_return_expr(ast expr)
// create_return_expr must be passed an ast_expr
//
ast parse_return()
{
    push_error_context("parse_return()") ;
    
    ast expr ;
    
    if(have(tk_return))
    {
        mustbe(tk_return) ;
    }
    else
    {
        fatal("Error!!!");
    }
    
    if(have(tk_term))
    {
        expr = parse_expr() ;
        mustbe(tk_semi) ;
        
        // return expr
        return create_return_expr(expr) ;
    }
    
    mustbe(tk_semi) ;
    
    pop_error_context() ;
    
    // just return
    return create_return() ;
}

// expr ::= term (infix_op term)*
// returns ast_expr: create_expr(vector<ast> expr)
// the vector passed to create_expr:
// . must be an odd length of at least 1, ie 1, 3, 5, ...
// . must be  a vector alternating between ast_term and ast_infix_op nodes, ending with an ast_term
//
ast parse_expr()
{
    push_error_context("parse_expr()") ;
    
    vector<ast> expr ;
    
    expr.push_back(parse_term()) ;
    
    while(have(tk_infix_op))
    {
        expr.push_back(parse_infix_op()) ;
        expr.push_back(parse_term()) ;
    }
    
    // the lenght of the vector must be odd, if nor call fatal error
    if(expr.size() % 2 != 0)
    {
        return create_expr(expr) ;
    }
    else
    {
        fatal_error("Error!!!") ;
    }
    
    pop_error_context() ;
    
    return -1 ;
}

// term ::= integer_constant | string_constant | 'true' | 'false' | 'null' | 'this' | '(' expr ')' | unary_op term | var_term
// returns ast_term: create_term(ast term)
// create_term must be passed one of: ast_int, ast_string, ast_bool, ast_null, ast_this, ast_expr,
//                                    ast_unary_op, ast_var, ast_array_index, ast_call_as_function, ast_call_as_method
//
// ast_int: create_int(int _constant)
// create_int must be passed an integer value in the range 0 to 32767
//
// ast_string: create_string(string _constant)
// create_string must be passed any legal Jack string literal
//
// ast_bool: create_bool(bool t_or_f)
// create_bool must be passed true or false
//
// ast_null: create_null()
//
// ast_this: create_this()
//
// ast_unary_op: create_unary_op(string op,ast term)
// create_unary_op must be passed:
// . op: the unary op
// . term: ast_term
//
ast parse_term()
{
    push_error_context("parse_term()") ;
    
    ast term ;
    
    // different kinds of terms
    if(have(tk_integerConstant))
    {
        int constant = token_ivalue() ;
        mustbe(tk_integerConstant) ;
        term = create_int(constant) ;

        return create_term(term) ;
    }
    else if(have(tk_stringConstant))
    {
        string str_constant = token_spelling() ;
        mustbe(tk_stringConstant) ;
        term = create_string(str_constant) ;
        
        return create_term(term) ;
    }
    if(have(tk_true))
    {
        term = create_bool(true) ;
        mustbe(tk_true) ;
        
        return create_term(term) ;
    }
    else if(have(tk_false))
    {
        term = create_bool(false) ;
        mustbe(tk_false) ;
        
        return create_term(term) ;
    }
    else if(have(tk_null))
    {
        term = create_null() ;
        mustbe(tk_null) ;
        
        return create_term(term) ;
    }
    else if(have(tk_this))
    {
        term = create_this() ;
        mustbe(tk_this) ;
        
        return create_term(term) ;
    }
    else if(have(tk_lrb))
    {
        mustbe(tk_lrb) ;
        term = parse_expr() ;
        mustbe(tk_rrb) ;
        
        return create_term(term) ;
    }
    else if(have(tk_unary_op))
    {
        string op = token_spelling(parse_unary_op()) ;
        term = parse_term() ;
        
        return create_term(create_unary_op(op, term)) ;
    }
    else if(have(tk_identifier))
    {
        term = parse_var_term() ;
        
        return create_term(term) ;
    }
    
    pop_error_context() ;
    
    // if the term is not in above, call fatal error
    fatal("Error!!!") ;
    
    return -1 ;
}

// var_term ::= identifier (index | id_call | call)?
// returns one of: ast_var, ast_array_index, ast_call_as_function or ast_call_as_method
//
// ast_var: create_var(string name,string segment,int offset,string type)
// create_ast_var must be passed:
// . name: the variable's name
// . segment: the variable's segment
// . offset: the variable's position in it's segment
// . type: the variable's type
//
// ast_array_index: create_array_index(ast var,ast index)
// create_ast_array_index must be passed:
// . var: ast_var - the array variable
// . index: ast_expr - the index into the array
//
// ast_call_as_function: create_call_as_function(string class_name,ast subr_call)
// create_ast_call_as_function must be passed:
// . class_name: name of the function's class
// . subr_call: ast_subr_call - the function's name within it's class and it's explicit arguments
//
// ast_call_as_method: create_call_as_method(string class_name,ast object,ast subr_call)
// create_ast_call_as_method must be passed:
// . class_name: name of the method's class
// . object: ast_expr - the object the method is applied to
// . subr_call: ast_subr_call - the method's name within it's class and it's explicit arguments
//
ast parse_var_term()
{
    push_error_context("parse_var_term()") ;
    
    Token token = mustbe(tk_identifier) ;
    string name = token_spelling(token) ;
    
    // case: tk_stop
    if(have(tk_stop))
    {
        ast subr_call = parse_id_call() ;
        st_variable subr_myvar = lookup_variables(subroutine_variable_table,name) ;
        st_variable class_myvar = lookup_variables(class_variable_table, name) ;
        
        // the variable is in subroutine_variable_table, it is method
        if(subr_myvar.offset != -1)
        {
            ast object = create_var(subr_myvar.name, subr_myvar.segment, subr_myvar.offset, subr_myvar.type) ;
            name = get_var_type(object) ;
            
            return create_call_as_method(name,object,subr_call) ;
        }
        // the variable is in class_variable_table, it is method
        else if(class_myvar.offset != -1)
        {
            ast object = create_var(class_myvar.name, class_myvar.segment, class_myvar.offset, class_myvar.type) ;
            name = get_var_type(object) ;
            
            return create_call_as_method(name,object,subr_call) ;
        }
        // not in the two tables, it is a function
        else
        {
            return create_call_as_function(name,subr_call) ;
        }
    }
    
    //case: tk_lrb
    if(have(tk_lrb))
    {
        ast object=create_this() ;
        ast subr_call = create_subr_call(name,parse_call()) ;
        
        return create_call_as_method(class_name, object, subr_call) ;
    }
    
    
    else
    {
        // case: [
        if(have(tk_lsb))
        {
            st_variable subr_myvar = lookup_variables(subroutine_variable_table,name) ;
            st_variable class_myvar = lookup_variables(class_variable_table,name) ;
            
            if(subr_myvar.offset != -1)
            {
                ast var = create_var(subr_myvar.name,subr_myvar.segment,subr_myvar.offset,subr_myvar.type) ;
                ast index = parse_index() ;
                
                return create_array_index(var,index) ;
                
            }
            else if(class_myvar.offset != -1)
            {
                ast var = create_var(class_myvar.name,class_myvar.segment,class_myvar.offset,class_myvar.type) ;
                ast index = parse_index() ;
                
                return create_array_index(var,index) ;
            }
            // if it is not exist in the two tables, call fatal error
            else
            {
                fatal("Error!!!") ;
            }
        }
        // case: variable
        else
        {
            st_variable subr_myvar = lookup_variables(subroutine_variable_table,name) ;
            st_variable class_myvar = lookup_variables(class_variable_table,name) ;
            
            if(subr_myvar.offset != -1)
            {
                ast var = create_var(subr_myvar.name,subr_myvar.segment,subr_myvar.offset,subr_myvar.type) ;
                
                return var ;
            }
            else if(class_myvar.offset != -1)
            {
                ast var = create_var(class_myvar.name,class_myvar.segment,class_myvar.offset,class_myvar.type) ;
                
                return var ;
            }
            // if it is not exist in the two tables, call fatal error
            else
            {
                fatal("Error!!!") ;
            }
        }
    }
    
    pop_error_context() ;
    
    fatal("Error!!!") ;
    
    return -1 ;
}

// index ::= '[' expr ']'
// returns ast_expr
ast parse_index()
{
    push_error_context("parse_index()") ;
    
    ast expr ;
    
    mustbe(tk_lsb) ;
    expr = parse_expr() ;
    mustbe(tk_rsb) ;
    
    pop_error_context() ;
    
    return expr ;
}

// id_call ::= '.' identifier call
// returns ast_subr_call: create_subr_call(string subr_name,ast expr_list)
// create_subr_call must be passed:
// . subr_name: the constructor, function or method's name within its class
// . expr_list: ast_expr_list - the explicit arguments to the call
//
ast parse_id_call()
{
    push_error_context("parse_id_call()") ;
    
    string subr_name ;
    ast expr_list ;
    
    mustbe(tk_stop) ;
    subr_name = token_spelling(mustbe(tk_identifier)) ;
    expr_list = parse_call() ;
    
    pop_error_context() ;
    
    return create_subr_call(subr_name, expr_list) ;
}

// call ::= '(' expr_list ')'
// returns ast_expr_list
//
ast parse_call()
{
    push_error_context("parse_call()") ;
    
    ast ast_expr_list ;
    
    mustbe(tk_lrb) ;
    ast_expr_list = parse_expr_list() ;
    mustbe(tk_rrb) ;
    
    pop_error_context() ;
    
    return ast_expr_list ;
}

// expr_list ::= (expr (',' expr)*)?
// returns ast_expr_list: create_expr_list(vector<ast> exprs)
// create_expr_list must be passed: a vector of ast_expr
//
ast parse_expr_list()
{
    push_error_context("parse_expr_list()") ;
    
    //using vector to srtore all exprs
    vector<ast> exprs ;
    
    if(have(tk_term))
    {
        exprs.push_back(parse_expr()) ;
        
        while(have(tk_comma))
        {
            mustbe(tk_comma) ;
            exprs.push_back(parse_expr()) ;
        }
    }
    
    pop_error_context() ;
    
    return create_expr_list(exprs) ;
}

// infix_op ::= '+' | '-' | '*' | '/' | '&' | '|' | '<' | '>' | '='
// returns ast_op: create_infix_op(string infix_op)
// create_infix_op must be passed:
// infix_op: the infix op
//
ast parse_infix_op()
{
    push_error_context("parse_infix_op()") ;
    
    string infix_op = token_spelling(mustbe(tk_infix_op)) ;
    
    pop_error_context() ;
    
    return create_infix_op(infix_op) ;
}

// unary_op ::= '-' | '~'
// returns Token for the unary_op
//
Token parse_unary_op()
{
    push_error_context("parse_unary_op()") ;
    
    if(have(tk_sub))
    {
        Token token = mustbe(tk_sub) ;
        
        return token ;
    }
    else if(have(tk_not))
    {
        Token token = mustbe(tk_not) ;
        
        return token ;
    }
    
    pop_error_context() ;
    
    fatal("Error!!!") ;
    
    return -1 ;
}

ast jack_parser()
{
    // read the first token to get the tokeniser initialised
    next_token() ;
    
    // construct tree and return as result
    return parse_class() ;
}

// main program
int main(int argc,char **argv)
{
    config_errors(iob_immediate);
    // parse a Jack class and print the abstract syntax tree as XML
    ast_print_as_xml(jack_parser(),4) ;
    
    // flush the output and any errors
    print_output() ;
    print_errors() ;
}

