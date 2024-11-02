#include<iostream>
#include<json/json.h>
#include<fstream>

using namespace Json;
using namespace std;

//写json文件
void writeJson()
{
	//[12,13.45,"hello, world",true,false,[1,2,"aa"],{"a":"b"}]
	//组织数据 --> 放到Value对象中
	Value root;
	root.append(12);
	root.append(13.45);
	root.append("hello, world");
	root.append(true);
	root.append(false);
	Value sub;
	sub.append(1);
	sub.append(2);
	sub.append("aa");
	root.append(sub);
	Value obj;
	obj["a"] = "b";
	root.append(obj);
	//将Value对象格式化为string
	// 格式化 -> 将对象转换为字符串 带换行符
	// 适合于查看信息或者写文件
	string jsonstring = root.toStyledString();
	// 将Value对象中的数据格式化 -> 字符串
	// 适合于网络数据的发送
	// 得到的字符串中没有换行符
	//FastWriter w;
	//string jsonstring = w.write(root);
	//写磁盘文件
	ofstream ofs;
	ofs.open("jsontest.json");
	ofs << jsonstring;
	ofs.close();
}
//读json文件
void readJson()
{
	//打开磁盘文件
	ifstream ifs;
	ifs.open("jsontest.json");
	Reader r;
	Value root;
	r.parse(ifs, root);
	//通过root对象取数据
	if (root.isArray())
	{
		for (int i = 0; i < root.size(); i++)
		{
			Value sub = root[i];
			if (sub.isInt())
			{
				cout << sub.asInt() << " ";
			}
			else if (sub.isString())
			{
				cout << sub.asString() << " ";
			}
			else if (sub.isBool())
			{
				cout << sub.asBool() << " ";
			}
		}
		cout << endl;
	}
	ifs.close();
}

int main()
{
	writeJson();
	readJson();
	return 0;
}