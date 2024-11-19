#include "minpy_lex.h"

#define RETURN_LEXITEM(tok) return LexItem(tok, currentLexeme, linenum)

// Skips whitespaces except '\n'
void Tokenizer::SkipWhitespace(std::istream& in, int& linenum, char & currentChar) {
    currentChar = in.get();
    while(isspace(currentChar) && currentChar != '\n') {
        currentChar = in.get();
    }
    in.putback(currentChar);
}

// Checks if identifier is a keyword or not.
LexItem Tokenizer::ident_or_keyword(int& linenum) {
    if(currentLexeme == "and") {
        RETURN_LEXITEM(Token::AND);
    }
    if(currentLexeme == "or") {
        RETURN_LEXITEM(Token::OR);
    }
    if(currentLexeme == "not") {
        RETURN_LEXITEM(Token::NOT);
    }
    if(currentLexeme == "True") {
        RETURN_LEXITEM(Token::TRUE);
    }
    if(currentLexeme == "False") {
        RETURN_LEXITEM(Token::FALSE);
    }
    RETURN_LEXITEM(Token::IDENT);
}

LexItem Tokenizer::GetNextToken(std::istream& in, int& linenum) {
    char c = 0;
    int transition = 0;
    // Clear the lexeme string
    currentLexeme.clear();
    
    // Set DFA start state
    state = TokenizerState::START;
    
    // Skip Whitespace (this calls in.putback if !isspace or is '\n')
    SkipWhitespace(in, linenum, c);
    
    while((c = in.get()) != EOF) {
        // Single-line comment handler
        if(c == '#') {
            while(c != '\n' && c != EOF) {
                c = in.get();
            }
            in.putback(c);
        }

        // DFA for tokenization
        switch(state) {
            case TokenizerState::START:
                transition = 0;
                transition  = (c == '_' || isalpha(c)) << 1;
                transition |= (isdigit(c)) << 2;
                transition |= (c == '.') << 3;
                transition |= (c == '"') << 7;
                transition |= (c == '\'') << 8;

                // Check if transtion is a power of 2
                if(transition && !(transition & (transition - 1))) {
                    currentLexeme += c;
                    state = static_cast<TokenizerState>(transition);
                } else {
                    // Single or double step state transitions
                    currentLexeme += c;
                    switch(c) {
                        case '\n':
                            linenum++;
                            RETURN_LEXITEM(NEWLINE);
                        case '=':
                            if(in.peek() == '=') {
                                currentLexeme += in.get();
                                RETURN_LEXITEM(EQ);
                            }
                            RETURN_LEXITEM(ASSIGN);
                        case '>':
                            if(in.peek() == '=') {
                                currentLexeme += in.get();
                                RETURN_LEXITEM(GEQ);
                            }
                            RETURN_LEXITEM(GT);
                        case '<':
                            if(in.peek() == '=') {
                                currentLexeme += in.get();
                                RETURN_LEXITEM(LEQ);
                            }
                            RETURN_LEXITEM(LT);
                        case '+':
                            RETURN_LEXITEM(PLUS);
                        case '-':
                            RETURN_LEXITEM(MINUS);
                        case '*':
                            if(in.peek() == '*') {
                                currentLexeme += in.get();
                                RETURN_LEXITEM(EXP);
                            }
                            RETURN_LEXITEM(MULT);
                        case '/':
                            if(in.peek() == '/') {
                                currentLexeme += in.get();
                                RETURN_LEXITEM(FLOORDIV);
                            }
                            RETURN_LEXITEM(DIV);
                        case '@':
                            RETURN_LEXITEM(SQRT);
                        case '(':
                            RETURN_LEXITEM(LPAREN);
                        case ')':
                            RETURN_LEXITEM(RPAREN);
                    }
                    state = TokenizerState::ERROR;
                    RETURN_LEXITEM(ERR);
                }
                break;

            case TokenizerState::IN_IDENT_OR_KEYWORD:
                if(c != '_' && !isalnum(c)) {
                    in.putback(c);
                    return ident_or_keyword(linenum);
                }
                currentLexeme += c;
                break;
            case TokenizerState::IN_INTEGER:
                if(!isdigit(c)) {
                    // Check leading zeroes error
                    if(currentLexeme.length() > 1 && currentLexeme[0] == '0') 
                        RETURN_LEXITEM(ERR);
                    
                    // Possible Transitions
                    if(c == 'e' || c == 'E') {
                        state = TokenizerState::IN_REAL_EXP_SIGN;
                    } else if(c == '.') {
                        state = TokenizerState::IN_REAL_INT_DOT;
                    } else {
                        in.putback(c);
                        RETURN_LEXITEM(Token::INTEGER);
                    }
                }
                currentLexeme += c;
                break;
            case TokenizerState::IN_REAL_DOT_INT:
                if(!isdigit(c)) {
                    // Check if there's more than one digit
                    if(currentLexeme.length() == 1) RETURN_LEXITEM(ERR);
                    
                    // Possible Transitions
                    if(c == 'e' || c == 'E') {
                        state = TokenizerState::IN_REAL_EXP_SIGN;
                    } else {
                        in.putback(c);
                        RETURN_LEXITEM(Token::REAL);
                    }
                }
                currentLexeme += c;
                break;
            case TokenizerState::IN_REAL_INT_DOT:
                if(isdigit(c)) {
                    currentLexeme += c;
                } else if(c == 'e' || c == 'E') {
                    state = TokenizerState::IN_REAL_EXP_SIGN;
                    currentLexeme += c;
                } else {
                    in.putback(c);
                    RETURN_LEXITEM(Token::REAL);
                }
                break;
            case TokenizerState::IN_REAL_EXP_SIGN:
                if(c == '+' || c == '-') {
                    currentLexeme += c;
                } else {
                    in.putback(c);
                }
                state = TokenizerState::IN_REAL_EXP;
                break;
            case TokenizerState::IN_REAL_EXP:
                if(!isdigit(c)) {
                    RETURN_LEXITEM(Token::REAL);
                }
                currentLexeme += c;
                break;
            case TokenizerState::IN_STRING_DQUOTE:
                if(c == '\"') {
                    currentLexeme += c;
                    RETURN_LEXITEM(Token::STRING);
                }
                if(c == '\n') {
                    RETURN_LEXITEM(ERR);
                }
                currentLexeme += c;
                break;
            case TokenizerState::IN_STRING_SQUOTE:
                // Really naive way to check if it's a char or slashed char
                if(c == '\'') {
                    currentLexeme += c;
                    if(currentLexeme.length() == 3)
                        RETURN_LEXITEM(Token::CHARACTER);
                    if(currentLexeme.length() == 4 && currentLexeme[1] == '\\')
                        RETURN_LEXITEM(Token::CHARACTER);
                    RETURN_LEXITEM(Token::STRING);
                }
                if(c == '\n') {
                    RETURN_LEXITEM(ERR);
                }
                currentLexeme += c;
                break;
        }
    }

    // Checks for final state
    switch(state) {
        case TokenizerState::START:
            break;
        case TokenizerState::IN_IDENT_OR_KEYWORD:
            return ident_or_keyword(linenum);
        case TokenizerState::IN_INTEGER:
            RETURN_LEXITEM(Token::INTEGER);
        case TokenizerState::IN_REAL_DOT_INT:
        case TokenizerState::IN_REAL_INT_DOT:
        case TokenizerState::IN_REAL_EXP_SIGN:
        case TokenizerState::IN_REAL_EXP:
            RETURN_LEXITEM(Token::REAL);
        case TokenizerState::IN_STRING_DQUOTE:
            if(currentLexeme[currentLexeme.size()-1] == '\"') RETURN_LEXITEM(Token::STRING);
            break;
        case TokenizerState::IN_STRING_SQUOTE:
            if(currentLexeme[currentLexeme.size()-1] == '\'') {
                if(currentLexeme.length() == 3)
                    RETURN_LEXITEM(Token::CHARACTER);
                if(currentLexeme.length() == 4 && currentLexeme[1] == '\\')
                    RETURN_LEXITEM(Token::CHARACTER);
                RETURN_LEXITEM(Token::STRING);
            }
            break;
    }

    if(c == EOF) return LexItem(Token::DONE, "", linenum);

    return LexItem();
}