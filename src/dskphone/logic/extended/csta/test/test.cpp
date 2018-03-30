#if 0
#include "cstaprocesser.h"
#include <fstream>

void SendMessage(yl::string strPath)
{
    //读取文件内容
    std::ifstream file(strPath.c_str());
    file.seekg(0, std::ifstream::end);
    int length = file.tellg();
    file.seekg(0, std::ifstream::beg);
    char * xml_stream = new char[length + 1];

    memset(xml_stream, 0, length + 1);
    file.read(xml_stream, length);


    msgObject obj;

    obj.message = 1;

    obj.ResetExtraData(length + 1, (LPCVOID)xml_stream);

    g_objCSTAManager.ProcessCSTA(obj);


    delete xml_stream;

}


void TestMakeCall()
{
    yl::string strPath;

    strPath = "/mnt/makeCall.xml";

    SendMessage(strPath);
}

void TestBindTransfer()
{
    yl::string strPath;

    strPath = "/mnt/BindTransfer.xml";

    SendMessage(strPath);

}

void TestHoldCall()
{
    yl::string strPath;

    strPath = "/mnt/holdCall.xml";

    SendMessage(strPath);

}

void TestRetrieveCall()
{
    yl::string strPath;

    strPath = "/mnt/retrieveCall.xml";

    SendMessage(strPath);

}

void TestClearCall()
{
    yl::string strPath;

    strPath = "/mnt/clearConnect.xml";

    SendMessage(strPath);

}

void TestAnswerCall()
{
    yl::string strPath;

    strPath = "/mnt/answerCall.xml";

    SendMessage(strPath);

}

#endif
