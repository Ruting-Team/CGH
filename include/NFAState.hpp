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
        
        typedef typename Global::NFAStateSet NFAStateSet;
        typedef typename Global::NFATransMap NFATransMap;
        
        typedef typename Global::NFAMapIter NFAMapIter;
        typedef typename Global::NFAStateSetIter NFAStateSetIter;
        typedef typename Global::NFATransMapIter NFATransMapIter;
        
        typedef typename Global::NFAStateSetConstIter NFAStateSetConstIter;
        typedef typename Global::NFATransMapConstIter NFATransMapConstIter;
        
        
    private:
        NFATransMap nfaTransMap;
        
    private:
        
        void getTargetStateSet(NFAStateSet& stateSet)
        {
            for(NFATransMapIter it = nfaTransMap.begin(); it != nfaTransMap.end(); it++)
                getTargetStateSetByChar(stateSet, it->first);
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
            for(NFAState* nfaState : epsilonClosure)
            {
                NFATransMapIter mapIt = nfaState -> nfaTransMap.find(character);
                if(mapIt != nfaState->nfaTransMap.end())
                    for(NFAState* state : mapIt -> second)
                    {
                        state -> getEpsilonClosure(stateSet);
                        stateSet.insert(state);
                    }
            }
        }
    public:
        NFATransMap& getNFATransMap(){return nfaTransMap;}
        const NFATransMap getNFATransMap() const {return nfaTransMap;}
        bool addNFATrans(Character character, NFAState *target)
        {
            NFATransMapIter mapIt = nfaTransMap.find(character);
            if(mapIt == nfaTransMap.end())//there is no trans which label is character, create a new state set.
            {
                NFAStateSet stateSet;
                stateSet.insert(target);
                nfaTransMap[character] = stateSet;
                return true;
            }
            else//insert target into exist state set.
                return mapIt -> second.insert(target).second;
        }
        bool addEpsilonTrans(NFAState *target)
        {
            return addNFATrans(Global::epsilon, target);
        }
        
        bool delNFATrans(Character character, const NFAState *target)
        {
            NFATransMapIter mapIt = nfaTransMap.find(character);
            if(mapIt == nfaTransMap.end())//there is no trans which label is character
                return false;
            else
            {
                NFAStateSetIter sIt = mapIt -> second.find(target);
                if(sIt == mapIt -> second.end()) return false;
                if(mapIt -> second.size() == 1) nfaTransMap.erase(mapIt);
                else mapIt -> second.erase(sIt);
                return true;
            }
        }
        
        //if this has the target state then delete it and return true
          //if this only has one target state then delete the map
          //else delete the state
        //else return false
        bool delNFATrans(const NFAState *target)
        {
            CharacterSet charSet;
            int count = 0;
            for(NFATransMapIter it = nfaTransMap.begin(); it != nfaTransMap.end(); it++)
            {
                NFAStateSetIter sIt = it -> second.find(const_cast<NFAState*>(target));
                if(sIt != it -> second.end())
                {
                    count++;
                    if(it -> second.size() == 1) charSet.insert(it -> first);
                    else it -> second.erase(sIt);
                }
            }
            if(count == 0) return false;
            for(Character c : charSet)
                nfaTransMap.erase(c);
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
            for(NFAState* state : mapIt -> second)
                if(epsilonClosure.insert(state).second) workSet.insert(state);
            for(NFAState* state : workSet)
                state -> getEpsilonClosure(epsilonClosure);
        }
        const NFAStateSet getTargetStateSet()
        {
            NFAStateSet stateSet;
            for(NFATransMapIter it = nfaTransMap.begin(); it != nfaTransMap.end(); it++)
                getTargetStateSetByChar(stateSet, it -> first);
            return stateSet;
            
        }
        const NFAStateSet getTargetStateSetByChar(Character character)
        {
            NFAStateSet epsilonClosure;
            getEpsilonClosure(epsilonClosure);
            if(character == Global::epsilon) return epsilonClosure;
            epsilonClosure.insert(this);
            NFAStateSet stateSet;
            for(NFAState* nfaState : epsilonClosure)
            {
                NFATransMapIter mapIter = nfaState -> nfaTransMap.find(character);
                if(mapIter != nfaState -> nfaTransMap.end())
                    for(NFAState* state : mapIter -> second)
                    {
                        state -> getEpsilonClosure(stateSet);
                        stateSet.insert(state);
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
            for(NFATransMapIter it = nfaTransMap.begin(); it != nfaTransMap.end(); it++)
            {
                NFAStateSet set = it -> second;
                for(NFAState* state : set)
                    cout<< getID() <<" "<< it -> first <<" "<<state -> getID()<<endl;
            }
        }
        friend NFA<Character>;
    };
    

    
}


#endif /* NFAState_hpp */
