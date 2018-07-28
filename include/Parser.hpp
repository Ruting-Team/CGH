//
//  Parser.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/7/3.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef PARSER_HPP
#define PARSER_HPP
// #include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <vector>
#include <tuple>

using namespace std;

namespace cgh {
    /**
     * Error Report Level
     */
    enum LogLevel {
        ERROR,
        DEBUG_,
        WARN,
        INFO,
    };
    
    /**
     * ErrorReport
     */
    class ErrorReport {
    public:
        /**
         * report some info, display in terminal
         * @param  info  [info string]
         * @param  level [report level, default INFO]
         * @return       []
         */
        void static report(string info, LogLevel level = INFO) {
            switch (level) {
                case ERROR:
                    cout << "ERROR: " << info << endl;
                    exit(-1);
                    break;
                case DEBUG_:
                    cout << "DEBUG: " << info << endl;
                    break;
                case WARN:
                    cout << "WARN: " << info << endl;
                    break;
                case INFO:
                    cout << "INFO: " << info << endl;
                    break;
                default:
                    break;
            }
        }
    };
    
    /**
     * super class of alphabet and transitions
     * store pointer in the RawFaData
     */
    class RawFaDataWithObj {
    public:
        virtual ~RawFaDataWithObj() = default;
    };
    
    /**
     * RawFaData
     * (States, InitialState, FinalStates, (Alphabet, Transitions))
     */
    class RawFaData {
    public:
        string type;
        
        int stateNumber;
        int initialState;
        set<int> finalStates;
        bool isDifinite;
        RawFaDataWithObj* alphabetAndTransitions; // Alphabet + Transitions
    };
    
    
    /**
     * RawFaDataWithChar
     * (Alphabet, Transitions) with char
     */
    class RawFaDataWithChar: public RawFaDataWithObj {
    public:
        set<char> alphabet;
        vector<tuple<int, char, int> > transitions;
        ~RawFaDataWithChar() {}
    };
    
    /**
     * RawFaDataWithInt
     * (Alphabet, Transitions) with int
     */
    class RawFaDataWithInt: public RawFaDataWithObj {
    public:
        set<int> alphabet;
        vector<tuple<int, int, int> > transitions;
        ~RawFaDataWithInt() {}
    };
    
    /**
     * RawFaDataWithString
     * (Alphabet, Transitions) with string
     */
    class RawFaDataWithString: public RawFaDataWithObj {
    public:
        set<string> alphabet;
        vector<tuple<int, string, int> > transitions;
        ~RawFaDataWithString(){}
    };
    
    /**
     * Parser
     */
    class Parser {
    private:
        string fileName;
        RawFaData rawData;
        fstream fin;
        
    public:
        /**
         * constructor
         * report error when fileName not found
         */
        Parser(string fileName): fileName(fileName) {
            fin.open(fileName, fstream::in);
            string info = fileName + " not found or open failed!";
            if (!fin.is_open()) {
                ErrorReport::report(info, ERROR);
            }
        }
        /**
         * parse and get raw fa data
         * @return [RawFaData]
         */
        RawFaData& parse();
        void close() {
            if (fin.is_open()) {
                fin.close();
            }
        }
    private:
        void parseComment();
        string parseType();
        template<class T>
        void parseAlphabet(set<T>& alphabet);
        int parseStateNumber();
        int parseInitialState();
        void parseFinalStates(set<int>& finalStates);
        template<class T>
        void parseTransitions(vector<tuple<int, T, int> >& transitions);
        
    };
}
#endif /* Parser_hpp */
