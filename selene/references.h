#pragma once

#include <cstddef>
#include "LuaRef.h"
#include <utility>

namespace Selene {

template<typename T>
class Reference {
    LuaRef _lifetime;
    T *_obj;
public:
    Reference(T &obj, LuaRef lifetime)
      : _lifetime(std::move(lifetime))
      , _obj(&obj) {}

    T& get() const {
        return *_obj;
    }

    operator T&() const {
        return get();
    }

    void _push(lua_State *l) const {
        _lifetime.Push(l);
    }
};

template<typename T>
class Pointer {
    LuaRef _lifetime;
    T *_obj;
public:
    Pointer(T *obj, LuaRef lifetime)
      : _lifetime(std::move(lifetime))
      , _obj(obj)
    {}

    Pointer(LuaRef lifetime)
      : Pointer(nullptr, std::move(lifetime))
    {}

    T* get() const {
        return _obj;
    }

    T* operator->() const {
        return _obj;
    }

    T& operator*() const {
        return *_obj;
    }

    operator bool() const {
        return _obj;
    }

    bool operator!() const {
        return !_obj;
    }

    friend bool operator==(std::nullptr_t, Pointer<T> const & ptr) {
        return nullptr == ptr._obj;
    }

    friend bool operator==(Pointer<T> const & ptr, std::nullptr_t) {
        return nullptr == ptr._obj;
    }

    friend bool operator!=(std::nullptr_t, Pointer<T> const & ptr) {
        return !(nullptr == ptr);
    }

    friend bool operator!=(Pointer<T> const & ptr, std::nullptr_t) {
        return !(nullptr == ptr);
    }

    friend bool operator==(Pointer<T> const & ptrA, Pointer<T> const & ptrB) {
        return ptrA.get() == ptrB.get();
    }

    friend bool operator!=(Pointer<T> const & ptrA, Pointer<T> const & ptrB) {
        return ptrA.get() != ptrB.get();
    }

    void _push(lua_State *l) const {
        _lifetime.Push(l);
    }
};

namespace detail {

template<typename T>
struct is_primitive<Selene::Reference<T>> {
    static constexpr bool value = true;
};

template <typename T>
inline Selene::Reference<T> _check_get(_id<Selene::Reference<T>>,
                                    lua_State *l, const int index) {
    T& result = _check_get(_id<T&>{}, l, index);
    lua_pushvalue(l, index);
    LuaRef lifetime(l, luaL_ref(l, LUA_REGISTRYINDEX));
    return {result, lifetime};
}

template <typename T>
inline Selene::Reference<T> _get(_id<Selene::Reference<T>>,
                              lua_State *l, const int index) {
    T& result = _get(_id<T&>{}, l, index);
    lua_pushvalue(l, index);
    LuaRef lifetime(l, luaL_ref(l, LUA_REGISTRYINDEX));
    return {result, lifetime};
}

template<typename T>
inline void _push(lua_State *l, Selene::Reference<T> const & ref) {
    ref._push(l);
}


template<typename T>
struct is_primitive<Selene::Pointer<T>> {
    static constexpr bool value = true;
};

template <typename T>
inline Selene::Pointer<T> _check_get(_id<Selene::Pointer<T>>,
                                    lua_State *l, const int index) {
    auto result = _check_get(_id<T*>{}, l, index);
    if(result) {
        lua_pushvalue(l, index);
        LuaRef lifetime(l, luaL_ref(l, LUA_REGISTRYINDEX));
        return {result, lifetime};
    } else {
        return {LuaRef(l)};
    }
}

template <typename T>
inline Selene::Pointer<T> _get(_id<Selene::Pointer<T>>,
                              lua_State *l, const int index) {
    auto result = _get(_id<T*>{}, l, index);
    if(result) {
        lua_pushvalue(l, index);
        LuaRef lifetime(l, luaL_ref(l, LUA_REGISTRYINDEX));
        return {result, lifetime};
    } else {
        return {LuaRef(l)};
    }
}

template<typename T>
inline void _push(lua_State *l, Selene::Pointer<T> const & ptr) {
    ptr._push(l);
}

}
}
