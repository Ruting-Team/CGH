//
//  FA.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/6/14.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef FA_hpp
#define FA_hpp

#include <fstream>
#include <iostream>

#include "State.hpp"

using namespace std;
namespace cgh{
    template <class Character> class DFA;
    template <class Character> class NFA;
    template <class Character> class DFAState;
    template <class Character> class NFAState;
    
    /// \brief FA is a virtual class which define a Finite Automaton.
    template <class Character>
    class FA
    {
        typedef DFA<Character> DFA;
        typedef NFA<Character> NFA;
        typedef Global<Character> Global;
        typedef DFAState<Character> DFAState;
        typedef NFAState<Character> NFAState;
        
        typedef typename Global::Word Word;
        typedef typename Global::FASet FASet;
        typedef typename Global::DFASet DFASet;
        typedef typename Global::FAList FAList;
        typedef typename Global::DFAStateSet DFAStateSet;
        typedef typename Global::DFATransMap DFATransMap;
        typedef typename Global::NFAStateSet NFAStateSet;
        typedef typename Global::NFATransMap NFATransMap;
        typedef typename Global::DFAState2Map DFAState2Map;
        typedef typename Global::NFAState2Map NFAState2Map;
        typedef typename Global::CharacterSet CharacterSet;
        typedef typename Global::DFAStateSetMap DFAStateSetMap;
        typedef typename Global::Char2DFAStateSetMap Char2DFAStateSetMap;
        typedef typename Global::DFAState2NFAStateMap DFAState2NFAStateMap;
        
    protected:
        Flag flag; /// < Records some attributes for this FA.
        CharacterSet alphabet;/// < A set of characters which in the label on the transitions.

        /// \brief Default construction without arguments, initialize flag to 0.
        FA():flag(0){}
        FA(FILE *file){}//todo
        virtual ~FA(){}
        void setDeterminateFlag(bool b){flag = b ? (flag | 1):(flag & ~1);}
        void setReachableFlag(bool b){flag = b ? (flag | (1<<1)):(flag & ~(1<<1));}
    private:
        static void getTransMapByStateSet(const DFAStateSet& stateSet, Char2DFAStateSetMap& nfaTransMap) {
            DFATransMap& transMap = (*stateSet.begin()) -> getDFATransMap();
            DFAStateSet workSet;
            for (auto& mapPair : transMap) {
                workSet.clear();
                for (DFAState* dfaState : stateSet) {
                    DFAState* state = dfaState -> getTargetStateByChar(mapPair.first);
                    if (!state) break;
                    workSet.insert(state);
                }
                if (workSet.size() == stateSet.size())
                    nfaTransMap[mapPair.first] = workSet;
            }
        }
        
        static void makeDFATrans(DFAState* preState, DFAStateSetMap &setMapping, const Char2DFAStateSetMap &nfaTransMap, DFA* dfa) {
            Char2DFAStateSetMap transMap;
            for (auto& mapPair : nfaTransMap) {
                auto setMapIter = setMapping.find(mapIter.second);
                DFAState* postState;
                if (setMapIter == setMapping.end()) {
                    transMap.clear();
                    FA::getTransMapByStateSet(mapPair.second, transMap);
                    if(DFA::allFinalState(mapPair.second)) postState = dfa -> mkDFAFinalState();
                    else postState = dfa -> mkDFAState();
                    setMapping[mapPair.second] = postState;
                    makeDFATrans(postState, setMapping, transMap, dfa);
                } else {
                    postState = (setMapping[mapPair.second]);
                }
                preState -> addDFATrans(mapPair.first, postState);
            }
        }

    public:
        /// \brief Judges whether this FA is determinate or not.
        ///
        /// ture means determinate, false means nonderminate.
        /// \return A boolean representing determinacy.
        bool isDeterminate() const {return (flag & 1) == 1;}

        /// \brief Judges whether this FA is reachable or not.
        ///
        /// ture means reachable, false means unreachable.
        /// reachable means all the states in this FA can be reached from initial state.
        /// \return A boolean representing reachability.
        bool isReachable() const {return (flag & 1 << 1) == (1 << 1);}

        CharacterSet& getAlphabet() {return alphabet;}
        const CharacterSet& getAlphabet() const {return alphabet;}
        
        void setAlphabet(const CharacterSet &charSet) {
            alphabet.clear();
            alphabet.insert(charSet.begin(),charSet.end());
        }
        void setAlphabet(const set<Character> &charSet) {
            alphabet.clear();
            alphabet.insert(charSet.begin(),charSet.end());
        }
        
        
        virtual bool isNULL() const = 0;
        virtual FA& operator &(const FA &fa) = 0;//intersection
        virtual FA& operator |(const FA &fa) = 0;//union
        virtual DFA& operator !( void ) = 0;//complement
        
        virtual DFA& determine( void ) = 0;
        virtual NFA& nondetermine( void ) = 0;
        virtual FA& concat(const FA &fa) = 0;//concatination
        
        virtual FA& rightQuotient(Character character) = 0;
        virtual FA& leftQuotient(Character character) = 0;
        
        virtual void removeUnreachableState() = 0;
        virtual void removeDeadState() = 0;
        
        //        virtual Word getOneRun() = 0;
        virtual bool isAccepted(const Word &word) = 0;//accepted
        virtual bool isAccepted(Character character) = 0;
        
        virtual void output()const = 0;
        virtual void print(string filename)const = 0;
        
        FA& operator -(const FA &fa)//minus
        {
            DFA& tempDFA = !const_cast<FA&>(fa);
            FA& res = *this & tempDFA;
            delete &tempDFA;
            return res;
        }
        
        
        static FA& intersectionFA(const FA& lhsfa, const FA& rhsfa)
        {
            return const_cast<FA&>(lhsfa) & rhsfa;
        }
        static FA& unionFA(const FA& lhsfa, const FA& rhsfa)
        {
            return const_cast<FA&>(lhsfa) | rhsfa;
        }
        static FA& concat(const FA& lhsfa, const FA& rhsfa)
        {
            return const_cast<FA&>(lhsfa).concat(rhsfa);
        }
        static FA& minus(const FA& lhsfa, const FA& rhsfa)
        {
            return const_cast<FA&>(lhsfa) - rhsfa;
        }
        
        static FA& multiIntersection(const FASet& faSet)
        {
            DFASet tempDFASet;
            DFAStateSet set;
            bool f = true;
            for(FASetConstIter it = faSet.begin(); it != faSet.end(); it++)
            {
                if((*it)->isNULL()) return FA::EmptyDFA();
                DFAState* iniState = NULL;
                DFA *tempDFA = &(*it)->determine();
                if(!(*it)->isDeterminate()) tempDFASet.insert(tempDFA);
                iniState = tempDFA->getInitialState();
                f &= iniState->isFinal();
                set.insert(iniState);
            }
            DFA *dfa = new DFA();
            DFAState* iniState = dfa->mkDFAInitialState();
            if(f) dfa->addFinalState(iniState);
            DFAStateSetMap setMapping;
            setMapping[set] = iniState;
            Char2DFAStateSetMap nfaTransMap;
            getTransMapByStateSet(set, nfaTransMap);
            makeDFATrans(iniState, setMapping, nfaTransMap, dfa);
            for(DFASetIter it = tempDFASet.begin(); it != tempDFASet.end(); it++)
                delete *it;
            if(dfa->getFinalStateSet().size() == 0)
            {
                delete dfa;
                return FA::EmptyDFA();
            }
            dfa->setReachableFlag(1);
            return *dfa;
        }
        //        static bool multiIntersectionAndDeterminEmptiness(const FASet &faSet);//todo
        static FA& multiConcatination(const FAList& faList)
        {
            NFA *nfa = new NFA();
            NFAState* iniState = nfa->mkNFAInitialState();
            NFAStateSet fStateSet;
            fStateSet.insert(iniState);
            DFAState2NFAStateMap dfaState2Map;
            NFAState2Map nfaState2Map;
            for(FAListConstIter it = faList.begin(); it != faList.end(); it++)
            {
                if((*it)->isNULL()) continue;
                NFAState* state = nfa->mkNFAState();
                for(NFAStateSetIter sIt = fStateSet.begin(); sIt != fStateSet.end(); sIt++)
                    (*sIt)->addEpsilonTrans(state);
                fStateSet.clear();
                nfa->clearFinalStateSet();
                if((*it)->isDeterminate())
                {
                    dfaState2Map.clear();
                    DFA &fa = (*it)->determine();
                    DFAState* iniState = fa.getInitialState();
                    if(iniState->isFinal()) nfa->addFinalState(state);
                    dfaState2Map[iniState] = state;
                    nfa->makeCopyTransByDFA(iniState, dfaState2Map);
                    
                }
                else
                {
                    nfaState2Map.clear();
                    NFA &fa = (*it)->nondetermine();
                    NFAState* iniState = fa.getInitialState();
                    if(iniState->isFinal()) nfa->addFinalState(state);
                    nfaState2Map[iniState] = state;
                    nfa->makeCopyTransByNFA(iniState, nfaState2Map);
                }
                fStateSet.insert(nfa->finalStateSet.begin(), nfa->finalStateSet.end());
            }
            return *nfa;
        }
        
        static FA& multiUnion(const FASet& faSet)
        {
            NFA *nfa = new NFA();
            NFAState* iniState = nfa->mkNFAInitialState();
            DFAState2NFAStateMap dfaState2Map;
            NFAState2Map nfaState2Map;
            for(FASetConstIter it = faSet.begin(); it != faSet.end(); it++)
            {
                if((*it)->isNULL()) continue;
                NFAState* state = nfa->mkNFAState();
                iniState->addEpsilonTrans(state);
                if((*it)->isDeterminate())
                {
                    dfaState2Map.clear();
                    DFA &fa = (*it)->determine();
                    DFAState* iniState = fa.getInitialState();
                    if(iniState->isFinal()) nfa->addFinalState(state);
                    dfaState2Map[iniState] = state;
                    nfa->makeCopyTransByDFA(iniState, dfaState2Map);
                }
                else
                {
                    nfaState2Map.clear();
                    NFA &fa = (*it)->nondetermine();
                    NFAState* iniState = fa.getInitialState();
                    if(iniState->isFinal()) nfa->addFinalState(state);
                    nfaState2Map[iniState] = state;
                    nfa->makeCopyTransByNFA(iniState, nfaState2Map);
                }
            }
            return *nfa;
        }
        
        virtual bool isEmpty() = 0;
        
        bool operator ==(const FA& fa )
        {
            DFA& cDFA = !(const_cast<FA&>(fa));
            DFA& iDFA = (*this & cDFA).determine();
            (*this & cDFA).output();
            if(!iDFA.isEmpty())
            {
                delete &cDFA;
                delete &iDFA;
                return false;
            }
            cDFA = !(*this);
            iDFA = (cDFA & fa).determine();
            if(!iDFA.isEmpty())
            {
                delete &cDFA;
                delete &iDFA;
                return false;
            }
            delete &cDFA;
            delete &iDFA;
            return true;
        }
        bool operator <=(const FA& fa )
        {
            DFA& cDFA = !(const_cast<FA&>(fa));
            DFA& iDFA = (*this & cDFA).determine();
            if(!iDFA.isEmpty())
            {
                delete &cDFA;
                delete &iDFA;
                return false;
            }
            delete &cDFA;
            delete &iDFA;
            return true;
        }
        static DFA& EmptyDFA()
        {
            DFA* dfa = new DFA();
            return *dfa;
        }
        static NFA& EmptyNFA()
        {
            NFA* nfa = new NFA();
            return *nfa; 
        }
        static DFA& CompleteFA(const CharacterSet &charSet)
        {
            DFA* dfa = new DFA();
            DFAState* iniState = dfa->mkDFAFinalState();
            dfa->setAlphabet(charSet);
            for(CharacterSetConstIter it = charSet.begin(); it != charSet.end(); it++)
                iniState->addDFATrans(*it, iniState);
            return *dfa;
        }
        static DFA& SigmaStarFA(const CharacterSet &charSet)
        {
            DFA* dfa = new DFA();
            DFAState* iniState = dfa->mkDFAInitialState();
            dfa->addFinalState(iniState);
            dfa->setAlphabet(charSet);
            for(CharacterSetIter it = charSet.begin(); it != charSet.end(); it++)
                iniState->addDFATrans(*it, iniState);
            return *dfa;
        }
        
        
        
        friend DFA;
        friend NFA;
        
    };
    
    
    
    
    
}

#endif /* FA_hpp */
