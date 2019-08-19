#ifndef __EASY_JSON_H__
#define __EASY_JSON_H__

#include <vector>
#include <string>
#include <map>
#include "json/json.h"

namespace Json {
    using namespace std;

/*======  Unmarshal实现  =======*/

    // 检测一个类 是否含有非静态非重载的unmarshal方法
    template<typename T>
    struct TestUnmarshalFunc {
        template<typename TT>
        static char func(decltype(&TT::unmarshal));
        template<typename TT>
        static int func(...);
        const static bool has = (sizeof(func<T>(NULL)) == sizeof(char));
    };

    //如果对象自身含有 unmarshal 方法，则调用对象的unmarshal
    template <typename T,typename enable_if<TestUnmarshalFunc<T>::has,int>::type = 0>
    inline bool Unmarshal(T& obj,const Json::Value &root){
        return obj.unmarshal(root);
    }

    //特例化基本类型 int long bool float double string
    template <int = 0>
    inline bool Unmarshal(int& obj,const Json::Value &root){
        if(!root.isIntegral())
            return false;
        obj = root.asInt();
        return true;
    }

    template <int = 0>
    inline bool Unmarshal(long& obj,const Json::Value &root){
        if(!root.isIntegral())
            return false;
        obj = root.asInt64();
        return true;
    }

    template <int = 0>
    inline bool Unmarshal(bool& obj,const Json::Value &root){
        if(!root.isBool())
            return false;
        obj = root.asBool();
        return true;
    }

    template <int = 0>
    inline bool Unmarshal(float & obj,const Json::Value &root){
        if(!root.isDouble())
            return false;
        obj = root.asFloat();
        return true;
    }

    template <int = 0>
    inline bool Unmarshal(double & obj,const Json::Value &root){
        if(!root.isDouble())
            return false;
        obj = root.asDouble();
        return true;
    }

    template <int = 0>
    inline bool Unmarshal(string & obj,const Json::Value &root){
        if(!root.isString())
            return false;
        obj = root.asString();
        return true;
    }


    //特例化stl容器或其他不含unmarshal方法的对象
    template <typename T>
    bool Unmarshal(vector<T>& ,const Json::Value&); //在模板相互循环调用对方时,必须先声明,使所有类型的模板可见!
    template <typename T>
    bool Unmarshal(map<string,T>& ,const Json::Value&);

    //vector
    template <typename T>
    bool Unmarshal(vector<T>& obj,const Json::Value& root){
        if(!root.isArray())
            return false;
        obj.clear();
        bool ret = true;
        for(int i=0;i<root.size();++i){
            T tmp;  //类型T一定要含有默认构造器！
            if(!Unmarshal(tmp,root[i]))
                ret = false;
            obj.push_back(tmp);
        }
        return ret;
    }

    //map
    template <typename T>
    bool Unmarshal(map<string,T>& obj,const Json::Value& root){
        if(!root.isObject())
            return false;
        obj.clear();
        auto mems = root.getMemberNames();
        bool ret = true;
        for(auto it = mems.begin();it!=mems.end();++it){
            if(!Unmarshal(obj[*it],root[*it]))
                ret = false;
        }
        return ret;
    }


/*======  Marshal实现  =======*/

    // 检测一个类 是否含有非静态非重载的marshal方法
    template<typename T>
    struct TestMarshalFunc {
        template<typename TT>
        static char func(decltype(&TT::marshal));
        template<typename TT>
        static int func(...);
        const static bool has = (sizeof(func<T>(NULL)) == sizeof(char));
    };

    template <typename T,typename enable_if<TestMarshalFunc<T>::has,int>::type = 0>
    inline void Marshal(const T& obj,Json::Value &root){
        obj.marshal(root);
    }
    template <typename T,typename enable_if<!TestMarshalFunc<T>::has,int>::type = 0>
    inline void Marshal(const T& obj,Json::Value &root){
        root = obj;
    }

    //特例化jsoncpp 不支持的基本类型
    template <int = 0>
    inline void Marshal(long obj,Json::Value &root){
        root = Int64(obj);
    }

    //特例化容器或没有实现marshal函数的类 vector map
    template <typename T>
    void Marshal(const vector<T>& ,Json::Value &);
    template <typename T>
    void Marshal(const map<string,T>& ,Json::Value &);

    // vector
    template <typename T>
    void Marshal(const vector<T>& obj,Json::Value &root){
        for(int i=0;i<obj.size();++i){
            Marshal(obj[i],root[i]);
        }
    }

    // map
    template <typename T>
    void Marshal(const map<string,T>& obj,Json::Value &root){
        for(auto it=obj.begin();it!=obj.end();++it){
            Marshal(it->second,root[it->first]);
        }
    }


/*========  string 版本  ===========*/
    template <typename T>
    void Marshal(const T& obj,string &s){
        Json::Value root;
        Json::FastWriter writer;
        Marshal(obj,root);
        if(root.isNull())
            s = "";
        else
            s = writer.write(root);
    }

    template <typename T>
    bool Unmarshal(T& obj,const string &s){
        Json::Reader reader;
        Json::Value root;
        if(!reader.parse(s,root))
            return false;
        return Unmarshal(obj,root);
    }
}


// 一些有用的宏,小写变量命名的宏只限本文件内使用

// 将任意宏完全展开为字符串,调试用
#define TO_STRING(...) __to_string__(__VA_ARGS__)
#define __to_string__(...) #__VA_ARGS__

// 统计可变参数个数的宏
#define COUNT(...) __count__(0, ##__VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define __count__(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, N, ...) N

// 将两个宏展开后,连接在一起
#define MACRO_CAT(a,b)  __macro_cat__(a,b)
#define __macro_cat__(a,b)  a##b

// 宏展开实现伪循环
#define __func_1(func,member)   func(member);
#define __func_2(func,member,...)     __func_1(func,member)     __func_1(func,__VA_ARGS__)
#define __func_3(func,member,...)     __func_1(func,member)     __func_2(func,__VA_ARGS__)
#define __func_4(func,member,...)     __func_1(func,member)     __func_3(func,__VA_ARGS__)
#define __func_5(func,member,...)     __func_1(func,member)     __func_4(func,__VA_ARGS__)
#define __func_6(func,member,...)     __func_1(func,member)     __func_5(func,__VA_ARGS__)
#define __func_7(func,member,...)     __func_1(func,member)     __func_6(func,__VA_ARGS__)
#define __func_8(func,member,...)     __func_1(func,member)     __func_7(func,__VA_ARGS__)
#define __func_9(func,member,...)     __func_1(func,member)     __func_8(func,__VA_ARGS__)
#define __func_10(func,member,...)    __func_1(func,member)     __func_9(func,__VA_ARGS__)
#define __func_11(func,member,...)    __func_1(func,member)     __func_10(func,__VA_ARGS__)
#define __func_12(func,member,...)    __func_1(func,member)     __func_11(func,__VA_ARGS__)
#define __func_13(func,member,...)    __func_1(func,member)     __func_12(func,__VA_ARGS__)
#define __func_14(func,member,...)    __func_1(func,member)     __func_13(func,__VA_ARGS__)
#define __func_15(func,member,...)    __func_1(func,member)     __func_14(func,__VA_ARGS__)
#define __func_16(func,member,...)    __func_1(func,member)     __func_15(func,__VA_ARGS__)
#define __func_17(func,member,...)    __func_1(func,member)     __func_16(func,__VA_ARGS__)
#define __func_18(func,member,...)    __func_1(func,member)     __func_17(func,__VA_ARGS__)
#define __func_19(func,member,...)    __func_1(func,member)     __func_18(func,__VA_ARGS__)
#define __func_20(func,member,...)    __func_1(func,member)     __func_19(func,__VA_ARGS__)

// 传入一个函数func和一个list，把func用在list的每个元素上
#define FOR_EACH(func,...)  MACRO_CAT(__func_,COUNT(__VA_ARGS__))(func,__VA_ARGS__)

//在一个对象内部生成 unmarshal 函数.
//尽力解析每个field,但不保证全部正确,未被正确解析的field保持原有值不变. 所有field 均被正确解析时返回true
#define UNMARSHAL_OBJ(...)                                  \
    bool unmarshal(const Json::Value& root){                \
        bool ret = true;                                    \
        FOR_EACH(__unmarshal_obj_each_field__,__VA_ARGS__)  \
        return ret;                                         \
    }
#define __unmarshal_obj_each_field__(field)         \
        if(!Json::Unmarshal(field,root[#field])){   \
            ret = false;                            \
        }

//在一个对象内部生成 marshal 函数
#define MARSHAL_OBJ(...)                                    \
    void marshal(Json::Value& root) const {                 \
        FOR_EACH(__marshal_obj_each_field__,__VA_ARGS__)    \
    }
#define __marshal_obj_each_field__(field)   Json::Marshal(field,root[#field]);


//让一个对象可以被 unmarshal和marshal
#define JSON_HELP(...)          \
    UNMARSHAL_OBJ(__VA_ARGS__)  \
    MARSHAL_OBJ(__VA_ARGS__)

#endif //__EASY_JSON_H__
