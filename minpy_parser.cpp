#include "minpy_parser.h"

void Parser::listDefinedVariables() {
    std::cout << "Currently defined variables and their values:" << std::endl;
    for(auto it = variable_map.begin(); it != variable_map.end(); it++) {
        std::cout << "\t" << it->first << " : " << it->second << std::endl;
    }
}

void Parser::PutbackToken(LexItem lexItem) {
    if(isTokenBufferFull()) {
        throw std::runtime_error("RuntimeError: This specific parser is only allowed at most 2 token buffers.");
    }
    tokenBuffer.push(lexItem);
}

LexItem Parser::GetNextToken(std::istream & in) {
    // If the token buffer is not empty, deque from it instead.
    if(!tokenBuffer.empty()) {
        currentToken = tokenBuffer.top();
        tokenBuffer.pop();
    } else {
        currentToken = tokenizer.GetNextToken(in, linenum);
    }
    if(debugPrintParserFlag) {
        std::cout << "ParserDebug - {" << currentToken.getLexeme() << ", " << currentToken.getToken() << "}" << std::endl;
    }
    return currentToken;
}

bool Parser::statement_list(std::istream & in) {
    LexItem lexItem = GetNextToken(in);
    if(lexItem == Token::NEWLINE) {
        return statement_list(in);
    }
    if(lexItem != DONE) {
        PutbackToken(lexItem);
        if(!statement(in)) {
            return false;
        }

        lexItem = GetNextToken(in);
        if(lexItem == Token::NEWLINE) {
            return statement_list(in);
        }
    }
    return true;
}

bool Parser::statement(std::istream & in) {
    std::string varLexeme;
    LexItem firstLexItem  = GetNextToken(in);
    LexItem secondLexItem = GetNextToken(in);

    // Check if it's an assignment statement
    if(firstLexItem == Token::IDENT && secondLexItem == Token::ASSIGN) {
        varLexeme = firstLexItem.getLexeme();
        PutbackToken(secondLexItem);
        if(!assignment_statement(in, varLexeme)) {
            return false;
        }
        return true;
    }

    // Else it's a "print statement" for an expression
    PutbackToken(secondLexItem);
    PutbackToken(firstLexItem);
    if(!print_statement(in)) {
        return false;
    }

    return true;
}

bool Parser::print_statement(std::istream & in) {
    Value result;
    if(!expression(in, result)) {
        return false;
    }
    
    LexItem lexItem = GetNextToken(in);
    if(lexItem != DONE && lexItem != NEWLINE) {
        std::cout << "PrintError: Invalid expresssion given (only one expression per line allowed)" << std::endl;
        return false;
    }
    PutbackToken(lexItem);
    std::cout << result << std::endl;

    return true;
}

bool Parser::assignment_statement(std::istream & in, std::string & lexeme) {
    if(!variable(in, lexeme)) {
        return false;
    }
    
    // Check equals sign
    LexItem lexItem = GetNextToken(in);
    if(lexItem != Token::ASSIGN) {
        return false;
    }

    // Evaluate expression
    Value result;
    lexItem = GetNextToken(in);
    if(lexItem == Token::NEWLINE || lexItem == Token::DONE){
        return false;
    }
    PutbackToken(lexItem);
    if(!expression(in, result)) {
        return false;
    }

    lexItem = GetNextToken(in);
    if(lexItem != DONE && lexItem != NEWLINE) {
        std::cout << "AssignmentError: Invalid assignment statement (only one statement per line allowed)" << std::endl;
        return false;
    }
    PutbackToken(lexItem);

    // Assign to variable (calls the copy constructor)
    variable_map[lexeme] = result;

    return true;
}

bool Parser::variable(std::istream & in, std::string & lexeme) {
    // Entering here, we assume the current token is IDENT
    
    // Declare if not in the variable map
    if(variable_map.find(lexeme) == variable_map.end()) {
        variable_map[lexeme] = Value();
    }

    return true;
}

bool Parser::expression(std::istream & in, Value & result) {
    // An expression simply starts with a valid conjunction
    if(!conjunction(in, result)) {
        return false;
    }
    return true;
}

bool Parser::conjunction(std::istream & in, Value & result) {
    Value valA, valB;
    bool hasSyntaxError = !disjunction(in,valA);
    result = valA;
    if(hasSyntaxError) return false;

    LexItem lexItem = GetNextToken(in);
    if(lexItem == AND) {
        if(!conjunction(in, valB)) {
            result = valB;
            return false;
        }
        result = result && valB;
        if(result.IsError()) {
            return false;
        }
    } else {
        PutbackToken(lexItem);
    }

    return true;
}

bool Parser::disjunction(std::istream & in, Value & result) {
    Value valA, valB;
    bool hasSyntaxError = !relation(in,valA);
    result = valA;
    if(hasSyntaxError) return false;

    LexItem lexItem = GetNextToken(in);
    if(lexItem == OR) {
        if(!disjunction(in, valB)) {
            result = valB;
            return false;
        }
        result = result || valB;
        if(result.IsError()) {
            return false;
        }
    } else {
        PutbackToken(lexItem);
    }

    return true;
}

bool Parser::relation(std::istream & in, Value & result) {
    Value valA, valB;
    bool hasSyntaxError = !addition(in,valA);
    result = valA;
    if(hasSyntaxError) return false;

    LexItem lexItem = GetNextToken(in);
    Token token = lexItem.getToken();
    if(token >= Token::EQ && token <= Token::LT) {
        if(!addition(in, valB)) {
            result = valB;
            return false;
        }
        switch(token) {
            case Token::EQ:
                result = result == valB;
                break;
            case Token::GEQ:
                result = result >= valB;
                break;
            case Token::LEQ:
                result = result <= valB;
                break;
            case Token::GT:
                result = result > valB;
                break;
            case Token::LT:
                result = result < valB;
                break;
        }
        if(result.IsError()) {
            return false;
        }
    } else {
        PutbackToken(lexItem);
    }

    return true;
}

bool Parser::addition(std::istream & in, Value & result) {
    Value valA, valB;
    bool hasSyntaxError = !product(in,valA);
    result = valA;
    if(hasSyntaxError) return false;

    LexItem lexItem = GetNextToken(in);
    if(lexItem == Token::PLUS || lexItem == Token::MINUS) {
        if(!addition(in, valB)) {
            result = valB;
            return false;
        }
        if(lexItem == Token::PLUS) {
            result = valA + valB;
        } else {
            result = valA - valB;
        }

        if(result.IsError()) {
            return false;
        }
    } else {
        PutbackToken(lexItem);
    }
    return true;
}

bool Parser::product(std::istream & in, Value & result) {
    Value valA, valB;
    bool hasSyntaxError = !exponential(in,valA);
    result = valA;
    if(hasSyntaxError) return false;

    LexItem lexItem = GetNextToken(in);
    if(lexItem == Token::MULT || lexItem == Token::DIV || lexItem == Token::FLOORDIV) {
        if(!product(in, valB)) {
            result = valB;
            return false;
        }
        if(lexItem == Token::MULT) {
            result = valA * valB;
        } else if(lexItem == Token::DIV) {
            result = valA / valB;
        } else {
            result = (valA / valB).floor();
        }

        if(result.IsError()) {
            return false;
        }
    } else {
        PutbackToken(lexItem);
    }
    return true;
}

bool Parser::exponential(std::istream & in, Value & result) {
    Value valA, valB;
    bool hasSyntaxError = !unary(in,valA);
    result = valA;
    if(hasSyntaxError) return false;

    LexItem lexItem = GetNextToken(in);
    if(lexItem == Token::EXP) {
        if(!exponential(in, valB)) {
            result = valB;
            return false;
        }
        result = valA.pow(valB);

        if(result.IsError()) {
            return false;
        }
    } else {
        PutbackToken(lexItem);
    }
    return true;
}

bool Parser::unary(std::istream & in, Value & result) {
    Value valA;
    std::stack<Token> unaryStack;
    
    LexItem lexItem = GetNextToken(in);
    while(lexItem == Token::PLUS || lexItem == Token::MINUS || lexItem == Token::NOT || lexItem == Token::SQRT) {
        unaryStack.push(lexItem.getToken());
        lexItem = GetNextToken(in);
    }
    PutbackToken(lexItem);

    if(!atomic(in, valA)) {
        return false;
    }
    result = valA;

    // Apply unary operator queue
    while(!unaryStack.empty()) {
        switch(unaryStack.top()) {
            case Token::PLUS:
                result = +result;
                break;
            case Token::MINUS:
                result = -result;
                break;
            case Token::NOT:
                result = !result;
                break;
            case Token::SQRT:
                result = result.sqrt();
                break;
        }
        unaryStack.pop();
    }

    return true;
}

bool Parser::atomic(std::istream & in, Value & result) {
    LexItem lexItem = GetNextToken(in);
    switch(lexItem.getToken()) {
        case Token::IDENT:
            if(variable_map.find(lexItem.getLexeme()) == variable_map.end()) {
                result = Value();
                return false;
            }
            result = variable_map[lexItem.getLexeme()];
            break;
        case Token::INTEGER:
            result = Value(stoi(lexItem.getLexeme()));
            break;
        case Token::REAL:
            result = Value(stod(lexItem.getLexeme()));
            break;
        case Token::STRING:
            if(lexItem.getLexeme().size() == 2) {
                result = Value("");
            } else {
                result = Value(lexItem.getLexeme().substr(1, lexItem.getLexeme().size() - 2).c_str());
            }
            break;
        case Token::CHARACTER:
            if(lexItem.getLexeme().size() == 4) {
                switch(lexItem.getLexeme()[2]) {
                    case 'n':
                        result = Value('\n');
                        break;
                    case 't':
                        result = Value('\t');
                        break;
                    default:
                        result = Value('#');
                }
            } else {
                result = Value(lexItem.getLexeme()[1]);
            }
            break;
        case Token::TRUE:
            result = Value(true);
            break;
        case Token::FALSE:
            result = Value(false);
            break;
        case Token::LPAREN:
            if(!expression(in, result)) {
                return false;
            }
            lexItem = GetNextToken(in);
            if(lexItem != Token::RPAREN) {
                return false;
            }
            break;
        default:
            result = Value();
            return false;
    }
    return true;
}