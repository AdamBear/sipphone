// 内存块类
#ifndef  _YEALINK_LIGHT_STL_MEMORY_
#define _YEALINK_LIGHT_STL_MEMORY_
struct TMemPlex     // warning variable length structure
{
    TMemPlex * pNext;

    void * data()
    {
        return this + 1;
    }

    static TMemPlex * create(TMemPlex *& pHead, unsigned int nMax, unsigned int cbElement)
    {
        assert(nMax > 0 && cbElement > 0);
        TMemPlex * p = (TMemPlex *) new unsigned char[sizeof(TMemPlex) + nMax * cbElement];
        p->pNext = pHead;
        pHead = p;
        return p;
    }

    void freeDataChain()
    {
        TMemPlex * p = this;
        while (p != NULL)
        {
            unsigned char * bytes = (unsigned char *) p;
            TMemPlex * pNextTemp = p->pNext;
            delete[] bytes;
            p = pNextTemp;
        }
    }       // free this one and links
};
#endif

