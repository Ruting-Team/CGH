//
//  Object.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/6/14.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef Object_hpp
#define Object_hpp
#include "Alias.hpp"

namespace cgh{
    using namespace std;

    class Object {
    public:
        virtual ~Object() {}

    };

    class Manage {
    public:
        static unordered_set<Object*> buffer;
        static void manage(Object* object) {
            buffer.insert(object);
        }
        static void del(Object* object) {
            buffer.erase(object);
            delete object;
        }
        ~Manage() {
            for (Object* object : buffer) {
                if(object) {
                    delete object;
                    object = nullptr;
                }
            }
        }
        template<class T>
        static void intersectSet(const unordered_set<T>& lhs, const unordered_set<T>& rhs, unordered_set<T>& res) {
            for (auto item : lhs) {
                if (rhs.count(item) > 0)
                    res.insert(item);
            }
        }

        template<class T>
        static unordered_set<T> intersectSet(const unordered_set<T>& lhs, const unordered_set<T>& rhs) {
            unordered_set<T> res;
            intersectSet(lhs, rhs, res);
            return res;
        }

        template<class T>
        static void unionSet(const unordered_set<T>& lhs, const unordered_set<T>& rhs, unordered_set<T>& res) {
            res.insert(lhs.begin(), lhs.end());
            res.insert(rhs.begin(), rhs.end());
        }

        template<class T>
        static unordered_set<T> unionSet(const unordered_set<T>& lhs, const unordered_set<T>& rhs) {
            unordered_set<T> res;
            unionSet(lhs, rhs, res);
            return res;
        }
    };
}
#endif /* Object_hpp */
