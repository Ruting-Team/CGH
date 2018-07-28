//
//  State.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/6/14.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef State_hpp
#define State_hpp

#include <climits>
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
#include "Global.hpp"
namespace cgh{
    using namespace std;
    
    
    template <class Character> class DFA;
    template <class Character> class NFA;
    template <class Character> class DFAState;
    template <class Character> class NFAState;
    
    

    class State
    {
        static ID counter;
    protected:
        ID id;
        Flag flag;
        State():id(counter++),flag(0){}
        State(ID id):id(id),flag(0){}
        virtual ~State(){}
    public:
        void setFinalFlag(bool b){flag = b ? (flag | 1):(flag & ~1);}
        void setVisitedFlag(bool b){flag = b ? (flag | (1<<1)):(flag & ~(1<<1));}
        void setValidFlag(bool b){flag = b ? (flag | (1<<2)):(flag & ~(1<<2));}
        void setEpsilonFlag(bool b){flag = b ? (flag | (1<<3)):(flag & ~(1<<3));}
    public:
        ID getID(){return id;}
        void setID(ID i){id = i;}
        bool isFinal(){return (flag & 1) == 1;}
        bool isVisited(){return (flag & 1<<1) == (1<<1);}
        bool isValid(){return (flag & (1<<2)) == (1<<2);}
        bool hasEpsilonTrans(){return (flag & (1<<3)) == (1<<3);}
        
        friend NFA<class Character>;
        friend DFA<class Character>;
        friend DFAState<class Character>;
        friend DFAState<class Character>;
    };
}
#endif /* State_hpp */
