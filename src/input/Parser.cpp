//
//  Parser.cpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/7/3.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#include "Parser.hpp"
namespace cgh {
    /**
     * parse the input file,
     *   get (alphabet, stateset, initalState, finalStates, transitions)
     */
    RawFaData& Parser::parse() {
        
        RawFaData* raw = new RawFaData();
        // parse type
        string type = parseType();
        // cout << "type: " << type << endl;
        RawFaData& rawData = *raw;
        
        rawData.type = type;
        // parse alphabet
        if (type == "int") {
            rawData.alphabetAndTransitions = new RawFaDataWithInt();
            RawFaDataWithInt* rawIntData = dynamic_cast<RawFaDataWithInt*>(rawData.alphabetAndTransitions);
            parseAlphabet<int>(rawIntData->alphabet);
        } else if (type == "char") {
            rawData.alphabetAndTransitions = new RawFaDataWithChar();
            RawFaDataWithChar* rawCharData = dynamic_cast<RawFaDataWithChar*>(rawData.alphabetAndTransitions);
            parseAlphabet<char>(rawCharData->alphabet);
        } else {
            rawData.alphabetAndTransitions = new RawFaDataWithString();
            RawFaDataWithString* rawStringData = dynamic_cast<RawFaDataWithString*>(rawData.alphabetAndTransitions);
            parseAlphabet<string>(rawStringData->alphabet);
        }
        
        
        // parse state number
        rawData.stateNumber = parseStateNumber();
        
        // parse initial state
        rawData.initialState = parseInitialState();
        
        // parse final states
        // set<int> finalStates;
        parseFinalStates(rawData.finalStates);
        
        
        // parse transitions
        
        if (type == "int") {
            RawFaDataWithInt* rawIntData = dynamic_cast<RawFaDataWithInt*>(rawData.alphabetAndTransitions);
            parseTransitions<int>(rawIntData->transitions);
        } else if (type == "char") {
            RawFaDataWithChar* rawCharData = dynamic_cast<RawFaDataWithChar*>(rawData.alphabetAndTransitions);
            parseTransitions<char>(rawCharData->transitions);
        } else {
            RawFaDataWithString* rawStringData = dynamic_cast<RawFaDataWithString*>(rawData.alphabetAndTransitions);
            parseTransitions<string>(rawStringData->transitions);
        }
        
        close();
        
        
        return rawData;
    }
    
    /**
     * skip comment lines starting with '#'
     */
    void Parser::parseComment() {
        string word;
        while (fin >> word) {
            if (word.find("#") == 0) {
                getline(fin, word);
            } else {
                break;
            }
        }
    }
    
    /**
     * parse line 1:
     *         type: int
     * #assume line 1 is word: $type_str#
     * @return template type: {"int", "char", ...}
     */
    string Parser::parseType() {
        parseComment();
        string type;
        fin >> type;
        return type;
    }
    
    /**
     * parse line 2:
     *         sigma: 1 2 3 4
     * #assume line 2 is:  word: $state_list#
     * @alphabet alphabet set
     */
    template<class T>
    void Parser::parseAlphabet(set<T>& alphabet) {
        parseComment();
        string line;
        getline(fin, line);
        stringstream stream;
        stream << line;
        T ch;
        while(stream >> ch) {
            alphabet.insert(ch);
        }
    }
    
    /**
     * parse line 3:
     *         states: 5
     *     # assume line 3 is: word: $num_of_states
     * @return number of states
     */
    int Parser::parseStateNumber() {
        parseComment();
        int number;
        fin >> number;
        return number;
    }
    
    /**
     * parse line 4:
     *         initial: 0
     * # assume line 4 is: word: $initial_state
     * @return initial_state
     */
    int Parser::parseInitialState() {
        parseComment();
        int initial;
        fin >> initial;
        return initial;
    }
    
    /**
     * parse line 5:
     *         final: 3, 4
     * # assume line 5 is: word: $states_list
     * @param finalStates final states set
     */
    void Parser::parseFinalStates(set<int>& finalStates) {
        parseComment();
        string line;
        getline(fin, line);
        stringstream stream;
        stream << line;
        int state;
        while(stream >> state) {
            finalStates.insert(state);
        }
    }
    
    /**
     * parse left lines
     *         transitions lines is: 0 1 1
     *     # assume transition line is: $src_state $charater $dst_state
     * @param transitions transitions
     */
    template<class T>
    void Parser::parseTransitions(vector<tuple<int, T, int> >& transitions) {
        parseComment();
        int src;
        T ch;
        int dst;
        while(fin >> src >> ch >> dst) {
            transitions.push_back(make_tuple(src, ch, dst));
        }
    }
    
}
