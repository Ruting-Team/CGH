//
//  PDSTrans.hpp
//  CGH
//
//  Created by 何锦龙 on 2018/7/14.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef PDSTrans_hpp
#define PDSTrans_hpp
#include "../State.hpp"
namespace cgh {
    class PDSState : public State {
    public:
        PDSState() : State() {}
        //PDSState(ID id) : (this -> count)(id) {}
    };
    
    template <class Character>
    class PDSTrans {
    protected:
        PDSState* sourceState;      ///< The source state for this PDSTrans.
        PDSState* targetState;      ///< The target state for this PDSTrans.
        Character character;        ///< The source chracter for this PDSTrans.
    public:

        PDSTrans() : sourceState(nullptr), targetState(nullptr) {}
        PDSTrans(PDSState* sState, PDSState* tState, Character c) : sourceState(sState), targetState(tState), character(c) {}

        PDSState* getSourceState() { return sourceState; }
        PDSState* getTargetState() { return targetState; }
        Character getChar() { return character; }
        
        const PDSState* getSourceState() const { return sourceState; }
        const PDSState* getTargetState() const { return targetState; }
        const Character getChar() const { return character; }
    };
    
    template <class Character>
    class PopPDSTrans : public PDSTrans<Character> {
    public:
        PopPDSTrans() : PDSTrans<Character>() {}
        PopPDSTrans(PDSState* sState, PDSState* tState, Character c) : PDSTrans<Character>(sState, tState, c) {}
        void output() {
            cout << this -> sourceState -> getID() << " " 
                 << this -> character << " "
                 << this -> targetState -> getID() << endl;
        }
    };
    
    template <class Character>
    class PushPDSTrans : public PDSTrans<Character> {
        typedef typename Alias4Char<Character>::Char2 Char2;
    private:
        Char2 stack;
    public:
        PushPDSTrans() : PDSTrans<Character>() {}
        PushPDSTrans(PDSState* sState, PDSState* tState, Character c, const Char2& s) : PDSTrans<Character>(sState, tState, c), stack(s.first, s.second) {}
        Char2& getStack() { return stack; }
        const Char2& getStack() const { return stack; }
        void output() {
            cout << this -> sourceState -> getID() << " " 
                 << this -> character << " "
                 << stack.first << ","
                 << stack.second << " "
                 << this -> targetState -> getID() << endl;
        }
    };
    
    template <class Character>
    class ReplacePDSTrans : public PDSTrans<Character> {
    private:
        Character stack;
    public:
        ReplacePDSTrans() : PDSTrans<Character>() {}
        ReplacePDSTrans(PDSState* sState, PDSState* tState, Character c, Character s) : PDSTrans<Character>(sState, tState, c), stack(s) {}
        Character getStack() { return stack; }
        const Character getStack() const { return stack; }
            void output() {
            cout << this -> sourceState -> getID() << " " 
                 << this -> character << " "
                 << stack << " "
                 << this -> targetState -> getID() << endl;
        }
    };
}


#endif /* PDSTrans_hpp */
