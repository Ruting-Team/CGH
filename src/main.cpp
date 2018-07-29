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
#include "parser.hpp"

#include <map>

using namespace std;
using namespace cgh;
template<> int Global<int>::epsilon = -1;

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
    if (config["type"] == "int") {
        Parser<int> parser;
        NFA<int>* nfa = parser.parse(fileName);
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
    if (config["type"] == "int") {
        Parser<int> parser;
        NFA<int>& nfa = *(parser.parse(fileName));
        cout<<"nfa:"<<endl;
        nfa.output();
        cout<<endl;
        
        if(op == "-d")
        {
            cout<<"determinate:"<<endl;
            DFA<int> &res = nfa.determine();
            res.output();
            res.print("res.dot");
        }
        else if(op == "-m")
        {
            cout<<"minimize:"<<endl;
            DFA<int> &res = nfa.determine().minimize();
            res.output();
            res.print("res.dot");
        }
        
        delete &nfa;
    }
}


void testBinaryOp(string fileName1, string fileName2, string op)
{
    if (config["type"] == "int") {
        Parser<int> parser;
        
        if(op != "-a")
        {
            NFA<int>& nfa1 = *(parser.parse(fileName1));
            NFA<int>& nfa2 = *(parser.parse(fileName2));
            cout<<"nfa1:"<<endl;
            nfa1.output();
            cout<<"nfa2:"<<endl;
            nfa2.output();
            cout<<endl;
            if(op == "-i")
            {
                cout<<"intersection:"<<endl;
                DFA<int> &res = ((nfa1 & nfa2)).determine().minimize();
                res.output();
                if(!res.isEmpty())
                    res.print("res.dot");
            }
            else if(op == "-u")
            {
                cout<<"union:"<<endl;
                DFA<int> &res = ((nfa1 | nfa2)).determine().minimize();
                res.output();
                if(!res.isEmpty())
                    res.print("res.dot");
            }
            else if(op == "-cat")
            {
                cout<<"concat:"<<endl;
                DFA<int> &res = (nfa1.concat(nfa2)).determine().minimize();
                res.output();
                if(!res.isEmpty())
                    res.print("res.dot");
            }
            
            delete &nfa1;
            delete &nfa2;
        }
        else
        {
            NFA<int>& nfa = *(parser.parse(fileName1));
            string str = fileName2;
            Global<int>::Word word;
            for(int i = 0; i < str.size(); i++)
                word.push_back((int)(str[i] - '0'));
            
            cout<<"nfa:"<<endl;
            nfa.output();
            cout<<endl;
            cout<<"word:" + str<<endl;
            if(nfa.isAccepted(word)) cout<<"Accepted"<<endl;
            else cout<<"Not Accepted"<<endl;
            
            delete &nfa;
        }
    }
}

void testMulOp(vector<string> fileNames, string op)
{
    if (config["type"] == "int")
    {
        Parser<int> parser;
        
        if(op == "-mc")
        {
            // multiple concatenation
            Global<int>::FAList faList;
            for(int i = 0; i < fileNames.size(); i++)
            {
                NFA<int> *nfa = parser.parse(fileNames[i]);
                cout<<"nfa" + to_string(i + 1) + ":"<<endl;
                nfa->output();
                cout<<endl;
                faList.push_back(nfa);
            }
            cout<<"multiConcatination:"<<endl;
            DFA<int>& res = (FA<int>::multiConcatination(faList)).determine().minimize();
            
            // delete
            for(typename Global<int>::FAListIter iter = faList.begin(); iter != faList.end(); iter++)
                delete dynamic_cast<NFA<int>*>(*iter);
            res.output();
            if(!res.isEmpty())
                res.print("res.dot");
        }
        else {
            Global<int>::FASet faSet;
            for(int i = 0; i < fileNames.size(); i++)
            {
                NFA<int> *nfa = parser.parse(fileNames[i]);
                cout<<"nfa" + to_string(i + 1) + ":"<<endl;
                nfa->output();
                cout<<endl;
                faSet.insert(nfa);
            }
            
            if(op == "-mi")
            {
                // multiple intersection
                cout<<"multiIntersection:"<<endl;
                DFA<int>& res = (FA<int>::multiIntersection(faSet)).determine().minimize();
                res.output();
                if(!res.isEmpty())
                    res.print("res.dot");
            }
            else if(op == "-mu")
            {
                // multiple union
                cout<<"multiUnion:"<<endl;
                DFA<int>& res = (FA<int>::multiUnion(faSet)).determine().minimize();
                res.output();
                if(!res.isEmpty())
                    res.print("res.dot");
            }
            // delete 
            for(typename Global<int>::FASetIter iter = faSet.begin(); iter != faSet.end(); iter++)
                delete dynamic_cast<NFA<int>*>(*iter);
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

