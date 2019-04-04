//
//  Label.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/6/14.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef Label_hpp
#define Label_hpp

#include "../Alias.hpp"

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

        const Character getUpper() const {
            return upper;
        }

        /// \brief Gets the lower character for this Label.
        /// \return Character.
        Character getLower() {
            return lower;
        }

        const Character getLower() const {
            return lower;
        }

        /// \brief ss the upper character for this Label.
        /// \param u the upper character.
        void setUpper(Character u) {
            upper = u;
        }

        /// \brief ss the lower character for this Label.
        /// \param u the lower character.
        void setLower(Character l) {
            lower = l;
        }

        bool operator == (const Label& label ) const {
            return (upper == label.upper) && (lower == label.lower);
        }

        bool operator != (const Label& label ) const {
            return (upper != label.upper) || (lower != label.lower);
        }

        friend ostream& operator << (ostream& out, const Label& label) {
            out << label.getUpper() << "," << label.getLower(); 
            return out;
        }

        friend DFT<Character>;


    };
}
namespace std {
    using namespace cgh;
    template<class T> 
    struct hash<Label<T> > {
    public:
        size_t operator()(const Label<T> &label) const {
            return std::hash<T>()(label.getUpper()) ^ std::hash<T>()(label.getLower());
        }
    };
}
#endif /* Label_hpp */
