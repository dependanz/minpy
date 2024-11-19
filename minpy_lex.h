#ifndef h_minpy_lex
#define h_minpy_lex

#include <map>
#include <cstdlib>
#include <cctype>
#include <string>
#include <iostream>

enum Token {
    ERR, DONE,
    IDENT, INTEGER, REAL, STRING, CHARACTER, TRUE, FALSE,
    ASSIGN,
    AND, OR, NOT,
    EQ, GEQ, LEQ, GT, LT,
    PLUS, MINUS, MULT, EXP, DIV, FLOORDIV,
    SQRT,
    NEWLINE,
    LPAREN, RPAREN
};

class LexItem {
    Token token;
    int linenum;
    std::string lexeme;
    
public:
    LexItem() : token(Token::ERR), linenum(-1), lexeme("") {}
    LexItem(Token token, std::string lexeme, int linenum) : token(token), lexeme(lexeme), linenum(linenum) {}
    Token getToken()  { return token; }
    std::string getLexeme() { return lexeme; }
    bool operator==(Token otherToken) const { return token == otherToken; }
    bool operator!=(Token otherToken) const { return token != otherToken; }
};

enum TokenizerState {
    START               = 0x1,
    IN_IDENT_OR_KEYWORD = 0x2,
    IN_INTEGER          = 0x4,
    IN_REAL_DOT_INT     = 0x8,
    IN_REAL_INT_DOT     = 0x10,
    IN_REAL_EXP_SIGN    = 0x20,
    IN_REAL_EXP         = 0x40,
    IN_STRING_DQUOTE    = 0x80,
    IN_STRING_SQUOTE    = 0x100,
    ERROR               = 0xFFF
};

class Tokenizer {
    TokenizerState state;
    std::string currentLexeme;

public:
    Tokenizer() : state(TokenizerState::START), currentLexeme("") {}

    void SkipWhitespace(std::istream& in, int& linenum, char & currentChar);
    LexItem ident_or_keyword(int& linenum);
    LexItem GetNextToken(std::istream& in, int& linenum);
};

#endif