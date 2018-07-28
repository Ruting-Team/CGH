//
//  NuXmvSolver.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/7/6.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef NuXmvSolver_hpp
#define NuXmvSolver_hpp

#include <stdio.h>
#include "FA.hpp"
using namespace cgh;
using namespace std;

class Var
{
protected:
    ID id;
public:
    Var() : id(-1) {}
    Var(ID i) : id(i){}
    ID getID() { return id; }
    void setID(ID i) { id = i; }
};
class StateVar : public Var
{
public:
    StateVar() : Var() {}
    StateVar(ID i) : Var(i) {}
    string getStr() { return "s" + to_string(id); }
};

class CharVar : public Var
{
public:
    CharVar() : Var() {}
    CharVar(ID i) : Var(i) {}
    void getStrVec(vector<string>& strVec, Size size)
    {
        int num = 0;
        for(ID i = 0; i < size; i++)
        {
            string str = "a" + to_string(num++);
            if(id & (1 << i)) str = str + " = TRUE";
            else str = str + " = FALSE";
            strVec.push_back(str);
        }
    }
};


class Trans
{
private:
    StateVar sourceVar;
    StateVar targetVar;
    CharVar charVar;
public:
    Trans(StateVar sv, CharVar cv, StateVar tv):sourceVar(sv), charVar(cv), targetVar(tv) {}
    Trans():sourceVar(), charVar(), targetVar() {}
    string getStr(ID id, Size size)
    {
        string preStr = "state" + to_string(id) + " = " + sourceVar.getStr();
        string sufStr = " : " + targetVar.getStr() + " ;";
        vector<string> strVec;
        charVar.getStrVec(strVec, size);
        for(ID i = 0; i < strVec.size(); i++)
            preStr = preStr + " & " + strVec[i];
        preStr = preStr + sufStr;
        return preStr;
    }
};


class Solver
{
protected:
    ID id;
    list<Trans*> transList;
    list<StateVar> stateVarList;
    list<StateVar> finalStateVarList;
    StateVar initialStateVar;
public:
    Solver() : id(-1) {}
    Solver(ID i) : id(i) {}
    ~Solver()
    {
        for(list<Trans*>::iterator it = transList.begin(); it != transList.end(); it++)
            delete *it;
    }
    
};

template <class Character>
class FASolver : public Solver
{
    typedef FA<Character> FA;
    typedef DFA<Character> DFA;
    typedef NFA<Character> NFA;
    typedef Global<Character> Global;
    typedef typename Global::Char2IDMap Char2IDMap;
    typedef typename Global::CharacterSet CharacterSet;
    typedef typename Global::NFAStateSet NFAStateSet;
    typedef typename Global::DFAStateSet DFAStateSet;
    typedef typename Global::NFATransMap NFATransMap;
    typedef typename Global::DFATransMap DFATransMap;
    typedef typename Global::NFAStateSetIter NFAStateSetIter;
    typedef typename Global::DFAStateSetIter DFAStateSetIter;
    typedef typename Global::NFATransMapIter NFATransMapIter;
    typedef typename Global::DFATransMapIter DFATransMapIter;
    typedef typename Global::CharacterSetIter CharacterSetIter;
private:
    list<Trans*> initialTransList;
    list<Var> charVarList;
    Char2IDMap char2IDMap;
    Size size;
private:
    void addTrans(Trans* trans) { transList.push_back(trans); }
    void addStateVar(StateVar stateVar) { stateVarList.push_back(stateVar); }
    void addFinalStateVar(StateVar stateVar) { finalStateVarList.push_back(stateVar); }
    void mkChar2IDMap(CharacterSet& charSet)
    {
        size = (ID) log10(charSet.size()) / log10(2);
        ID num = 0;
        for(CharacterSetIter it = charSet.begin(); it != charSet.end(); it++)
            char2IDMap[*it] = num++;
    }
    
    void loadTransAndStateVar(NFA* nfa)
    {
        NFAStateSet &stateSet = nfa->getStateSet();
        for(NFAStateSetIter it = stateSet.begin(); it != stateSet.end(); it++)
        {
            StateVar stateVar((*it)->getID());
            addStateVar(stateVar);
            NFATransMap &map = (*it)->getNFATransMap();
            for(NFATransMapIter mIt = map.begin(); mIt != map.end(); mIt++)
            {
                NFAStateSet &set = mIt->second;
                for(NFAStateSetIter sIt = set.begin(); it != set.end(); it++)
                    addTrans(new Trans(stateVar, CharVar(char2IDMap[mIt->first]), StateVar((*sIt)->getID())));
            }
        }
    }
    void loadFinalStateVar(NFA* nfa)
    {
        NFAStateSet &finalStateSet = nfa->getFinalStateSet();
        for(NFAStateSetIter it = finalStateSet.begin(); it != finalStateSet.end(); it++)
            addFinalStateVar(StateVar((*it)->getID()));
    }
    
    void loadTransAndStateVar(NFA* nfa, Char2IDMap& char2IDMap)
    {
        NFAStateSet &stateSet = nfa->getStateSet();
        for(NFAStateSetIter it = stateSet.begin(); it != stateSet.end(); it++)
        {
            StateVar stateVar((*it)->getID());
            addStateVar(stateVar);
            NFATransMap &map = (*it)->getNFATransMap();
            for(NFATransMapIter mIt = map.begin(); mIt != map.end(); mIt++)
            {
                NFAStateSet &set = mIt->second;
                for(NFAStateSetIter sIt = set.begin(); sIt != set.end(); sIt++)
                {
                    addTrans(new Trans(stateVar, CharVar(char2IDMap[mIt->first]), StateVar((*sIt)->getID())));
                }
            }
        }
    }
    
    void loadTransAndStateVar(DFA* dfa)
    {
        DFAStateSet &stateSet = dfa->getStateSet();
        for(DFAStateSetIter it = stateSet.begin(); it != stateSet.end(); it++)
        {
            StateVar stateVar((*it)->getID());
            addStateVar(stateVar);
            DFATransMap &map = (*it)->getDFATransMap();
            for(DFATransMapIter mIt = map.begin(); mIt != map.end(); mIt++)
                addTrans(new Trans(stateVar, CharVar(char2IDMap[mIt->first]), StateVar(mIt->second->getID())));
            
        }
    }
    void loadFinalStateVar(DFA* dfa)
    {
        DFAStateSet &finalStateSet = dfa->getFinalStateSet();
        for(DFAStateSetIter it = finalStateSet.begin(); it != finalStateSet.end(); it++)
            addFinalStateVar(StateVar((*it)->getID()));
    }
    
    void loadTransAndStateVar(DFA* dfa, Char2IDMap& char2IDMap)
    {
        DFAStateSet &stateSet = dfa->getStateSet();
        for(DFAStateSetIter it = stateSet.begin(); it != stateSet.end(); it++)
        {
            StateVar stateVar((*it)->getID());
            addStateVar(stateVar);
            DFATransMap &map = (*it)->getDFATransMap();
            for(DFATransMapIter mIt = map.begin(); mIt != map.end(); mIt++)
                addTrans(new Trans(stateVar, CharVar(char2IDMap[mIt->first]), StateVar(mIt->second->getID())));
        }
    }
public:
    FASolver() : Solver() {}
    FASolver(ID i) : Solver(i) {}
    FASolver(ID i, NFA* nfa, Char2IDMap& char2IDMap) : Solver(i)
    {
        initialStateVar.setID(nfa->getInitialState()->getID());
        loadTransAndStateVar(nfa, char2IDMap);
        loadFinalStateVar(nfa);
    }
    
    FASolver(ID i, NFA* nfa) : Solver(i)
    {
        initialStateVar.setID(nfa->getInitialState()->getID());
        mkChar2IDMap(nfa->getAlphabet());
        loadTransAndStateVar(nfa, char2IDMap);
        loadFinalStateVar(nfa);
        
    }
    
    FASolver(ID i, DFA* dfa, Char2IDMap& char2IDMap) : Solver(i)
    {
        initialStateVar.setID(dfa->getInitialState()->getID());
        loadTransAndStateVar(dfa, char2IDMap);
        loadFinalStateVar(dfa);
    }
    
    FASolver(ID i, DFA* dfa) : Solver(i)
    {
        initialStateVar.setID(dfa->getInitialState()->getID());
        mkChar2IDMap(dfa->getAlphabet());
        loadTransAndStateVar(dfa, char2IDMap);
        loadFinalStateVar(dfa);
    }
    
    ~FASolver() {}
    
    string getStateStr() { return "state" + to_string(id); }
    string getStateVar()
    {
        string str = getStateStr() + " : {";
        for(list<StateVar>::iterator it = stateVarList.begin(); it != stateVarList.end(); it++)
            str = str + " " + (*it).getStr() + ",";
        str = str + " p };";
        return str;
    }
    
    string getInitialStateVar()
    {
        return "init(" + getStateStr() + ") := " + initialStateVar.getStr() + ";";
    }
    
    string getFinalStateVar()
    {
        string stateStr = getStateStr();
        string str = "(" + stateStr + " = " + finalStateVarList.front().getStr();
        for(list<StateVar>::iterator it = ++finalStateVarList.begin(); it != finalStateVarList.end(); it++)
            str = str + " | " + stateStr + " = " + (*it).getStr();
        str = str + ")";
        return str;
    }
    
    void getCharVar(vector<string>& strVec)
    {
        for(list<Var>::iterator it = charVarList.begin(); it != charVarList.end(); it++)
        {
            string str = "a" + to_string((*it).getID()) + " : boolean;";
            strVec.push_back(str);
        }
    }
    
    void getTrans(vector<string>& strVec, Size size)
    {
        strVec.push_back("next(" + getStateStr() + ") := case");
        for(list<Trans*>::iterator it = transList.begin(); it != transList.end(); it++)
            strVec.push_back((*it)->getStr(id, size));
        strVec.push_back("TRUE : p;");
        strVec.push_back("esac;");
    }
    
    void getTrans(vector<string>& strVec)
    {
        strVec.push_back("next(" + getStateStr() + ") := case");
        for(list<Trans*>::iterator it = transList.begin(); it != transList.end(); it++)
            strVec.push_back((*it)->getStr(id, size));
        strVec.push_back("TRUE : p");
        strVec.push_back("esac;");
    }
};

template <class Character>
class NuXmvSolver
{
    typedef Global<Character> Global;
    typedef DFA<Character> DFA;
    typedef NFA<Character> NFA;
    typedef FASolver<Character> FASolver;
    typedef typename Global::CharacterSet CharacterSet;
    typedef typename Global::Char2IDMap Char2IDMap;
    typedef typename Global::CharacterSetIter CharacterSetIter;
    typedef typename Global::FASet FASet;
    typedef typename Global::FAList FAList;
    typedef typename Global::FASetIter FASetIter;
    typedef typename Global::FAListIter FAListIter;
private:
    vector<string> VAR;
    vector<string> ASSIGN_INI;
    vector<string> ASSIGN_NEXT;
    vector<string> INVARSPEC;
    vector<string> SMV;
private:
    void loadFASolver(FASolver& faSolver)
    {
        loadVAR(faSolver);
        loadASSIGN_INI(faSolver);
        loadASSIGN_NEXT(faSolver);
        loadINVARSPEC(faSolver);
    }
    
    void loadFASolver(FASolver& faSolver, Size size)
    {
        loadVAR(faSolver);
        loadASSIGN_INI(faSolver);
        loadASSIGN_NEXT(faSolver, size);
        loadINVARSPEC(faSolver);
    }
    
    void loadVAR(FASolver& faSolver)
    {
        VAR.push_back(faSolver.getStateVar());
    }
    void loadASSIGN_INI(FASolver& faSolver)
    {
        ASSIGN_INI.push_back(faSolver.getInitialStateVar());
    }
    void loadASSIGN_NEXT(FASolver& faSolver)
    {
        vector<string> strVec;
        faSolver.getTrans(strVec);
        ASSIGN_NEXT.insert(ASSIGN_NEXT.end(), strVec.begin(), strVec.end());
    }
    void loadASSIGN_NEXT(FASolver& faSolver, Size size)
    {
        vector<string> strVec;
        faSolver.getTrans(strVec, size);
        ASSIGN_NEXT.insert(ASSIGN_NEXT.end(), strVec.begin(), strVec.end());
    }
    void loadINVARSPEC(FASolver& faSolver)
    {
        INVARSPEC.push_back(faSolver.getFinalStateVar());
    }
    void getSMV(vector<string>& strVec)
    {
        strVec.push_back("MODULE main");
        strVec.push_back("VAR");
        for(int i = 0; i < VAR.size(); i++)
            strVec.push_back(VAR[i]);
        strVec.push_back("ASSIGN");
        for(int i = 0; i < ASSIGN_INI.size(); i++)
            strVec.push_back(ASSIGN_INI[i]);
        for(int i = 0; i < ASSIGN_NEXT.size(); i++)
            strVec.push_back(ASSIGN_NEXT[i]);
        strVec.push_back("INVARSPEC");
        string str = "!(" + INVARSPEC[0];
        for(int i = 1; i < INVARSPEC.size(); i++)
            str = str + " & " + INVARSPEC[i];
        str = str + ");";
        strVec.push_back(str);
    }
public:
    NuXmvSolver(FASet& faSet, CharacterSet& charSet)
    {
        Size size = (ID) ceil((log10(charSet.size()) / log10(2)));
        ID num = 0;
        Char2IDMap char2IDMap;
        CharacterSetIter it = charSet.begin();
        for(ID i = 0; (i < (1 << size)) & (it != charSet.end()); i++)
            char2IDMap[*it++] = i;
        for(FASetIter it = faSet.begin(); it != faSet.end(); it++)
        {
            if((*it)->isDeterminate())
            {
                FASolver faSolver(num++, &(*it)->determine(), char2IDMap);
                loadFASolver(faSolver, size);
            }
            else
            {
                FASolver faSolver(num++, &(*it)->nondetermine(), char2IDMap);
                loadFASolver(faSolver, size);
            }
        }
        for(ID i = 0; i < size; i++)
            VAR.push_back("a" + to_string(i) + " : boolean;");
        getSMV(SMV);
    }
    int determineEmptiness()
    {
        ofstream out("/Users/iEric/Downloads/CGH-Pro/out.txt");
        if (out.is_open())
        {
            for(int i = 0; i < SMV.size(); i++)
                out << SMV[i] << endl;
            out.close();
        }
        char buf_ps[2048];
        string result;
        FILE* ptr = NULL;
        regex patten_true(".*invariant.+true");
        regex patten_false(".*invariant.+false");
        if((ptr = popen("./nuXmv-1.1.1-Darwin/bin/nuXmv out.txt", "r")) != NULL)
        {
            
            while(fgets(buf_ps, 2048, ptr) != NULL)
                result.append(buf_ps);
            pclose(ptr);
            ptr = NULL;
        }
        system("rm out.txt");
        if(regex_search(result, patten_true)) return 1;
        else if(regex_search(result, patten_false)) return 0;
        else return -1;
    }
    
    
};


#endif /* NuXmvSolver_hpp */
