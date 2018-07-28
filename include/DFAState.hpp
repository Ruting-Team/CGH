//
//  DFAState.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/7/3.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef DFAState_hpp
#define DFAState_hpp

#include "State.hpp"
namespace cgh{
    template <class Character> class DFA;
    template <class Character>
    class DFAState : public State
    {
    public:
        typedef Global<Character> Global;
        typedef typename Global::CharacterSet CharacterSet;
        typedef typename Global::CharacterSetIter CharacterSetIter;
        
        typedef typename Global::DFAStateSet DFAStateSet;
        typedef typename Global::DFATransMap DFATransMap;
        typedef typename Global::DFAMapIter DFAMapIter;
        
        typedef typename Global::DFAStateSetIter DFAStateSetIter;
        typedef typename Global::DFATransMapIter DFATransMapIter;
        
        typedef typename Global::DFAStateSetConstIter DFAStateSetConstIter;
        typedef typename Global::DFATransMapConstIter DFATransMapConstIter;
        
        
    private:
        DFATransMap dfaTransMap;
        void getTargetStateSet(DFAStateSet& stateSet)
        {
            for(DFATransMapIter it = dfaTransMap.begin(); it != dfaTransMap.end(); it++)
                stateSet.insert(it->second);
        }
    public:
        DFATransMap& getDFATransMap() {return dfaTransMap;}
        const DFATransMap& getDFATransMap() const {return dfaTransMap;}
        bool addDFATrans(Character character, DFAState *target)
        {
            if(dfaTransMap.find(character) != dfaTransMap.end()) return false;
            dfaTransMap[character] = target;
            return true;
        }
        bool delDFATrans(Character character, const DFAState *target)
        {
            DFATransMapIter mapIt = dfaTransMap.find(character);
            if(mapIt != dfaTransMap.end() && mapIt->second == target)
            {
                dfaTransMap.erase(mapIt);
                return true;
            }
            return false;
        }
        bool delDFATrans(const DFAState *target)
        {
            CharacterSet charSet;
            for(DFATransMapIter it = dfaTransMap.begin(); it != dfaTransMap.end(); it++)
                if(it->second == target)
                    charSet.insert(it->first);
            if(charSet.size() == 0) return false;
            for(CharacterSetIter it = charSet.begin(); it != charSet.end(); it++)
                dfaTransMap.erase(*it);
            return true;
        }
        bool delDFATrans(Character character)
        {
            DFATransMapIter mapIt = dfaTransMap.find(character);
            if(mapIt == dfaTransMap.end()) return false;
            dfaTransMap.erase(mapIt);
            return true;
        }
        const DFAStateSet getTargetStateSet()
        {
            DFAStateSet stateSet;
            for(DFATransMapIter it = dfaTransMap.begin(); it != dfaTransMap.end(); it++)
                stateSet.insert(it->second);
            return stateSet;
        }
        DFAState* getTargetStateByChar(Character character)
        {
            DFATransMapIter mapIt = dfaTransMap.find(character);
            if(mapIt != dfaTransMap.end()) return mapIt->second;
            DFAState* null = NULL;
            return null;
        }
        void output(){
            DFATransMapIter iter;
            for(iter = dfaTransMap.begin(); iter != dfaTransMap.end(); iter++) {
                cout<< getID()<<" "<<iter->first<<" "<<iter->second->getID()<<endl;
            }
        }
        friend DFA<Character>;
    };
    
}

#endif /* DFAState_hpp */
