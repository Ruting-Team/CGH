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
using namespace std;
using namespace cgh;
template<> int Global<int>::epsilon = -1;


template <class T>
class Data
{
private:
    T t;
public:
    Data()
    {
        t = 1;
    }
};

int main(int argc, char const *argv[])
{
    cout << "Hello, CGH!" << endl;
    if(argc > 1)
    {
        string fileName1 = "";
        string fileName2 = "";
        string op = argv[1];
        if(argc == 3)
        {
            fileName1 = argv[2];
            Parser parser(fileName1);
            RawFaData *data = &parser.parse();
            NFA<int> nfa(*data);
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
        }
        else if(argc == 4)
        {
            if(op != "-a")
            {
                fileName1 = argv[2];
                fileName2 = argv[3];
                Parser parser1(fileName1);
                Parser parser2(fileName2);
                RawFaData *data1 = &parser1.parse();
                RawFaData *data2 = &parser2.parse();
                NFA<int> nfa1(*data1);
                NFA<int> nfa2(*data2);
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
            }
            else
            {
                fileName1 = argv[2];
                string str = argv[3];
                Global<int>::Word word;
                for(int i = 0; i < str.size(); i++)
                    word.push_back((int)(str[i] - '0'));
                Parser parser(fileName1);
                RawFaData *data = &parser.parse();
                NFA<int> nfa(*data);
                cout<<"nfa:"<<endl;
                nfa.output();
                cout<<endl;
                cout<<"word:" + str<<endl;
                if(nfa.isAccepted(word)) cout<<"Accepted"<<endl;
                else cout<<"Not Accepted"<<endl;
            }
        }
        else if(argc > 4) 
        {
            if(op == "-mi")
            {
                Global<int>::FASet faSet;
                for(int i = 2; i < argc; i++)
                {
                    Parser parser(argv[i]);
                    RawFaData *data = &parser.parse();
                    NFA<int> *nfa = new NFA<int>(*data);
                    cout<<"nfa" + to_string(i - 1) + ":"<<endl;
                    nfa->output();
                    cout<<endl;
                    faSet.insert(nfa);
                }
                cout<<"multiIntersection:"<<endl;
                DFA<int>& res = (FA<int>::multiIntersection(faSet)).determine().minimize();
                for(typename Global<int>::FASetIter iter = faSet.begin(); iter != faSet.end(); iter++)
                    delete dynamic_cast<NFA<int>*>(*iter);
                res.output();
                if(!res.isEmpty())
                    res.print("res.dot");
            }
            else if(op == "-mu")
            {
                Global<int>::FASet faSet;
                for(int i = 2; i < argc; i++)
                {
                    Parser parser(argv[i]);
                    RawFaData *data = &parser.parse();
                    NFA<int> *nfa = new NFA<int>(*data);
                    cout<<"nfa" + to_string(i - 1) + ":"<<endl;
                    nfa->output();
                    cout<<endl;
                    faSet.insert(nfa);
                }
                cout<<"multiUnion:"<<endl;
                DFA<int>& res = (FA<int>::multiUnion(faSet)).determine().minimize();
                for(typename Global<int>::FASetIter iter = faSet.begin(); iter != faSet.end(); iter++)
                    delete dynamic_cast<NFA<int>*>(*iter);
                res.output();
                if(!res.isEmpty())
                    res.print("res.dot");
            }
            else if(op == "-mc")
            {
                Global<int>::FAList faList;
                for(int i = 2; i < argc; i++)
                {
                    Parser parser(argv[i]);
                    RawFaData *data = &parser.parse();
                    NFA<int> *nfa = new NFA<int>(*data);
                    cout<<"nfa" + to_string(i - 1) + ":"<<endl;
                    nfa->output();
                    cout<<endl;
                    faList.push_back(nfa);
                }
                cout<<"multiConcatination:"<<endl;
                DFA<int>& res = (FA<int>::multiConcatination(faList)).determine().minimize();
                for(typename Global<int>::FAListIter iter = faList.begin(); iter != faList.end(); iter++)
                    delete dynamic_cast<NFA<int>*>(*iter);
                res.output();
                if(!res.isEmpty())
                    res.print("res.dot");
            }
        }
    }
    return 0;
    
}

