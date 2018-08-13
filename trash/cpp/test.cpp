#include <iostream>
#include <cstddef> // size_t
#include <cstring> // strlen, str_cpy

#include <stddef.h>

struct String {
    String():
        str_(NULL),
        size_(0) {
    }

	String(const char *str = "") {
        init_empty_str(strlen(str));
        str_ = strncpy(str_, str, size_);
    }

	String(size_t n, char c) {
        init_empty_str(n);
        memset(str_, c, size_);
    }

    String(const String &c) {
        init_empty_str(c.len());
        str_ = strncpy(str_, c.str(), c.len());
    }

    String &operator=(const String &s) {
        if (this != &s)
            String(s).swap(*this);
        return *this;
    }

    ~String() {
        free();
    }

    size_t len() const {
        return size_;
    }

    char const *str() const {
        return str_;
    }

    char operator[] (size_t n) const {
        return str_[n];
    }

    char at(size_t idx) const {
        printf("const\n");
        return str_[idx];
    }

    char &at(size_t idx) {
        printf("non-const\n");
        return str_[idx];
    }

    void swap(String &s) {
        std::swap(str_, s.str_);
        std::swap(size_, s.size_);
    }

    void free() {
        delete [] str_;
    }

    void append(String &s) {
        if (s.len() == 0)
            return;

        char *new_str = new char[this->len() + s.len() + 1];
        if (!new_str) {
            return;
        }

        char *ptr = new_str;

        for (int n = 0; n < len(); n++)
            *ptr++ = str_[n];
        for (int n = 0; n < s.len(); n++)
            *ptr++ = s[n];
        *ptr = '\0';

        free();
        str_ = new_str;
        size_ = strlen(new_str);
    }

private:
    void init_empty_str(size_t size) {
        size_ = size;
        str_ = new char[size_+1];
        str_[size_] = '\0';
    }

	size_t size_;
	char *str_;
};

struct Expression
{
    virtual double evaluate() const = 0;
    virtual ~Expression() {}
};

struct Number : Expression
{
    explicit Number(double value)
        : value(value)
    {}

    double evaluate() const {
        return value;
    }

private:
    double value;
};

struct BinaryOperation : Expression
{
    BinaryOperation(Expression const * left, char op, Expression const * right)
        : left(left), op(op), right(right)
    { }

    ~BinaryOperation() {
        delete left;
        delete right;
    }

    double evaluate() const {
        switch (op) {
            case '+':
                return left->evaluate() + right->evaluate();
            case '-':
                return left->evaluate() - right->evaluate();
            case '*':
                return (double) left->evaluate() * right->evaluate();
            case '/':
                return (double) left->evaluate() / right->evaluate();
            default:
                return 0;
        }
    }

private:
    Expression const * left;
    Expression const * right;
    char op;
};

bool check_equals(Expression const *left, Expression const *right)
{
    int *vptr1 = (int *)left;
    int *vptr2 = (int *)right;

    return *vptr1 == *vptr2;
}

int main()
{
    printf("DBG: %d\n", check_equals(new Number(10),
                new BinaryOperation(new Number(20), '+', new Number(20))));
    return 0;
}
