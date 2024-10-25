Task 2:

```cpp
int g(int &&t) { return 1; }
int g(int &t) { return 2; }
template <typename T>
void h(T &&t) { g(std::forward<T> (t)); }
const int x = 1; int z = h(x);
```

Let's understand what the typename T in function h will be:

Because:

1) There is a context of type deduction for type T

2) The argument has type T&&

The type T will be a forwarding reference. It will deduced to const int &, because x in expression h(x) is a lvalue, and the parameter t in function h, after reference collapsing, will have type const int&.

In std::forward: (not actually std::forward implementation, just pseudocode, which reflects how it behaves)

```cpp
template<typename S>
S&& forward(std::remove_reference_t<S>& a) {
return static_cast<S&&>(a);
}
```

It was parameterized with T, which is const int &, so S will be const int &. So, the static cast will be done with type S&&, which after reference collapsing will be const int &. So, std::forward will return const int &

Function g has two overloads: one, which accepts int& and int&&. The return value of std::forward is const int &, so it cannot be binded with int&&, because it is a lvalue reference, but it also cannot bind with int&, because it would lose constness. There is not a single viable candidate, so the overload resolution will fail
