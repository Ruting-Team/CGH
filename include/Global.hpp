//
//  Global.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/7/3.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef Global_hpp
#define Global_hpp

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

    template <class Character> class Label;
    template<class T> 
    struct hash<Label<T> > {
    public:
        size_t operator()(const Label<T> &label) const {
            return std::hash<T>()(label.upper) ^ std::hash<T>()(label.lower);
        }
    };

}
namespace cgh {
    typedef size_t ID;
    typedef char Flag;
    typedef char Size;
    
    template <class Character> class FA;
    template <class Character> class DFA;
    template <class Character> class SmartDFA;
    template <class Character> class NFA;
    template <class Character> class DFAState;
    template <class Character> class NFAState;
    template <class Character> class PDSTrans;
    template <class Character> class PopPDSTrans;
    template <class Character> class PushPDSTrans;
    template <class Character> class ReplacePDSTrans;

    template <class Character> class Label;
    template <class Character> class NTDState;

    /****************** PDSState ******************/

    class PDSState;
    typedef unordered_set<PDSState*> PDSStateSet;
    
    /****************** Global ******************/

    template <class Character>
    class Global {
    public:

        /***************** Character  *****************/
        
        static Character epsilon;
        typedef vector<Character> Word;
        typedef pair<Character, Character> Char2;
        typedef unordered_set<Character> CharacterSet;
        typedef typename CharacterSet::iterator CharacterSetIter;
        typedef typename CharacterSet::const_iterator CharacterSetConstIter;
        
        /***************** NFAState  *****************/
        
        typedef unordered_set<NFAState<Character>*> NFAStateSet;
        typedef unordered_map<NFAState<Character>*, NFAState<Character>*> NFAState2Map;
        typedef unordered_map<Character, NFAStateSet> NFATransMap;
        typedef unordered_map<NFAState<Character>*, DFAState<Character>*> NFAState2DFAStateMap;
        typedef unordered_map<NFAState<Character>*, NFAStateSet> NFAState2NFAStateSetMap;
        typedef unordered_map<NFAStateSet, DFAState<Character>*> NFAStateSet2DFAStateMap;
        
        typedef typename NFAStateSet::iterator NFAStateSetIter;
        typedef typename NFATransMap::iterator NFATransMapIter;
        typedef typename NFAState2Map::iterator NFAState2MapIter;
        typedef typename NFAState2NFAStateSetMap::iterator NFAState2NFAStateSetMapIter;
        typedef typename NFAStateSet2DFAStateMap::iterator NFAStateSet2DFAStateMapIter;

        typedef typename NFAStateSet::const_iterator NFAStateSetConstIter;
        typedef typename NFATransMap::const_iterator NFATransMapConstIter;
        typedef typename NFAState2NFAStateSetMap::const_iterator NFAState2NFAStateSetMapConstIter;
        
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
        
        typedef typename DFAStateSet::iterator DFAStateSetIter;
        typedef typename DFATransMap::iterator DFATransMapIter;
        typedef typename DFAState2Map::iterator DFAState2MapIter;
        typedef typename DFAStateSetMap::iterator DFAStateSetMapIter;
        typedef typename DFAStatePairMap::iterator DFAStatePairMapIter;
        typedef typename Char2DFAState2Map::iterator Char2DFAState2MapIter;
        typedef typename Char2DFAStateSetMap::iterator Char2DFAStateSetMapIter;
        typedef typename DFAState2NFAStateMap::iterator DFAState2NFAStateMapIter;
        typedef typename DFAState2DFAStateSetMap::iterator DFAState2DFAStateSetMapIter;
        
        typedef typename DFAStateSet::const_iterator DFAStateSetConstIter;
        typedef typename DFATransMap::const_iterator DFATransMapConstIter;
        typedef typename DFAState2Map::const_iterator DFAState2MapConstIter;
        typedef typename DFAStatePairMap::const_iterator DFAStatePairMapConstIter;
        typedef typename DFAStateSetMap::const_iterator DFAStateSetMapConstIter;
        typedef typename Char2DFAState2Map::const_iterator Char2DFAState2MapConstIter;
        typedef typename Char2DFAStateSetMap::const_iterator Char2DFAStateSetMapConstIter;
        typedef typename DFAState2DFAStateSetMap::const_iterator DFAState2DFAStateSetMapConstIter;
        
        /***************** FA  *****************/
        
        typedef list<FA<Character>*> FAList;
        typedef unordered_set<FA<Character>*> FASet;
        typedef unordered_set<DFA<Character>*> DFASet;
        typedef unordered_set<NFA<Character>*> NFASet;
        typedef unordered_map<Character, ID> Char2IDMap;

        typedef typename FASet::iterator FASetIter;
        typedef typename DFASet::iterator DFASetIter;
        typedef typename FAList::iterator FAListIter;

        typedef typename FASet::const_iterator FASetConstIter;
        typedef typename FAList::const_iterator FAListConstIter;
        
        /***************** PDSTrans  *****************/
        
        typedef list<PDSTrans<Character>*> PDSTransList;
        typedef list<PopPDSTrans<Character>*> PopPDSTransList;
        typedef list<PushPDSTrans<Character>*> PushPDSTransList;
        typedef list<ReplacePDSTrans<Character>*> ReplacePDSTransList;
        typedef unordered_map<PDSState*, NFAState<Character>*> PDSState2NFAStateMap;

        /***************** NTDState *****************/

        typedef unordered_set<NTDState<Character>*> NTDStateSet;
        typedef unordered_set<Label<Character>> Labels;
        typedef unordered_map<Character, Labels> Char2LabelsMap;
   };
    
};
#endif /* Global_hpp */

