// test token groups and context strings
#include "tokeniser.h"
#include <iostream>
#include <vector>

using namespace std ;
using namespace Assignment_Tokeniser ;

// token_is test
void token_is_test(Token token,TokenKind kind)
{
    cout << "    token_is(token," << token_kind_to_string(kind) << ") returns " ;
    cout << (token_is(token,kind) ? "true" : "false") ;
    cout << endl;
}

// display token with context
void display_context(Token token,int count)
{
    // its string representation
    cout << "token: " << count << endl ;
    cout << "    " << token_to_string(token) << endl ;

    // check it against some group memberships
    cout << "token group memberships:" << endl ;
    token_is_test(token,tk_identifier) ;
    token_is_test(token,tk_integer) ;
    token_is_test(token,tk_float) ;
    token_is_test(token,tk_scientific) ;
    token_is_test(token,tk_keyword) ;
    token_is_test(token,tk_symbol) ;

    // its context string
    cout << "token_context:" << endl ;
    cout << token_context(token) << endl ;
    cout << "----------------" << endl ;
}

// this main program tokenises standard input and
// prints details of every 5th token in reverse order
int main(int argc,char **argv)
{
    Token token ;
    vector<Token> tokens ;
    int count = 0 ;

    // remember each token as it is read
    token = next_token() ;
    while ( token_kind(token) != tk_eoi )
    {
        tokens.push_back(token) ;
        count++ ;
        token = next_token() ;
    }
    cout << "Read " << count << " tokens" << endl << endl ;

    if ( count == 0 ) return 0 ;

    cout << "Token Contexts:" << endl << endl ;

    // display first token's context
    display_context(tokens[0],0) ;

    // display last token's context and every 5th before it in reverse order
    for ( int i = count - 1 ; i > 0 ; i -= 5 )
    {
        display_context(tokens[i],i) ;
    }

    return 0 ;
}
