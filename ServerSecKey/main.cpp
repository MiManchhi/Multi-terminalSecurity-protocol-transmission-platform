#include<iostream>
#include"ServerOP.h"

using namespace std;

int main()
{
	ServerOP* op = new ServerOP("server.json");
	op->startServer();
	return 0;
}