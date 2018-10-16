#ifndef __TYPE__
#define __TYPE__

#include "common.h"

class Field;
class AstNode;

class Type {
    enum {BASIC, ARRAY, STRUCTURE} kind;
    enum BASIC {TYPE_INT, TYPE_FLOAT};
    union {
        enum BASIC basic;
        struct {
            Type *elem;
            size_t size;
        } array;
        struct {
            string name;
            vector<Field> fields;
        } structure;
    };
public:
    Type();
    Type(AstNode *specifier);
    Type(const Type& type);
    ~Type();
    Type& operator=(const Type& type);

    bool equalArray(const Type &type);
    bool equalStructure(const Type &type);
    bool operator==(const Type &type);
    bool isBasic();
    string getStructName() const;
};

class Field {
    string name;
    Type type;
    friend class Type;
public:
    Field& operator=(const Field &field);
};


#endif