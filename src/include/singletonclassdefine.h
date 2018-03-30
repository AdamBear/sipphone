#ifndef __SINGLETON_CLASS_DEFINE_H__
#define __SINGLETON_CLASS_DEFINE_H__


#define DECLARE_SINGLETON_CLASS(className) \
        public:\
            static className& GetInstance() DECLARE_ATTR;\
        private:\
            className();\
            ~className();

#define GET_SINGLETON_INSTANCE(className)  className::GetInstance()

#define IMPLEMENT_GETINSTANCE(className) \
        className& className::GetInstance() \
        {\
            static className s_Instance;\
            return s_Instance;\
        }

////////////////////////////for example begin/////////////////////////////////
#if 0
// CMyTest.h:
class CMyTest
{
    DECLARE_SINGLETON_CLASS(CMyTest)

public:
    void TestFun();
}
#define _objMyTest GET_SINGLETON_INSTANCE(CMyTest)

// CMyTest.cpp:
IMPLEMENT_GETINSTANCE(CMyTest)

CMyTest::CMyTest() {}
CMyTest::~CMyTest() {}

void CMyTest::TestFun() {}

#endif  // #if 0
////////////////////////////for example end///////////////////////////////////

#endif // __SINGLETON_CLASS_DEFINE_H__
