#ifndef ENGINE_ENUM_HPP
#define ENGINE_ENUM_HPP

#include "DataFile.hpp"

#include <string>

#define ARGS_COUNT_DETAIL_MAGIC2(                                                    \
    _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, \
    _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34,  \
    _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50,  \
    _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, N, ...) N

#define ARGS_COUNT_DETAIL_MAGIC1(...) ARGS_COUNT_DETAIL_MAGIC2(__VA_ARGS__)

#define ARGS_COUNT_DETAIL_RSEQ                                                      \
    63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, \
    43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, \
    23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, \
    1, 0

#define ARGS_COUNT(...) ARGS_COUNT_DETAIL_MAGIC1(__VA_ARGS__, ARGS_COUNT_DETAIL_RSEQ)

#define CONCATENATE_DETAIL_CONCAT(a, b) a ## b
#define CONCATENATE(a, b) CONCATENATE_DETAIL_CONCAT(a, b)

#define ENUM_DETAIL_DECL_BASE1(name)                                \
    class name : public ::engine::DataFile::Saveable                \
    {                                                               \
    public:                                                         \
        name();                                                     \
                                                                    \
        bool operator == (const name &right) const;                 \
        bool operator != (const name &right) const;                 \
                                                                    \
        void expose(::engine::DataFile::Node &node) override;       \
        void fromString(const ::std::string &str);                  \
        ::std::string toString() const;                             \
                                                                    \
        static const name                                           \

#define ENUM_DETAIL_DECL_BASE2(name) ;    \
    private:                              \
        static constexpr size_t k_count =

#define ENUM_DETAIL_DECL_BASE3 ;                            \
        static constexpr const char *k_strings[k_count] = {

#define ENUM_DETAIL_DECL_BASE4 }; \
        size_t m_value;           \
    }

#define ENUM_DETAIL_DEF_BASE1(scope, name) ;                                                   \
    scope::name()                                                                              \
    {                                                                                          \
        static size_t number{};                                                                \
        if(number < k_count) {                                                                 \
            m_value = number;                                                                  \
            ++number;                                                                          \
        }                                                                                      \
        else m_value = 0;                                                                      \
    }                                                                                          \
                                                                                               \
    bool scope::operator == (const name &right) const                                          \
    {                                                                                          \
        return m_value == right.m_value;                                                       \
    }                                                                                          \
                                                                                               \
    bool scope::operator != (const name &right) const                                          \
    {                                                                                          \
        return !(*this == right);                                                              \
    }                                                                                          \
                                                                                               \
    void scope::expose(::engine::DataFile::Node &node)                                         \
    {                                                                                          \
        if(node.getActivityType() == ::engine::DataFile::Activity::Type::Saving) {             \
            auto str = toString();                                                             \
            node.var(str, "enum");                                                             \
        }                                                                                      \
        else if(node.getActivityType() == ::engine::DataFile::Activity::Type::Loading) {       \
            std::string str;                                                                   \
            node.var(str, "enum");                                                             \
            fromString(str);                                                                   \
        }                                                                                      \
    }                                                                                          \
                                                                                               \
    void scope::fromString(const ::std::string &str)                                           \
    {                                                                                          \
        for(size_t i = 0; i < k_count; ++i) {                                                  \
            if(str == k_strings[i]) {                                                          \
                m_value = i;                                                                   \
                return;                                                                        \
            }                                                                                  \
        }                                                                                      \
        m_value = 0;                                                                           \
    }                                                                                          \
                                                                                               \
    ::std::string scope::toString() const                                                      \
    {                                                                                          \
        if(m_value < k_count) {                                                                \
            return k_strings[m_value];                                                         \
        }                                                                                      \
        return k_strings[0];                                                                   \
    }                                                                                          \
                                                                                               \
    constexpr const char *scope::k_strings[];

#define ENUM_DECL(...) CONCATENATE(ENUM_DECL_, ARGS_COUNT(__VA_ARGS__)) (__VA_ARGS__)
#define ENUM_DEF(...) CONCATENATE(ENUM_DEF_, ARGS_COUNT(__VA_ARGS__)) (__VA_ARGS__)

#define ENUM_DECL_2(name, a)     \
    ENUM_DETAIL_DECL_BASE1(name) \
    a                            \
    ENUM_DETAIL_DECL_BASE2(name) \
    1                            \
    ENUM_DETAIL_DECL_BASE3       \
    #a                           \
    ENUM_DETAIL_DECL_BASE4

#define ENUM_DECL_3(name, a, b)  \
    ENUM_DETAIL_DECL_BASE1(name) \
    a, b                         \
    ENUM_DETAIL_DECL_BASE2(name) \
    2                            \
    ENUM_DETAIL_DECL_BASE3       \
    #a, #b                       \
    ENUM_DETAIL_DECL_BASE4

#define ENUM_DECL_4(name, a, b, c) \
    ENUM_DETAIL_DECL_BASE1(name)   \
    a, b, c                        \
    ENUM_DETAIL_DECL_BASE2(name)   \
    3                              \
    ENUM_DETAIL_DECL_BASE3         \
    #a, #b, #c                     \
    ENUM_DETAIL_DECL_BASE4

#define ENUM_DECL_5(name, a, b, c, d) \
    ENUM_DETAIL_DECL_BASE1(name)      \
    a, b, c, d                        \
    ENUM_DETAIL_DECL_BASE2(name)      \
    4                                 \
    ENUM_DETAIL_DECL_BASE3            \
    #a, #b, #c, #d                    \
    ENUM_DETAIL_DECL_BASE4

#define ENUM_DECL_6(name, a, b, c, d, e) \
    ENUM_DETAIL_DECL_BASE1(name)         \
    a, b, c, d, e                        \
    ENUM_DETAIL_DECL_BASE2(name)         \
    5                                    \
    ENUM_DETAIL_DECL_BASE3               \
    #a, #b, #c, #d, #e                   \
    ENUM_DETAIL_DECL_BASE4

#define ENUM_DECL_7(name, a, b, c, d, e, f) \
    ENUM_DETAIL_DECL_BASE1(name)            \
    a, b, c, d, e, f                        \
    ENUM_DETAIL_DECL_BASE2(name)            \
    6                                       \
    ENUM_DETAIL_DECL_BASE3                  \
    #a, #b, #c, #d, #e, #f                  \
    ENUM_DETAIL_DECL_BASE4

#define ENUM_DECL_8(name, a, b, c, d, e, f, g) \
    ENUM_DETAIL_DECL_BASE1(name)               \
    a, b, c, d, e, f, g                        \
    ENUM_DETAIL_DECL_BASE2(name)               \
    7                                          \
    ENUM_DETAIL_DECL_BASE3                     \
    #a, #b, #c, #d, #e, #f, #g                 \
    ENUM_DETAIL_DECL_BASE4

#define ENUM_DECL_9(name, a, b, c, d, e, f, g, h) \
    ENUM_DETAIL_DECL_BASE1(name)                  \
    a, b, c, d, e, f, g, h                        \
    ENUM_DETAIL_DECL_BASE2(name)                  \
    8                                             \
    ENUM_DETAIL_DECL_BASE3                        \
    #a, #b, #c, #d, #e, #f, #g, #h                \
    ENUM_DETAIL_DECL_BASE4

#define ENUM_DECL_10(name, a, b, c, d, e, f, g, h, i) \
    ENUM_DETAIL_DECL_BASE1(name)                      \
    a, b, c, d, e, f, g, h, i                         \
    ENUM_DETAIL_DECL_BASE2(name)                      \
    9                                                 \
    ENUM_DETAIL_DECL_BASE3                            \
    #a, #b, #c, #d, #e, #f, #g, #h, #i                \
    ENUM_DETAIL_DECL_BASE4

#define ENUM_DECL_11(name, a, b, c, d, e, f, g, h, i, j) \
    ENUM_DETAIL_DECL_BASE1(name)                         \
    a, b, c, d, e, f, g, h, i, j                         \
    ENUM_DETAIL_DECL_BASE2(name)                         \
    10                                                   \
    ENUM_DETAIL_DECL_BASE3                               \
    #a, #b, #c, #d, #e, #f, #g, #h, #i, #j               \
    ENUM_DETAIL_DECL_BASE4

#define ENUM_DECL_12(name, a, b, c, d, e, f, g, h, i, j, k) \
    ENUM_DETAIL_DECL_BASE1(name)                            \
    a, b, c, d, e, f, g, h, i, j, k                         \
    ENUM_DETAIL_DECL_BASE2(name)                            \
    11                                                      \
    ENUM_DETAIL_DECL_BASE3                                  \
    #a, #b, #c, #d, #e, #f, #g, #h, #i, #j, #k              \
    ENUM_DETAIL_DECL_BASE4

#define ENUM_DECL_13(name, a, b, c, d, e, f, g, h, i, j, k, l) \
    ENUM_DETAIL_DECL_BASE1(name)                               \
    a, b, c, d, e, f, g, h, i, j, k, l                         \
    ENUM_DETAIL_DECL_BASE2(name)                               \
    12                                                         \
    ENUM_DETAIL_DECL_BASE3                                     \
    #a, #b, #c, #d, #e, #f, #g, #h, #i, #j, #k, #l             \
    ENUM_DETAIL_DECL_BASE4

#define ENUM_DECL_14(name, a, b, c, d, e, f, g, h, i, j, k, l, m) \
    ENUM_DETAIL_DECL_BASE1(name)                                  \
    a, b, c, d, e, f, g, h, i, j, k, l, m                         \
    ENUM_DETAIL_DECL_BASE2(name)                                  \
    13                                                            \
    ENUM_DETAIL_DECL_BASE3                                        \
    #a, #b, #c, #d, #e, #f, #g, #h, #i, #j, #k, #l, #m            \
    ENUM_DETAIL_DECL_BASE4

#define ENUM_DECL_15(name, a, b, c, d, e, f, g, h, i, j, k, l, m, n) \
    ENUM_DETAIL_DECL_BASE1(name)                                     \
    a, b, c, d, e, f, g, h, i, j, k, l, m, n                         \
    ENUM_DETAIL_DECL_BASE2(name)                                     \
    14                                                               \
    ENUM_DETAIL_DECL_BASE3                                           \
    #a, #b, #c, #d, #e, #f, #g, #h, #i, #j, #k, #l, #m, #n           \
    ENUM_DETAIL_DECL_BASE4

#define ENUM_DECL_16(name, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) \
    ENUM_DETAIL_DECL_BASE1(name)                                        \
    a, b, c, d, e, f, g, h, i, j, k, l, m, n, o                         \
    ENUM_DETAIL_DECL_BASE2(name)                                        \
    15                                                                  \
    ENUM_DETAIL_DECL_BASE3                                              \
    #a, #b, #c, #d, #e, #f, #g, #h, #i, #j, #k, #l, #m, #n, #o          \
    ENUM_DETAIL_DECL_BASE4

#define ENUM_DECL_17(name, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) \
    ENUM_DETAIL_DECL_BASE1(name)                                           \
    a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p                         \
    ENUM_DETAIL_DECL_BASE2(name)                                           \
    16                                                                     \
    ENUM_DETAIL_DECL_BASE3                                                 \
    #a, #b, #c, #d, #e, #f, #g, #h, #i, #j, #k, #l, #m, #n, #o, #p         \
    ENUM_DETAIL_DECL_BASE4

#define ENUM_DEF_3(scope, name, a)     \
    const scope scope::a;              \
    ENUM_DETAIL_DEF_BASE1(scope, name)

#define ENUM_DEF_4(scope, name, a, b)  \
    const scope scope::a;              \
    const scope scope::b;              \
    ENUM_DETAIL_DEF_BASE1(scope, name)

#define ENUM_DEF_5(scope, name, a, b, c) \
    const scope scope::a;                \
    const scope scope::b;                \
    const scope scope::c;                \
    ENUM_DETAIL_DEF_BASE1(scope, name)

#define ENUM_DEF_6(scope, name, a, b, c, d) \
    const scope scope::a;                   \
    const scope scope::b;                   \
    const scope scope::c;                   \
    const scope scope::d;                   \
    ENUM_DETAIL_DEF_BASE1(scope, name)

#define ENUM_DEF_7(scope, name, a, b, c, d, e) \
    const scope scope::a;                      \
    const scope scope::b;                      \
    const scope scope::c;                      \
    const scope scope::d;                      \
    const scope scope::e;                      \
    ENUM_DETAIL_DEF_BASE1(scope, name)

#define ENUM_DEF_8(scope, name, a, b, c, d, e, f) \
    const scope scope::a;                         \
    const scope scope::b;                         \
    const scope scope::c;                         \
    const scope scope::d;                         \
    const scope scope::e;                         \
    const scope scope::f;                         \
    ENUM_DETAIL_DEF_BASE1(scope, name)

#define ENUM_DEF_9(scope, name, a, b, c, d, e, f, g) \
    const scope scope::a;                            \
    const scope scope::b;                            \
    const scope scope::c;                            \
    const scope scope::d;                            \
    const scope scope::e;                            \
    const scope scope::f;                            \
    const scope scope::g;                            \
    ENUM_DETAIL_DEF_BASE1(scope, name)

#define ENUM_DEF_10(scope, name, a, b, c, d, e, f, g, h) \
    const scope scope::a;                                \
    const scope scope::b;                                \
    const scope scope::c;                                \
    const scope scope::d;                                \
    const scope scope::e;                                \
    const scope scope::f;                                \
    const scope scope::g;                                \
    const scope scope::h;                                \
    ENUM_DETAIL_DEF_BASE1(scope, name)

#define ENUM_DEF_11(scope, name, a, b, c, d, e, f, g, h, i) \
    const scope scope::a;                                   \
    const scope scope::b;                                   \
    const scope scope::c;                                   \
    const scope scope::d;                                   \
    const scope scope::e;                                   \
    const scope scope::f;                                   \
    const scope scope::g;                                   \
    const scope scope::h;                                   \
    const scope scope::i;                                   \
    ENUM_DETAIL_DEF_BASE1(scope, name)

#define ENUM_DEF_12(scope, name, a, b, c, d, e, f, g, h, i, j) \
    const scope scope::a;                                      \
    const scope scope::b;                                      \
    const scope scope::c;                                      \
    const scope scope::d;                                      \
    const scope scope::e;                                      \
    const scope scope::f;                                      \
    const scope scope::g;                                      \
    const scope scope::h;                                      \
    const scope scope::i;                                      \
    const scope scope::j;                                      \
    ENUM_DETAIL_DEF_BASE1(scope, name)

#define ENUM_DEF_13(scope, name, a, b, c, d, e, f, g, h, i, j, k) \
    const scope scope::a;                                         \
    const scope scope::b;                                         \
    const scope scope::c;                                         \
    const scope scope::d;                                         \
    const scope scope::e;                                         \
    const scope scope::f;                                         \
    const scope scope::g;                                         \
    const scope scope::h;                                         \
    const scope scope::i;                                         \
    const scope scope::j;                                         \
    const scope scope::k;                                         \
    ENUM_DETAIL_DEF_BASE1(scope, name)

#define ENUM_DEF_14(scope, name, a, b, c, d, e, f, g, h, i, j, k, l) \
    const scope scope::a;                                            \
    const scope scope::b;                                            \
    const scope scope::c;                                            \
    const scope scope::d;                                            \
    const scope scope::e;                                            \
    const scope scope::f;                                            \
    const scope scope::g;                                            \
    const scope scope::h;                                            \
    const scope scope::i;                                            \
    const scope scope::j;                                            \
    const scope scope::k;                                            \
    const scope scope::l;                                            \
    ENUM_DETAIL_DEF_BASE1(scope, name)

#define ENUM_DEF_15(scope, name, a, b, c, d, e, f, g, h, i, j, k, l, m) \
    const scope scope::a;                                               \
    const scope scope::b;                                               \
    const scope scope::c;                                               \
    const scope scope::d;                                               \
    const scope scope::e;                                               \
    const scope scope::f;                                               \
    const scope scope::g;                                               \
    const scope scope::h;                                               \
    const scope scope::i;                                               \
    const scope scope::j;                                               \
    const scope scope::k;                                               \
    const scope scope::l;                                               \
    const scope scope::m;                                               \
    ENUM_DETAIL_DEF_BASE1(scope, name)

#define ENUM_DEF_16(scope, name, a, b, c, d, e, f, g, h, i, j, k, l, m, n) \
    const scope scope::a;                                                  \
    const scope scope::b;                                                  \
    const scope scope::c;                                                  \
    const scope scope::d;                                                  \
    const scope scope::e;                                                  \
    const scope scope::f;                                                  \
    const scope scope::g;                                                  \
    const scope scope::h;                                                  \
    const scope scope::i;                                                  \
    const scope scope::j;                                                  \
    const scope scope::k;                                                  \
    const scope scope::l;                                                  \
    const scope scope::m;                                                  \
    const scope scope::n;                                                  \
    ENUM_DETAIL_DEF_BASE1(scope, name)

#define ENUM_DEF_17(scope, name, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) \
    const scope scope::a;                                                     \
    const scope scope::b;                                                     \
    const scope scope::c;                                                     \
    const scope scope::d;                                                     \
    const scope scope::e;                                                     \
    const scope scope::f;                                                     \
    const scope scope::g;                                                     \
    const scope scope::h;                                                     \
    const scope scope::i;                                                     \
    const scope scope::j;                                                     \
    const scope scope::k;                                                     \
    const scope scope::l;                                                     \
    const scope scope::m;                                                     \
    const scope scope::n;                                                     \
    const scope scope::o;                                                     \
    ENUM_DETAIL_DEF_BASE1(scope, name)

#define ENUM_DEF_18(scope, name, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) \
    const scope scope::a;                                                        \
    const scope scope::b;                                                        \
    const scope scope::c;                                                        \
    const scope scope::d;                                                        \
    const scope scope::e;                                                        \
    const scope scope::f;                                                        \
    const scope scope::g;                                                        \
    const scope scope::h;                                                        \
    const scope scope::i;                                                        \
    const scope scope::j;                                                        \
    const scope scope::k;                                                        \
    const scope scope::l;                                                        \
    const scope scope::m;                                                        \
    const scope scope::n;                                                        \
    const scope scope::o;                                                        \
    const scope scope::p;                                                        \
    ENUM_DETAIL_DEF_BASE1(scope, name)

#endif // ENGINE_ENUM_HPP
