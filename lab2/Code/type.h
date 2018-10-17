#ifndef __TYPE__
#define __TYPE__

#include "common.h"

class Symbol;
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
            vector<Symbol> fields;
        } structure;
    };
    int line_no;
public:
    Type();
    explicit Type(AstNode *specifier);
    Type(AstNode *varDec, const Type &type);
    Type(const Type& type);
    ~Type();
    Type& operator=(const Type& type);

    bool isBasic() { return kind == BASIC; };
    string getStructName() const { return structure.name; };
    int getLineNo() const { return line_no; };

    bool equalArray(const Type &type);
    bool equalStructure(const Type &type);
    bool operator==(const Type &type);

    void fieldConflict();
};

#endif