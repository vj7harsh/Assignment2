#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>

#include "lexer.h"
#include "inputbuf.h"

using namespace std;

string reserved[] = { "END_OF_FILE",
    "PUBLIC", "PRIVATE", "EQUAL",
    "COLON", "COMMA", "SEMICOLON",
    "LBRACE", "RBRACE", "ID", "ERROR"
};

#define KEYWORDS_COUNT 5
string keyword[] = { "public", "private" };

LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    tmp.token_type = ERROR;
}

Token LexicalAnalyzer::SkipComment()
{
    char c;
    inputBuffer.GetChar(c);
    while (!inputBuffer.EndOfInput() && c != '\n') {
        inputBuffer.GetChar(c);
    }
    line_no++;

    if (!inputBuffer.EndOfInput()) {
        inputBuffer.UngetChar(c);
    }
    return GetToken();
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    inputBuffer.GetChar(c);
    line_no += (c == '\n');

    while (!inputBuffer.EndOfInput() && isspace(c)) {
        space_encountered = true;
        inputBuffer.GetChar(c);
        line_no += (c == '\n');
    }

    if (!inputBuffer.EndOfInput()) {
        inputBuffer.UngetChar(c);
    }
    return space_encountered;
}

bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return (TokenType) (i + 1);
        }
    }
    return ERROR;
}

Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    inputBuffer.GetChar(c);

    if (isalpha(c)) {
        tmp.lexeme = "";
        while (!inputBuffer.EndOfInput() && isalnum(c)) {
            tmp.lexeme += c;
            inputBuffer.GetChar(c);
        }
        if (!inputBuffer.EndOfInput()) {
            inputBuffer.UngetChar(c);
        }
        tmp.line_no = line_no;
        if (IsKeyword(tmp.lexeme))
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        else
            tmp.token_type = ID;
    } else {
        if (!inputBuffer.EndOfInput()) {
            inputBuffer.UngetChar(c);
        }
        tmp.lexeme = "a";
        tmp.token_type = ERROR;
    }
    return tmp;
}

void LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);
}

Token LexicalAnalyzer::GetToken()
{
    char c;
    if (!tokens.empty()) {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    inputBuffer.GetChar(c);
    switch (c) {
        case '/':
            char a;
            inputBuffer.GetChar(a);
            if (a == '/') {
                return SkipComment();
            } else {
                inputBuffer.UngetChar(a);
            }
        case '=':
            tmp.token_type = EQUAL;
            return tmp;
        case ':':
            tmp.token_type = COLON;
            return tmp;
        case ',':
            tmp.token_type = COMMA;
            return tmp;
        case ';':
            tmp.token_type = SEMICOLON;
            return tmp;
        case '{':
            tmp.token_type = LBRACE;
            return tmp;
        case '}':
            tmp.token_type = RBRACE;
            return tmp;
        case 'p':
            char c1, c2, c3, c4, c5, c6; 
            inputBuffer.GetChar(c1); 
            inputBuffer.GetChar(c2); 
            inputBuffer.GetChar(c3); 
            inputBuffer.GetChar(c4); 
            inputBuffer.GetChar(c5);
            inputBuffer.GetChar(c6);
            if (c1 == 'u' && c2 == 'b' && c3 == 'l' && c4 == 'i' && c5 == 'c') {
                tmp.token_type = PUBLIC;
                inputBuffer.UngetChar(c6);
                return tmp;
            } else if (c1 == 'r' && c2 == 'i' && c3 == 'v' && c4 == 'a' && c5 == 't' && c6 == 'e') {
                tmp.token_type = PRIVATE;
                return tmp;
            } else {
                inputBuffer.UngetChar(c6);
                inputBuffer.UngetChar(c5); 
                inputBuffer.UngetChar(c4); 
                inputBuffer.UngetChar(c3); 
                inputBuffer.UngetChar(c2); 
                inputBuffer.UngetChar(c1);
            }

        default:
            if (isdigit(c)) {
                inputBuffer.UngetChar(c);
            } else if (isalpha(c)) {
                inputBuffer.UngetChar(c);
                return ScanIdOrKeyword();
            } else if (inputBuffer.EndOfInput())
                tmp.token_type = END_OF_FILE;
            else
                tmp.token_type = ERROR;
            return tmp;
    }
}
