

#ifndef LUADROID_FUNC_INFO_H
#define LUADROID_FUNC_INFO_H

#include <string>
#include <unordered_map>
#include <vector>
#include "myarray.h"

class JavaType;

struct Import {
    typedef Map<String, JavaType *> TypeCache;
    Vector<String> packages;
    TypeCache stubbed;

    Import() {
        packages.push_back("java.lang.");
        packages.push_back("java.util.");
        packages.push_back("android.view.");
        packages.push_back("android.widget.");
        packages.push_back("android.app.");
    }

    Import(Import &&other) : packages(std::move(other.packages)),
                             stubbed(std::move(other.stubbed)) {
    }

    Import(const Import &other) : packages(other.packages), stubbed(other.stubbed) {
    }
};

struct CrossThreadLuaObject;

struct BaseFunction {
    int javaRefCount = 0;

    virtual bool isLocal()=0;

    virtual ~BaseFunction() {};
};

class FuncInfo : public BaseFunction {
    Import *imported = nullptr;
    Array<CrossThreadLuaObject> upvalues;
public:
    const Array<char> funcData;
    const lua_CFunction cFunc;
    const bool isCFunc;
#if LUA_VERSION_NUM >502
    int globalIndex=0;
#endif

    explicit FuncInfo(const lua_CFunction func) : cFunc(func), isCFunc(true) {}

    explicit FuncInfo(Array<char> &&ptr) : funcData(std::forward<Array<char>>(ptr)), cFunc(nullptr),
                                           isCFunc(false) {}

    FuncInfo(FuncInfo &&other) : upvalues(std::move(other.upvalues)),
                                 funcData(std::move(other.funcData)), cFunc(other.cFunc),
                                 isCFunc(other.isCFunc), imported(other.imported) {
        other.imported = nullptr;
    }

    inline void setImport(const Import *imported) {
        this->imported = imported == nullptr ? new Import() : new Import(*imported);
    }

    inline Import *getImport() const {
        return imported;
    }

    inline void setUpValues(Array<CrossThreadLuaObject> &&upvalues) {
        this->upvalues = std::forward<Array<CrossThreadLuaObject>>(upvalues);
    }

    inline const Array<CrossThreadLuaObject> &getUpValues() const {
        return upvalues;
    }

    FuncInfo &operator=(const FuncInfo &)= delete;

    FuncInfo &operator=(FuncInfo &&o)= delete;

    virtual bool isLocal() { return false; };

    virtual ~FuncInfo() {
        delete imported;
    }
};

class LocalFunctionInfo : public BaseFunction {
    lua_State *L;
public:
    LocalFunctionInfo(lua_State *L) : L(L) {}

    virtual bool isLocal() { return true; };

    virtual ~LocalFunctionInfo() {
        lua_pushlightuserdata(L, this);
        lua_pushnil(L);
        lua_settable(L, LUA_REGISTRYINDEX);
    };
};

#endif //LUADROID_FUNCINFO_H
