#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include <stdlib.h>

#include "lexer.h"
#include "inputbuf.h"
#include "parser.h"

using namespace std;

void Parser::parse_program() {
    currentScope = "::";
    scopes.push(currentScope);
    Token token = lexer.GetToken();
    if (token.token_type == ID) {
        Token token2 = lexer.GetToken();
        if (token2.token_type == COMMA || token2.token_type == SEMICOLON) {
            lexer.UngetToken(token2);
            lexer.UngetToken(token);
            parse_global_vars();
            parse_scope();
        } else if (token2.token_type == LBRACE) {
            lexer.UngetToken(token2);
            lexer.UngetToken(token);
            parse_scope();
        } else {
            Syntax_Error();
        }
    } else {
        Syntax_Error();
    }
}

void Parser::parse_global_vars() {
    Token token = lexer.GetToken();
    if (token.token_type == ID) {
        lexer.UngetToken(token);
        parse_var_list(1);
    } else {
        lexer.UngetToken(token);
        return;
    }
    token=lexer.GetToken();
    if (token.token_type != SEMICOLON) {
        Syntax_Error();
    }
}

void Parser::parse_var_list(int pubpriv) {
    Token token = lexer.GetToken();
    if (token.token_type == ID) {
        Token token2 = lexer.GetToken();
        if (token2.token_type == COMMA) {
            parse_var_list(pubpriv);
        } else {
            lexer.UngetToken(token2);
        }

        struct sTableItem newItem;
        newItem.scope = currentScope;
        newItem.pubpriv = pubpriv;
        newItem.name = token.lexeme;

        struct sTable *newTable = new struct sTable;
        newTable->item = newItem;
        newTable->next = table;
        table = newTable;
    } else {
        Syntax_Error(); 
    }

}

void Parser::parse_scope() {
    Token token = lexer.GetToken();
    Token token2 = lexer.GetToken();
    if (token.token_type == ID && token2.token_type == LBRACE) {
        scopes.push(currentScope);
        currentScope = token.lexeme;
        parse_public_vars();
        parse_private_vars();
        parse_stmt_list();
        currentScope = scopes.top();
        scopes.pop();
        Token token3 = lexer.GetToken();
        if (token3.token_type == RBRACE) {
        } else {
            Syntax_Error();
        }
    } else {
        Syntax_Error();
    }

}

void Parser::parse_public_vars() {
    Token token = lexer.GetToken();
    if (token.token_type == PUBLIC) {
        Token token2 = lexer.GetToken();
        if (token2.token_type == COLON) {
            parse_var_list(1);
            Token token3 = lexer.GetToken();
            if (token3.token_type == SEMICOLON) {
            } else {
                Syntax_Error();
            }
        } else {
            Syntax_Error();
        }
    } else {
        lexer.UngetToken(token);
    }

}

void Parser::parse_private_vars(){
    Token token = lexer.GetToken();
    if (token.token_type == PRIVATE) {
        Token token2 = lexer.GetToken();
        cout << token2.lexeme;
        if (token2.token_type == COLON) {
            parse_var_list(0);
            Token token3 = lexer.GetToken();
            if (token3.token_type == SEMICOLON) {
            } else {
                Syntax_Error();
            }
        } else {
            Syntax_Error();
        }
    } else {
        lexer.UngetToken(token);
    }

}

void Parser::parse_stmt_list() {
    Token token = lexer.GetToken();
    if (token.token_type == ID) {
        Token token2 = lexer.GetToken();
        if (token2.token_type == EQUAL) {
            lexer.UngetToken(token2);
            lexer.UngetToken(token);
            parse_stmt();
        } else if (token2.token_type == LBRACE) {
            lexer.UngetToken(token2);
            lexer.UngetToken(token);
            parse_scope();
        } else {
            Syntax_Error();
        }
        Token token3 = lexer.GetToken();
        if (token3.token_type == ID) {
            lexer.UngetToken(token3);
            parse_stmt_list();
        } else {
            lexer.UngetToken(token3);
        }
    } else {
        Syntax_Error();
    }

}

string Parser::find_scope_by_ID(string id) {
    struct sTable currentCheck = *table;
    while (!(
        (currentCheck.item.name.empty() || currentCheck.next == NULL) ||
        (currentCheck.item.name == id && (currentCheck.item.scope == currentScope || currentCheck.item.pubpriv)))) {
        currentCheck = *currentCheck.next;
    }
    if (!currentCheck.item.name.empty() && currentCheck.item.scope == "::") {
        return "::";
    } else if (!currentCheck.item.name.empty() && currentCheck.item.name == id ) {
        return currentCheck.item.scope + ".";
    } else {
        return "?.";
    }
}

void Parser::parse_stmt() {
    Token token = lexer.GetToken();
    if (token.token_type == ID) {
        Token token2 = lexer.GetToken();
        if (token2.token_type == EQUAL) {
            Token token3 = lexer.GetToken();
            Token token4 = lexer.GetToken();
            if ( token3.token_type == ID && token4.token_type == SEMICOLON) {
                cout << find_scope_by_ID(token.lexeme) << token.lexeme << " = " << find_scope_by_ID(token3.lexeme) << token3.lexeme << endl;
            } else {
                Syntax_Error();
            }
        } else {
            lexer.UngetToken(token2);
            lexer.UngetToken(token);
            parse_scope();
        }
    } else {
        Syntax_Error();
    }

}

void Parser::Syntax_Error() {
    cout << "Syntax Error" << endl;
    exit(1);
}

int main()
{
    Parser parser;
    parser.table = new struct sTable();
    parser.parse_program();
    return 0;
}