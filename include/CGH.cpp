#include "CGH.hpp"
ID State::counter = 0;
ID Var::counter = 0;
ID Value::counter = 0;
unordered_set<Object*> Manage::buffer;
Manage manage;
