//
//  TrPDSTrans.hpp
//  CGH
//
//  Created by 何锦龙 on 2018/7/14.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef TrPDSTrans_hpp
#define TrPDSTrans_hpp
#include "../State.hpp"

namespace cgh {
    class TrPDSState : public State {
    public:
    };
    
    template <class Character>
    class TrPDSTrans {
    protected:
        TrPDSState* sourceState;
        TrPDSState* targetState;
        Character character;
        DFT<Character>* dft;
    public:
        TrPDSTrans() : sourceState(nullptr), targetState(nullptr), dft(nullptr) {
        }

        TrPDSTrans(TrPDSState* sState, TrPDSState* tState, Character c, DFT<Character>* t) : sourceState(sState), targetState(tState), character(c), dft(t) {
        }

        TrPDSState* getSourceState() { 
            return sourceState;
        }

        TrPDSState* getTargetState() {
            return targetState; 
        }

        Character getChar() { 
            return character; 
        }

        DFT<Character>* getDFT() {
            return dft;
        }
        
        TrPDSState* getSourceState() const { return sourceState; }
        TrPDSState* getTargetState() const { return targetState; }
        Character getChar() const { return character; }
        DFT<Character>* getDFT() const { return dft; }
    };
    
    template <class Character>
    class PopTrPDSTrans : public TrPDSTrans<Character> {
    public:
        PopTrPDSTrans() : TrPDSTrans<Character>() {
        }

        PopTrPDSTrans(TrPDSState* sState, TrPDSState* tState, Character c, DFT<Character>* t) : TrPDSTrans<Character>(sState, tState, c, t) {}
    };
    
    template <class Character>
    class PushTrPDSTrans : public TrPDSTrans<Character> {
        typedef typename Alias4Char<Character>::Char2 Char2;
    private:
        Char2 stack;
    public:
        PushTrPDSTrans() : TrPDSTrans<Character>() {}
        PushTrPDSTrans(TrPDSState* sState, TrPDSState* tState, Character c, const Char2& s, DFT<Character>* t) : TrPDSTrans<Character>(sState, tState, c, t), stack(s.first, s.second) {}
        Char2& getStack() { return stack; }
        const Char2& getStack() const { return stack; }
    };
    
    template <class Character>
    class ReplaceTrPDSTrans : public TrPDSTrans<Character> {
    private:
        Character stack;
    public:
        ReplaceTrPDSTrans() : TrPDSTrans<Character>() {}
        ReplaceTrPDSTrans(TrPDSState* sState, TrPDSState* tState, Character c, Character s, DFT<Character>* t) : TrPDSTrans<Character>(sState, tState, c, t), stack(s) {}
        Character getStack() { return stack; }
        Character getStack() const { return stack; }
    };
}


#endif /* TrPDSTrans_hpp */
