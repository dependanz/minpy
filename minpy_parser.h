#ifndef h_minpy_parser
#define h_minpy_parser

#include <stack>

#include "minpy_lex.h"
#include "minpy_val.h"
                                
class Parser {
    int linenum;
    bool consoleMode;
    Tokenizer tokenizer;
    LexItem currentToken;
    std::stack<LexItem> tokenBuffer;
    std::map<std::string, Value> variable_map;

    bool debugPrintParserFlag = false;
    bool debugPrintTokenizerFlag = false;
public:
    Parser() : currentToken(), tokenizer(), linenum(0) {}
    Parser(bool consoleMode) : consoleMode(consoleMode), currentToken(), tokenizer(), linenum(0) {}
    
    void toggleDebugPrintParser()    { debugPrintParserFlag    = !debugPrintParserFlag;    }
    void toggleDebugPrintTokenizer() { debugPrintTokenizerFlag = !debugPrintTokenizerFlag; }
    LexItem GetCurrentToken() { return currentToken; }
    int GetLinenum() { return linenum; }
    bool isTokenBufferFull() { return tokenBuffer.size() == 2; }
    void clearTokenBuffer()  {
        currentToken = LexItem();
        while(!tokenBuffer.empty()) { tokenBuffer.pop(); }
    }
    
    void listDefinedVariables();
    void PutbackToken(LexItem lexItem);
    LexItem GetNextToken(std::istream & in);

    /**
     * Non-terminal symbol functions
     */
    bool statement_list      (std::istream & in);
    bool statement           (std::istream & in);
    bool print_statement     (std::istream & in);
    bool assignment_statement(std::istream & in, std::string & lexeme);
    bool variable            (std::istream & in, std::string & lexeme);
    bool expression          (std::istream & in, Value & result);
    bool conjunction         (std::istream & in, Value & result);
    bool disjunction         (std::istream & in, Value & result);
    bool relation            (std::istream & in, Value & result);
    bool addition            (std::istream & in, Value & result);
    bool product             (std::istream & in, Value & result);
    bool exponential         (std::istream & in, Value & result);
    bool unary               (std::istream & in, Value & result);
    bool atomic              (std::istream & in, Value & result);
};

#endif