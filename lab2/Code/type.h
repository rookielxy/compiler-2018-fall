#ifndef __TYPE__
#define __TYPE__

#include "common.h"

class Symbol;
class AstNode;

class Type {
    enum {BASIC, ARRAY, STRUCTURE} kind;
    enum {TYPE_INT, TYPE_FLOAT} basic;
    string str;
    struct {
        Type *elem;
        size_t size;
    } array;
    struct {
        string name;
        vector<Symbol> fields;
    } structure;
    int line_no;
public:
    Type() = default;
    explicit Type(AstNode *specifier);
    explicit Type(bool integer);
    Type(const Type &type);
    Type(AstNode *varDec, Type *type);
    ~Type();

    bool isBasic() { return kind == BASIC; };
    string getStructName() const { return structure.name; };
    string getName() const { return str; }
    int getLineNo() const { return line_no; };

    bool equalArray(const Type &type);
    bool equalStructure(const Type &type);
    bool operator==(const Type &type);

    void fieldConflict();
};

#endif