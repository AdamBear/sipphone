#ifndef __KRIME_H__
#define __KRIME_H__

#ifdef IF_FEATURE_KOREANIME
class CAutomataKR
{
public:
    CAutomataKR(void);
    ~CAutomataKR(void);

public:
    void        Clear();                    // Initializing Buffer
    WCHAR       SetKeyCode(int
                           nKeyCode);  // Got a key-code (Combination Korean word when entered fixed int code value)

    WCHAR       ingWord;        // writing Word
    yl::wstring     completeText;   // complete Text

private:
    enum    HAN_STATUS      // status of word combination
    {
        HS_FIRST = 0,       // initial consonant
        HS_FIRST_V,         // consonant + consonant
        HS_FIRST_C,         // vowel + vowel
        HS_MIDDLE_STATE,    // initial consonant + vowel + vowel
        HS_END,             // initial consonant + a neutral vowel + final consonant
        HS_END_STATE,       // initial consonant + a neutral vowel + consonant + consonant
        HS_END_EXCEPTION    // initial consonant + a neutral vowel + final consonant(Double consonant)
    };

    int         m_nStatus;      // Status of word combination
    int
    m_nPhonemez[5]; // Phoneme[initial consonant,a neutral vowel,final consonant,Double consonant1,Double consonant2]

    WCHAR       m_completeWord; // Complete Word


    // Transformation
    int         ToInitial(int nKeyCode);    // To initial consonant
    int         ToFinal(int nKeyCode);      // To final consonant


    // Decompose
    void        DecomposeConsonant();       // Decompose Consonant


    // Mixing
    BOOL        MixInitial(int nKeyCode);   // Mixing initial consonant
    BOOL        MixFinal(int nKeyCode);     // Mixing final consonant

    BOOL        MixVowel(int * nCurCode, int nInCode);  // Mixing vowel


    // Combination(Completion Korean)
    WCHAR       CombineHangle(int cho, int jung, int jong);
    WCHAR       CombineHangle(int status);
    void        CombineIngWord(int status);

    int         DownGradeIngWordStatus(WCHAR word); //Downgrading combination Word Status
};
#endif // #ifdef IF_FEATURE_KOREANIME
#endif // __KRIME_H__

