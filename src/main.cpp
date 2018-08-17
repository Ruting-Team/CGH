//
//  main.cpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/6/14.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#include "NFA.hpp"
#include "DFA.hpp"
#include "NuXmvSolver.hpp"
#include "Parser.hpp"

#include <map>

using namespace std;
using namespace cgh;
template<> char Global<char>::epsilon = 0;

map<string, string> config; /* config */

/**
* read configuration form fileName
* @param fileName the configuration file
*/
void readConfig(string fileName)
{
    fstream cfg(fileName);
    string info = fileName + " not found or open failed!";
    if (!cfg.is_open()) {
        cout << info << endl;
        exit(-1);
    }
    string key;
    string val;
    while (cfg >> key) {
        cfg >> val;
        config[key] = val;
    }
    cfg.close();
}

/**
* test construct NFA
*/
void testDump(string fileName) {
    if (config["type"] == "char") {
        Parser<char> parser;
        NFA<char>* nfa = parser.parse(fileName);
        cout<<"nfa:"<<endl;
        nfa->output();
        cout<<endl;
        delete nfa;
    }
}

/**
* test unitary operation
*/
void testUnitaryOp(string fileName, string op) {
    NFA<char> *nfa;
    if(op.find("-reg") != string::npos)
        nfa = new NFA<char>(fileName);
    else
        if(config["type"] == "char")
        {
            Parser<char> parser;
            nfa = parser.parse(fileName);
        }
    cout<<"nfa:"<<endl;
    nfa -> output();
    cout<<endl;
    
    if(op.find("-d") != string::npos)
    {
        cout<<"determinate:"<<endl;
        DFA<char> &res = nfa -> determine();
        res.output();
        res.print("res.dot");
        delete &res;
    }
    else if(op.find("-m") != string::npos)
    {
        cout<<"minimize:"<<endl;
        DFA<char> &temp = nfa -> determine();
        DFA<char> &res = temp.minimize();
        delete &temp;
        res.output();
        res.print("res.dot");
        delete &res;
    }
    delete nfa;
    
}



void testBinaryOp(string fileName1, string fileName2, string op)
{
    if(op.find("-a") == string::npos)
    {
        NFA<char> *nfa1;
        NFA<char> *nfa2;
        if(op.find("-reg") != string::npos)
        {
            nfa1 = new NFA<char>(fileName1);
            nfa2 = new NFA<char>(fileName2);
        }
        else
            if(config["type"] == "char")
            {
                Parser<char> parser;
                nfa1 = parser.parse(fileName1);
                nfa2 = parser.parse(fileName2);
            }
        cout<<"nfa1:"<<endl;
        nfa1 -> output();
        cout<<"nfa2:"<<endl;
        nfa2 -> output();
        cout<<endl;
        if(op == "-i")
        {
            cout<<"intersection:"<<endl;
            DFA<char> &res = ((*nfa1 & *nfa2)).determine().minimize();
            res.output();
            if(!res.isEmpty())
                res.print("res.dot");
            delete &res;
        }
        else if(op == "-u")
        {
            cout<<"union:"<<endl;
            DFA<char> &res = ((*nfa1 | *nfa2)).determine().minimize();
            res.output();
            if(!res.isEmpty())
                res.print("res.dot");
            delete &res;
        }
        else if(op == "-cat")
        {
            cout<<"concat:"<<endl;
            DFA<char> &res = (nfa1 -> concat(*nfa2)).determine().minimize();
            res.output();
            if(!res.isEmpty())
                res.print("res.dot");
            delete &res;
        }
        
        delete nfa1;
        delete nfa2;
    }
    else
    {
        NFA<char> *nfa;
        if(op.find("-reg") != string::npos)
            nfa = new NFA<char>(fileName1);
        else
            if(config["type"] == "char")
            {
                Parser<char> parser;
                nfa = parser.parse(fileName1);
            }
        string str = fileName2;
        Global<char>::Word word;
        for(int i = 0; i < str.size(); i++)
            word.push_back((char)(str[i]));
        
        cout<<"nfa:"<<endl;
        DFA<char> &res = nfa -> determine();
        res.output();
        cout<<endl;
        cout<<"word:" + str<<endl;
        if(res.isAccepted(word)) cout<<"Accepted"<<endl;
        else cout<<"Not Accepted"<<endl;
        delete &res;
        delete nfa;
    }
        
        
}

void testMulOp(vector<string> fileNames, string op)
{
    if (config["type"] == "char")
    {
        Parser<char> parser;
        
        if(op.find("-mc") != string::npos)
        {
            // multiple concatenation
            Global<char>::FAList faList;
            for(int i = 0; i < fileNames.size(); i++)
            {
                NFA<char>* nfa;
                if(op.find("-reg") != string::npos)
                    nfa = new NFA<char>(fileNames[i]);
                else
                    nfa = parser.parse(fileNames[i]);
                cout<<"nfa" + to_string(i + 1) + ":"<<endl;
                nfa->output();
                cout<<endl;
                faList.push_back(nfa);
            }
            cout<<"multiConcatination:"<<endl;
            DFA<char>& res = (FA<char>::multiConcatination(faList)).determine().minimize();
            
            // delete
            for(typename Global<char>::FAListIter iter = faList.begin(); iter != faList.end(); iter++)
                delete dynamic_cast<NFA<char>*>(*iter);
            res.output();
            if(!res.isEmpty())
                res.print("res.dot");
        }
        else {
            Global<char>::FASet faSet;
            for(int i = 0; i < fileNames.size(); i++)
            {
                NFA<char>* nfa;
                if(op.find("-reg") != string::npos)
                    nfa = new NFA<char>(fileNames[i]);
                else
                    nfa = parser.parse(fileNames[i]);
                cout<<"nfa" + to_string(i + 1) + ":"<<endl;
                nfa->output();
                cout<<endl;
                faSet.insert(nfa);
            }
            
            if(op.find("-mi") != string::npos)
            {
                // multiple intersection
                cout<<"multiIntersection:"<<endl;
                DFA<char>& res = (FA<char>::multiIntersection(faSet)).determine().minimize();
                res.output();
                if(!res.isEmpty())
                    res.print("res.dot");
            }
            else if(op.find("-mu") != string::npos)
            {
                // multiple union
                cout<<"multiUnion:"<<endl;
                DFA<char>& res = (FA<char>::multiUnion(faSet)).determine().minimize();
                res.output();
                if(!res.isEmpty())
                    res.print("res.dot");
            }
            // delete 
            for(typename Global<char>::FASetIter iter = faSet.begin(); iter != faSet.end(); iter++)
                delete dynamic_cast<NFA<char>*>(*iter);
        }
    }
}

void printUsage()
{
    cout << "CGH Usage:\n\n";
    cout << "unitary op: ${BIN} ${op} ${ARG}\n";
    cout << "\tsupported unitary op:\n \t\t-d(determination) -m(minimization)\n";
    cout << "binary op: ${BIN} ${op} ${ARG1} ${ARG2}\n";
    cout << "\tsupported op:\n\t\t -i(intersection) -u(union) -cat(concatenation) -a(accept)\n";
    cout << "multiple op: ${BIN} ${op} ${ARG1} ${ARG2} ...\n";
    cout << "\tsupported op:\n\t\t -mi(intersection) -mu(union) -mc(concatenation)\n";
    
    exit(0);
}

int main(int argc, char const *argv[])
{
    cout << "Hello, CGH!" << endl;
    readConfig("cgh.cfg");
    if(argc > 1)
    {
        
        string op = argv[1];
        
        if (op == "-h" || op == "--help")
            printUsage();
        
        string fileName1 = "";
        string fileName2 = "";
        if(argc == 2)
        {
            // test
            fileName1 = argv[1];
            testDump(fileName1);
        }
        else if(argc == 3)
        {
            // test unitary operation
            fileName1 = argv[2];
            testUnitaryOp(fileName1, op);
        }
        else if(argc == 4)
        {
            // test binary operation
            fileName1 = argv[2];
            fileName2 = argv[3];
            testBinaryOp(fileName1, fileName2, op);
        }
        else if(argc > 4)
        {
            // test multiple operation
            vector<string> fileNames;
            for (int i=2; i<argc; i++)
            {
                fileNames.push_back(argv[i]);
            }
            testMulOp(fileNames, op);
        }
    } 
    else {
        printUsage();
    }
    return 0;
    
}

