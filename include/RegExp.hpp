//
//  RegularExp.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/7/15.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef RegExp_hpp
#define RegExp_hpp

#include "Global.hpp"
#include <stack>
namespace cgh {
    
    class Char {
    public:
        typedef unordered_set<char> CharValues;
    private:
        bool type;               ///< 0 -> char, 1 -> opt.
        CharValues charValues;   ///< the value in Char.
        NFA<char>* nfa;          ///< the NFA for Char.
    public:
        /// \brief Default construction function
        Char() : nfa(nullptr) {}

        /// \brief Construction function for optChar or Char.
        /// \param c The value of Char.
        /// \param t The type of Char.
        Char(char c, bool t = 0) : type(t), charValues{c}, nfa(nullptr) {}

        /// \brief Construction function for Chars.
        /// \param vs The valus of Char.
        Char(const CharValues& vs) : type(0), charValues(vs.begin(), vs.end()), nfa(nullptr) {}

        /// \brief Gets whether this Char is Opt or not.
        /// \return Boolean.
        bool isOpt() { return type; }

        /// \brief Gets the charValues for this Char.
        /// \return the reference of CharValues.
        CharValues& getChar() { return charValues; }

        /// \brief Gets the NFA for this Char.
        /// \return the pointer of NFA.
        NFA<char>* getNFA() { return nfa; }

        /// \brief Sets the NFA for this Char.
        /// \param n The NFA for this Char.
        void setNFA(NFA<char>* n) { nfa = n; }

        /// \brief Gets the Opt for this Char.
        /// 
        /// if isOpt() return the id of Opt.
        /// return 0, otherwise.
        /// \return int.
        char getOpt() { 
            if (!isOpt()) return 0;
            return *charValues.begin();
        }

        /// \brief Gets whether this Char is Binary Opt.
        /// return Boolean.
        bool isBinaryOpt() { return isCatOpt() || isUnionOpt(); }

        /// \brief Gets whether this Char is Unit Opt.
        /// return Boolean.
        bool isUnitOpt() { return isStarOpt() || isPlusOpt() || isQustionOpt(); }

        /// \brief Gets whether this Char is Concatenation Opt.
        /// return Boolean.
        bool isCatOpt() { return getOpt() == '.'; }

        /// \brief Gets whether this Char is Union Opt.
        /// return Boolean.
        bool isUnionOpt() { return getOpt() == '|'; } 

        /// \brief Gets whether this Char is LeftBracket Opt.
        /// return Boolean.
        bool isLeftBracketOpt() { return getOpt() == '('; }

        /// \brief Gets whether this Char is RightBracket Opt.
        /// return Boolean.
        bool isRightBracketOpt() { return getOpt() == ')'; }

        /// \brief Gets whether this Char is Star Opt.
        /// return Boolean.
        bool isStarOpt() { return getOpt() == '*'; }

        /// \brief Gets whether this Char is Plus Opt.
        /// return Boolean.
        bool isPlusOpt() { return getOpt() == '+'; }

        /// \brief Gets whether this Char is Qustion Opt.
        /// return Boolean.
        bool isQustionOpt() { return getOpt() == '?';}
        
    };

    template <class Character>
    class RegExp {
    private:
        typedef vector<Char*> Chars;
        typedef unordered_set<char> CharSet;
    private:
        Chars regEx;        ///< the regular expression processed.
        CharSet optSet;     ///< the opt set.

        void initOptSet() {
            optSet.insert('|');
            optSet.insert('*');
            optSet.insert('+');
            optSet.insert('?');
            optSet.insert('(');
            optSet.insert(')');
        }

        Char* mkChar(char c) {
            Char* ch = new Char(c);
            regEx.push_back(ch);
            return ch;
        }

        Char* mkOptChar(char c) {
            Char* ch = new Char(c, 1);
            regEx.push_back(ch);
            return ch;
        }

        Char* mkChar(const Char::CharValues& vs) {
            Char* ch = new Char(vs);
            regEx.push_back(ch);
            return ch;
        }

        Char* mkChar(const string& str) {
            Char::CharValues vs;
            if (str == ":digit:" || str == ":d:") {
                for (char c = '0'; c <= '9'; c++) {
                    vs.insert(c);
                }
            } else if (str == ":alpha:") {
                for (char c = 'a'; c <= 'z'; c++) {
                    vs.insert(c);
                }
                for (char c = 'A'; c <= 'Z'; c++) {
                    vs.insert(c);
                }
            } else if (str == ":lower:") {
                for (char c = 'a'; c <= 'z'; c++) {
                    vs.insert(c);
                }
            } else if (str == ":upper:") {
                for (char c = 'A'; c <= 'Z'; c++) {
                    vs.insert(c);
                }
            } else if (str == ":alnum:" || str == ":w:") {
                for (char c = '0'; c <= '9'; c++) {
                    vs.insert(c);
                }
                for (char c = 'a'; c <= 'z'; c++) {
                    vs.insert(c);
                }
                for (char c = 'A'; c <= 'Z'; c++) {
                    vs.insert(c);
                }
            } else if (str == ":.:") {
                for (char c = 0; c <= 127; c++) {
                    vs.insert(c);
                    if (c == 127) break;
                }
            } else {
                ID pos = str.find('-');
                if (pos == string::npos) {
                    for (ID i = 0; i < str.length(); i++) {
                        vs.insert(str[i]);
                    }
                } else {
                    ID start = 0;
                    while (pos != string::npos) {
                        for (ID i = start; i < pos - 1; i++) {
                            vs.insert(str[i]);
                        }
                        for (char c = str[pos - 1]; c <= str[pos + 1]; c++) {
                            vs.insert(c);
                        }
                        start = pos + 2;
                        pos = str.find('-', start);
                    }
                    for (ID i = start; i < str.length(); i++) {
                        vs.insert(str[i]);
                    }
                }
            }
            return mkChar(vs);
        }
        void repeat(Char* c, Char* opt) {
            NFA<Character>* nfa = c -> getNFA();
            if (!nfa) {
                nfa = new NFA<Character>();
                c -> setNFA(nfa);
                if (opt -> isStarOpt()) {
                    NFAState<Character>* state = nfa -> mkFinalState();
                    nfa -> setInitialState(state);
                    for (char ch : c -> getChar()) {
                        state -> addNFATrans(ch, state);
                    }
                } else if (opt -> isPlusOpt()) {
                    NFAState<Character>* iniState = nfa -> mkInitialState();
                    NFAState<Character>* finState = nfa -> mkFinalState();
                    for (char ch : c -> getChar()) {
                        iniState -> addNFATrans(ch, finState);
                        finState -> addNFATrans(ch, finState);
                    }
                } else if (opt -> isQustionOpt()) {
                    NFAState<Character>* iniState = nfa -> mkInitialState();
                    NFAState<Character>* finState = nfa -> mkFinalState();
                    nfa -> addFinalState(iniState);
                    for (char ch : c -> getChar()) {
                        iniState -> addNFATrans(ch, finState);
                    }
                }
            } else {
                if (opt -> isStarOpt()) {
                    NFAState<Character>* iniState = nfa -> getInitialState();
                    for (NFAState<Character>* state : nfa -> getFinalStateSet()) {
                        state -> addEpsilonTrans(iniState);
                    }
                    nfa -> addFinalState(iniState);
                } else if (opt -> isPlusOpt()) {
                    NFAState<Character>* iniState = nfa -> getInitialState();
                    for (NFAState<Character>* state : nfa -> getFinalStateSet()) {
                        state -> addEpsilonTrans(iniState);
                    }
                } else if (opt -> isQustionOpt()) {
                    nfa -> addFinalState(nfa -> getInitialState());
                }
            }
        }

        void combine(Char* lc, Char* rc, Char* opt) {
            NFA<Character>* lhsNFA = lc -> getNFA();
            NFA<Character>* rhsNFA = rc -> getNFA();
            if (!lhsNFA && !rhsNFA) {
                NFA<Character>* nfa = new NFA<Character>();
                lc -> setNFA(nfa);
                if (opt -> isCatOpt()) {
                    NFAState<Character>* iniState = nfa -> mkInitialState();
                    NFAState<Character>* finState = nfa -> mkFinalState();
                    NFAState<Character>* state = nfa -> mkState();
                    for (char ch : lc -> getChar()) {
                        iniState -> addNFATrans(ch, state);
                    }
                    for (char ch : rc -> getChar()) {
                        state -> addNFATrans(ch, finState);
                    }
                } else if (opt -> isUnionOpt()) {
                    NFAState<Character>* iniState = nfa -> mkInitialState();
                    NFAState<Character>* finState = nfa -> mkFinalState();
                    for (char ch : lc -> getChar()) {
                        iniState -> addNFATrans(ch, finState);
                    }
                    for (char ch : rc -> getChar()) {
                        iniState -> addNFATrans(ch, finState);
                    }
                }
            } else if (lhsNFA && !rhsNFA) {
                if (opt -> isCatOpt()) {
                    NFAState<Character>* finState = lhsNFA -> mkState();
                    for (NFAState<Character>* state : lhsNFA -> getFinalStateSet()) {
                        for (char ch : rc -> getChar()) {
                            state -> addNFATrans(ch, finState);
                        }
                    }
                    lhsNFA -> clearFinalStateSet();
                    lhsNFA -> addFinalState(finState);
                } else if (opt -> isUnionOpt()) {
                    NFAState<Character>* iniState = lhsNFA -> getInitialState();
                    for (NFAState<Character>* state : lhsNFA -> getFinalStateSet()) {
                        for (char ch : rc -> getChar()) {
                            iniState -> addNFATrans(ch, state);
                        }
                    }
                }
            } else if (!lhsNFA && rhsNFA) {
                if (opt -> isCatOpt()) {
                    NFAState<Character>* state = rhsNFA -> mkState();
                    NFAState<Character>* iniState = rhsNFA -> getInitialState();
                    for (char ch : rc -> getChar()) {
                        state -> addNFATrans(ch, iniState);
                    }
                    rhsNFA -> setInitialState(state);
                } else if (opt -> isUnionOpt()) {
                    NFAState<Character>* iniState = rhsNFA -> getInitialState();
                    for (NFAState<Character>* state : rhsNFA -> getFinalStateSet()) {
                        for (char ch : rc -> getChar()) {
                            iniState -> addNFATrans(ch, state);
                        }
                    }
                }
                lc -> setNFA(rhsNFA);
            } else if (lhsNFA && rhsNFA) {
                if (opt -> isCatOpt()) {
                    lhsNFA -> getStateSet().insert(rhsNFA -> getStateSet().begin(), rhsNFA -> getStateSet().end());
                    NFAState<Character>* iniState = rhsNFA -> getInitialState();
                    for (NFAState<Character>* state : lhsNFA -> getFinalStateSet()) {
                        state -> addEpsilonTrans(iniState);
                    }
                    lhsNFA -> clearFinalStateSet();
                    for (NFAState<Character>* state : rhsNFA -> getFinalStateSet()) {
                        lhsNFA -> addFinalState(state);
                    }
                    rhsNFA -> getStateSet().clear();
                    delete rhsNFA;
                } else if (opt -> isUnionOpt()) {
                    lhsNFA -> getStateSet().insert(rhsNFA -> getStateSet().begin(), rhsNFA -> getStateSet().end());
                    NFAState<Character>* iniState = lhsNFA -> mkState();
                    NFAState<Character>* lhsIniState = lhsNFA -> getInitialState();
                    NFAState<Character>* rhsIniState = rhsNFA -> getInitialState();
                    iniState -> addEpsilonTrans(lhsIniState);
                    iniState -> addEpsilonTrans(rhsIniState);
                    for (NFAState<Character>* state : rhsNFA -> getFinalStateSet()) {
                        lhsNFA -> addFinalState(state);
                    }
                    lhsNFA -> setInitialState(iniState);
                    rhsNFA -> getStateSet().clear();
                    delete rhsNFA;
                }
            }
        }
    public:
        /// \brief Default construction function.
        RegExp() {
            initOptSet();
        }

        /// \brief Construction function with param str.
        /// \param str The regular expression in string.
        RegExp(const string& str) {
            initOptSet();
            parse(str);
        }

        /// \brief Desconstruction function.
        ~RegExp() {
            for (Char* c : regEx) {
                delete c;
            }
        }

        /// \brief Parse param str into regEx.
        /// \param str The regular expression in string.
        void parse(const string& str) {
            for (ID i = 0; i < str.length(); i++) {
                if (optSet.count(str[i]) > 0) {
                    if (str[i] == '(' && i > 0 && str[i - 1] != '|' && str[i - 1] != '(') {
                        mkOptChar('.');
                    }
                    mkOptChar(str[i]);
                } else {
                    if (i > 0 && str[i - 1] != '|' && str[i - 1] != '(') {
                        mkOptChar('.');
                    }
                    if (str[i] == '[') {
                        ID pos = str.find(']', i);
                        if (pos == string::npos) exit(1);
                        mkChar(str.substr(i + 1, pos - i - 1));
                        i = pos;
                    } else if (str[i] == '\\') {
                        mkChar(str[++i]);
                    } else if (str[i] == '.') {
                        mkChar(":.:");
                    } else {
                        mkChar(str[i]);
                    }
                }
            }
        }

        /// \brief Trans regular expression to suffix expression.
        /// \param suffixExp The suffix expression.
        void toSuffixExp(Chars& suffixExp) {
            stack<Char*> optStack;
            for (Char* c : regEx) {
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
                NFA<Character>* nfa = new NFA<Character>();
                NFAState<Character>* iniState = nfa -> mkInitialState();
                NFAState<Character>* finState = nfa -> mkFinalState();
                for (char c : regEx[0] -> getChar()) {
                    iniState -> addNFATrans(c, finState);
                }
                nfa -> mkAlphabet();
                return nfa;
            }
            Chars suffixExp;
            toSuffixExp(suffixExp);
            stack<Char*> charStack;
            for (Char* c : suffixExp) {
                if (!c -> isOpt()) {
                    charStack.push(c);
                } else {
                    if (c -> isUnitOpt()) {
                        repeat(charStack.top(), c);
                    } else {
                        Char* rc = charStack.top();
                        charStack.pop();
                        combine(charStack.top(), rc, c);
                    }
                }
            }
            NFA<Character>* nfa = charStack.top() -> getNFA();
            nfa -> mkAlphabet();
            return nfa;
        }
    };
}

#endif /* RegExp_hpp */


