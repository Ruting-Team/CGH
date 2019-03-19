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
        static vector<Object*> buffer;
        static void manage(Object* object) {
            buffer.push_back(object);
        }
        ~Manage() {
            for (Object* object : buffer) {
                if(object) {
                    delete object;
                    object = nullptr;
                }
            }
        }
    };
}
#endif /* Object_hpp */
