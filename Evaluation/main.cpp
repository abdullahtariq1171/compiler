//
//  main.cpp
//  Evaluation
//
//  Created by Muhammad Abdullah on 26/04/2017.
//  Copyright © 2017 Muhammad Abdullah. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include "InputPair.h"
#include <sstream>
#include <vector>
#include "translator.h"
#include "virtualMachine.h"

const string LEXEME_OUTPUT_FILE = "words.txt", MACHINE_CODE_FILE = "machineCode.txt", INSTRUCTION_CODE_FILE = "instructionCodes.txt";

using namespace std;

//LEX STARTS HERE
#define TOKENIZE_REGEX "\\/\\*(\\*(?!\\/)|[^*])*\\*\\/|[/]{2,}.*|[a-zA-Z][a-zA-Z0-9]*|[0-9]+|\\[|\\]|\\(|\\)|\\{|\\}|\\+|\\-|\\*|\\/|==|\\=|<>|<=|<|>=|>|\\;"

const string reserve_words[] = {"void", "int", "char", "return", "while", "if", "else", "cout", "cin"};

enum Lexexe {
    T_INT , T_CHAR, T_VOID ,
    
    T_IF = 256, T_ELSE = 257, T_WHILE = 258, T_RETURN = 259, T_COUT = 260 , T_CIN = 261,
    
    T_ADD = 262 , T_MINUS  =263, T_MULTIPLY = 264 , T_DIVIDE = 265,
    
    T_LT = '<', T_LE = 266, T_GT = '>', T_GE = 267, T_EQ = 268, T_NE = 269,
    
    T_COMMENT = 270,

    T_IDENTIFIER = 300,
    T_NUMERIC = 600,
    
    T_SEMICOLON = ';', T_ASSIGN = '=',
    
    T_L_S_PAREN = '(', T_R_S_PAREN = ')',
    T_L_SQ_PAREN = '[',T_R_SQ_PAREN = ']',
    T_L_L_PAREN = '{',T_R_L_PAREN = '}',
    
    T_UNKNOWN = 0
};

struct token_lexeme{
    Lexexe lexeme;
    union {
        string stringValue[50]; // holds lexeme value if string/identifier
        int intValue; // holds lexeme value if integer
        char puntuation; // holds the puntuation value
    } id;
};


vector<string> split_code(const std::string& input)
{
    vector<string> tokens;
    std::regex r(TOKENIZE_REGEX);
    for(std::sregex_iterator i = std::sregex_iterator(input.begin(), input.end(), r); i != std::sregex_iterator(); ++i)
    {
        smatch m = *i;
        tokens.push_back(m.str());
        //        cout << m.str()<<endl;// << " at position " << m.position() << '\n';
    }
    return tokens;
}

Lexexe find_lexeme(string token)
{
    const char* chr = token.c_str();
    //    std::regex comment{"[\\/]{2,}.*"};
    std::regex comment{"\\/\\*(\\*(?!\\/)|[^*])*\\*\\/|[\\/]{2,}.*"};
    std::regex identifier{"[a-zA-Z][a-zA-Z0-9]*"};
    std::regex numbers{"[0-9]+"};
    std::regex common_char{"\\[|\\]|\\(|\\)|\\{|\\}|\\+|\\-|\\*|\\/|==|\\=|<>|!\\=|<=|<|>=|>|\\:|\\;"};
    
    std::cmatch results;
    if(std::regex_search(chr, results, comment))
    {
        return T_COMMENT;
    }
    else if(std::regex_search(chr, results, identifier))
    {
        //        cout << chr << " : Identifier \n" ;
        if(std::find(std::begin(reserve_words), std::end(reserve_words), token) == std::end(reserve_words))
        {
            return T_IDENTIFIER;
        }
        else if(token == "void") return T_VOID;
        else if(token == "int") return T_INT;
        else if(token == "char") return T_CHAR;
        else if(token == "return") return T_RETURN;
        else if(token == "while") return T_WHILE;
        else if(token == "if") return T_IF;
        else if(token == "else") return T_ELSE;
        else if(token == "cout") return T_COUT;
        else if(token == "cin") return T_CIN;
    }
    else if(std::regex_search(chr, results, numbers))
    {
        //        cout << chr << " : NUMERIC \n";
        return T_NUMERIC;
    }
    else if (std::regex_search(chr, results, common_char))
    {
        if(token.length() == 1)
        {
            switch (token[0])
            {
                case '[':
                    return T_L_SQ_PAREN;
                case ']':
                    return T_R_SQ_PAREN;
                case '(':
                    return T_L_S_PAREN;
                case ')':
                    return T_R_S_PAREN;
                case '{':
                    return T_L_L_PAREN;
                case '}':
                    return T_R_L_PAREN;
                case '+':
                    return T_ADD;
                case '-':
                    return T_MINUS;
                case '*':
                    return T_MULTIPLY;
                case '=':
                    return T_ASSIGN;
                case '<':
                    return T_LT;
                case '>':
                    return T_GT;
                case '/':
                    return T_DIVIDE;
                case ';':
                    return T_SEMICOLON;
                default:
                    return T_UNKNOWN;
            }
        }else if (token.length() == 2)
        {
            if(token == "==")
                return T_EQ;
            else if(token == "<>")
                return T_NE;
            else if(token == "<=")
                return T_LE;
            else if(token == ">=")
                return T_GE;
            else return T_UNKNOWN;
        }
    }
    return T_UNKNOWN;
    
}

//LEX ENDS HERE


//PARSER START HERE

//vector<std::string> id_vector;


//ofstream tree("my_parse_tree.txt");

//int indent = -1 ;
//void indentation(string fun_name){
//    indent++;
//    for (int i = 0 ; i < indent; i++) {
//        tree << "- ";
//    }
//    tree << fun_name <<endl;
//}
//void deindent() {indent--; }
//
//vector<InputPair> tokens;
//std::vector<InputPair>::iterator curr;


//it != identifiers.end(); ++it) {
//    tablefs << *it << "\n";

//void nextToken() { ++curr; }
//
//void match(string rhs)
//{
//    //    cout << "COMPARING " << curr->token <<endl;
//    if(curr == tokens.end()){
//        cout << "END OF INPUT ERROR!!!";
//        exit(1);
//    }
//    
//    if (curr->token == rhs)
//    {
//        string temp = rhs;
//        if (curr->token == "ID" && std::find(id_vector.begin(), id_vector.end(),curr->lexeme) == id_vector.end())
//        {
//            id_vector.push_back(curr->lexeme);
//        }
//        //      cout << "--- "<< id_vector.size() <<" ---> : " << rhs <<endl;
//        nextToken();
//    }else
//    {
//        cout << "ERROR!!! " << curr->token <<"But Expecting: => " << rhs <<endl;
//        exit(1);
//    }
//}


void lexFunction(string inputCode)
{
    //LEX STARTS HERE
    //    std::ifstream ifs("/Users/abdullah/Google Drive/spring'17/Complier Construction/Assignment Sol/A1-lex/A1-lex/files/code1.cm");
    std::ifstream ifs1(inputCode);
    std::string code( (std::istreambuf_iterator<char>(ifs1)), (std::istreambuf_iterator<char>()));
    ifs1.close();
    
    //    cout <<  code;
    ofstream tablefs("table.txt"), lexemefs(LEXEME_OUTPUT_FILE);
    vector<std::string> identifiers;
    vector<string> tokens1 = split_code(code);
    for(std::vector<string>::iterator it = tokens1.begin(); it != tokens1.end(); ++it)
    {
        Lexexe lex =  find_lexeme(*it);
        if(lex == Lexexe::T_COMMENT) //ignore all comments
            continue;
        
        if(lex == Lexexe::T_IDENTIFIER)
        {
            lexemefs << "ID" << ":" << *it << endl;
            if (std::find(identifiers.begin(), identifiers.end(),*it) == identifiers.end())
            {
                identifiers.push_back(*it);
            }
        }
        else if(lex == Lexexe::T_NUMERIC)
        {
            lexemefs << "NUM" << ":" << *it<< endl;
        }
        else if(lex == Lexexe::T_EQ || lex == Lexexe::T_NE || lex == Lexexe::T_LE || lex == Lexexe::T_GE ||
                lex == Lexexe::T_GT || lex == Lexexe::T_LT)
        {
            lexemefs << "RE" << ":" << *it<< endl;
        }
        //const string reserve_words[] = {"void", "int", "char", "return", "while", "if", "else", "cout", "cin"};
        else if(lex == Lexexe::T_INT)
            lexemefs << "INT" << ":^\n";
        else if(lex == Lexexe::T_VOID)
            lexemefs << "VOID" << ":^\n";
        else if(lex == Lexexe::T_CHAR)
            lexemefs << "CHAR" << ":^\n";
        else if(lex == Lexexe::T_RETURN)
            lexemefs << "RETURN" << ":^\n";
        else if(lex == Lexexe::T_WHILE)
            lexemefs << "WHILE" << ":^\n";
        else if(lex == Lexexe::T_IF)
            lexemefs << "IF" << ":^\n";
        else if(lex == Lexexe::T_ELSE)
            lexemefs << "ELSE" << ":^\n";
        else if(lex == Lexexe::T_COUT)
            lexemefs << "COUT" << ":^\n";
        else if(lex == Lexexe::T_CIN)
            lexemefs << "CIN" << ":^\n";
        else if(lex == Lexexe::T_ASSIGN)
            lexemefs << "AS" << ":^\n";
        else if(lex == Lexexe::T_UNKNOWN){
            lexemefs << "UNKNOWN" << ":" << *it<< endl;
        }
        else{
            lexemefs << *it << ":^\n" ;
        }
    }
    for(std::vector<string>::iterator it = identifiers.begin(); it != identifiers.end(); ++it)
        tablefs << *it << "\n";
    tablefs.close(), lexemefs.close();
    
    //LEX ENDS HERE

}


int main(int argc, const char * argv[])
{
    if(argc < 2 )
    {
        cout<<"Input File Missing";
        return 1;
    }

    lexFunction(argv[1]);
    TranslationMachineMain(LEXEME_OUTPUT_FILE);
    VirtualMachine(MACHINE_CODE_FILE, "symbolTable.txt", INSTRUCTION_CODE_FILE);
    return 0;
}

