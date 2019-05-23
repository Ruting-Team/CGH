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
#include <tuple>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
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
    template <class Character> class TrPDS;
    template <class Character> class TrPDSTrans;
    template <class Character> class PopTrPDSTrans;
    template <class Character> class PushTrPDSTrans;
    template <class Character> class ReplaceTrPDSTrans;

    template <class Character> class Label;
    template <class Character> class FT;
    template <class Character> class NFTState;
    template <class Character> class DFTState;
    template <class Character> class NFT;
    template <class Character> class DFT;
    template <class Character> class TrNFA;
    template <class Character> class TrNFAState;

    template <class Character> class Parser;
    template <class Character> class NFAParser;
    template <class Character> class NFTParser;

    class ErrorReport;

    class PDSState;
    class TrPDSState;

    class Var;
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
        typedef unordered_set<Character> Characters;
    };
        

    template <class Character>
    class Alias4FA {
    public:
        
        /***************** NFAState  *****************/
        
        typedef unordered_set<NFAState<Character>*> NFAStates;
        typedef unordered_map<NFAState<Character>*, NFAState<Character>*> NFAState2Map;
        typedef unordered_map<Character, NFAStates> NFATransMap;
        typedef unordered_map<NFAState<Character>*, DFAState<Character>*> NFAState2DFAStateMap;
        typedef unordered_map<NFAState<Character>*, NFAStates> NFAState2NFAStatesMap;
        typedef unordered_map<NFAStates, DFAState<Character>*> NFAStates2DFAStateMap;

        typedef pair<NFAState<Character>*, Character> StateChar;
        typedef pair<NFAState<Character>*, typename Alias4Char<Character>::Char2> StateChar2;
        typedef set<StateChar> StateChars;
        typedef set<StateChar2> StateChar2s;
        typedef unordered_map<Character, StateChars> Char2StateCharsMap;
        typedef unordered_map<Character, StateChar2s> Char2StateChar2sMap;
        typedef unordered_map<NFAState<Character>*, Char2StateCharsMap> NeedMap;
        typedef unordered_map<NFAState<Character>*, Char2StateChar2sMap> Need2Map;
        typedef unordered_map<StateChar, NFAState<Character>*> PostStarMap;
        
        /***************** DFAState  *****************/
        
        typedef pair<DFAState<Character>*, DFAState<Character>*> DFAState2;
        typedef unordered_set<DFAState<Character>*> DFAStates;
        typedef unordered_map<Character, DFAState<Character>*> DFATransMap;
        typedef unordered_map<DFAState<Character>*, DFAState<Character>*> DFAState2Map;
        typedef unordered_map<DFAState2, DFAState<Character>*> DFAStatePairMap;
        typedef unordered_map<DFAStates, DFAState<Character>*> DFAStateSetMap;
        typedef unordered_map<Character, DFAState2> Char2DFAState2Map;
        typedef unordered_map<DFAState<Character>*, NFAState<Character>*> DFAState2NFAStateMap;
        typedef unordered_map<Character, DFAStates> Char2DFAStatesMap;
        typedef unordered_map<DFAState<Character>*, DFAStates> DFAState2DFAStatesMap;
        
        /***************** FA  *****************/
        
        typedef list<const FA<Character>*> FAList;
        typedef vector<const FA<Character>*> FAVec;
        typedef unordered_set<const FA<Character>*> FAs;
        typedef unordered_set<const DFA<Character>*> DFAs;
        typedef unordered_map<Character, ID> Char2IDMap;

    };

    /****************** Alias4PDS ******************/

    template <class Character>
    class Alias4PDS {
    public:

        typedef unordered_set<PDSState*> PDSStates;
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
        typedef typename Alias4Char<Character>::Words Words;
        typedef unordered_set<FT<Character>*> FTs;
        typedef unordered_set<DFT<Character>*> DFTs;
        typedef unordered_set<DFTState<Character>*> DFTStates;
        typedef pair<DFT<Character>*, DFT<Character>*> DFT2;
        typedef pair<DFT<Character>*, Label<Character>> DFTLabelPair;
        typedef pair<DFTState<Character>*, DFTState<Character>* > DFTState2;
        typedef unordered_map<ID, DFTs> ID2DFTsMap;
        typedef unordered_map<DFTState<Character>*, DFTState<Character>*> DFTState2Map;
        typedef unordered_map<DFT2, DFT<Character>*> DFTPairMap;
        typedef unordered_map<Character, DFTState<Character>*> Char2DFTStateMap;
        typedef unordered_map<DFTState<Character>*, Words> DFTState2WordsMap;
        typedef unordered_map<DFTLabelPair, DFT<Character>*> DFTLabel2DFTMap;
        typedef unordered_map<DFTState2, NFTState<Character>*> DFTStatePairMap;
        typedef unordered_map<Character, unordered_set<Label<Character> > > Char2LabelsMap;
        typedef unordered_map<DFT<Character>*, DFT<Character>* > DFT2Map;
    };

    /****************** Alias4TrNFA ******************/

    template <class Character>
    class Alias4TrNFA {
    public:
        typedef unordered_set<TrNFAState<Character>*> TrNFAStates;
        typedef unordered_map<DFT<Character>*, TrNFAStates> DFT2TrNFAStatesMap;
        typedef unordered_map<Character, DFT2TrNFAStatesMap> TrNFATransMap;
        typedef unordered_map<TrNFAState<Character>* , TrNFAState<Character>*> TrNFAState2Map;
        typedef unordered_map<TrPDSState*, TrNFAState<Character>*> TrPDSState2TrNFAStateMap;

    };

    /****************** Alias4TrPDS ******************/

    template <class Character>
    class Alias4TrPDS {
    public:

        typedef unordered_set<TrPDSState*> TrPDSStates;
        typedef list<TrPDSTrans<Character>*> TrPDSTransList;
        typedef list<PopTrPDSTrans<Character>*> PopTrPDSTransList;
        typedef list<PushTrPDSTrans<Character>*> PushTrPDSTransList;
        typedef list<ReplaceTrPDSTrans<Character>*> ReplaceTrPDSTransList;
        typedef unordered_map<TrPDSState*, NFAState<Character>*> TrPDSState2NFAStateMap;
    };

};
#endif /* Alias_hpp */

