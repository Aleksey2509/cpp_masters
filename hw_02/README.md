Task 1:

```cpp
template <class T1, class T2> struct Pair {
    template<class U1 = T1, class U2 = T2> Pair(U1&&, U2&&) {}
};
struct S { S() = default; }; struct E { explicit E() = default; };
int f(Pair<E, E>) { return 1; }
int f(Pair<S, S>) { return 2; }
static_assert(f({{}, {}}) == 2, "");
```

1. foo(X{}) is a unqualified name by definition from 6.5.3 4): it is a name that does not immediately follow a nested-name-specifier or the . or -> in a
class member access expression (7.6.1.5), possibly after a template keyword or ~. 

2. From 6.5.3: the unqualified lookup performs an unqualified search. It immediatly finds two declarations of f

3. Candidate set is now evaluated to a two functions

4. Now, the candidates are checked for being viable (12.2.3): the argument number is correct (one argument in call corresponds to one argument in both  definition). According to 12.2.3 4: there shall exist for each argument an implicit conversion sequence.

5. The first declaration: int f(Pair<E, E>). So, an object of type Pair<E,E> must be constructed from expression {{}, {}}. This is a list inizialization syntax. Class Pair is not an aggregate, because it has a user-defined constructor, and has a single constructor, so it is chosen as a single candidate. The parameters U1&& and U2&&, which are E&& and E&&, are list-initialized. In their case, when choosing the constructor, the same logic goes as for Pair earlier, but now an explicit constructor is chosen, which, according to 12.2.2.8 is ill-formed. So, there does not exist an implicit conversion for the first candidate function.

6. Doing the same logic for second candidate we get, that it has a implicit conversion sequence, because the constructor S is implicit.

7. There is only one viable candidate, which is int f(Pair<S, S>), so the overload resolution is a success, and this function should be called

Task 2:


```cpp
struct Foo {};
struct Bar : Foo {};
struct Baz : Foo {};
struct X {
    operator Bar() { std::cout << "Bar\n"; return Bar{}; }
    operator Baz() const { std::cout << "Baz\n"; return Baz{}; }
};
void foo(const Foo &f) {}

int main() { foo(X{}); }
```

1. foo(X{}) is a unqualified name by definition from 6.5.3 4): it is a name that does not immediately follow a nested-name-specifier or the . or -> in a
class member access expression (7.6.1.5), possibly after a template keyword or ~. 

2. From 6.5.3: the unqualified lookup performs an unqualified search. The unqualified search first searches in scope of function main, finds nothing there, then it goes to global scope, and finds a single declaration

3. Candidate set is now evaluated to a single function

4. Now, the candidate is checked for being viable (12.2.3): the argument number is correct (one argument in call corresponds to one argument in definition). Now, an existence of implicit conversion sequence must be confirmed.

5. In this case, there exists two implicit conversion sequences. Roughly speaking, the two are:

    a) X -> Bar -> Foo

    b) X -> Baz -> Foo

   So, the candidates are viable.

6. From 12.2.4: the chains of implicit conversions must be established for both sequences to decide, which is best

7. From 12.2.2.1: 2.: A member function with implicit object parameter is considered to have an extra first parameter, representing the object for which the member function is called. 4.: the type of the implicit object parameter is  “lvalue reference to cv X” for functions declared without a ref-qualifier, where the cv is the cv-qualification on the member function declaration.So, our operator Bar() and operator Baz() have the following signature in context of overload resolution: operator Bar(X&) and operator Baz(const X&)

8. Although X{} is a rvalue, from 12.2.2.1: 5: For implicit object member functions declared without a ref-qualifier , even if
the implicit object parameter is not const-qualified, an rvalue can be bound to the parameter as long as in all
other respects the argument can be converted to the type of the implicit object parameter.. And in note below: The fact that such an argument is an rvalue does not affect the ranking of implicit conversion sequences. So, this does not add anything into implicit conversion sequence

9. With all that the implicit conversion sequences will be:

    Qualification conversion (X -> const X) -> user conversion (const X& -> Bar) -> binding rvalue to const lvalue reference -> derived-to-base conversion (const Bar& -> const Foo&)

    User conversion (X -> Bar) -> binding rvalue to const lvalue reference -> derived-to-base conversion (Bar -> Foo)

10. From 3.3: User-defined conversion sequence U1 is a better conversion sequence than another user-defined conversion
sequence U2 if they contain the same user-defined conversion function or constructor or they initialize
the same class in an aggregate initialization and in either case the second standard conversion sequence
of U1 is better than the second standard conversion sequence of U2. The second standard conversion sequence, as defined in 12.2.4.2.3 is a sequence of conversions after doing the user conversion. The second standart sequence is equal in both cases, so the candidates are equal, and the overload resolution should fail.
