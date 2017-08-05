#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <cassert>
using namespace std;


int VirtualMachine(string machineCodeFile, string relAddFile, string instCodeFile)
{
    std::ifstream codeFile(machineCodeFile), relativeAddrFile(relAddFile), instructionCode(instCodeFile);
    
    if (!codeFile.is_open() || !relativeAddrFile.is_open() || !instructionCode.is_open())
    {
        cout << "Input File Couldn't be opened, try again";
        return 1;
    }
    

    string line, temp;
    int count = 0;
    int index = 0 ;
    
    vector<vector<int>> q;
    
    while (std::getline(codeFile, line))
    {
        istringstream iss(line, istringstream::in);
        index = 0;
        vector<int> row;
        while( iss >> temp )
        {
            row.push_back(atoi(temp.c_str()));
            //            cout<< atoi(temp.c_str()) << ", ";
        }
        //        cout << endl;
        q.push_back(row);
    }
    
    int value;
    std::map<int, string> rA;
    
    while (std::getline(relativeAddrFile, line))
    {
        count++ ;
        istringstream iss(line, istringstream::in);
        
        iss >> temp;
        iss >> value;
        rA[value] = temp;
    }
    
    std::map<std::string, int> iC;
    line = "", temp = "";

    
    enum InstructionCode {
        ADDITION,
        SUBTRACTION,
        MULTIPLICATION,
        DIVISION,
        ASSIGN,
        INPUT,
        OUTPUT,
        IFGE,
        GOTO,
        GT,
        IFLE,
        LT,
        EQ,
        NEQ,
        Last
    };

    
    std::unordered_map<int, InstructionCode> code;

    while (std::getline(instructionCode, line))
    {
        count++ ;
        istringstream iss(line, istringstream::in);
        
        iss >> temp;
        iss >> value;
        iC[temp] = value;
        
        if(temp == "ADDITION")
            code[value] = ADDITION;
        else if(temp == "SUBTRACTION")
            code[value] = SUBTRACTION;
        else if(temp == "MULTIPLICATION")
            code[value] = MULTIPLICATION;
        else if(temp == "DIVISION")
            code[value] = DIVISION;
        else if(temp == "ASSIGN")
            code[value] = ASSIGN;
        else if(temp == "INPUT")
            code[value] = INPUT;
        else if(temp == "OUTPUT")
            code[value] = OUTPUT;
        else if(temp == "IFGE")
            code[value] = IFGE;
        else if(temp == "GOTO")
            code[value] = GOTO;
        else if(temp == "GT")
            code[value] = GT;
        else if(temp == "IFLE")
            code[value] = IFLE;
        else if(temp == "LT")
            code[value] = LT;
        else if(temp == "EQ")
            code[value] = EQ;
        else if(temp == "NEQ")
            code[value] = NEQ;
    }
    
    codeFile.close();
    relativeAddrFile.close();
    instructionCode.close();
    
    
    std::map<std::string, int> ds;
    std::vector<vector<int>>::iterator it = q.begin();
    while (it != q.end())
    {
        vector<int> row = *it;
//        cout << row.at(0) << " => ";
        
        switch (row.at(0))
        {
            case INPUT:
            {
                //                cout << "Input Command\n";
                int relativeAddr = row.at(1);
                string vriableName = rA[relativeAddr];
                cout << "Input Value of Variable: " << vriableName << endl;
                cin>>ds[vriableName];
                break;
            }
            case OUTPUT:
            {
                int relativeAddr = row.at(1);
                string vriableName= rA[relativeAddr];
                cout << "\nOutput Value of Variable " << vriableName << ": "<<ds[vriableName]<<endl;
                break;
            }
            case '+':
            {
                int operand1 = row.at(1), operand2 = row.at(2), ansTarget = row.at(3);
                
                string operand1Name = rA[operand1],operand2Name = rA[operand2], targetName = rA[ansTarget];
      //          cout << targetName << " = " << operand1Name << " + " << operand2Name <<endl;
                ds[targetName] = ds[operand1Name] + ds[operand2Name];
                break;
            }
            case '-':
            {
                int operand1 = row.at(1), operand2 = row.at(2), ansTarget = row.at(3);
                string operand1Name = rA[operand1],operand2Name = rA[operand2], targetName = rA[ansTarget];
      //          cout << targetName << " = " << operand1Name << " - " << operand2Name <<endl;
                ds[targetName] = ds[operand1Name] - ds[operand2Name];
                break;
            }
            case '*':
            {
                int operand1 = row.at(1), operand2 = row.at(2), ansTarget = row.at(3);
                
                string operand1Name = rA[operand1],operand2Name = rA[operand2], targetName = rA[ansTarget];
       //         cout << targetName << " = " << operand1Name << " * " << operand2Name <<endl;
                ds[targetName] = ds[operand1Name] * ds[operand2Name];
                break;
            }
            case '/':
            {
                int operand1 = row.at(1), operand2 = row.at(2), ansTarget = row.at(3);
                string operand1Name = rA[operand1],operand2Name = rA[operand2], targetName = rA[ansTarget];
     //           cout << targetName << " = " << operand1Name << " / " << operand2Name <<endl;
                ds[targetName] = ds[operand1Name] / ds[operand2Name];
                break;
            }
            case '=':
            {
                int operand1 = row.at(1), ansTarget = row.at(2);
                string operand1Name = rA[operand1], targetName = rA[ansTarget];
       //         cout << targetName << " = " << operand1Name << endl;
                ds[targetName] = ds[operand1Name];
                break;
            }
            case GOTO:
            {
                int ansTarget = row.at(1);
        //        cout << " GOTO ::: " << ansTarget << endl;
                ansTarget -= 1 ; // 1 for o indexing, 1 for ++it at end of switch
                it = q.begin();
                while (ansTarget--)
                    ++it;
                break;
            }
            case IFGE:
            {
                int operand1 = row.at(1), operand2 = row.at(2), ansTarget = row.at(3);
                string operand1Name = rA[operand1] ,operand2Name = rA[operand2], targetName = rA[ansTarget];
      //          cout << " IFGE "<< operand1Name << " => " << operand2Name << " then " << ansTarget << endl;
                ansTarget -= 1 ; //same reaason as in GOTO
                if (ds[operand1Name] >= ds[operand2Name])
                {
                    it = q.begin();
                    while (ansTarget--)
                        ++it;
                }
                break;
            }
            case GT:
            {
                int operand1 = row.at(1), operand2 = row.at(2), ansTarget = row.at(3);
                string operand1Name = rA[operand1] ,operand2Name = rA[operand2], targetName = rA[ansTarget];
                cout << " IFGE "<< operand1Name << " > " << operand2Name << " then " << ansTarget << endl;
                ansTarget -= 1 ; //same reaason as in GOTO
                if (ds[operand1Name] > ds[operand2Name])
                {
                    it = q.begin();
                    while (ansTarget--)
                        ++it;
                }
                break;
            }
            case IFLE:
            {
                int operand1 = row.at(1), operand2 = row.at(2), ansTarget = row.at(3);
                string operand1Name = rA[operand1] ,operand2Name = rA[operand2], targetName = rA[ansTarget];
      //          cout << " IFGE "<< operand1Name << " > " << operand2Name << " then " << ansTarget << endl;
                ansTarget -= 1 ; //same reaason as in GOTO
                if (ds[operand1Name] <= ds[operand2Name])
                {
                    it = q.begin();
                    while (ansTarget--)
                        ++it;
                }
                break;
            }
            case LT:
            {
                int operand1 = row.at(1), operand2 = row.at(2), ansTarget = row.at(3);
                string operand1Name = rA[operand1] ,operand2Name = rA[operand2], targetName = rA[ansTarget];
       //         cout << " IFGE "<< operand1Name << " > " << operand2Name << " then " << ansTarget << endl;
                ansTarget -= 1 ; //same reaason as in GOTO
                if (ds[operand1Name] < ds[operand2Name])
                {
                    it = q.begin();
                    while (ansTarget--)
                        ++it;
                }
                break;
            }
            default:
                cout << "DEfAULT => New Instruction\n";
                break;
        }
        ++it;
    }
    
    cout << "**********\n";
    cout << "outputting RS: \n";
    for(auto const &ent1 : ds)
        cout << ent1.first << " " << ent1.second <<"\n";
    
    return 0;
    
    cout << "**********\n";
    for(std::vector<vector<int>>::iterator it = q.begin(); it != q.end(); ++it)
    {
        for (std::vector<int>::iterator inner = it->begin() ; inner != it->end() ; ++inner)
            cout << *inner << " ";
        cout << endl;
    }
    for(auto const &ent1 : rA)
        cout << ent1.first << " " << ent1.second <<"\n";
    
    for(auto const &ent1 : iC)
        cout << ent1.first << " " << ent1.second <<"\n";
    return 0;
}

#endif
