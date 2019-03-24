//
//  Value.hpp
//  CGH
//
//  Created by 何锦龙 on 2018/7/6.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef Value_hpp 
#define Value_hpp 
#include "../Object.hpp"

namespace cgh {
    /// \brief Values in NuXmv.
    class Value {
    protected:
        ID id;              ///< the identity for this Value.
        string name;        ///< the name for this Value.
    public:
        static ID counter;          ///< the static counter for class Value.

        /// \brief Default construction function.
        Value() : id(counter++), name("") {
        }

        /// \brief Constrction with param.
        /// \param i Id for this Value.
        Value(ID i) : id(i), name("") {
            counter = id + 1;
        }

        /// \brief Gets Id for this Value.
        /// \return ID.
        ID getID() {return id;}
        ID getID() const {return id;}

        /// \brief Gets name for this Value.
        /// \return string.
        string& getName() {return name;}
        const string& getName() const {return name;}
    };

    /// \brief Enum Value in NuXmv.
    class EnumValue : public Value {
    public:
        EnumValue() : Value() {
            name = "v" + to_string(id);
        }

        EnumValue(ID i) : Value(i) {
            name = "v" + to_string(id);
        }

        EnumValue(const string& str) : Value() {
            name = str;
        }
    };

    /// \brief Integer Value in NuXmv.
    class IntValue : public Value {
    };

    /// \brief Boolean Value in NuXmv.
    class BoolValue : public Value {
    };
};
#endif /* Value_hpp*/

