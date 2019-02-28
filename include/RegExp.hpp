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
namespace cgh {
    template <class Character> class NFA;
    template <class Character> class NFAState;
    
    class Char {
    public:
        typedef unordered_set<int> CharValues;
    private:
        int type;               ///< 0 -> char, 1 -> chars, others -> opt. 
        CharValues charValues;  ///< the value in Char.
    public:
        Char() {}
        Char(int c, bool t = 0) {
            if (t) {
                type = c;
            } else {
                type = 0;
                charValues.insert(c);
            }
        }
        Char(const CharValues& vs) : type(1), charValues(vs.begin(), vs.end()) {}
        /// \brief Gets whether this Char is Opt or not.
        /// \return Boolean.
        bool isOpt() { return type > 1; }

        /// \brief Gets the charValues for this Char.
        /// \return the reference of CharValues.
        CharValues& getChar() { return charValues; }

        /// \brief Gets the Opt for this Char.
        /// 
        /// if isOpt() return the id of Opt.
        /// return 0, otherwise.
        /// \return int.
        int getOpt() { 
            if (!isOpt()) return 0;
            return type;
        }

        /// \brief Gets whether this Char is Binary Opt.
        /// return Boolean.
        bool isBinaryOpt() { return isCatOpt() || isUnionOpt(); }

        /// \brief Gets whether this Char is Unit Opt.
        /// return Boolean.
        bool isUnitOpt() { return isStarOpt() || isPlusOpt() || isQustionOpt(); }

        /// \brief Gets whether this Char is Concatenation Opt.
        /// return Boolean.
        bool isCatOpt() { return getOpt() == 128; }

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

    class RegEx {
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

        Char* mkChar(int c) {
            Char* ch = new Char(c);
            regEx.push_back(ch);
            return ch;
        }

        Char* mkOptChar(int c) {
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


    public:
        /// \brief Default construction function.
        RegEx() {
            initOptSet();
        }

        /// \brief Construction function with param str.
        /// \param str The regular expression in string.
        RegEx(const string& str) {
            initOptSet();
            parse(str);
        }

        /// \brief Parse param str into regEx.
        /// \param str The regular expression in string.
        void parse(const string& str) {
            for (ID i = 0; i < str.length(); i++) {
                if (optSet.count(str[i]) > 0) {
                    if (str[i] == '(' && i > 0 && str[i - 1] != '|' && str[i - 1] != '(') {
                        mkOptChar(128);
                    }
                    mkOptChar(str[i]);
                } else {
                    if (i > 0 && str[i - 1] != '|' && str[i - 1] != '(') {
                        mkOptChar(128);
                    }
                    if (str[i] == '[') {
                        ID pos = str.find(']', i);
                        if (pos == string::npos) exit(1);
                        mkChar(str.substr(i + 1, pos - i - 1));
                        i = pos;
                    } else if (str[i] == '\\') {
                        mkChar(str[++i]);
                    } else {
                        mkChar(str[i]);
                    }
                }
            }
            for (Char* c : regEx) {
                if (c -> isOpt()) {
                    if (c -> getOpt() == 128) {
                        cout << '.' ;
                    } else {
                        cout << char(c -> getOpt());
                    }
                } else {
                    if (c -> getChar().size() > 1) {
                        cout << '[' ;
                    }
                    for (int ch : c -> getChar()) {
                        cout << char(ch);
                    }
                    if (c -> getChar().size() > 1) {
                        cout << ']' ;
                    }
                }
            }
            cout << endl;
        }
    };
    
    
    //class RegEx
    //{
    //protected:
    //    unordered_set<char> optSet;
    //    string regEx;
    //public:
    //    RegEx() {}
    //    string getRegEx(){return regEx;}
    //    virtual bool isRegEx() = 0;
    //    bool isOpt(char c) { return optSet.find(c) != optSet.end(); }
    //    bool isUnintOpt(char c) { return c == '*' || c == '+' || c == ':'; }
    //    virtual bool isLeft(char c) = 0;
    //    virtual bool isRight(char c) = 0;
    //};
    //
    //template<class Character>
    //class BasicRegEx : public RegEx
    //{
    //public:
    //    typedef Global<Character> Global;
    //    typedef BasicChar<Character> BasicChar;
    //    typedef NFA<Character> NFA;
    //    typedef NFAState<Character> NFAState;
    //    
    //    typedef typename Global::NFAState2Map NFAState2Map;
    //public:
    //    BasicRegEx()
    //    {
    //        optSet.insert('(');
    //        optSet.insert(')');
    //        optSet.insert('+');
    //        optSet.insert('*');
    //        optSet.insert('?');
    //        optSet.insert('|');
    //    }
    //    BasicRegEx(const string& str)
    //    {
    //        regEx = str;
    //        optSet.insert('(');
    //        optSet.insert(')');
    //        optSet.insert('+');
    //        optSet.insert('*');
    //        optSet.insert('?');
    //        optSet.insert('|');
    //    }
    //    bool isRegEx()
    //    {
    //        int count = 0;
    //        for(ID i = 0; i < regEx.size(); i++)
    //        {
    //            if(regEx[i] == '|')
    //            {
    //                if(i == regEx.size() -1)
    //                    return false;
    //                else if(isLeftOpt(regEx[i + 1]))
    //                    return false;
    //            }
    //            else if(regEx[i] == '(') count++;
    //            else if(regEx[i] == ')') count--;
    //            else if(isUnintOpt(regEx[i]))
    //                if(i < regEx.size() -1)
    //                    if(isUnintOpt(regEx[i + 1]))
    //                        return false;
    //        }
    //        if(count != 0) return false;
    //        return true;
    //    }
    //    bool isLeft(char c) { return c != '|' && c != '('; }
    //    bool isLeftOpt(char c) { return optSet.find(c) != optSet.end() && c != '(';}
    //    bool isRight(char c) { return optSet.find(c) == optSet.end() || c == '('; }
    //    void mkComplementRegEx(vector<BasicChar*>& res)
    //    {
    //        if(!isRegEx()) return;
    //        ID length = regEx.length();
    //        for(ID i = 0; i < length; i++)
    //        {
    //            if(regEx[i] == '\\' && i < length - 1)
    //            {
    //                res.push_back(new BasicChar(regEx[++i], 0));
    //                if(i < length - 1 && isLeft(regEx[i + 1]))
    //                    res.push_back(new BasicChar(128, 3));
    //            }
    //            else
    //            {
    //                if (regEx[i] == '(' || regEx[i] == ')')
    //                    res.push_back(new BasicChar(regEx[i], 1));
    //                else if(regEx[i] == '|')
    //                    res.push_back(new BasicChar(regEx[i], 2));
    //                else if(regEx[i] == '*' || regEx[i] == '+' || regEx[i] == '?')
    //                    res.push_back(new BasicChar(regEx[i], 4));
    //                else
    //                    res.push_back(new BasicChar(regEx[i], 0));
    //                if(i < length - 1 && isLeft(regEx[i]) && isRight(regEx[i + 1]))
    //                    res.push_back(new BasicChar(128, 3));
    //            }
    //        }
    //    }
    //    void toPostfixEx(vector<BasicChar*>& res)
    //    {
    //        vector<BasicChar*> source;
    //        mkComplementRegEx(source);
    //        if(source.size() == 1)
    //        {
    //            if(!source[0] -> isOpt())
    //            {
    //                source.push_back(new BasicChar(128, 3));
    //                source.push_back(new BasicChar(0, 0));
    //            }
    //        }
    //        stack<BasicChar*> stack;
    //        for(BasicChar* basicChar : source)
    //        {
    //            if(!basicChar -> isOpt())
    //                res.push_back(basicChar);
    //            else if(basicChar -> isLeftBracket())
    //                stack.push(basicChar);
    //            else if(basicChar -> isRightBracket())
    //            {
    //                while(!stack.empty() && !stack.top() -> isLeftBracket())
    //                {
    //                    res.push_back(stack.top());
    //                    stack.pop();
    //                }
    //                stack.pop();
    //            }
    //            else
    //            {
    //                while(!stack.empty() && stack.top() -> getType() >= basicChar ->getType())
    //                {
    //                    res.push_back(stack.top());
    //                    stack.pop();
    //                }
    //                stack.push(basicChar);
    //            }
    //        }
    //        while(!stack.empty())
    //        {
    //            res.push_back(stack.top());
    //            stack.pop();
    //        }
    //    }
    //    
    //    NFA* mkNFA()
    //    {
    //        vector<BasicChar*> postfix;
    //        toPostfixEx(postfix);
    //        stack<BasicChar*> stack;
    //        for(BasicChar* basicChar: postfix)
    //        {
    //            if(!basicChar -> isOpt())
    //                stack.push(basicChar);
    //            else
    //            {
    //                BasicChar* rhsChar = stack.top();
    //                NFA* rhsNFA = rhsChar -> getNFA();
    //                if(basicChar -> isStar())
    //                {
    //                    if(!rhsNFA)
    //                    {
    //                        rhsNFA = new NFA();
    //                        NFAState* state = rhsNFA -> mkInitialState();
    //                        rhsNFA -> addFinalState(state);
    //                        state -> addNFATrans(rhsChar -> getChar(), state);
    //                        stack.top() -> setNFA(rhsNFA);
    //                    }
    //                    else
    //                    {
    //                        for(NFAState* state : rhsNFA -> getFinalStateSet())
    //                            state -> addEpsilonTrans(rhsNFA -> getInitialState());
    //                        rhsNFA -> addFinalState(rhsNFA -> getInitialState());
    //                    }
    //                }
    //                else if(basicChar -> isQustion())
    //                {
    //                    if(!rhsNFA)
    //                    {
    //                        rhsNFA = new NFA();
    //                        NFAState* iState = rhsNFA -> mkInitialState();
    //                        NFAState* fState = rhsNFA -> mkFinalState();
    //                        rhsNFA -> addFinalState(iState);
    //                        iState -> addNFATrans(rhsChar -> getChar(), fState);
    //                        stack.top() -> setNFA(rhsNFA);
    //                    }
    //                    else
    //                    {
    //                        rhsNFA -> addFinalState(rhsNFA -> getInitialState());
    //                    }
    //                }
    //                else if(basicChar -> isPlus())
    //                {
    //                    if(!rhsNFA)
    //                    {
    //                        rhsNFA = new NFA();
    //                        NFAState* iniState = rhsNFA -> mkInitialState();
    //                        NFAState* finState = rhsNFA -> mkFinalState();
    //                        iniState -> addNFATrans(rhsChar -> getChar(), finState);
    //                        finState -> addNFATrans(rhsChar -> getChar(), finState);
    //                        stack.top() -> setNFA(rhsNFA);
    //                    }
    //                    else
    //                    {
    //                        NFAState* finState = rhsNFA -> mkState();
    //                        NFAState2Map state2Map;
    //                        for(NFAState* state : rhsNFA -> getFinalStateSet())
    //                            state -> addEpsilonTrans(finState);
    //                        rhsNFA -> clearFinalStateSet();
    //                        rhsNFA -> addFinalState(finState);
    //                        state2Map[rhsNFA -> getInitialState()] = finState;
    //                        rhsNFA -> cpTransByNFA(rhsNFA -> getInitialState(), state2Map);
    //                        rhsNFA -> getFinalStateSet().erase(finState);
    //                        for(NFAState* state : rhsNFA -> getFinalStateSet())
    //                            state -> addEpsilonTrans(finState);
    //                    }
    //                }
    //                else if(basicChar -> isUnionOpt())
    //                {
    //                    stack.pop();
    //                    if(!rhsNFA)
    //                    {
    //                        NFA* lhsNFA = stack.top() -> getNFA();
    //                        if(!lhsNFA)
    //                        {
    //                            lhsNFA = new NFA();
    //                            NFAState* iniState = lhsNFA -> mkInitialState();
    //                            NFAState* finState = lhsNFA -> mkFinalState();
    //                            iniState -> addNFATrans(stack.top() -> getChar(), finState);
    //                            iniState -> addNFATrans(rhsChar -> getChar(), finState);
    //                            stack.top() -> setNFA(lhsNFA);
    //                        }
    //                        else
    //                        {
    //                            lhsNFA -> getInitialState() -> addNFATrans(rhsChar -> getChar(), lhsNFA -> mkFinalState());
    //                        }
    //                    }
    //                    else
    //                    {
    //                        NFA* lhsNFA = stack.top() -> getNFA();
    //                        if(!lhsNFA)
    //                        {
    //                            lhsNFA = new NFA();
    //                            NFAState* state = lhsNFA -> mkInitialState();
    //                            state -> addNFATrans(stack.top() -> getChar(), lhsNFA -> mkFinalState());
    //                            NFAState2Map state2Map;
    //                            NFAState* iniState = rhsNFA -> getInitialState();
    //                            state2Map[iniState] = state;
    //                            if(iniState -> isFinal())
    //                                lhsNFA -> addFinalState(state);
    //                            lhsNFA -> cpTransByNFA(iniState, state2Map);
    //                            stack.top() -> setNFA(lhsNFA);
    //                        }
    //                        else
    //                        {
    //                            NFAState* state = lhsNFA -> getInitialState();
    //                            NFAState2Map state2Map;
    //                            NFAState* iniState = rhsNFA -> getInitialState();
    //                            state2Map[iniState] = state;
    //                            if(iniState -> isFinal())
    //                                lhsNFA -> addFinalState(state);
    //                            lhsNFA -> cpTransByNFA(iniState, state2Map);
    //                        }
    //                        delete rhsNFA;
    //                    }
    //                    
    //                }
    //                else if(basicChar -> isCatOpt())
    //                {
    //                    stack.pop();
    //                    if(!rhsNFA)
    //                    {
    //                        NFA* lhsNFA = stack.top() -> getNFA();
    //                        if(!lhsNFA)
    //                        {
    //                            lhsNFA = new NFA();
    //                            NFAState* state = lhsNFA -> mkState();
    //                            lhsNFA -> mkInitialState() -> addNFATrans(stack.top() -> getChar(), state);
    //                            state -> addNFATrans(rhsChar -> getChar(), lhsNFA -> mkFinalState());
    //                            stack.top() -> setNFA(lhsNFA);
    //                        }
    //                        else
    //                        {
    //                            NFAState* state = lhsNFA -> mkState();
    //                            for(NFAState* finState : lhsNFA -> getFinalStateSet())
    //                                finState -> addNFATrans(rhsChar -> getChar(), state);
    //                            lhsNFA -> clearFinalStateSet();
    //                            lhsNFA -> addFinalState(state);
    //                        }
    //                    }
    //                    else
    //                    {
    //                        NFA* lhsNFA = stack.top() -> getNFA();
    //                        if(!lhsNFA)
    //                        {
    //                            lhsNFA = new NFA();
    //                            NFAState* state = lhsNFA -> mkState();
    //                            lhsNFA -> mkInitialState() -> addNFATrans(stack.top() -> getChar(), state);
    //                            NFAState2Map state2Map;
    //                            NFAState* iniState = rhsNFA -> getInitialState();
    //                            state2Map[iniState] = state;
    //                            if(iniState -> isFinal())
    //                                lhsNFA -> addFinalState(state);
    //                            lhsNFA -> cpTransByNFA(iniState, state2Map);
    //                            stack.top() -> setNFA(lhsNFA);
    //                        }
    //                        else
    //                        {
    //                            NFAState* state = lhsNFA -> mkState();
    //                            for(NFAState* finState : lhsNFA -> getFinalStateSet())
    //                                finState -> addEpsilonTrans(state);
    //                            lhsNFA -> clearFinalStateSet();                                
    //                            NFAState2Map state2Map;
    //                            NFAState* iniState = rhsNFA -> getInitialState();
    //                            state2Map[iniState] = state;
    //                            if(iniState -> isFinal())
    //                                lhsNFA -> addFinalState(state);
    //                            lhsNFA -> cpTransByNFA(iniState, state2Map);
    //                        }
    //                        delete rhsNFA;
    //                    }
    //                }
    //            }
    //        }
    //        return stack.top() -> getNFA();
    //    }
    //    
    //};
}

#endif /* RegExp_hpp */


