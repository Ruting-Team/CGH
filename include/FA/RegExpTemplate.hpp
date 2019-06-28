//
//  RegularExp.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/7/15.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef RegExpTemplate_hpp
#define RegExpTemplate_hpp

#include "../State.hpp"
namespace cgh {

    template <class Character>
    class Config {
    public:
        static Character leftBracket;
        static Character rightBracket;
        static Character qustion;
        static Character plus;
        static Character star;
        static Character concat;
        static Character mid;
        static Character dot;
    };
    
    template <class Character>
    class CharTemplate {
    public:
    private:
        Character character;        ///< the value in CharTemplate.
        NFA<Character>* nfa;        ///< the NFA for CharTemplate.
    public:
        /// \brief Default construction function
        CharTemplate() : nfa(nullptr) {}

        /// \brief Construction function for optCharTemplate or CharTemplate.
        /// \param c The value of CharTemplate.
        /// \param t The type of CharTemplate.
        CharTemplate(Character c) : character(c), nfa(nullptr) {}

        /// \brief Gets whether this CharTemplate is Opt or not.
        /// \return Boolean.
        bool isOpt() { return isUnitOpt() || isBinaryOpt(); }

        /// \brief Gets the Chracter for this CharTemplate.
        /// \return Chracter.
        Character getCharTemplate() { return character; }

        /// \brief Gets the NFA for this CharTemplate.
        /// \return the pointer of NFA.
        NFA<Character>* getNFA() { return nfa; }

        /// \brief Sets the NFA for this CharTemplate.
        /// \param n The NFA for this CharTemplate.
        void setNFA(NFA<Character>* n) { nfa = n; }

        /// \brief Gets whether this CharTemplate is Binary Opt.
        /// return Boolean.
        bool isBinaryOpt() { return isCatOpt() || isUnionOpt(); }

        /// \brief Gets whether this CharTemplate is Unit Opt.
        /// return Boolean.
        bool isUnitOpt() { return isStarOpt() || isPlusOpt() || isQustionOpt(); }

        /// \brief Gets whether this CharTemplate is Concatenation Opt.
        /// return Boolean.
        bool isCatOpt() { return character == Config<Character>::concat; }

        /// \brief Gets whether this CharTemplate is Union Opt.
        /// return Boolean.
        bool isUnionOpt() { return character == Config<Character>::mid; }

        /// \brief Gets whether this CharTemplate is LeftBracket Opt.
        /// return Boolean.
        bool isLeftBracketOpt() { return character == Config<Character>::leftBracket; }

        /// \brief Gets whether this CharTemplate is RightBracket Opt.
        /// return Boolean.
        bool isRightBracketOpt() { return character == Config<Character>::rightBracket; }

        /// \brief Gets whether this CharTemplate is Star Opt.
        /// return Boolean.
        bool isStarOpt() { return character == Config<Character>::star; }

        /// \brief Gets whether this CharTemplate is Plus Opt.
        /// return Boolean.
        bool isPlusOpt() { return character == Config<Character>::plus; }

        /// \brief Gets whether this CharTemplate is Qustion Opt.
        /// return Boolean.
        bool isQustionOpt() { return character == Config<Character>::qustion; }

        /// \brief Gets whether this CharTemplate is ALL Opt.
        /// return Boolean.
        bool isAllOpt() {return character == Config<Character>::dot; }

    };

    template <class Character>
    class RegExpTemplate {
    private:
        typedef vector<CharTemplate<Character>*> CharTemplates;
        typedef typename Alias4Char<Character>::Characters Characters;
        typedef typename Alias4Char<Character>::Word Word;
    private:
        CharTemplates regEx;           ///< the regular expression processed.
        Characters optSet;     ///< the opt set.
        Characters alphabet;     ///< the opt set.

        void initOptSet() {
            optSet.insert(Config<Character>::leftBracket);
            optSet.insert(Config<Character>::rightBracket);
            optSet.insert(Config<Character>::qustion);
            optSet.insert(Config<Character>::plus);
            optSet.insert(Config<Character>::star);
            optSet.insert(Config<Character>::concat);
            optSet.insert(Config<Character>::mid);
        }

        CharTemplate<Character>* mkCharTemplate(Character c) {
            CharTemplate<Character>* ch = new CharTemplate<Character>(c);
            regEx.push_back(ch);
            return ch;
        }

        void repeat(CharTemplate<Character>* c, CharTemplate<Character>* opt) {
            NFA<Character>* nfa = c -> getNFA();
            if (!nfa) {
                nfa = new NFA<Character>(alphabet);
                c -> setNFA(nfa);
                Characters chars;
                if (c -> isAllOpt()) {
                    chars.insert(alphabet.begin(), alphabet.end());
                } else {
                    chars.insert(c -> getCharTemplate());
                }
                if (opt -> isStarOpt()) {
                    NFAState<Character>* state = nfa -> mkFinalState();
                    nfa -> setInitialState(state);
                    for (Character ch : chars) {
                        state -> addTrans(ch, state);
                    }
                } else if (opt -> isPlusOpt()) {
                    NFAState<Character>* iniState = nfa -> mkInitialState();
                    NFAState<Character>* finState = nfa -> mkFinalState();
                    for (Character ch : chars) {
                        iniState -> addTrans(ch, finState);
                        finState -> addTrans(ch, finState);
                    }
                } else if (opt -> isQustionOpt()) {
                    NFAState<Character>* iniState = nfa -> mkInitialState();
                    NFAState<Character>* finState = nfa -> mkFinalState();
                    nfa -> addFinalState(iniState);
                    for (Character ch : chars) {
                        iniState -> addTrans(ch, finState);
                    }
                }
            } else {
                if (opt -> isStarOpt()) {
                    NFAState<Character>* iniState = nfa -> getInitialState();
                    for (NFAState<Character>* state : nfa -> getFinalStates()) {
                        state -> addEpsilonTrans(iniState);
                    }
                    nfa -> addFinalState(iniState);
                } else if (opt -> isPlusOpt()) {
                    NFAState<Character>* iniState = nfa -> getInitialState();
                    for (NFAState<Character>* state : nfa -> getFinalStates()) {
                        state -> addEpsilonTrans(iniState);
                    }
                } else if (opt -> isQustionOpt()) {
                    nfa -> addFinalState(nfa -> getInitialState());
                }
            }
        }

        void combine(CharTemplate<Character>* lc, CharTemplate<Character>* rc, CharTemplate<Character>* opt) {
            NFA<Character>* lhsNFA = lc -> getNFA();
            NFA<Character>* rhsNFA = rc -> getNFA();
            Characters lchars, rchars;
            if (lc -> isAllOpt()) {
                lchars.insert(alphabet.begin(), alphabet.end());
            } else {
                lchars.insert(lc -> getCharTemplate());
            }
            if (rc -> isAllOpt()) {
                rchars.insert(alphabet.begin(), alphabet.end());
            } else {
                rchars.insert(rc -> getCharTemplate());
            }
            if (!lhsNFA && !rhsNFA) {
                NFA<Character>* nfa = new NFA<Character>();
                lc -> setNFA(nfa);
                if (opt -> isCatOpt()) {
                    NFAState<Character>* iniState = nfa -> mkInitialState();
                    NFAState<Character>* finState = nfa -> mkFinalState();
                    NFAState<Character>* state = nfa -> mkState();
                    for (Character ch : lchars) {
                        iniState -> addTrans(ch, state);
                    }
                    for (Character ch : rchars) {
                        state -> addTrans(ch, finState);
                    }
                } else if (opt -> isUnionOpt()) {
                    NFAState<Character>* iniState = nfa -> mkInitialState();
                    NFAState<Character>* finState = nfa -> mkFinalState();
                    for (Character ch : lchars) {
                        iniState -> addTrans(ch, finState);
                    }
                    for (Character ch : rchars) {
                        iniState -> addTrans(ch, finState);
                    }
                }
            } else if (lhsNFA && !rhsNFA) {
                if (opt -> isCatOpt()) {
                    NFAState<Character>* finState = lhsNFA -> mkState();
                    for (NFAState<Character>* state : lhsNFA -> getFinalStates()) {
                        for (Character ch : rchars) {
                            state -> addTrans(ch, finState);
                        }
                    }
                    lhsNFA -> clearFinalStates();
                    lhsNFA -> addFinalState(finState);
                } else if (opt -> isUnionOpt()) {
                    NFAState<Character>* iniState = lhsNFA -> getInitialState();
                    for (NFAState<Character>* state : lhsNFA -> getFinalStates()) {
                        for (Character ch : rchars) {
                            iniState -> addTrans(ch, state);
                        }
                    }
                }
            } else if (!lhsNFA && rhsNFA) {
                if (opt -> isCatOpt()) {
                    NFAState<Character>* state = rhsNFA -> mkState();
                    NFAState<Character>* iniState = rhsNFA -> getInitialState();
                    for (Character ch : lchars) {
                        state -> addTrans(ch, iniState);
                    }
                    rhsNFA -> setInitialState(state);
                } else if (opt -> isUnionOpt()) {
                    NFAState<Character>* iniState = rhsNFA -> getInitialState();
                    for (NFAState<Character>* state : rhsNFA -> getFinalStates()) {
                        for (Character ch : lchars) {
                            iniState -> addTrans(ch, state);
                        }
                    }
                }
                lc -> setNFA(rhsNFA);
            } else if (lhsNFA && rhsNFA) {
                if (opt -> isCatOpt()) {
                    lhsNFA -> getStates().insert(rhsNFA -> getStates().begin(), rhsNFA -> getStates().end());
                    NFAState<Character>* iniState = rhsNFA -> getInitialState();
                    for (NFAState<Character>* state : lhsNFA -> getFinalStates()) {
                        state -> addEpsilonTrans(iniState);
                    }
                    lhsNFA -> clearFinalStates();
                    for (NFAState<Character>* state : rhsNFA -> getFinalStates()) {
                        lhsNFA -> addFinalState(state);
                    }
                    rhsNFA -> getStates().clear();
                    delete rhsNFA;
                } else if (opt -> isUnionOpt()) {
                    lhsNFA -> getStates().insert(rhsNFA -> getStates().begin(), rhsNFA -> getStates().end());
                    NFAState<Character>* iniState = lhsNFA -> mkState();
                    NFAState<Character>* lhsIniState = lhsNFA -> getInitialState();
                    NFAState<Character>* rhsIniState = rhsNFA -> getInitialState();
                    iniState -> addEpsilonTrans(lhsIniState);
                    iniState -> addEpsilonTrans(rhsIniState);
                    for (NFAState<Character>* state : rhsNFA -> getFinalStates()) {
                        lhsNFA -> addFinalState(state);
                    }
                    lhsNFA -> setInitialState(iniState);
                    rhsNFA -> getStates().clear();
                    delete rhsNFA;
                }
            }
        }
    public:
        /// \brief Default construction function.
        RegExpTemplate() {
            initOptSet();
        }

        /// \brief Construction function with param str.
        /// \param str The regular expression in string.
        RegExpTemplate(const Word& word, const Characters& chars) : alphabet(chars.begin(), chars.end()) {
            initOptSet();
            parse(word);
        }

        /// \brief Desconstruction function.
        ~RegExpTemplate() {
            for (CharTemplate<Character>* c : regEx) {
                delete c;
            }
        }

        /// \brief Parse param str into regEx.
        /// \param str The regular expression in string.
        void parse(const Word& word) {
            for (ID i = 0; i < word.size(); i++) {
                if (optSet.count(word[i]) > 0) {
                    if (word[i] == Config<Character>::leftBracket && i > 0 && word[i - 1] != Config<Character>::mid && word[i - 1] != Config<Character>::rightBracket) {
                        mkCharTemplate(Config<Character>::concat);
                    }
                    mkCharTemplate(word[i]);
                } else {
                    if (i > 0 && word[i - 1] != Config<Character>::mid && word[i - 1] != Config<Character>::leftBracket) {
                        mkCharTemplate(Config<Character>::concat);
                    }
                    mkCharTemplate(word[i]);
                }
            }
        }

        /// \brief Trans regular expression to suffix expression.
        /// \param suffixExp The suffix expression.
        void toSuffixExp(CharTemplates& suffixExp) {
            stack<CharTemplate<Character>*> optStack;
            for (CharTemplate<Character>* c : regEx) {
                if (!c -> isOpt()) {
                    suffixExp.push_back(c);
                } else {
                    if (c -> isLeftBracketOpt()) {
                        optStack.push(c);
                    } else if (c -> isRightBracketOpt()) {
                        while (!optStack.empty() && !optStack.top() -> isLeftBracketOpt()) {
                            suffixExp.push_back(optStack.top());
                            optStack.pop();
                        }
                        optStack.pop();
                    } else if (c -> isBinaryOpt()) { 
                        while (!optStack.empty() && !optStack.top() -> isLeftBracketOpt()) {
                            suffixExp.push_back(optStack.top());
                            optStack.pop();
                        }
                        optStack.push(c);
                    } else if (c -> isUnitOpt()) {
                        while (!optStack.empty() && optStack.top() -> isUnitOpt()) {
                            suffixExp.push_back(optStack.top());
                            optStack.pop();
                        }
                        optStack.push(c);
                    }
                }
            }
            while (!optStack.empty()) {
                suffixExp.push_back(optStack.top());
                optStack.pop();
            }
        }

        /// \brief Makes NFA.
        /// \return The pointer of NFA.
        NFA<Character>* mkNFA() {
            if (regEx.size() == 1) {
                NFA<Character>* nfa = new NFA<Character>(alphabet);
                NFAState<Character>* iniState = nfa -> mkInitialState();
                NFAState<Character>* finState = nfa -> mkFinalState();
                Characters chars;
                if (regEx[0] -> isAllOpt()) {
                    chars.insert(alphabet.begin(), alphabet.end());
                } else {
                    chars.insert(regEx[0] -> getCharTemplate());
                }
                for (Character c : chars) {
                    iniState -> addTrans(c, finState);
                }
                return nfa;
            }
            CharTemplates suffixExp;
            toSuffixExp(suffixExp);
            stack<CharTemplate<Character>*> charStack;
            for (CharTemplate<Character>* c : suffixExp) {
                if (!c -> isOpt()) {
                    charStack.push(c);
                } else {
                    if (c -> isUnitOpt()) {
                        repeat(charStack.top(), c);
                    } else {
                        CharTemplate<Character>* rc = charStack.top();
                        charStack.pop();
                        combine(charStack.top(), rc, c);
                    }
                }
            }
            NFA<Character>* nfa = charStack.top() -> getNFA();
            return nfa;
        }
    };
}

#endif /* RegExpTemplate_hpp */


