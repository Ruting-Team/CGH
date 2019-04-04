//
//  Parser.hpp
//  CGH
//
//  Created by 何锦龙 on 2018/7/3.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef PARSER_HPP
#define PARSER_HPP
#include "Alias.hpp"

using namespace std;

namespace cgh {
    /// \brief Error Report Level
    enum LogLevel {
        ERROR,
        DEBUG_,
        WARN,
        INFO,
    };
    
    /// \brief Reports the error info.
    class ErrorReport {
    public:
         /// \brief Reports some info and displays in terminal
         /// \param info  [info string]
         /// \param level [report level, default INFO]
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
    
    /// \brief Parses from file.
    template <class Character>
    class Parser {
        typedef typename Alias4Char<Character>::Characters Characters;
        typedef unordered_set<ID> IDs;
    protected:
        Characters alphabet;
        IDs finalStates;
        IDs states;
        ID initialState;

    public:
        /// \brief Defualt construction.
        Parser() { 
        }

    protected:
        /// \brief Skips comment lines starting with '#'
        /// \param fin Input stream
        void parseComment(fstream& fin) {
            string word;
            while (fin >> word) {
                if (word.find("#") == 0) {
                    getline(fin, word);
                } else {
                    break;
                }
            }
        }

        /// \brief Parses the alphabet. 
        ///
        /// Example : sigma: 1 2 3 4
        /// Assume line 2 is:  word: $state_list#
        /// \param fin input stream
        void parseAlphabet(fstream& fin) {
            parseComment(fin);
            string line;
            getline(fin, line);
            stringstream stream;
            stream << line;
            Character ch;
            while(stream >> ch) {
                alphabet.insert(ch);
            }
        }

        /// \brief Parses the number of states. 
        ///
        /// Example : states: 5
        /// Assume line 3 is: word: $num_of_states
        /// \param fin input stream
        /// \return number of states
        ID parseStateNumber(fstream& fin) {
            parseComment(fin);
            ID number;
            fin >> number;
            return number;
        }
        
        /// \brief Parses the initial state. 
        ///
        /// Example : initial: 0
        /// Assume line 4 is: word: $initial_state
        /// \param fin input stream
        /// \return initial_state
        void parseInitialState(fstream& fin) {
            parseComment(fin);
            fin >> initialState;
        }
        
        /// \brief Parses the final states.
        ///
        /// Example : final: 3, 4
        /// Assume line 5 is: word: $states_list
        /// \param fin input stream
        /// \param finalStates final states set
        void parseFinalStates(fstream& fin) {
            parseComment(fin);
            string line;
            getline(fin, line);
            stringstream stream;
            stream << line;
            ID state;
            while(stream >> state) {
                finalStates.insert(state);
            }
        }
    };
}




#endif /* Parser_hpp */
