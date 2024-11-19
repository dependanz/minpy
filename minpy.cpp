#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "minpy_parser.h"
#include "minpy_builtins.h"

using namespace std;

/**
 *  Console Mode
 */
void minpy_console() {
    char c;
    Parser parser(true);
    bool parserResult = false;
    string current_statement;
    cout << "###########################################" << endl;
    cout << "# minpy: A Pythonic-ish Calculator        #" << endl;
    cout << "#-----------------------------------------#" << endl;
    cout << "# Type 'dir()' to list defined variables. #" << endl;
    cout << "# Type 'quit()' to exit.                  #" << endl;
    cout << "###########################################" << endl << endl;

    while(current_statement != "quit()") {
        cout << CONSOLE_PROMPT;

        // Get next statement from stdin
        current_statement.clear();
        parser.clearTokenBuffer();
        while((c = cin.get()) != '\n') { current_statement += c; }

        if(current_statement == "") continue;

        // Special inputs
        if(current_statement == "import this") {
            cout << IMPORT_THIS_MESSAGE << endl;
        } else if(current_statement == "help()") {
            cout << HELP_MESSAGE << endl;
        } else if(current_statement == "dir()") {
            parser.listDefinedVariables();
        } else if(current_statement != "quit()") {
            // Parse/Interpret statement
            istringstream in(current_statement);
            parserResult = parser.statement(in);

            if(!parserResult) {
                cout << "File \"<stdin:not-really>\", line 1, in <module:not-really>" << endl;
                cout << "RuntimeError: ..." << endl;
            }
        }
    }
}

/**
 *  File Interpreter Mode
 */
int minpy_interpreter(int argc, char** argv) {
    Parser parser(false);
    bool parserResult = false;

    if(argc != 2) {
        cout << USAGE_MESSAGE << endl;
        return 1;
    }

    // Read file
    ifstream inFile(argv[1]);
    if(inFile.fail()) {
        cout << FILE_NOT_FOUND_MESSAGE(argv[1]) << endl;
        return 1;
    }

    parserResult = parser.statement_list(inFile);
    
    inFile.close();
    
    if(!parserResult) {
        cout << "SyntaxError: Error found at line " << parser.GetLinenum() << endl;
        return 1;
    }

    return 0;
}

int main(int argc, char** argv) {
    if(argc == 1) {
        minpy_console();
    } else {
        return minpy_interpreter(argc, argv);
    }
	return 0;
}
