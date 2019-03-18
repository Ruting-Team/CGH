//
//  Transducer.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/7/3.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef Transducer_hpp
#define Transducer_hpp

#include "../FA/FA.hpp"

namespace cgh {
    
    /// \brief A class of Nondeterministic Finite Automaton.
    template <class Character>
    class Transducer {
        typedef typename Alias4Char<Character>::Word Word;
        //SmartDTD<Character> minimize() const {
        //    if (isMinimalTD()) return SmartDTD<Character>(&determinizeTD(), 0);
        //    return SmartDTD<Character>(&const_cast<DTD<Character>&>(determinizeTD()).minimize(), 1);
        //}
    protected:
        //virtual bool isMinimalTD() = 0;
        //virtual DTD<Character>& determinizeTD() = 0;
    public:
        /// \brief Gets a Transducer which is the composition of param lhstd and param rhstd.
        ///
        /// A static function.
        /// \param lhstd A const reference Transducer.
        /// \param rhstd A const reference Transducer.
        /// \return A reference of Transducer.
        static Transducer& composition(Transducer& lhstd, Transducer& rhstd) {
        }

        Transducer& operator * (const Transducer& td) {
            return composition(*this, td);
        }
        //virtual Transducer& leftQuotientByChar(Character character) = 0;
        //virtual Transducer& leftQuotientByWord(Word& word) = 0;
        //virtual Transducer& rightQuotientByChar(Character character) = 0;
        //virtual Transducer& rightQuotientByWord(Word& word) = 0;

    };
};

#endif /* Transducer_hpp */

