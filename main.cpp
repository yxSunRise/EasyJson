#include <iostream>
#include <vector>
#include <map>
#include "easy_json.h"
using namespace std;

struct Student {
    long    id;
    bool    sex;
    double  score;
    string  name;

    JSON_HELP(id,sex,score,name)
};
struct Teacher {
    string          name;
    int             subject;
    vector<Student> stus;

    JSON_HELP(name,subject,stus)
};


// 对应于结构 map<string,Teacher> 的json
string ori = R"(
{
  "Tea_1": {
    "name": "Tea_1",
    "subject": 3,
    "stus": [
      {
        "id": 201721020126,
        "sex": false,
        "score": 80,
        "name": "Stu.a"
      },
      {
        "id": 201101101537,
        "sex": true,
        "score": 0,
        "name": "Stu.b"
      }
    ]
  },
  "Tea_2": {
    "name": "Tea_2",
    "subject": 1,
    "stus": [
      {
        "id": 201521020128,
        "sex": true,
        "score": 59,
        "name": "Stu.c"
      }
    ]
  }
}
)";

int main() {
    map<string,Teacher> tchs;

    // 从json字符串反序列化对象
    bool ret = Json::Unmarshal(tchs,ori);
    if(!ret){
        cout<<"反序列失败"<<endl;
        return 0;
    }else{
        cout<<"反序列成功"<<endl;
    }

    // 序列化对象到 json字符串
    cout<<"输出对象序列化的json:"<<endl;
    string obj2json;
    Json::Marshal(tchs,obj2json);
    cout<<obj2json;
}