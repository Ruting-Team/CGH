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
#include <regex>
#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
//#include <fiostream>
using namespace std;
namespace std {
    template<typename T, typename U>
    class hash<pair<T, U> >
    {
    public:
        size_t operator()(const pair<T, U> &p) const {
            return std::hash<T>()(p.first) ^ std::hash<U>()(p.second);
        }
    };
    template<typename T>
    class hash<unordered_set<T> >
    {
    public:
        size_t operator()(const unordered_set<T> &hashSet) const {
            size_t size = 0;
            for(typename unordered_set<T>::const_iterator it = hashSet.begin(); it != hashSet.end(); it++)
                size = size ^ std::hash<T>()(*it);
            return size;
        }
    };
}
namespace cgh
{
    typedef size_t ID;
    typedef char Flag;
    typedef char Size;
    
    template <class Character> class FA;
    template <class Character> class DFA;
    template <class Character> class NFA;
    template <class Character> class DFAState;
    template <class Character> class NFAState;
    template <class Character> class PDSTrans;
    template <class Character> class PushPDSTrans;
    template <class Character> class ReplacePDSTrans;
    class PDSState;
    typedef unordered_set<PDSState*> PDSStateSet;
    typedef typename PDSStateSet::iterator PDSStateSetIter;
    typedef typename PDSStateSet::const_iterator PDSStateSetConstIter;
    
    template <class Character>
    class Global
    {
        typedef FA<Character> FA;
        typedef DFA<Character> DFA;
        typedef NFA<Character> NFA;
        typedef DFAState<Character> DFAState;
        typedef NFAState<Character> NFAState;
        typedef PDSTrans<Character> PDSTrans;
        typedef PushPDSTrans<Character> PushPDSTrans;
        typedef ReplacePDSTrans<Character> ReplacePDSTrans;
        
        
    public:
        /*****************  DEF Character  *****************/
        
        static Character epsilon;
        typedef unordered_set<Character> CharacterSet;
        typedef vector<Character> Word;
        typedef pair<Character, Character> Char2;
        typedef typename CharacterSet::iterator CharacterSetIter;
        
        
        /*****************  DEF NFAState  *****************/
        
        typedef unordered_set<NFAState*> NFAStateSet;
        typedef unordered_map<Character, NFAStateSet> NFATransMap;
        typedef unordered_map<NFATransMap, bool> NFAMapIter;
        typedef unordered_map<NFAState*, NFAStateSet> NFAState2NFAStateSetMap;
        typedef unordered_map<NFAState*, NFAState*> NFAState2Map;
        typedef unordered_map<DFAState*, NFAState*> DFAState2NFAStateMap;
        typedef unordered_map<NFAState*, DFAState*> NFAState2DFAStateMap;
        typedef unordered_map<NFAStateSet, DFAState*> NFAStateSet2DFAStateMap;
        
        
        typedef typename NFAStateSet::iterator NFAStateSetIter;
        typedef typename NFATransMap::iterator NFATransMapIter;
        typedef typename NFAStateSet::const_iterator NFAStateSetConstIter;
        typedef typename NFATransMap::const_iterator NFATransMapConstIter;
        
        
        typedef typename NFAState2Map::iterator NFAState2MapIter;
        typedef typename DFAState2NFAStateMap::iterator DFAState2NFAStateMapIter;
        
        typedef typename NFAState2NFAStateSetMap::iterator NFAState2NFAStateSetMapIter;
        typedef typename NFAState2NFAStateSetMap::const_iterator NFAState2NFAStateSetMapConstIter;
        typedef typename NFAStateSet2DFAStateMap::iterator NFAStateSet2DFAStateMapIter;
        
        /*****************  DEF DFAState  *****************/
        
        typedef pair<DFAState*, DFAState*> DFAState2;
        typedef unordered_set<DFAState*> DFAStateSet;
        typedef unordered_map<Character, DFAState*> DFATransMap;
        typedef unordered_map<DFATransMap, bool> DFAMapIter;
        typedef unordered_map<DFAState*, DFAState*> DFAState2Map;
        typedef unordered_map<DFAState*, DFAStateSet> DFAState2DFAStateSetMap;
        typedef unordered_map<Character, DFAState2> Char2DFAState2Map;
        typedef unordered_map<Character, DFAStateSet> Char2DFAStateSetMap;
        typedef unordered_map<DFAState2, DFAState*> DFAStatePairMap;
        typedef unordered_map<DFAStateSet, DFAState*> DFAStateSetMap;
        
        typedef typename DFAStateSet::iterator DFAStateSetIter;
        typedef typename DFATransMap::iterator DFATransMapIter;
        typedef typename DFAState2Map::iterator DFAState2MapIter;
        typedef typename DFAStatePairMap::iterator DFAStatePairMapIter;
        typedef typename DFAStateSetMap::iterator DFAStateSetMapIter;
        typedef typename Char2DFAState2Map::iterator Char2DFAState2MapIter;
        typedef typename Char2DFAStateSetMap::iterator Char2DFAStateSetMapIter;
        typedef typename DFAState2DFAStateSetMap::iterator DFAState2DFAStateSetMapIter;
        
        typedef typename DFAStateSet::const_iterator DFAStateSetConstIter;
        typedef typename DFATransMap::const_iterator DFATransMapConstIter;
        typedef typename DFAState2Map::const_iterator DFAState2MapConstIter;
        typedef typename DFAStatePairMap::const_iterator DFAStatePairMapConstIter;
        typedef typename DFAStateSetMap::const_iterator DFAStateSetMapConstIter;
        typedef typename Char2DFAState2Map::const_iterator Char2DFAState2MapConstIter;
        typedef typename Char2DFAStateSetMap::const_iterator Char2DFAStateSetMapConstIter;
        typedef typename DFAState2DFAStateSetMap::const_iterator DFAState2DFAStateSetMapConstIter;
        
        /*****************  DEF FA  *****************/
        
        typedef unordered_set<FA*> FASet;
        typedef unordered_set<DFA*> DFASet;
        typedef unordered_set<NFA*> NFASet;
        typedef unordered_map<Character, ID> Char2IDMap;
        typedef list<FA*> FAList;
        typedef typename FASet::iterator FASetIter;
        typedef typename DFASet::iterator DFASetIter;
        typedef typename FAList::iterator FAListIter;
        typedef typename FASet::const_iterator FASetConstIter;
        typedef typename FAList::const_iterator FAListConstIter;
        
        /*****************  DEF PDSTrans  *****************/
        
        typedef list<PDSTrans> PDSTransList;
        typedef list<PushPDSTrans> PushPDSTransList;
        typedef list<ReplacePDSTrans> ReplacePDSTransList;
        typedef unordered_map<PDSState*, NFAState*> PDSState2NFAStateMap;
        
        typedef typename PDSTransList::iterator PDSTransListIter;
        typedef typename PushPDSTransList::iterator PushPDSTransListIter;
        typedef typename ReplacePDSTransList::iterator ReplacePDSTransListIter;
        typedef typename PDSState2NFAStateMap::iterator PDSState2NFAStateMapIter;
        
        typedef typename PDSTransList::const_iterator PDSTransListConstIter;
        typedef typename PushPDSTransList::const_iterator PushPDSTransListConstIter;
        typedef typename ReplacePDSTransList::const_iterator ReplacePDSTransListConstIter;
        
        
        
        
        
    };
    
};
#endif /* Global_hpp */

