//
//  Parser.hpp
//  CGH
//
//  Created by 何锦龙 on 2018/7/3.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef PARSER_HPP
#define PARSER_HPP
#include "../CGH.hpp"

using namespace std;

namespace cgh {
    
    /// \brief Parses from file.
    template <class Character>
    class Parser {
        typedef typename Alias4Char<Character>::Characters Characters;
        typedef unordered_set<ID> IDs;
    protected:
        Characters alphabet;
        IDs states;

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

        /// \brief Parses the final states.
        ///
        /// Example : final: 3, 4
        /// Assume line 5 is: word: $states_list
        /// \param fin input stream
        /// \param finalStates final states set
        void parseStates(fstream& fin, IDs& states) {
            parseComment(fin);
            string line;
            getline(fin, line);
            stringstream stream;
            stream << line;
            ID state;
            while(stream >> state) {
                states.insert(state);
            }
        }
    };
}




#endif /* Parser_hpp */
