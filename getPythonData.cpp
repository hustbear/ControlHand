#include "stdafx.h"
#include "getPythonData.h"


getPythonData::getPythonData()
{
}
void getPythonData::initPython() {
	Py_SetPythonHome("F:/Python27/");
	Py_Initialize();//ʹ��python֮ǰ��Ҫ����Py_Initialize();����������г�ʼ�� 
	pModule = PyImport_ImportModule("pytest");
	if (!pModule) // ����ģ��ʧ��
	{
		cout << "[ERROR] Python get module failed." << endl;
		return ;
	}
	pFunc = PyObject_GetAttrString(pModule, "caculate");
	pFuncTumb= PyObject_GetAttrString(pModule, "caculateTumb");
	/*
	if (!pFunc && !pFuncTumb) // ����ģ��ʧ��
	{
		cout << "[ERROR] Python get pFunc failed." << endl;
		return ;
	}*/
}
int getPythonData::getGloveGesture(string s1)
{
	//��������:
	PyObject *pArgs = PyTuple_New(1);//�������õĲ������ݾ�����Ԫ�����ʽ�����,2��ʾ��������
	PyObject *st = Py_BuildValue("s", s1);
	PyTuple_SetItem(pArgs, 0, st);//0--���,i��ʾ����int�ͱ���
	PyObject *pReturn = NULL;
	pReturn = PyEval_CallObject(pFunc, pArgs);//���ú���
	//������ֵת��Ϊint����
	int result;
	PyArg_Parse(pReturn, "i", &result);//i��ʾת����int�ͱ���
	//Py_Finalize();
	return result;
}

float getPythonData::getTumbData(string s2)
{
	//��������:
	PyObject *pArgs = PyTuple_New(1);//�������õĲ������ݾ�����Ԫ�����ʽ�����,2��ʾ��������
	PyObject *st = Py_BuildValue("s", s2);
	PyTuple_SetItem(pArgs, 0, st);//0--���,i��ʾ����int�ͱ���
	PyObject *pReturn = NULL;
	pReturn = PyEval_CallObject(pFuncTumb, pArgs);//���ú���
											  //������ֵת��Ϊint����
	float result;
	PyArg_Parse(pReturn, "f", &result);//i��ʾת����int�ͱ���
									   //Py_Finalize();
	return result;
}




getPythonData::~getPythonData()
{
}
