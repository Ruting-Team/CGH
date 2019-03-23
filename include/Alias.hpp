//
//  Alias.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/7/3.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef Alias_hpp
#define Alias_hpp

#include <climits>
#include <math.h>
#include <set>
#include <list>
#include <queue>
#include <stack>
#include <regex>
#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
using namespace std;
namespace std {

    template<typename T, typename U>
    struct hash<pair<T, U> > {
    public:
        size_t operator()(const pair<T, U> &p) const {
            return std::hash<T>()(p.first) ^ std::hash<U>()(p.second);
        }
    };

    template<typename T>
    struct hash<unordered_set<T> > {
    public:
        size_t operator()(const unordered_set<T> &hashSet) const {
            size_t size = 0;
            for (auto item : hashSet) {
                size = size ^ std::hash<T>()(item);
            }
            return size;
        }
    };
}
namespace cgh {
    typedef size_t ID;
    typedef char Flag;
    typedef char Size;
    
    template <class Character> class FA;
    template <class Character> class DFA;
    template <class Character> class NFA;
    template <class Character> class DFAState;
    template <class Character> class NFAState;
    template <class Character> class PDS;
    template <class Character> class PDSTrans;
    template <class Character> class PopPDSTrans;
    template <class Character> class PushPDSTrans;
    template <class Character> class ReplacePDSTrans;

    template <class Character> class Label;
    template <class Character> class FT;
    template <class Character> class NFTState;
    template <class Character> class DFTState;
    template <class Character> class NFT;
    template <class Character> class DFT;

    class PDSState;class Var;

    class Value;
    class Atomic;
    class Conjunction;
    class Condition;
    class Transition;
    class NuXmv;

    typedef vector<Value*> Values;
    typedef vector<Var*> Vars;
    typedef vector<Atomic*> Atomics;
    typedef vector<Conjunction*> DNF;
    typedef vector<Transition*> Transitions;
    typedef vector<Transition*> Transitions;
    typedef vector<Condition*> Conditions;
    
    /****************** Alias4Char ******************/

    template <class Character>
    class Alias4Char {
    public:

        typedef vector<Character> Word;
        typedef vector<Word> Words;
        typedef pair<Character, Character> Char2;
        typedef unordered_set<Character> CharacterSet;
    };
        

    template <class Character>
    class Alias4FA {
    public:
        
        /***************** NFAState  *****************/
        
        typedef unordered_set<NFAState<Character>*> NFAStateSet;
        typedef unordered_map<NFAState<Character>*, NFAState<Character>*> NFAState2Map;
        typedef unordered_map<Character, NFAStateSet> NFATransMap;
        typedef unordered_map<NFAState<Character>*, DFAState<Character>*> NFAState2DFAStateMap;
        typedef unordered_map<NFAState<Character>*, NFAStateSet> NFAState2NFAStateSetMap;
        typedef unordered_map<NFAStateSet, DFAState<Character>*> NFAStateSet2DFAStateMap;
        
        /***************** DFAState  *****************/
        
        typedef pair<DFAState<Character>*, DFAState<Character>*> DFAState2;
        typedef unordered_set<DFAState<Character>*> DFAStateSet;
        typedef unordered_map<Character, DFAState<Character>*> DFATransMap;
        typedef unordered_map<DFAState<Character>*, DFAState<Character>*> DFAState2Map;
        typedef unordered_map<DFAState2, DFAState<Character>*> DFAStatePairMap;
        typedef unordered_map<DFAStateSet, DFAState<Character>*> DFAStateSetMap;
        typedef unordered_map<Character, DFAState2> Char2DFAState2Map;
        typedef unordered_map<DFAState<Character>*, NFAState<Character>*> DFAState2NFAStateMap;
        typedef unordered_map<Character, DFAStateSet> Char2DFAStateSetMap;
        typedef unordered_map<DFAState<Character>*, DFAStateSet> DFAState2DFAStateSetMap;
        
        /***************** FA  *****************/
        
        typedef list<FA<Character>*> FAList;
        typedef unordered_set<FA<Character>*> FASet;
        typedef unordered_set<DFA<Character>*> DFASet;
        typedef unordered_set<NFA<Character>*> NFASet;
        typedef unordered_map<Character, ID> Char2IDMap;

    };

    /****************** Alias4PDS ******************/

    template <class Character>
    class Alias4PDS {
    public:

        typedef unordered_set<PDSState*> PDSStateSet;
        typedef list<PDSTrans<Character>*> PDSTransList;
        typedef list<PopPDSTrans<Character>*> PopPDSTransList;
        typedef list<PushPDSTrans<Character>*> PushPDSTransList;
        typedef list<ReplacePDSTrans<Character>*> ReplacePDSTransList;
        typedef unordered_map<PDSState*, NFAState<Character>*> PDSState2NFAStateMap;
    };

    /****************** Alias4FT ******************/

    template <class Character>
    class Alias4FT {
    public:
        typedef unordered_set<Label<Character> > Labels;
        typedef unordered_map<Character, Labels> Char2LabelsMap;
        typedef unordered_map<Character, DFTState<Character>*> DFTTransMap;
        typedef unordered_set<DFTState<Character>*> DFTStateSet;
        typedef pair<DFTState<Character>*, DFTState<Character>* > DFTState2;
        typedef unordered_map<DFTState2, NFTState<Character>*> DFTStatePairMap;
    };
    
};
#endif /* Alias_hpp */

