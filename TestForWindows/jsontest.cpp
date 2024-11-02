#include<iostream>
#include<json/json.h>
#include<fstream>

using namespace Json;
using namespace std;

//дjson�ļ�
void writeJson()
{
	//[12,13.45,"hello, world",true,false,[1,2,"aa"],{"a":"b"}]
	//��֯���� --> �ŵ�Value������
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
	//��Value�����ʽ��Ϊstring
	// ��ʽ�� -> ������ת��Ϊ�ַ��� �����з�
	// �ʺ��ڲ鿴��Ϣ����д�ļ�
	string jsonstring = root.toStyledString();
	// ��Value�����е����ݸ�ʽ�� -> �ַ���
	// �ʺ����������ݵķ���
	// �õ����ַ�����û�л��з�
	//FastWriter w;
	//string jsonstring = w.write(root);
	//д�����ļ�
	ofstream ofs;
	ofs.open("jsontest.json");
	ofs << jsonstring;
	ofs.close();
}
//��json�ļ�
void readJson()
{
	//�򿪴����ļ�
	ifstream ifs;
	ifs.open("jsontest.json");
	Reader r;
	Value root;
	r.parse(ifs, root);
	//ͨ��root����ȡ����
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