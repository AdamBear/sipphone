#include "../ETLLib/ETLLib.hpp"
#include "automatakr.h"
#include "koreanime.h"
#include "interfacedefine.h"

#ifdef IF_FEATURE_KOREANIME
const int BASE_CODE = 0xac00;       // Basic sound(A)
const int LIMIT_MIN = 0xac00;       // Sound Range MIN(A)
const int LIMIT_MAX = 0xd7a3;       // Sound Range MAX

// Sound Table
WCHAR SOUND_TABLE[68] =
{
    /* initial consonant 19character 0 ~ 18 */
    0x3131, 0x3132, 0x3134, 0x3137, 0x3138,
    0x3139, 0x3141, 0x3142, 0x3143, 0x3145,
    0x3146, 0x3147, 0x3148, 0x3149, 0x314A,
    0x314B, 0x314C, 0x314D, 0x314E,
    /* a neutral vowel 21character 19 ~ 39 */
    0x314F, 0x3150, 0x3151, 0x3152, 0x3153,
    0x3154, 0x3155, 0x3156, 0x3157, 0x3158,
    0x3159, 0x315A, 0x315B, 0x315C, 0x315D,
    0x315E, 0x315F, 0x3160, 0x3161, 0x3162,
    0x3163,
    /* final consonant 28character 40 ~ 67 */
    0x0020, 0x3131, 0x3132, 0x3133, 0x3134,
    0x3135, 0x3136, 0x3137, 0x3139, 0x313A,
    0x313B, 0x313C, 0x313D, 0x313E, 0x313F,
    0x3140, 0x3141, 0x3142, 0x3144, 0x3145,
    0x3146, 0x3147, 0x3148, 0x314A, 0x314B,
    0x414C, 0x314D, 0x314E
};

// Mixing initial consonant table
int MIXED_CHO_CONSON[14][3] =
{
    { 0, 0, 15}, // ぁ,ぁ,せ
    {15, 0, 1}, // せ,ぁ,あ
    { 1, 0, 0}, // あ,ぁ,ぁ

    { 3, 3, 16}, // ぇ,ぇ,ぜ
    {16, 3, 4}, // ぜ,ぇ,え
    { 4, 3, 3}, // え,ぇ,ぇ

    { 7, 7, 17}, // げ,げ,そ
    {17, 7, 8}, // そ,げ,こ
    { 8, 7, 7}, // こ,げ,げ

    { 9, 9, 10}, // さ,さ,ざ
    {10, 9, 9}, // ざ,さ,さ

    {12, 12, 14}, // じ,じ,ず
    {14, 12, 13}, // ず,じ,す
    {13, 12, 12} // す,じ,じ
};

// Mixing initial consonant,a neutral vowel vowel Table
int MIXED_VOWEL[21][3] =
{
    {19, 19, 21}, // た,た,ち
    {21, 19, 19}, // ち,た,た

    {19, 39, 20}, // た,び,だ
    {21, 39, 22}, // ち,び,ぢ

    {23, 23, 25}, // っ,っ,づ
    {25, 23, 23}, // づ,っ,っ

    {23, 39, 24}, // っ,び,つ
    {25, 39, 26}, // づ,び,て

    {27, 27, 31}, // で,で,に
    {31, 27, 27}, // に,で,で

    {27, 19, 28}, // で,た,と
    {28, 39, 29}, // と,び,ど

    {27, 39, 30}, // で,び,な

    {32, 32, 36}, // ぬ,ぬ,ば
    {36, 32, 32}, // ば,ぬ,ぬ

    {32, 23, 33}, // ぬ,っ,ね
    {33, 39, 34}, // ね,び,の

    {32, 39, 35}, // ぬ,び,は

    {39, 39, 37}, // び,び,ぱ
    {37, 39, 38}, // ぱ,び,ひ
    {38, 39, 39} // ひ,び,び
};

// Mixing final consonant table
int MIXED_JONG_CONSON[22][3] =
{
    {41, 41, 64}, // ぁ,ぁ,せ
    {64, 41, 42}, // せ,ぁ,あ
    {42, 41, 41}, // あ,ぁ,ぁ

    {41, 59, 43}, // ぁ,さ,ぃ

    {44, 62, 45}, // い,じ,ぅ
    {44, 67, 46}, // い,ぞ,う

    {47, 47, 65}, // ぇ,ぇ,ぜ
    {65, 47, 47}, // ぜ,ぇ,ぇ

    {48, 41, 49}, // ぉ,ぁ,お
    {48, 56, 50}, // ぉ,け,か

    {48, 57, 51}, // ぉ,げ,が
    {51, 57, 54}, // が,げ,く

    {48, 59, 52}, // ぉ,さ,き
    {48, 47, 53}, // ぉ,ぇ,ぎ
    {48, 67, 55}, // ぉ,ぞ,ぐ

    {57, 57, 66}, // げ,げ,そ
    {66, 57, 57}, // そ,げ,げ

    {57, 59, 58}, // げ,さ,ご

    {59, 59, 60}, // さ,さ,ざ
    {60, 59, 59}, // ざ,さ,さ

    {62, 62, 63}, // じ,じ,ず
    {63, 62, 62} // ず,じ,じ
};

// Dividing final consonant table
int DIVIDE_JONG_CONSON[13][3] =
{
    {41, 41, 42}, // ぁ,ぁ,あ
    {41, 59, 43}, // ぁ,さ,ぃ
    {44, 62, 45}, // い,じ,ぅ
    {44, 67, 46}, // い,ぞ,う
    {48, 41, 49}, // ぉ,ぁ,お
    {48, 56, 50}, // ぉ,け,か
    {48, 57, 51}, // ぉ,げ,が
    {48, 66, 54}, // ぉ,そ,く
    {48, 59, 52}, // ぉ,さ,き
    {48, 65, 53}, // ぉ,ぜ,ぎ
    {48, 67, 55}, // ぉ,ぞ,ぐ
    {57, 59, 58}, // げ,さ,ご
    {59, 59, 60} // さ,さ,ざ
};

CAutomataKR::CAutomataKR(void)
{
    Clear();
}

CAutomataKR::~CAutomataKR(void)
{
}

// Initializing Buffer
void CAutomataKR::Clear()
{
    m_nStatus       = HS_FIRST;
    completeText.clear();
    ingWord         = 0;
    m_completeWord  = 0;
}

// Input the Key-code & combination (Combination Korean word when entered fixed int code value)
WCHAR CAutomataKR::SetKeyCode(int nKeyCode)
{
    m_completeWord = 0;

    // Input the Hot-key
    if (nKeyCode < 0)
    {
        m_nStatus = HS_FIRST;

        if (nKeyCode == KEY_CODE_SPACE) // Spacing
        {
            if (ingWord != 0)
            {
                completeText += ingWord;
            }
            else
            {
                completeText += _T(' ');
            }

            ingWord = 0;
        }
        else if (nKeyCode == KEY_CODE_ENTER) // Writing go down
        {
            if (ingWord != 0)
            {
                completeText += ingWord;
            }

            completeText += L"\r\n";

            ingWord = 0;
        }
        else if (nKeyCode == KEY_CODE_BACKSPACE) // Remove character
        {
            if (ingWord == 0)
            {
                if (completeText.size() > 0)
                {
                    if (completeText[completeText.size() - 1] == L'\n')
                    {
                        completeText = completeText.substr(0, completeText.size() - 2);
                    }
                    else
                    {
                        completeText = completeText.substr(0, completeText.size() - 1);
                    }
                }
            }
            else
            {
                m_nStatus = DownGradeIngWordStatus(ingWord);
            }
        }

        return m_completeWord;
    }



    switch (m_nStatus)
    {
    case HS_FIRST:
        // initial consonant
        m_nPhonemez[0]  = nKeyCode;
        ingWord         = SOUND_TABLE[m_nPhonemez[0]];
        m_nStatus       = nKeyCode > 18 ? HS_FIRST_C : HS_FIRST_V;
        break;

    case HS_FIRST_C:
        // vowel + vowel
        if (nKeyCode > 18)  // vowel
        {
            if (MixVowel(&m_nPhonemez[0], nKeyCode) == FALSE)
            {
                m_completeWord = SOUND_TABLE[m_nPhonemez[0]];
                m_nPhonemez[0] = nKeyCode;
            }
        }
        else                // consonant
        {
            m_completeWord  = SOUND_TABLE[m_nPhonemez[0]];
            m_nPhonemez[0]  = nKeyCode;
            m_nStatus       = HS_FIRST_V;
        }
        break;

    case HS_FIRST_V:
        // consonant + consonant
        if (nKeyCode > 18)  // vowel
        {
            m_nPhonemez[1]  = nKeyCode;
            m_nStatus       = HS_MIDDLE_STATE;
        }
        else                // consonant
        {
            if (!MixInitial(nKeyCode))
            {
                m_completeWord  = SOUND_TABLE[m_nPhonemez[0]];
                m_nPhonemez[0]  = nKeyCode;
                m_nStatus       = HS_FIRST_V;
            }
        }
        break;

    case HS_MIDDLE_STATE:
        // initial consonant + vowel + vowel
        if (nKeyCode > 18)
        {
            if (MixVowel(&m_nPhonemez[1], nKeyCode) == FALSE)
            {
                m_completeWord  = CombineHangle(1);
                m_nPhonemez[0]  = nKeyCode;
                m_nStatus       = HS_FIRST_C;
            }
        }
        else
        {
            int jungCode = ToFinal(nKeyCode);

            if (jungCode > 0)
            {
                m_nPhonemez[2]  = jungCode;
                m_nStatus       = HS_END_STATE;
            }
            else
            {
                m_completeWord  = CombineHangle(1);
                m_nPhonemez[0]  = nKeyCode;
                m_nStatus       = HS_FIRST;
            }
        }
        break;

    case HS_END:
        // initial consonant + a neutral vowel + final consonant
        if (nKeyCode > 18)
        {
            m_completeWord  = CombineHangle(1);
            m_nPhonemez[0]  = nKeyCode;
            m_nStatus       = HS_FIRST;
        }
        else
        {
            int jungCode = ToFinal(nKeyCode);
            if (jungCode > 0)
            {
                m_nPhonemez[2]  = jungCode;
                m_nStatus       = HS_END_STATE;
            }
            else
            {
                m_completeWord  = CombineHangle(1);
                m_nPhonemez[0]  = nKeyCode;
                m_nStatus       = HS_FIRST;
            }
        }
        break;

    case HS_END_STATE:
        // initial consonant + a neutral vowel + consonant(final consonant) + consonant(final consonant)
        if (nKeyCode > 18)
        {
            m_completeWord = CombineHangle(1);

            m_nPhonemez[0]  = ToInitial(m_nPhonemez[2]);
            m_nPhonemez[1]  = nKeyCode;
            m_nStatus       = HS_MIDDLE_STATE;
        }
        else
        {
            int jungCode = ToFinal(nKeyCode);
            if (jungCode > 0)
            {
                m_nStatus = HS_END_EXCEPTION;

                if (!MixFinal(jungCode))
                {
                    m_completeWord  = CombineHangle(2);
                    m_nPhonemez[0]  = nKeyCode;
                    m_nStatus       = HS_FIRST_V;
                }
            }
            else
            {
                m_completeWord  = CombineHangle(2);
                m_nPhonemez[0]  = nKeyCode;
                m_nStatus       = HS_FIRST_V;
            }
        }
        break;

    case HS_END_EXCEPTION:
        // initial consonant + a neutral vowel + final consonant(Double consonant)
        if (nKeyCode > 18)
        {
            DecomposeConsonant();
            m_nPhonemez[1]  = nKeyCode;
            m_nStatus       = HS_MIDDLE_STATE;
        }
        else
        {
            int jungCode = ToFinal(nKeyCode);
            if (jungCode > 0)
            {
                m_nStatus = HS_END_EXCEPTION;

                if (!MixFinal(jungCode))
                {
                    m_completeWord  = CombineHangle(2);
                    m_nPhonemez[0]  = nKeyCode;
                    m_nStatus       = HS_FIRST_V;
                }
            }
            else
            {
                m_completeWord  = CombineHangle(2);
                m_nPhonemez[0]  = nKeyCode;
                m_nStatus       = HS_FIRST_V;
            }
        }
        break;
    }

    // Word status when seeing
    CombineIngWord(m_nStatus);

    // Making the complete Text
    if (m_completeWord != 0)
    {
        completeText += m_completeWord;
    }

    return m_completeWord;
}

// Switching to initial consonant
int CAutomataKR::ToInitial(int nKeyCode)
{
    switch (nKeyCode)
    {
    case 41:
        return 0;  // ぁ
    case 42:
        return 1;  // あ
    case 44:
        return 2;  // い
    case 47:
        return 3;  // ぇ
    case 48:
        return 5;  // ぉ
    case 56:
        return 6;  // け
    case 57:
        return 7;  // げ
    case 59:
        return 9;  // さ
    case 60:
        return 10; // ざ
    case 61:
        return 11; // し
    case 62:
        return 12; // じ
    case 63:
        return 14; // ず
    case 64:
        return 15; // せ
    case 65:
        return 16; // ぜ
    case 66:
        return 17; // そ
    case 67:
        return 18; // ぞ
    }
    return -1;
}

// Switching to final consonant
int CAutomataKR::ToFinal(int nKeyCode)
{
    switch (nKeyCode)
    {
    case 0:
        return 41;  // ぁ
    case 1:
        return 42;  // あ
    case 2:
        return 44;  // い
    case 3:
        return 47;  // ぇ
    case 5:
        return 48;  // ぉ
    case 6:
        return 56;  // け
    case 7:
        return 57;  // げ
    case 9:
        return 59;  // さ
    case 10:
        return 60; // ざ
    case 11:
        return 61; // し
    case 12:
        return 62; // じ
    case 14:
        return 63; // ず
    case 15:
        return 64; // せ
    case 16:
        return 65; // ぜ
    case 17:
        return 66; // そ
    case 18:
        return 67; // ぞ
    }
    return -1;
}

// Decompose Consonant
void CAutomataKR::DecomposeConsonant()
{
    int i = 0;
    if (m_nPhonemez[3] > 40 || m_nPhonemez[4] > 40)
    {
        do
        {
            if (DIVIDE_JONG_CONSON[i][2] == m_nPhonemez[2])
            {
                m_nPhonemez[3] = DIVIDE_JONG_CONSON[i][0];
                m_nPhonemez[4] = DIVIDE_JONG_CONSON[i][1];

                m_completeWord = CombineHangle(3);
                m_nPhonemez[0]   = ToInitial(m_nPhonemez[4]);
                return;
            }
        }
        while (++i < 13);
    }

    m_completeWord = CombineHangle(1);
    m_nPhonemez[0]   = ToInitial(m_nPhonemez[2]);
}

// Mixing initial consonant
BOOL CAutomataKR::MixInitial(int nKeyCode)
{
    if (nKeyCode >= 19)
    {
        return FALSE;
    }

    int i = 0;
    do
    {
        if (MIXED_CHO_CONSON[i][0] == m_nPhonemez[0] && MIXED_CHO_CONSON[i][1] == nKeyCode)
        {
            m_nPhonemez[0] = MIXED_CHO_CONSON[i][2];
            return TRUE;
        }
    }
    while (++i < 14);

    return FALSE;
}

// Mixing final consonant
BOOL CAutomataKR::MixFinal(int nKeyCode)
{
    if (nKeyCode <= 40)
    {
        return FALSE;
    }

    int i = 0;
    do
    {
        if (MIXED_JONG_CONSON[i][0] == m_nPhonemez[2] && MIXED_JONG_CONSON[i][1] == nKeyCode)
        {
            m_nPhonemez[3] = m_nPhonemez[2];
            m_nPhonemez[4] = nKeyCode;
            m_nPhonemez[2] = MIXED_JONG_CONSON[i][2];

            return TRUE;
        }
    }
    while (++i < 22);

    return FALSE;
}

// Mixing vowel
BOOL CAutomataKR::MixVowel(int * currentCode, int inputCode)
{
    int i = 0;
    do
    {
        if (MIXED_VOWEL[i][0] == * currentCode && MIXED_VOWEL[i][1] == inputCode)
        {
            * currentCode = MIXED_VOWEL[i][2];
            return TRUE;
        }
    }
    while (++i < 21);

    return FALSE;
}

// Korean combination
WCHAR CAutomataKR::CombineHangle(int cho, int jung, int jong)
{
    // initial consonant * 21 * 28 + (a neutral vowel - 19) * 28 + (final consonant - 40) + BASE_CODE;
    return BASE_CODE - 572 + jong + cho * 588 + jung * 28;
}

WCHAR CAutomataKR::CombineHangle(int status)
{
    switch (status)
    {
    //initial consonant + a neutral vowel
    case 1:
        return CombineHangle(m_nPhonemez[0], m_nPhonemez[1], 40);

    //initial consonant + a neutral vowel + final consonant
    case 2:
        return CombineHangle(m_nPhonemez[0], m_nPhonemez[1], m_nPhonemez[2]);

    //initial consonant + a neutral vowel + Double consonant01
    case 3:
        return CombineHangle(m_nPhonemez[0], m_nPhonemez[1], m_nPhonemez[3]);
    }
    return ' ';
}

void CAutomataKR::CombineIngWord(int status)
{
    switch (m_nStatus)
    {
    case HS_FIRST:
    case HS_FIRST_V:
    case HS_FIRST_C:
        ingWord = SOUND_TABLE[m_nPhonemez[0]];
        break;

    case HS_MIDDLE_STATE:
    case HS_END:
        ingWord = CombineHangle(1);
        break;

    case HS_END_STATE:
    case HS_END_EXCEPTION:
        ingWord = CombineHangle(2);
        break;
    }
}

int CAutomataKR::DownGradeIngWordStatus(WCHAR word)
{
    int iWord = word;

    //In the case there is only nitial consonant
    if (iWord < LIMIT_MIN || iWord > LIMIT_MAX)
    {
        ingWord = 0;

        return HS_FIRST;
    }

    //Rule for text code
    //iWord = firstWord * (21*28)
    //      + middleWord * 28
    //      + lastWord * 27
    //      + BASE_CODE;
    //
    int totalWord   = iWord - BASE_CODE;
    int iFirstWord  = totalWord / 28 / 21;  //initial consonant
    int iMiddleWord = totalWord / 28 % 21;  //a neutral vowel
    int iLastWord   = totalWord % 28;       //final consonant

    m_nPhonemez[0] = iFirstWord; //Saving initial consonant

    if (iLastWord == 0) //In the case there is no final consonant
    {
        ingWord = SOUND_TABLE[m_nPhonemez[0]];

        return HS_FIRST_V;
    }

    m_nPhonemez[1] = iMiddleWord + 19; //Saving final consonant

    iLastWord += 40;

    for (int i = 0; i < 13; i++)
    {
        if (iLastWord == DIVIDE_JONG_CONSON[i][2])
        {
            ingWord = CombineHangle(3);
            m_nPhonemez[2] = DIVIDE_JONG_CONSON[i][0]; // Saving final consonant
            return HS_END_EXCEPTION;
        }
    }

    ingWord = CombineHangle(1);

    return HS_MIDDLE_STATE;
}
#endif // #ifdef IF_FEATURE_KOREANIME
