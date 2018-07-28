//
//  NFAState.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/7/2.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef NFAState_hpp
#define NFAState_hpp

#include "State.hpp"
namespace cgh{
    template <class Character> class NFA;
    
    template <class Character>
    class NFAState : public State
    {
    public:
        typedef Global<Character> Global;
        typedef typename Global::CharacterSet CharacterSet;
        typedef typename Global::CharacterSetIter CharacterSetIter;
        
        typedef typename Global::NFAStateSet NFAStateSet;
        typedef typename Global::NFATransMap NFATransMap;
        typedef typename Global::NFAMapIter NFAMapIter;
        
        typedef typename Global::NFAStateSetIter NFAStateSetIter;
        typedef typename Global::NFATransMapIter NFATransMapIter;
        
        typedef typename Global::NFAStateSetConstIter NFAStateSetConstIter;
        typedef typename Global::NFATransMapConstIter NFATransMapConstIter;
        
        
    private:
        NFATransMap nfaTransMap;
        void getTargetStateSet(NFAStateSet& stateSet)
        {
            NFAStateSet tempSet;
            for(NFATransMapIter it = nfaTransMap.begin(); it != nfaTransMap.end(); it++)
            {
                tempSet.clear();
                getTargetStateSetByChar(tempSet, it->first);
                if(tempSet.size() > 0) stateSet.insert(tempSet.begin(), tempSet.end());
            }
        }
        void getTargetStateSetByChar(NFAStateSet& stateSet, Character character)
        {
            if(character == Global::epsilon)
            {
                getEpsilonClosure(stateSet);
                return;
            }
            NFAStateSet epsilonClosure;
            getEpsilonClosure(epsilonClosure);
            epsilonClosure.insert(this);
            for(NFAStateSetIter it = epsilonClosure.begin(); it != epsilonClosure.end(); it++)
            {
                NFAState* nfaState = *it;
                NFATransMapIter mapIt = nfaState->nfaTransMap.find(character);
                if(mapIt != nfaState->nfaTransMap.end())
                    for(NFAStateSetIter it = mapIt->second.begin(); it != mapIt->second.end(); it++)
                    {
                        (*it)->getEpsilonClosure(stateSet);
                        stateSet.insert(*it);
                    }
            }
        }
    public:
        NFATransMap& getNFATransMap(){return nfaTransMap;}
        const NFATransMap getNFATransMap() const {return nfaTransMap;}
        bool addNFATrans(Character character, NFAState *target)
        {
            NFATransMapIter mapIt = nfaTransMap.find(character);
            if(mapIt == nfaTransMap.end())
            {
                NFAStateSet stateSet;
                stateSet.insert(target);
                nfaTransMap[character] = stateSet;
                return true;
            }
            else
                return mapIt->second.insert(target).second;
        }
        bool addEpsilonTrans(NFAState *target)
        {
            return addNFATrans(Global::epsilon, target);
        }
        bool delNFATrans(Character character, const NFAState *target)
        {
            NFATransMapIter mapIt = nfaTransMap.find(character);
            if(mapIt == nfaTransMap.end())
                return false;
            else
            {
                NFAStateSetIter sIt = mapIt->second.find(target);
                if(sIt == mapIt->second.end()) return false;
                if(mapIt->second.size() == 1) nfaTransMap.erase(mapIt);
                else mapIt->second.erase(sIt);
                return true;
            }
        }
        bool delNFATrans(const NFAState *target)
        {
            CharacterSet charSet;
            int count = 0;
            for(NFATransMapIter it = nfaTransMap.begin(); it != nfaTransMap.end(); it++)
            {
                NFAStateSetIter sIt = it->second.find(const_cast<NFAState*>(target));
                if(sIt != it->second.end())
                {
                    count++;
                    if(it->second.size() == 1) charSet.insert(it->first);
                    else it->second.erase(sIt);
                }
            }
            if(count == 0) return false;
            for(CharacterSetIter it = charSet.begin(); it != charSet.end(); it++)
                nfaTransMap.erase(*it);
            return true;
        }
        bool delNFATrans(Character character)
        {
            NFATransMapIter mapIt = nfaTransMap.find(character);
            if(mapIt == nfaTransMap.end()) return false;
            nfaTransMap.erase(mapIt);
            return true;
        }
        void getEpsilonClosure(NFAStateSet& epsilonClosure)
        {
            NFATransMapIter mapIt = nfaTransMap.find(Global::epsilon);
            if(mapIt == nfaTransMap.end()) return;
            NFAStateSet workSet;
            for(NFAStateSetIter it = mapIt->second.begin(); it != mapIt->second.end(); it++)
                if(epsilonClosure.insert(*it).second) workSet.insert(*it);
            for(NFAStateSetIter it = workSet.begin(); it != workSet.end(); it++)
                (*it)->getEpsilonClosure(epsilonClosure);
        }
        const NFAStateSet getTargetStateSet()
        {
            NFAStateSet stateSet;
            NFAStateSet tempSet;
            for(NFATransMapIter it = nfaTransMap.begin(); it != nfaTransMap.end(); it++)
            {
                tempSet.clear();
                getTargetStateSetByChar(tempSet, it->first);
                if(tempSet.size() > 0) stateSet.insert(tempSet.begin(), tempSet.end());
            }
            return stateSet;
            
        }
        const NFAStateSet getTargetStateSetByChar(Character character)
        {
            NFAStateSet epsilonClosure;
            getEpsilonClosure(epsilonClosure);
            if(character == Global::epsilon) return epsilonClosure;
            epsilonClosure.insert(this);
            NFAStateSet stateSet;
            for(NFAStateSetIter it = epsilonClosure.begin(); it != epsilonClosure.end(); it++)
            {
                NFAState* nfaState = *it;
                NFATransMapIter mapIter = nfaState->nfaTransMap.find(character);
                if(mapIter != nfaState->nfaTransMap.end())
                    for(NFAStateSetIter it = mapIter->second.begin(); it != mapIter->second.end(); it++)
                    {
                        (*it)->getEpsilonClosure(stateSet);
                        stateSet.insert(*it);
                    }
            }
            return stateSet;
        }
        NFAMapIter NFAMapIterator(Character c)
        {
            NFATransMapIter it = nfaTransMap.find(c);
            bool b = true;
            if(it == nfaTransMap.end()) b = false;
            return NFAMapIter(it, b);
        }
        void output(){
            NFATransMapIter iter;
            for(iter = nfaTransMap.begin(); iter != nfaTransMap.end(); iter++) {
                NFAStateSet set = iter->second;
                NFAStateSetIter sIter;
                for(sIter = set.begin(); sIter != set.end(); sIter++)
                    cout<< getID()<<" "<<iter->first<<" "<<(*sIter)->getID()<<endl;
            }
        }
        friend NFA<Character>;
    };
    

    
}


#endif /* NFAState_hpp */
