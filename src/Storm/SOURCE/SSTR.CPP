/****************************************************************************
*
*  SSTR.CPP
*  String manipulation functions
*
*  By Michael O'Brien (5/29/97)
*
***/

#include "pch.h"
#pragma  hdrstop

#define  QUOTECHAR  '\"'

typedef union _PACKED {
  DWORD dword;
  BYTE  byte[4];
} PACKED, *PACKEDPTR;

static const DWORD s_hashtable[16] = {0x486e26ee,0xdcaa16b3,0xe1918eef,0x202dafdb,
                                      0x341c7dc7,0x1c365303,0x40ef2d37,0x65fd5e49,
                                      0xd6057177,0x904ece93,0x1c38024f,0x98fd323b,
                                      0xe3061ae7,0xa39b0fa1,0x9797f25f,0xe4444563};

/****************************************************************************
*
*  MACROS
*
*  These macros implement routines to copy strings either a byte or dword at
*  at time, and to search for null terminators either a byte or dword at a
*  time.
*
*  To check a dword for a null terminator, the macros add the 32-bit
*  constant 0x7EFEFEFF to the dword value.  This causes an overflow in each
*  byte in the dword unless that byte is zero.  The macros then xor the
*  result with the original to see which bits have changed, in order to
*  determine whether there were any bytes that didn't overflow.  This test is
*  fast (it executes in three clock cycles on an Intel system) and 100%
*  accurate in identifying null terminators where they exist.  There is one
*  case (0x80010101) where it falsely identifies a null terminator where none
*  exists, so the code in the macros that is executed in response to a
*  positive result has to test all four bytes for a null terminator, not just
*  three.
*
*  These macros are designed to give good performance on all systems, but are
*  especially tweaked for MSVC on Intel systems, where they generate code
*  that is the same as the most efficient possible assembly language
*  implementation of the algorithm.  This tweaking adds some complexity to
*  the code, as follows:
*
*    - The code to handle a dword with a null terminator in it is placed
*      in a separate loop from the code that handles non-terminating dwords,
*      rather than a separate "if" statement in the same loop, because
*      Microsoft's compiler always generates an unconditional jump rather
*      than a conditional one to a target address that can be reached from
*      two or more jump statements.
*
*    - The code that tests for overflows in the byte components of a dword
*      tricks the compiler into thinking that the result of the test is used
*      for more than just the conditional jump, so that the compiler will
*      generate an "and" rather than a "test" instruction, because "and" is
*      fully pairable on a Pentium processor while "test" with an immediate
*      is not.
*
*    - The code that checks for null bytes in a dword also emulates a "not"
*      instruction by subtracting the value from 0xFFFFFFFF.  Subtracting a
*      value from a register and then restoring the contents of that register
*      requires a total of one clock cycle on a Pentium, which is the same as
*      a "not" instruction; however, it increases the opportunities for the
*      compiler to reshuffle code in order to avoid delays caused by data
*      dependence, and that makes a critical speed difference in this case.
*
*    - The same code also tricks the compiler into thinking that the constant
*      0xFFFFFFFF might be changed at some point, so that it will store it
*      in a register rather than using it as an immediate value, because
*      otherwise MSVC generates code to load the immediate value into a
*      register once for each iteration of the loop.
*
*  Most of these tweaks are separated out in an Intel-specific macro, so that
*  a more straightforward implementation of the algorithm gets used on non-
*  Intel systems.
*
***/

//===========================================================================
#ifdef _X86_
  static DWORD s_check_markresultused;
  #define  INITDWORDOPERATIONS                                              \
      DWORD check_modnum;                                                   \
      DWORD check_notnum = 0xFFFFFFFF
  #define  CHECKFORNULLBYTES(num)                                           \
      (check_modnum  = (num)+0x7EFEFEFF,                                    \
       check_notnum -= (num),                                               \
       check_modnum ^= check_notnum,                                        \
       check_notnum |= (num),                                               \
       check_modnum &= 0x81010101)
  #define  CHECK_ENDLOOP                                                    \
      s_check_markresultused = check_modnum
#else
  #define  INITDWORDOPERATIONS
  #define  CHECKFORNULLBYTES(num)                                           \
      (((((num)+0x7EFEFEFF) ^ (num)) & 0x81010100) != 0x81010100)
  #define  CHECK_ENDLOOP
#endif

//===========================================================================
#ifdef _X86_
  #define  CHECKFORNULLBYTE0(packed)  (!packed.byte[0])
  #define  CHECKFORNULLBYTE1(packed)  (!packed.byte[1])
  #define  CHECKFORNULLBYTE2(packed)  (!(packed.dword & 0x00FF0000))
  #define  CHECKFORNULLBYTE3(packed)  (!(packed.dword & 0xFF000000))
#else
  #define  CHECKFORNULLBYTE0(packed)  (!packed.byte[0])
  #define  CHECKFORNULLBYTE1(packed)  (!packed.byte[1])
  #define  CHECKFORNULLBYTE2(packed)  (!packed.byte[2])
  #define  CHECKFORNULLBYTE3(packed)  (!packed.byte[3])
#endif

//===========================================================================
#define  BEGINSKIP

//===========================================================================
#define  SKIPLEADINGBYTES                                                   \
  for (;                                                                    \
       (DWORD)currdest & 3;                                                 \
       ++currdest)                                                          \
    if (!*currdest)                                                         \
      goto endskip

//===========================================================================
#define  SKIPALIGNEDDWORDS                                                  \
  do {                                                                      \
    PACKED packed = *(PACKEDPTR)currdest;                                   \
    currdest += sizeof(PACKED);                                             \
    if (!CHECKFORNULLBYTES(packed.dword))                                   \
      continue;                                                             \
    if (CHECKFORNULLBYTE0(packed)) {                                        \
      currdest -= 4;                                                        \
      goto endskip;                                                         \
    }                                                                       \
    if (CHECKFORNULLBYTE1(packed)) {                                        \
      currdest -= 3;                                                        \
      goto endskip;                                                         \
    }                                                                       \
    if (CHECKFORNULLBYTE2(packed)) {                                        \
      currdest -= 2;                                                        \
      goto endskip;                                                         \
    }                                                                       \
    if (CHECKFORNULLBYTE3(packed)) {                                        \
      currdest -= 1;                                                        \
      goto endskip;                                                         \
    }                                                                       \
    CHECK_ENDLOOP;                                                          \
  } while (1)

//===========================================================================
#define  ENDSKIP                                                            \
  endskip:

//===========================================================================
#define  BEGINCOPY                                                          \
  DWORD negoffset = (DWORD)-(int)(destsize-(currdest-dest))

//===========================================================================
#define  COPYLEADINGBYTES                                                   \
  while (((DWORD)source & 3) && negoffset)                                  \
    if (!(*(enddest+negoffset++) = *source++))                              \
      goto endcopy;

//===========================================================================
#define  COPYALIGNEDDWORDS                                                  \
  if ((int)(negoffset += 3) < 0) {                                          \
    enddest -= 3;                                                           \
                                                                            \
    PACKED packed = *(PACKEDPTR)source;                                     \
    source += sizeof(PACKED);                                               \
    while (!CHECKFORNULLBYTES(packed.dword)) {                              \
      *(PACKEDPTR)(enddest+negoffset) = packed;                             \
      if ((int)(negoffset += 4) >= 0)                                       \
        goto donealigned;                                                   \
      packed = *(PACKEDPTR)source;                                          \
      source += sizeof(PACKED);                                             \
    }                                                                       \
    for (;;) {                                                              \
      if (CHECKFORNULLBYTE0(packed)) {                                      \
        *(enddest+negoffset) = packed.byte[0];                              \
        negoffset += 1;                                                     \
        goto endcopy;                                                       \
      }                                                                     \
      if (CHECKFORNULLBYTE1(packed)) {                                      \
        *(LPWORD)(enddest+negoffset) = (WORD)(packed.dword);                \
        negoffset += 2;                                                     \
        goto endcopy;                                                       \
      }                                                                     \
      if (CHECKFORNULLBYTE2(packed)) {                                      \
        *(LPWORD)(enddest+negoffset) = (WORD)(packed.dword);                \
        *(enddest+negoffset+2) = 0;                                         \
        negoffset += 3;                                                     \
        goto endcopy;                                                       \
      }                                                                     \
      *(PACKEDPTR)(enddest+negoffset) = packed;                             \
      negoffset += sizeof(PACKED);                                          \
      if (CHECKFORNULLBYTE3(packed))                                        \
        goto endcopy;                                                       \
      CHECK_ENDLOOP;                                                        \
      if ((int)negoffset >= 0)                                              \
        goto donealigned;                                                   \
      packed = *(PACKEDPTR)source;                                          \
      source += sizeof(PACKED);                                             \
    }                                                                       \
                                                                            \
    donealigned:                                                            \
    enddest += 3;                                                           \
  }                                                                         \
  negoffset -= 3

//===========================================================================
#define  COPYTRAILINGBYTES                                                  \
  while (negoffset)                                                         \
    if (!(*(enddest+negoffset++) = *source++))                              \
      goto endcopy;                                                         \
  *enddest = 0

//===========================================================================
#define  ENDCOPY                                                            \
  endcopy:                                                                  \
  currdest = enddest+(negoffset-1);

/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

//===========================================================================
LPTSTR APIENTRY SStrChr (LPCTSTR string,
                         char    ch,
                         BOOL    reverse) {
  LPTSTR last = NULL;
  if (reverse) {
    for (LPCTSTR curr = string; *curr; ++curr)
      if (*curr == ch)
        last = (LPTSTR)curr;
  }
  else {
    for (LPCTSTR curr = string; *curr; ++curr)
      if (*curr == ch)
        return (LPTSTR)curr;
  }
  return last;
}

//===========================================================================
DWORD APIENTRY SStrCopy (LPTSTR  dest,
                         LPCTSTR source,
                         DWORD   destsize) {
  if (!destsize--)
    return 0;

  INITDWORDOPERATIONS;
  LPTSTR currdest = dest;
  LPTSTR enddest  = dest+destsize;

  BEGINCOPY;
  COPYLEADINGBYTES;
  COPYALIGNEDDWORDS;
  COPYTRAILINGBYTES;
  ENDCOPY;

  return currdest-dest;
}

//===========================================================================
DWORD APIENTRY SStrHash (LPCTSTR string,
                         DWORD   flags,
                         DWORD   seed) {
  DWORD result = seed ? seed : 0x7FED7FED;
  DWORD adjust = 0xEEEEEEEE;
  DWORD ch;
  if (flags & SSTR_HASH_CASESENSITIVE)
    while ((ch = (DWORD)(BYTE)*(string++)) != 0) {
      result  = (result+adjust) ^ (s_hashtable[ch >> 4] - s_hashtable[ch & 0x0F]);
      adjust += ch+result+(adjust << 5)+3;
    }
  else
    while ((ch = (DWORD)(BYTE)*(string++)) != 0) {
      if ((ch >= (DWORD)'a') && (ch <= (DWORD)'z'))
        ch = ch+(DWORD)'A'-(DWORD)'a';
      if (ch == (DWORD)'/')
        ch = (DWORD)'\\';
      result  = (result+adjust) ^ (s_hashtable[ch >> 4] - s_hashtable[ch & 0x0F]);
      adjust += ch+result+(adjust << 5)+3;
    }
  if (!result)
    ++result;
  return result;
}

//===========================================================================
DWORD APIENTRY SStrLen (LPCTSTR string) {

  INITDWORDOPERATIONS;
  LPCTSTR currdest = string;

  BEGINSKIP;
  SKIPLEADINGBYTES;
  SKIPALIGNEDDWORDS;
  ENDSKIP;

  return currdest-string;
}

//===========================================================================
void APIENTRY SStrPack (LPTSTR  dest,
                        LPCTSTR source,
                        DWORD   destsize) {
  if (!destsize--)
    return;

  INITDWORDOPERATIONS;
  LPTSTR currdest = dest;
  LPTSTR enddest  = dest+destsize;

  // FORCE THE DESTINATION BUFFER TO BE NULL TERMINATED
  if (destsize != SSTR_UNBOUNDED)
    *enddest = 0;

  BEGINSKIP;
  SKIPLEADINGBYTES;
  SKIPALIGNEDDWORDS;
  ENDSKIP;

  BEGINCOPY;
  COPYLEADINGBYTES;
  COPYALIGNEDDWORDS;
  COPYTRAILINGBYTES;
  ENDCOPY;

}

//===========================================================================
void APIENTRY SStrTokenize (LPCTSTR *string,
                            LPTSTR   buffer,
                            DWORD    bufferchars,
                            LPCTSTR  whitespace,
                            BOOL    *quoted) {
  BOOL    checkquotes = (SStrChr(whitespace,QUOTECHAR) != NULL);
  BOOL    inquotes    = FALSE;
  BOOL    usedquotes  = FALSE;
  LPCTSTR currsource  = *string;

  // SKIP PAST ALL LEADING WHITESPACE
  while ((*currsource) && SStrChr(whitespace,*currsource)) {
    if (checkquotes && (*currsource == QUOTECHAR)) {
      usedquotes = TRUE;
      inquotes   = !inquotes;
    }
    ++currsource;
  }

  // COPY THE TOKEN
  DWORD destchars = 0;
  char  ch;
  while ((ch = *currsource) != 0)
    if (checkquotes && (ch == QUOTECHAR)) {
      if (destchars && !inquotes)
        break;
      usedquotes = TRUE;
      inquotes   = !inquotes;
      ++currsource;
      if (!inquotes)
        break;
    }
    else if (inquotes || !SStrChr(whitespace,ch)) {
      if (destchars+1 < bufferchars)
        buffer[destchars++] = ch;
      ++currsource;
    }
    else {
      ++currsource;
      break;
    }

  // NULL TERMINATE THE BUFFER
  if (destchars < bufferchars)
    buffer[destchars] = 0;

  // RETURN AN UPDATED POINTER INTO THE SOURCE STRING
  *string = currsource;

  // RETURN TO THE APPLICATION A BOOLEAN TELLING WHETHER THIS TOKEN
  // WAS IN QUOTES
  if (quoted)
    *quoted = usedquotes;

}
