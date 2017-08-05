#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <algorithm>
#include <list>
#include <unordered_map>
#include "InputPair.h"

enum Type {
    INT = 0 ,CHAR = 1, FUN
};

int line_count = 0 ;

class SymbolTable
{
    class Data {
    public:
        Type type;
        int addr;
        Data(const Type _t,const int _a):type(_t),addr(_a){}
    };
    
    std::unordered_map<std::string, Data*> table;
public:
    SymbolTable()
    {
    }
    
    void add(string symbol, Type type, int addr)
    {
        table[symbol] = new Data(type,addr);
    }
    
    int getAddr(string symbol){
        return table[symbol]->addr;
    }
    
    Type getType(string symbol) {
        return table[symbol]->type;
    }
    
    void printToFile(string fName = "symbolTable.txt")
    {
        ofstream output(fName);
        for(auto i : table)
            output << i.first << "\t" << (i.second)->addr << "\t" << (i.second)->type << "\n";
        output.close();
    }
    
};
SymbolTable symbols;

class Temp {
public:
    static int n;
    string var;
    Temp(SymbolTable &_tbl, int _a, Type _t)
    {
        var = "t" + to_string(n);
        _tbl.add(var, _t, _a);
        n++;
        int temp = _tbl.getAddr(var);
        cout << temp;
    }
};

int Temp::n = 0;

vector<std::string> id_vector;

ofstream tree("my_parse_tree.txt");

vector<vector<string>> machineCode;

int relative_offset = 0 ;


enum OP_CODES  {
    ADDITION = 0, SUBTRACTION = 1, MULTIPLICATION = 2, DIVISION = 3, ASSIGN = 4, INPUT = 5 , OUTPUT = 6,
    IFGE = 7, GOTO = 8, GT = 9, IFLE = 10, LT = 11, EQ = 12, NEQ = 13,  Last};

int indent = -1 ;
void indentation(string fun_name){
    indent++;
    for (int i = 0 ; i < indent; i++) {
        tree << "- ";
    }
    tree << fun_name <<endl;
}
void deindent() {indent--; }

vector<InputPair> tokens;
std::vector<InputPair>::iterator curr;

void backpatch(const int k, const int v)
{
    machineCode.at(k).push_back(to_string(v));
}


void nextToken() { ++curr; }

void match(string rhs)
{
    //    cout << "COMPARING " << curr->token <<endl;
    if(curr == tokens.end()){
        cout << "END OF INPUT ERROR!!!";
        exit(0);
    }
    if (curr->token == rhs)
    {
        string temp = rhs;
        if (curr->token == "ID" && std::find(id_vector.begin(), id_vector.end(),curr->lexeme) == id_vector.end())
        {
            id_vector.push_back(curr->lexeme);
        }
        //      cout << "--- "<< id_vector.size() <<" ---> : " << rhs <<endl;
        nextToken();
    }else
    {
        cout << "ERROR!!! " << curr->token <<"But Expecting: => " << rhs <<endl;
        exit(0);
    }
}



//functions prototypes written at bottom of this file

string E();
string EP(string i);
string T();
string TP(string i);
string F();

void PL();
void T1();
void L();
void PL_();
void OE();
void id_factor(InputPair);
//void RHS();
void arg();
void argP();
void raiseError()
{
    cout << "\n\nUn Expected Token Error\n\n";
    exit(4);
}

OP_CODES RE_opcode(string re)
{
    if(re == ">") return GT;
    if(re == ">=") return IFGE;
    if(re == "<") return LT;
    if(re == "<=") return IFLE;

    if(re == "==") return EQ;
    if(re == "!=") return NEQ;

    return Last;
}

//add Q for this
void P()
{
    indentation("P()");
    
    if (curr->token == "INT" || curr->token == "CHAR")
    {
        //function defination OR variable def
        T1();
        match("ID");
        if (curr->token == "(" ) // function defination
        {
            match("(");
            PL();
            match(")");
            match("{");
            L();
            match("}");
        }else if(curr->token == ";")  // variable without initialization
        {
            match(";");
        }
        P();
    }
    else if (curr->token == "VOID")
    {
        //function defination
        match("VOID");
        match("ID");
        match("(");
        PL();
        match(")");
        match("{");
        L();
        match("}");
        P();
    }else;
    deindent();
}

void T1()
{
    indentation("T1()");
    if (curr->token == "INT")
    {
        match("INT");
    }
    else if(curr->token == "CHAR" )
    {
        match("CHAR");
    }
    else raiseError();
    deindent();
}

void PL()
{
    indentation("PL()");
    /*
     if (curr->token == "VOID")
     {
     match("VOID"); //SPECIAL CHECK TO CATER SAMPLE CODE
     }else
     */
    if (curr->token == "CHAR" || curr->token == "INT")
    {
        T1();
        match("ID");
        PL_();
    }
    else ;
    deindent();
}

void PL_()
{
    indentation("PL_()");
    if (curr->token == ",")
    {
        match(",");
        PL();
    }
    deindent();
}

void L()
{
    indentation("L()");
    if (curr->token == "CIN")
    {
        match("CIN");
        match("RE");
        match("RE");
        InputPair prev = *curr;
        match("ID");
        match(";");
        vector<string> row;
        row.push_back(to_string(INPUT));
        row.push_back(to_string(symbols.getAddr(prev.lexeme)));
        machineCode.push_back(row);
        L();
    }
    else if (curr->token == "COUT")
    {
        match("COUT");
        match("RE");
        match("RE");
        InputPair prev = *curr;
        match("ID");
        match(";");
        vector<string> row;
        row.push_back(to_string(OUTPUT));
        row.push_back(to_string(symbols.getAddr(prev.lexeme)));
        machineCode.push_back(row);
        L();
    }
    else
        if(curr->token == "INT" || curr->token == "CHAR")
        {
            InputPair temp = *curr;
            int relAddr = relative_offset;
            if (curr->token == "INT")
                relative_offset += 4;
            else
                relative_offset += 1;
            
            T1();
            InputPair prev = *curr;
            match("ID");
            if (temp.token == "INT")
                symbols.add(prev.lexeme, INT, relAddr);
            else if(temp.token == "CHAR")
                 symbols.add(prev.lexeme, CHAR, relAddr);
            match(";");
            L();
        }else if (curr->token == "ID")
        {
            InputPair prev = *curr;
            match("ID");
            id_factor(prev);
            L();
        }
        else if (curr->token == "WHILE")
        {
            match("WHILE");
            match("(");
            InputPair prev1 = *curr;
            E();
            InputPair re = *curr;
            match("RE");
            InputPair prev2 = *curr;
            E();
            match(")");
            
            int BE_t = machineCode.size();
            
            vector<string> row;
            row.push_back(to_string(RE_opcode(re.lexeme)));
            row.push_back(to_string(symbols.getAddr(prev1.lexeme)));
            row.push_back(to_string(symbols.getAddr(prev2.lexeme)));
            machineCode.push_back(row);
            
            int BE_f = machineCode.size();
            
            row.clear();
            row.push_back(to_string(GOTO));
            machineCode.push_back(row);
            
            backpatch(BE_t,machineCode.size());
            
            match("{");
            L();
            match("}");
            
            row.clear();
            row.push_back(to_string(GOTO));
            row.push_back(to_string(BE_t));
            machineCode.push_back(row);
            
            backpatch(BE_f,machineCode.size());
            
            L();
        }else if (curr->token == "IF")
        {
            match("IF");
            match("(");
            InputPair prev1 = *curr;
            E();
            InputPair re = *curr;
            match("RE");
            InputPair prev2 = *curr;
            E();
            match(")");
            int BE_t = machineCode.size();
            
            vector<string> row;
            row.push_back(to_string(RE_opcode(re.lexeme)));
            row.push_back(to_string(symbols.getAddr(prev1.lexeme)));
            row.push_back(to_string(symbols.getAddr(prev2.lexeme)));
            machineCode.push_back(row);
            
            int BE_f = machineCode.size();
            
            row.clear();
            row.push_back(to_string(GOTO));
            machineCode.push_back(row);
            
            backpatch(BE_t, machineCode.size());
            
            match("{");
            L();
            match("}");
            
            int S_e = machineCode.size();
            
            row.clear();
            row.push_back(to_string(GOTO));
            machineCode.push_back(row);
            
            backpatch(BE_f, machineCode.size());
            
            OE();
            
            backpatch(S_e, machineCode.size());
            
            L();
        }else if (curr->token == "RETURN")
        {
            match("RETURN");
            E();
            match(";");
            L();
        }else ;
    deindent();
}

void id_factor(InputPair prev)
{
    indentation("id_factor()");
    if (curr->token == "AS")
    {
        //        InputPair prev = *curr;
        match("AS");
        string v = E();
        match(";");
        
        vector<string> row;
        row.push_back(to_string(int('=')));
        row.push_back(to_string(symbols.getAddr(v)));
        row.push_back(to_string(symbols.getAddr(prev.lexeme)));
        machineCode.push_back(row);
    }else if (curr->token == "(")
    {
        match("(");
        arg();
        match(")");
        match(";");
    }
    deindent();
}

void arg()
{
    indentation("arg()");
    if(curr->token == "ID")
    {
        match("ID");
        argP();
    }
    deindent();
}

void argP()
{
    indentation("argP()");
    if(curr->token == ",")
    {
        match(",");
        arg();
    }
    deindent();
}
void OE()
{
    indentation("OE()");
    if (curr->token == "ELSE")
    {
        match("ELSE");
        match("{");
        L();
        match("}");
    }
    deindent();
}

//void RHS()
//{
//    indentation("RHS()");
//    if (curr->token == "LITERAL")
//        match("LITERAL");
//    else if (curr->token == "ID")
//        match("ID");
//    deindent();
//}
//void RE()
//{
//    if(curr->token == "RE")
//        match(<#string rhs#>)
//}






void TranslationMachineMain(string lex_output)
{
    std::ifstream ifs(lex_output);
    string line, temp;
    int count = 0;
    while (std::getline(ifs, line))
    {
        //		cout << line <<endl;
        count++ ;
        //        line.erase(line.end()-1);
        //        line.erase(line.begin());
        //        cout << line;
        istringstream iss(line);
        
        InputPair pair;
        getline(iss, pair.token, ':');
        getline(iss, pair.lexeme, ':');
        tokens.push_back(pair);
        //        cout << " => " << pair.token << ", " <<pair.lexeme<<endl;
    }
    ifs.close();
    
    //    cout << tokens.size()<<endl<<endl;
    curr = tokens.begin();
    
    //    return 0;
    P();
    
    if (curr != tokens.end())
    {
        cout << "Error Occured, curr != tokens.end() !!\n";
        exit(2);
    }
    
    //    for (; curr != tokens.end();)
    //    {
    //        cout << "Token end => " << (*curr).token << " - " << (*curr).lexeme <<endl;
    //        nextToken();
    //    }
    
    /*
     ofstream identifiers("identifiers.txt");
     //    cout << "identifiers.is_open() " << identifiers.is_open() << endl;
     for(std::vector<string>::iterator it = id_vector.begin(); it != id_vector.end(); ++it)
     identifiers << *it << "\n";
     */
    
    tree.close();
    //    identifiers.close();
    
    symbols.printToFile();
    ofstream mCode("machineCode.txt");
    //    cout << "identifiers.is_open() " << identifiers.is_open() << endl;
    for(std::vector<vector<string>>::iterator it = machineCode.begin(); it != machineCode.end(); ++it)
    {
        for (vector<string>::iterator inner = (*it).begin() ; inner != it->end() ; ++inner)
            mCode << *inner << "\t";
        mCode << "\n";
    }
    
    mCode.close();


    ofstream instCod("instructionCodes.txt");
    for (int item = ADDITION; item != Last; ++item)
    {
        switch (item) {
            case ADDITION:
                instCod << "ADDITION " << item << "\n";
                break;
            case SUBTRACTION:
                instCod << "SUBTRACTION " << item << "\n";
                break;
            case MULTIPLICATION:
                instCod << "MULTIPLICATION " << item << "\n";
                break;
            case DIVISION:
                instCod << "DIVISION " << item << "\n";
                break;
            case ASSIGN:
                instCod << "ASSIGN " << item << "\n";
                break;
            case INPUT:
                instCod << "INPUT " << item << "\n";
                break;
            case OUTPUT:
                instCod << "OUTPUT " << item << "\n";
                break;
            case IFGE:
                instCod << "IFGE " << item << "\n";
                break;
            case GOTO:
                instCod << "GOTO " << item << "\n";
                break;
            case GT:
                instCod << "GT " << item << "\n";
                break;
            case IFLE:
                instCod << "IFLE " << item << "\n";
                break;
            case LT:
                instCod << "LT " << item << "\n";
                break;
            case EQ:
                instCod << "EQ " << item << "\n";
                break;
            case NEQ:
                instCod << "NEQ " << item << "\n";
                break;
            default:
                instCod << "dummy " << item << "\n";
                break;
        }
    }
    instCod.close();
}


//expresion related stuff

string E() {
    indentation("E()");
    string i = T();
    string s= EP(i);
    deindent();
    return s;
}

string EP(string i)
{
    string ret;
    indentation("EP()");
    if (curr->token == "+")
    {
        match("+");
        string v = T();
        
        Temp t(symbols, relative_offset, INT);
        relative_offset += 4;
        
        vector<string> row;
        row.push_back(to_string(int('+')));
        row.push_back(to_string(symbols.getAddr(i)));
        row.push_back(to_string(symbols.getAddr(v)));
        row.push_back(to_string(symbols.getAddr(t.var)));
        machineCode.push_back(row);
        
        ret = EP(t.var);
    }else if (curr->token == "-")
    {
        match("-");
        string v = T();
        
        Temp t(symbols, relative_offset, INT);
        relative_offset += 4;
        
        vector<string> row;
        row.push_back(to_string(int('-')));
        row.push_back(to_string(symbols.getAddr(i)));
        row.push_back(to_string(symbols.getAddr(v)));
        row.push_back(to_string(symbols.getAddr(t.var)));
        machineCode.push_back(row);
        
        ret = EP(t.var);
        
    }else
    {
        ret = i ;
    }
    deindent();
    return ret;
}

string T() {
    indentation("T()");
    string i = F();
    string v = TP(i);
    deindent();
    return v;
}

string TP(string i)
{
    string ret = i;
    indentation("TP()");
    if (curr->token == "*")
    {
        match("*");
        string v = F();
        Temp t(symbols, relative_offset, INT);
        relative_offset += 4;
        
        vector<string> row;
        row.push_back(to_string(int('*')));
        row.push_back(to_string(symbols.getAddr(i)));
        row.push_back(to_string(symbols.getAddr(v)));
        row.push_back(to_string(symbols.getAddr(t.var)));
        machineCode.push_back(row);
        
        ret = TP(t.var);
    }else if (curr->token == "/")
    {
        match("/");
        string v = F();
        Temp t(symbols, relative_offset, INT);
        relative_offset += 4;
        vector<string> row;
        row.push_back(to_string(int('/')));
        row.push_back(to_string(symbols.getAddr(i)));
        row.push_back(to_string(symbols.getAddr(v)));
        row.push_back(to_string(symbols.getAddr(t.var)));
        machineCode.push_back(row);
        
        ret = TP(t.var);
    }else
    {
        ret = i;
    }
    deindent();
    return ret;
}

string F()
{
    string ret = "" ;
    indentation("F()");
    if(curr->token == "ID")
    {
        InputPair prev = *curr;
        match("ID");
        ret = prev.lexeme;
    }
    else if (curr->token == "NUM")
    {
        InputPair prev = *curr;
        match("NUM");
        Temp t(symbols, relative_offset, INT);
        ret = t.var;
    }
    else if (curr->token == "(")
    {
        cout << "I AM HERE, COME AND SEE ME";
        int temp;
        cin>>temp;
        ret = E();
        match(")");
    }
    else
    {
        cout << "\nError, terminator required\n";
        exit(0);
    }
    deindent();
    return ret;
}

#endif
