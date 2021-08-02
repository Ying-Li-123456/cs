#ifndef ASSIGNMENT_TOKENISER_H
#define ASSIGNMENT_TOKENISER_H

#include <string>

// Interface to the assignment tokeniser
namespace Assignment_Tokeniser
{
    // a shorthand for string
    typedef std::string string ;

    // the type of a token returned by the tokeniser
    typedef int Token ;

    // ***** BNF *****
    //
    // *** SYNTAX
    // * ASCII literals inside ' '
    // * grouping ( )
    // * ASCII ranges -
    // * alternatives |
    // * 0 or 1 ?
    // * 0 or more *
    // * 1 or more +
    //
    // *** TOKEN RULES
    // * Grammar rule ::= Definition
    //
    // * input ::= (wspace | token)* eoi
    //
    // * wspace ::= '\n' | ' '
    //
    // * token ::= identifier | number | string | keyword | symbol | eol_comment | adhoc_comment
    //
    // * identifier ::= (letter|'_') (letter|digit|'_'|'$'|'.')*
    //   - letter ::= 'a'-'z'|'A'-'Z'
    //
    // * number ::= integer | float | scientific
    //
    // * integer ::= zero | digits
    //   - zero ::= '0'
    //   - digits ::= digit19 digit*
    //   - digit19  ::= '1'-'9'
    //   - digit  ::= '0'-'9'
    //
    // * float ::= integer fraction
    //   - fraction ::= '.' digit+
    //
    // * scientific ::= integer fraction? exponent
    //   - exponent ::= ('e'|'E') ('+'|'-')? digits0123
    //   - digits0123 ::= zero | digits123
    //   - digits123 ::= digit19 digit? digit?
    //
    // * string ::= '"' instring* '"'
    //   - instring ::= ' '|'!'|'#'-'~'
    //
    // * keyword ::= 'do'|'for'|'pointer'|'real'|'this' 
    //   - keywords each have their own TokenKind
    //
    // * symbol ::= '@'|';'|':'|'!'|','|'.'|'=='|'<=>'|'}'|'{'|')'|'('|']'|'['|'/'
    //   - symbols each have their own TokenKind
    //
    // * eol_comment ::= '//' eol_comment_char* '\n'
    //   - eol_comment_char ::= ' '-'~'
    //
    // * adhoc_comment ::= '/*' adhoc_comment_char* '*/'
    //   - adhoc_comment_char ::= eol_comment_char|'\n'
    //
    // ** ERROR HANDLING
    // * when next_token() is called
    //   * if the first character cannot start a token, the tk_eoi token is returned
    // * after the first character of a token has been found
    //   * if the next character cannot extend the current token,
    //     * if the characters read so far can form a token, that token is returned
    //     * if the characters read so far cannot form a token, the tk_eoi token is returned
    // * once the tk_eoi token is returned,
    //   * ch must be set to the EOF marker to simulate end of input
    //   * all future calls to nextch() will not read any more input
    //   * all future calls to next_token() will return the tk_eoi token
    //
    // ** INPUT PREPROCESSING - not required for MILESTONE SUBMISSIONS
    // * nextch() replaces tab characters ('\t') with one or more spaces so that the current line length becomes a multiple of four characters
    // *          if a tab is read nextch() returns a space,
    // *          if the tab must be replaced by 3 spaces then the next 2 calls of nextch() also return a space
    // * nextch() never sets ch to be a tab character
    // * nextch() discards carriage return characters ('\r') that are immediately followed by a newline character ('\n')
    // * nextch() replaces carriage return characters that are not immediately followed by a newline character with a newline character
    // * nextch() never sets ch to be a carriage return character
    // * only characters assigned to ch can appear in the results of calls to token_context()
    //
    // ** CHANGES TO TOKEN SPELLING - not required for MILESTONE SUBMISSIONS
    // * the '"' and '"' enclosing a string do not appear in the token's spelling
    // * the '//' and '\n' enclosing an eol_comment do not appear in the token's spelling
    // * the '/*' and '*/' enclosing an adhoc_comment do not appear in the token's spelling
    // * the token's spelling for a float must be modified as follows:
    //   * all trailing '0's after the first digit are removed from the fraction
    //   * these modifications are not displayed in the results of calls to token_context()
    // * the token's spelling for a scientific must be modified as follows:
    //   * the value of the number must be expressed in scientific notation, eg 712.34e4 -> 7.1234e+6
    //   * if the value is 0, the spelling is "0.0e+0"
    //   * there must be a single non-zero digit before the '.'
    //   * there must be at least one digit after the '.'
    //   * there must be no trailing '0's after the first digit after the '.'
    //   * the exponent must be signed
    //   * these modifications are not displayed in the results of calls to token_context()
    //
    // ** KEYWORDS - not required for MILESTONE SUBMISSIONS
    // * all the keywords satisfy the grammar rule for an identifier
    //   * keywords must be identified by
    //     1. recognising a complete identifier
    //     2. calling string_to_token_kind() to get a TokenKind for the identifier
    //     3. calling token_kind_is() to see if the TokenKind returned belongs to a keyword
    //
    // ** FINAL SUBMISSION Testing
    // * the final submission tests use input files that may contain tab ('\t') and carriage return ('\r') characters.
    // * do not expect to pass most final submission tests until you have properly implemented the input preprocessing.
    //
    // ** EXPECTED TEST OUTPUTS
    // * you should consult the expected test outputs if you need help interpreting the token definitions
    //


    // The kinds of token that are recognised
    enum TokenKind
    {
        // MILESTONE SUBMISSION Tokens to be recognised:

        tk_space,           // ' '
        tk_newline,         // '\n'

        tk_identifier,      // identifier
        tk_integer,         // integer

        tk_at,              // '@'
        tk_semi,            // ';'
        tk_colon,           // ':'
        tk_not,             // '!'
        tk_comma,           // ','
        tk_stop,            // '.'
        tk_lcb,             // '{'
        tk_rcb,             // '}'
        tk_lrb,             // '('
        tk_rrb,             // ')'
        tk_lsb,             // '['
        tk_rsb,             // ']'
        tk_div,             // '/'

        ////////////////////

        // FINAL SUBMISSION Tokens to be recognised: all of the above and

        tk_eol_comment,     // '//' eol_comment_char* '\n', '//' and '\n' are not part of the spelling
        tk_adhoc_comment,   // '/*' adhoc_comment_char* '*/', '/*' and '*/' are not part of the spelling

        tk_float,           // float
        tk_scientific,      // scientific
        tk_string,          // '"' instring* '"', '"' and '"' are not part of the spelling

        tk_do,              // 'do'
        tk_for,             // 'for'
        tk_pointer,         // 'pointer'
        tk_real,            // 'real'
        tk_this,            // 'this'


        tk_eq,              // '=='
        tk_spaceship,       // '<=>'

        ////////////////////

        tk_eoi,             // end of input reached

        tk_oops,            // for error handling

                            // the remaining tokens are not produced by the tokeniser
                            // these are provided to describe groups of tokens

        tk_wspace,          // one of tk_space or tk_newline
        tk_number,          // one of tk_integer, tk_float or tk_scientific
        tk_keyword,         // one of tk_do, tk_for, tk_pointer, tk_real or tk_this
        tk_symbol           // one of tk_at, tk_semi, tk_colon, tk_not, tk_comma, tk_stop, tk_eq,
                            //        tk_spaceship, tk_lcb, tk_rcb, tk_lrb, tk_rrb, tk_lsb, tk_rsb or tk_div
    } ;

    // ***** the following are implemented in the pre-compiled lib/.../lib.a object file *****

    // return a string representation of the given token kind
    extern string token_kind_to_string(TokenKind kind) ;

    // create a new Token object and return its ID
    extern Token new_token(TokenKind kind,string spelling,int start_line,int start_column,int length) ;

    // return the TokenKind for the given token
    extern TokenKind token_kind(Token token) ;

    // return the characters that make up the given token
    extern string token_spelling(Token token) ;

    // return the line number where the given token was found, the first line is line number 1
    extern int token_line(Token token) ;

    // return the column position where the given token starts on its line, the first character is in column number 1
    extern int token_column(Token token) ;

    // return the length of the token before any modifications were made to its spelling
    extern int token_length(Token token) ;

    // return a string representation of the given token
    extern string token_to_string(Token token) ;

    // is the token of the given kind or does it belong to the given grouping?
    extern bool token_is(Token token,TokenKind kind_or_grouping) ;

    // read the next character from the input, this may or may not be standard input during testing
    extern char read_char() ;

    // the next call of read_char() returns the same character as the last call, no new input is read
    extern void repeat_char() ;

    // if s is a keyword it returns its token kind otherwise it returns notfound
    // ie keyword_kind("do",tk_oops) returns tk_do, keyword_kind("==",tk_oops) returns tk_oops
    extern TokenKind keyword_kind(string s,TokenKind notfound) ;

    // ***** the assignment involves implementing the following functions by completing tokeniser.cpp *****

    // read the next token from standard input
    extern Token next_token() ;

    // generate a context string for the given token
    // all lines are shown after the preprocssing of special characters
    // it shows the line before the token, with a '$' marking the end of line,
    // the line containing the token up to the end of the token, and
    // a line of spaces ending with a ^ under the token's first character to mark its position
    // the lines are prefixed by line numbers, right justified in four spaces, followed by ": ", eg "  45: "
    // NOTE: if a token includes newline character(s), only display the token before the first newline followed by a '$'
    // NOTE: this function is not required for the milestone submission
    extern string token_context(Token token) ;
}
#endif //ASSIGNMENT_TOKENISER_H
