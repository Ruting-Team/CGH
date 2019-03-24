//  NuXmv.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/7/6.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef NuXmv_hpp
#define NuXmv_hpp
#include "EnumVar.hpp"

using namespace std;

namespace cgh {
    class NuXmv{
    protected:
        Vars vars;                      ///< the Vars for this NuXmv.
        Values values;                  ///< the whole values for this NuXmv.
        Condition* targetConfig;        ///< the vefication configuration for this NuXmv.
        Conditions conditions;          ///< the Condition vector used to store Condition made in this NuXmv.

    public:
        /// \brief Default construction function.
        NuXmv() : targetConfig(nullptr) {
        }

        /// \brief Desconstruction function.
        ~NuXmv() {
            for (Var* var: vars) {
                delete var;
            }
            for (Value* value : values) {
                delete value;
            }
            for (Condition* condition : conditions) {
                delete condition;
            }
        }

        /// \brief Makes a EnumVar for this NuXmv.
        /// \param vs The available values for this Var.
        EnumVar* mkEnumVar(const Values& vs) {
            EnumVar* var = new EnumVar(vs);
            vars.push_back(var);
            return var;
        }

        /// \brief Makes a EnumVar for this NuXmv.
        /// \param vs The available values for this Var.
        /// \param v The initial value for this Var.
        EnumVar* mkEnumVar(const Values& vs, Value* v) {
            EnumVar* var = new EnumVar(vs, v);
            vars.push_back(var);
            return var;
        }

        /// \brief Makes a EnumVar for this NuXmv.
        /// \param i The id for this Var.
        /// \param vs The available values for this Var.
        EnumVar* mkEnumVar(ID i, const Values& vs) {
            EnumVar* var = new EnumVar(i, vs);
            vars.push_back(var);
            return var;
        }

        /// \brief Makes a EnumVar for this NuXmv.
        /// \param str The name for this Var.
        /// \param vs The available values for this Var.
        EnumVar* mkEnumVar(const string& str, const Values& vs) {
            EnumVar* var = new EnumVar(str, vs);
            vars.push_back(var);
            return var;
        }

        /// \brief Makes a EnumVar for this NuXmv.
        /// \param i The id for this Var.
        /// \param vs The available values for this Var.
        /// \param v The initial value for this Var.
        EnumVar* mkEnumVar(ID i, const Values& vs, Value* v) {
            EnumVar* var = new EnumVar(i, vs, v);
            vars.push_back(var);
            return var;
        }

        /// \brief Makes a EnumVar for this NuXmv.
        /// \param str The name for this Var.
        /// \param vs The available values for this Var.
        /// \param v The initial value for this Var.
        EnumVar* mkEnumVar(const string& str, const Values& vs, Value* v) {
            EnumVar* var = new EnumVar(str, vs, v);
            vars.push_back(var);
            return var;
        }

        /// \brief Makes a Value for this NuXmv.
        /// \return Value pointer.
        EnumValue* mkEnumValue() {
            EnumValue* value = new EnumValue();
            values.push_back(value);
            return value;
        }

        /// \brief Makes a Value for this NuXmv.
        /// \param id Id for this Value.
        /// \return Value pointer.
        EnumValue* mkEnumValue(ID id) {
            EnumValue* value = new EnumValue(id);
            values.push_back(value);
            return value;
        }

        /// \brief Makes a Value for this NuXmv.
        /// \param str Name for this Value.
        /// \return Value pointer.
        EnumValue* mkEnumValue(const string& str) {
            EnumValue* value = new EnumValue(str);
            values.push_back(value);
            return value;
        }

        Condition* mkCondition() {
            Condition* condition = new Condition();
            conditions.push_back(condition);
            return condition;
        }

        Condition* mkTargetConfig() {
            targetConfig = mkCondition();
            return targetConfig;
        }

        string getVAR() {
            string res = "VAR\n";
            for (Var* var: vars) {
                res += var -> getVAR();
            }
            return res;
        }

        string getASSIGN_INIT() {
            string res = "ASSIGN\n";
            for (Var* var: vars) {
                res += var -> getASSIGN_INIT();
            }
            return res;
        }

        string getASSIGN_NEXT() {
            string res = "";
            for (Var* var: vars) {
                res += var -> getASSIGN_NEXT();
            }
            return res;
        }

        string getINVARSPEC() {
            string targetConfigStr = "TRUE";
            if (targetConfig) targetConfigStr = targetConfig -> getStr();
            return "INVARSPEC\n!(" + targetConfigStr + ");";
        }

        string getPreSMV() {
            return getVAR() + getASSIGN_INIT() + getASSIGN_NEXT();
        }

        string getSMV() {
            return getVAR() + getASSIGN_INIT() + getASSIGN_NEXT() + getINVARSPEC();
        }
    };
};


#endif /* NuXmv_hpp */
