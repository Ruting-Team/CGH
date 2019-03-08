//
//  Label.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/6/14.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef Label_hpp
#define Label_hpp

#include "Global.hpp"

namespace cgh{
    template<class Character>
    class Label {
    private:
        Character upper;    ///< the upper character of a Label.
        Character lower;    ///< the lower character of a Label.

    public:
        /// \brief Default construction function.
        Label() {}

        /// \brief Construction function to set upper and lower.
        /// \param u The upper character.
        /// \param l The lower character.
        Label(Character u, Character l) : upper(u), lower(l) {}

        /// \brief Gets the upper character for this Label.
        /// \return Character.
        Character getUpper() {
            return upper;
        }

        /// \brief Gets the lower character for this Label.
        /// \return Character.
        Character getLower() {
            return lower;
        }

        /// \brief Sets the upper character for this Label.
        /// \param u the upper character.
        void setUpper(Character u) {
            upper = u;
        }

        /// \brief Sets the lower character for this Label.
        /// \param u the lower character.
        void setLower(Character l) {
            lower = l;
        }

        /// \brief Decides whether this Label is epsilon.
        /// \return Boolean
        bool isEpsilon() {
            return upper == Global<Character>::epsilon && lower == Global<Character>::epsilon;
        }
            
        bool operator == (const Label& label ) {
            return upper == label.upper && lower == label.lower;
        }
    };
}
#endif /* Label_hpp */
