#pragma once
#include <any>

class Any {
 private:
  struct Any_Base {
    virtual ~Any_Base() = default;
    virtual Any_Base* clone() = 0;
  };

  template <typename T>
  struct Any_Derived : Any_Base {
    T object;
    Any_Derived(const T& object) : object(object) {}
    Any_Base* clone() override { return new Any_Derived<T>(object); }
  };

  Any_Base* ptr = nullptr;

 public:
  Any() = default;

  template <typename T>
  Any(const T& object) : ptr(new Any_Derived<T>(object)) {}

  Any(const Any& other) : ptr(other.ptr->clone()) {}

  Any(Any&& other) {
    Swap(other);
    other.ptr = nullptr;
  }

  ~Any() { delete ptr; }

  Any& operator=(const Any& other) {
    if (ptr == other.ptr)
      return *this;
    Reset();
    ptr = other.ptr->clone();
    return *this;
  }

  Any& operator=(Any&& other) {
    if (ptr == other.ptr)
      return *this;
    Reset();
    Swap(other);
    return *this;
  }

  template <typename T>
  Any& operator=(const T& object) {
    Reset();
    ptr = new Any_Derived<T>(object);
    return *this;
  }

  void Reset() {
    delete ptr;
    ptr = nullptr;
  }

  void Swap(Any& other) { std::swap(ptr, other.ptr); }

  bool HasValue() const { return ptr; }

  template <typename T>
  friend T AnyCast(const Any&);
};

template <typename T, typename... Args>
Any MakeAny(Args&&... args) {
  T* object = new T(std::forward<Args>(args)...);
  return Any(*object);
}

template <typename T>
T AnyCast(const Any& other) {
  if (!other.HasValue())
    throw std::bad_any_cast();
  Any::Any_Derived<T>* ptr;
  ptr = dynamic_cast<Any::Any_Derived<T>*>(other.ptr);
  if (ptr) {
    return ptr->object;
  }
  throw std::bad_any_cast();
}