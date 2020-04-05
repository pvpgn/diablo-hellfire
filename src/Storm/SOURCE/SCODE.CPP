/****************************************************************************
*
*  SCODE.CPP
*  Storm S-Code compiler
*
*  By Michael O'Brien (4/8/96)
*
***/

#include "pch.h"
#pragma  hdrstop

#define  BUFFERSIZE    1024

#define  EQUALITY      "="
#define  OPERATIONS    "&|^+-"
#define  LOGICALOPS    "&|^+-"
#define  PORTIONS      "1234"
#define  REGISTERS     "WSDTABC01"
#define  INDEXREGS     "WABC"
#define  SIZES         "124"

#define  REG_UNDEF     0
#define  REG_WORK      1
#define  REG_DEST      2
#define  REG_SOURCE    3
#define  REG_TABLE     4
#define  REG_A         5
#define  REG_B         6
#define  REG_C         7
#define  REG_CONST0    8
#define  REG_CONST1    9
#define  NUMREGS       10

#define  OP_MOVE       0
#define  OP_AND        1
#define  OP_OR         2
#define  OP_XOR        3
#define  OP_ADD        4
#define  OP_SUB        5
#define  OP_NOT        6
#define  OP_SWAP       7
#define  NUMOPS        8

#define  USE_UNUSED    0
#define  USE_INTER     1
#define  USE_CONST     2
#define  USE_POINTER   3
#define  USE_CACHE     4

#define  CACHE_DEST    0
#define  CACHE_SOURCE  1
#define  CACHE_TABLE   2
#define  CACHE_CONST0  3
#define  CACHE_CONST1  4
#define  NUMCACHE      5

#define  CHARTOID(c)       (SStrChr(regidtable,(c))-regidtable)
#define  COMPAREREGS(a,b)  (*(LPDWORD)&(a) == *(LPDWORD)&(b))
#define  COPYREG(d,s)      (*(LPDWORD)&(d) = *(LPDWORD)&(s))
#define  FATALERROR(c)     do {                   \
                             if (firsterror)      \
                               *firsterror = (c); \
                             ClearQueue();        \
                             return 0;            \
                           } while (0)
#define  IDTOCHAR(i)       regidtable[i]
#define  OPTOID(c)         (SStrChr(opidtable,(c))-opidtable)
#define  ZEROREG(r)        *(LPDWORD)&(r) = 0

typedef struct _BUF {
    LPBYTE data;
    DWORD  bytes;
} BUF, *BUFPTR;

typedef struct _REG {
    BYTE id;
    BYTE portion;
    BYTE indexid;
    BYTE indirect;
} REG, *REGPTR;

NODEDECL(INST) {
    REG dest;
    REG source;
    int op;
    int opsize;
} *INSTPTR;

NODEDECL(STREAM) {
    BOOL   flags;
    DWORD  checkvalue;
    LPBYTE executeptr;
    LPBYTE prologstreambase;
    DWORD  prologbytes;
    LPBYTE prologstreamexec[4];
    LPBYTE epilogstreambase;
    DWORD  epilogbytes;
    LPBYTE epilogstreamexec[4];
    LPBYTE loopstreambase;
    DWORD  loopbytes;
    LPBYTE loopstreamexec[1];
} *STREAMPTR;

static BUF          s_codebuf = {NULL,0};
static LIST(INST)   s_instlist;
static REG          s_nullreg = {0,0,0,0};
static BUF          s_retbuf  = {NULL,0};
static LIST(STREAM) s_streamlist;

/****************************************************************************
*
*  INTEL X86 CODE GENERATOR
*
***/

static const BYTE s_intelx86opencodetable[NUMOPS][3][2] =
    {{{0x8B,0},{0x89,0},{0xC7,0}}, // =
     {{0x23,0},{0x21,0},{0x81,4}}, // &
     {{0x0B,0},{0x09,0},{0x81,1}}, // |
     {{0x33,0},{0x31,0},{0x81,6}}, // ^
     {{0x03,0},{0x01,0},{0x81,0}}, // +
     {{0x2B,0},{0x29,0},{0x81,5}}, // -
     {{0xF7,2},{0xF7,2},{0xF7,2}}, // ~
     {{0xC1,1},{0xC1,1},{0xC1,1}}};// @
static const BYTE s_intelx86regencodetable32[NUMREGS] =
    {4,0,7,6,5,1,2,3,0,0};
static const BYTE s_intelx86regencodetable8[NUMREGS][2] =
    {{0,0},{0,4},{0,0},{0,0},{0,0},
     {1,5},{2,6},{3,7},{0,0},{0,0}};

//===========================================================================
static LPBYTE IntelX86GenerateCode (LPBYTE dest, INSTPTR inst) {

  // PERFORM SPECIAL PROCESSING FOR INCREMENT INSTRUCTIONS
  if ((inst->op == OP_ADD) && !inst->source.id) {
    *dest++ = (inst->opsize > 1) ? 0x81 : 0xFF;
    *dest++ = 0xC0 | s_intelx86regencodetable32[inst->dest.id];
    if (inst->opsize > 1) {
      *(LPDWORD)dest = inst->opsize;
      dest += sizeof(DWORD);
    }
    return dest;
  }

  // REPLACE "R=0" WITH "R^=R", WHICH IS FEWER BYTES IN THE INTEL ARCHITECTURE
  if ((inst->op == OP_MOVE) &&
      (inst->dest.id == REG_CONST0) &&
      (!inst->source.indirect) &&
      (!inst->source.indexid)) {
    inst->op = OP_XOR;
    COPYREG(inst->dest,s_nullreg);
  }

  // DETERMINE THE INSTRUCTION ENCODING
  int operandtype = 0;
  if ((inst->source.id == REG_CONST0) ||
      (inst->source.id == REG_CONST1) ||
      !inst->source.id)
    operandtype = 2;
  else if (inst->dest.indirect || inst->dest.indexid)
    operandtype = 1;
  BYTE instenc = s_intelx86opencodetable[inst->op][operandtype][0];
  BYTE instreg = s_intelx86opencodetable[inst->op][operandtype][1];
  if (inst->opsize == 1)
    instenc &= 0xFE;

  // DETERMINE THE VALUES OF THE MODR/M AND SIB BYTES
  BYTE modrm  = 0;
  BYTE sib    = 0;
  BOOL usesib = 0;
  {
    REGPTR simplereg  = operandtype ? &inst->source : &inst->dest;
    REGPTR complexreg = operandtype ? &inst->dest   : &inst->source;
    if (complexreg->indirect && (!complexreg->indexid) &&
        ((complexreg->id == REG_SOURCE) || (complexreg->id == REG_DEST)))
      modrm = s_intelx86regencodetable32[complexreg->id];
    else if (complexreg->indirect || complexreg->indexid) {
      modrm  = 4;
      usesib = 1;
      if (complexreg->id == REG_TABLE)
        if (complexreg->indexid)
          sib = s_intelx86regencodetable32[complexreg->indexid]
                  | (s_intelx86regencodetable32[REG_TABLE] << 3);
        else
          modrm = 0x45;
      else
        sib = s_intelx86regencodetable32[complexreg->id]
                | (s_intelx86regencodetable32[complexreg->indexid] << 3);
    }
    else
      modrm = 0xC0 | (complexreg->portion
                       ? s_intelx86regencodetable8[complexreg->id][complexreg->portion-1]
                       : s_intelx86regencodetable32[complexreg->id]);
    if ((operandtype == 2) || instreg)
      modrm |= (instreg << 3);
    else
      modrm |= simplereg->portion
                 ? (s_intelx86regencodetable8[simplereg->id][simplereg->portion-1] << 3)
                 : (s_intelx86regencodetable32[simplereg->id] << 3);
  }

  // WRITE A SIZE OVERRIDE PREFIX IF WE ARE DEALING WITH TWO-BYTE DATA
  if (inst->opsize == 2)
    *dest++ = 0x66;

  // WRITE THE OPCODE, MODR/M BYTE, AND THE SIB BYTE IF NECESSARY
  *dest++ = instenc;
  *dest++ = modrm;
  if (usesib)
    *dest++ = sib;

  // WRITE A CONSTANT IF NECESSARY
  if (inst->op == OP_SWAP)
    *dest++ = 16;
  else if ((inst->source.id == REG_CONST0) || (inst->source.id == REG_CONST1)) {
    *(LPDWORD)dest = (inst->source.id == REG_CONST1) ? 0xFFFFFFFF : 0;
    dest += inst->opsize;
  }

  return dest;
}

//===========================================================================
static LPBYTE IntelX86GenerateReturn (LPBYTE dest) {

  // ON INTEL PROCESSORS, WE USE A JUMP INSTEAD OF A RETURN, AND FILL IN
  // THE JUMP TARGET AT EXECUTE TIME
  *dest++ = 0xE9;
  *(LPDWORD)dest = 0;
  dest += sizeof(DWORD);

  return dest;
}

/****************************************************************************
*
*  TEXT CODE GENERATOR
*
***/

static const LPSTR s_textopencodetable[NUMOPS] =
    {"move ","and  ","or   ","xor  ",
     "add  ","sub  ","not  ","ror  "};
static const char  s_textregencodetable[NUMREGS+1] =
    "?wdstabc01";

//===========================================================================
static LPBYTE TextGenerateCode (LPBYTE dest, INSTPTR inst) {

  // PERFORM SPECIAL PROCESSING FOR INCREMENT INSTRUCTIONS
  if ((inst->op == OP_ADD) && !inst->source.id) {
    wsprintf((LPSTR)dest,
             "inc  %c,%u\n",
             s_textregencodetable[inst->dest.id],
             inst->opsize);
    return dest+SStrLen((LPSTR)dest);
  }

  // ADD THE INSTRUCTION TEXT
  dest += SStrCopy((LPSTR)dest,s_textopencodetable[inst->op]);

  // ADD THE TEXT FOR EACH REGISTER
  for (int regnum = 1; regnum >= 0; --regnum) {
    REGPTR reg = regnum ? &inst->dest : &inst->source;
    if (reg->id) {
      if (!regnum)
        *dest++ = ',';
      if (reg->indirect || reg->indexid)
        *dest++ = '[';

      // ADD THE REGISTER BASE NAME
      if (reg->id == REG_CONST1) {
        switch (inst->opsize) {
          case 1: dest += SStrCopy((LPSTR)dest,"0FFh");        break;
          case 2: dest += SStrCopy((LPSTR)dest,"0FFFFh");      break;
          case 4: dest += SStrCopy((LPSTR)dest,"0FFFFFFFFh");  break;
        }
      }
      else
        *dest++ = s_textregencodetable[reg->id];

      // ADD THE PORTION IDENTIFIER
      if ((inst->opsize < 4) && !(reg->indirect || reg->indexid))
        if (inst->opsize == 2)
          *dest++ = 'x';
        else
          *dest++ = (reg->portion == 1) ? 'l' : 'h';

      // ADD THE INDEX REGISTER NAME
      if (reg->indexid) {
        *dest++ = '+';
        *dest++ = s_textregencodetable[reg->indexid];
      }

      if (reg->indirect || reg->indexid)
        *dest++ = ']';
    }
  }

  *dest++ = '\n';
  return dest;
}

//===========================================================================
static LPBYTE TextGenerateReturn (LPBYTE dest) {
  return dest+SStrCopy((LPSTR)dest,"ret\n");
}

/****************************************************************************
*
*  COMPILER FRONT-END (PLATFORM INDEPENDENT)
*
***/

static const char cacheidtable[] = "DST01";
static const char regidtable[]   = " WDSTABC01";
static const char opidtable[]    = "=&|^+-~@";

static inline BOOL IsRegisterUsed (LPCSTR codestring, BYTE regnum, BOOL singleequation);
static void QueueInstruction (int opsize, REG dest, REG operand1, REG operand2, char operation);

//===========================================================================
static void ClearQueue () {
  s_instlist.Clear();
}

//===========================================================================
static int FindLargestUnindexedAccess (BYTE regid) {
  int result = 0;
  ITERATELIST(INST,s_instlist,curr)
    if ((((curr->dest.id == regid) &&
          (!curr->dest.indexid) &&
          (curr->dest.indirect)) ||
         ((curr->source.id == regid) &&
          (!curr->source.indexid) &&
          (curr->source.indirect))) &&
        (curr->opsize > result))
      result = curr->opsize;
  return result;
}

//===========================================================================
static BOOL GenerateCode (LPCSTR codestring, LPCSTR *firsterror, BOOL pseudocode) {
  if (firsterror)
    *firsterror = NULL;

  VALIDATEBEGIN;
  VALIDATE(codestring);
  VALIDATEEND;

  // VERIFY THAT THE CODE STRING IS IN A VALID FORMAT
  if (SStrLen(codestring) < 3)
    FATALERROR(codestring);

  // DETERMINE HOW EACH REGISTER IS USED IN THIS CODE STRING.  A REGISTER
  // MAY NOT BE USED AT ALL, OR IT MAY BE USED IN ONE OF THE FOLLOWING
  // FOUR WAYS:
  // 1. TO STORE INTERMEDIATE RESULTS (EX: A=S D=A)
  // 2. TO STORE A CONSTANT VALUE (EX: D=S^A)
  // 3. TO STORE A VALUE THAT IS USED AND MODIFIED IN EACH ITERATION OF
  //    THE LOOP (EX: A=A+B D=A)
  // 4. TO STORE A POINTER (EX: D=S)
  // THE SECOND AND THIRD CASES ARE TREATED THE SAME, BECAUSE THEY BOTH
  // COMPLETELY PREVENT US FROM USING THE REGISTER AS A WORK REGISTER.
  // WE IDENTIFY THESE CASES BY LOOKING FOR REGISTERS THAT ARE USED WITHOUT
  // PREVIOUSLY HAVING BEEN SET.
  int reguse[NUMREGS];
  {
    for (int loop = 1; loop < NUMREGS; ++loop) {
      BOOL foundany       = FALSE;
      BOOL foundpermanent = FALSE;
      {
        BOOL   set    = FALSE;
        BOOL   seteeq = FALSE;
        LPCSTR curr   = codestring;
        while (*curr) {
          if (*curr == regidtable[loop])
            if ((*(curr+1) == '=') ||
                (*(curr+1) && SStrChr(PORTIONS,*(curr+1)) && (*(curr+2) == '='))) {
              foundany = TRUE;
              seteeq   = TRUE;
            }
            else {
              foundany = TRUE;
              if (!set)
                foundpermanent = TRUE;
            }
          if (*curr == ' ') {
            set    = seteeq;
            seteeq = FALSE;
          }
          ++curr;
        }
      }
      switch (loop) {

        case REG_WORK:
          reguse[loop] = USE_INTER;
          break;

        case REG_SOURCE:
        case REG_DEST:
        case REG_TABLE:
          reguse[loop] = foundany ? USE_POINTER : USE_UNUSED;
          break;

        case REG_CONST0:
        case REG_CONST1:
          reguse[loop] = USE_CONST;
          break;

        default:
          reguse[loop] = foundany ? foundpermanent ? USE_CONST
                                                   : USE_INTER
                                  : USE_UNUSED;
          break;

      }
    }
  }

  // START TRAVERSING THE CODE STRING FROM LEFT TO RIGHT
  ClearQueue();
  BYTE   cache[NUMCACHE] = {0,0,0,0,0};
  LPCSTR curr            = codestring;
  REG    destreg         = {0,0,0,0};
  REG    holdreg         = {0,0,0,0};
  char   holdop          = 0;
  int    opsize          = 4;
  do {

    // IF WE HIT A SIZE CHARACTER, CHANGE THE CURRENT OPERATION SIZE
    if ((*curr) &&
        ((curr == codestring) || (*(curr-1) == ' ')) &&
        SStrChr(SIZES,*curr))
      opsize = (*curr)-'0';

    // IF WE HIT AN OPERATOR, SAVE IT AS THE NEXT OPERATION
    else if ((*curr) && SStrChr(OPERATIONS,*curr))
      holdop = *curr;

    // IF WE HIT A REGISTER NAME, THEN PROCESS ANY BYTE MODIFIERS AND
    // INDEXES, AND THEN:
    // 1. SAVE IT AS THE DESTINATION REGISTER
    // 2. SAVE IT AS THE SOURCE REGISTER FOR THE NEXT OPERATION, OR
    // 3. PERFORM THE CURRENT OPERATION AND SAVE THE RESULT AS THE SOURCE
    //    REGISTER FOR THE NEXT OPERATION
    // IF WE HIT WHITESPACE, THEN PERFORM A SPECIAL CASE OPERATION TO
    // STORE THE RESULT.
    else if ((!*curr) || (*curr == ' ') ||
             SStrChr(REGISTERS,*curr)) {

      // DECODE THE REGISTER, PORTION IDENTIFIERS, AND INDEX REGISTERS
      BOOL retire = ((!*curr) || (*curr == ' '));
      REG  reg    = {0,0,0,0};
      if (!retire) {
        reg.id       = CHARTOID(*curr);
        reg.indirect = (reguse[reg.id] == USE_POINTER);
        BOOL again;
        do {
          again = FALSE;
          if ((*(curr+1)) && SStrChr(INDEXREGS,*(curr+1))) {
            reg.indexid = CHARTOID(*(curr+1));
            ++curr;
            again = TRUE;
          }
          if ((*(curr+1)) && SStrChr(PORTIONS,*(curr+1))) {
            reg.portion = *(curr+1)-'0';
            ++curr;
            again = TRUE;
          }
          if (again && ((reg.id == REG_CONST0) || (reg.id == REG_CONST1)))
            FATALERROR(curr);
        } while (again);
      }

      // IF WE DON'T YET HAVE A DESTINATION REGISTER OR A SOURCE REGISTER
      // FOR THE NEXT OPERATION, SAVE THIS REGISTER
      if (!destreg.id)
        if (*(curr+1) == '=')
          if ((reg.id == REG_CONST0) || (reg.id == REG_CONST1))
            FATALERROR(curr);
          else {
            ++curr;
            COPYREG(destreg,reg);
            ZEROREG(holdreg);
            holdop = 0;
          }
        else {
          if (!retire)
            FATALERROR(curr+1);
        }
      else if (!holdreg.id) {
        COPYREG(holdreg,reg);
        holdop = 0;
      }

      // OTHERWISE, PERFORM THE SAVED OPERATION
      else {

        // IF EITHER THE SOURCE REGISTER OR DESTINATION REGISTER IS USED AS
        // AN OPERAND, FIND A PLACE TO STORE THE RESULT OF THE INDIRECTION.
        // SIMILARLY, IF A CONSTANT VALUE IS USED IN ANYTHING BUT A SIMPLE
        // LOGICAL OPERATION, FIND A PLACE TO STORE THE VALUE.
        // CACHE THE RESULT IF THERE IS A FREE REGISTER, THE VALUE WILL BE
        // USED AGAIN, AND THERE IS NO INDEXING INVOLVED.
        if ((!retire) || (reguse[destreg.id] == USE_POINTER)) {
          for (int operandnum = 0; operandnum <= 1; ++operandnum)
            for (int regnum = 0; regnum <= 3; ++regnum) {
              REG  *regptr   = operandnum ? &reg : &holdreg;
              BYTE *cacheptr = &cache[regnum];
              int   checkid;
              int   checkuse;
              switch (regnum) {
                case CACHE_DEST:   checkid = REG_DEST;    checkuse = USE_POINTER;  break;
                case CACHE_SOURCE: checkid = REG_SOURCE;  checkuse = USE_POINTER;  break;
                case CACHE_TABLE:  checkid = REG_TABLE;   checkuse = USE_POINTER;  break;
                case CACHE_CONST0: checkid = REG_CONST0;  checkuse = USE_CONST;    break;
                case CACHE_CONST1: checkid = REG_CONST1;  checkuse = USE_CONST;    break;
              }
              if ((regptr->id == checkid) &&
                  (reguse[checkid] == checkuse) &&
                  ((regnum == CACHE_DEST)   ||
                   (regnum == CACHE_SOURCE) ||
                   (regnum == CACHE_TABLE)  ||
                   (!holdop) ||
                   (!SStrChr(LOGICALOPS,holdop))))

                // IF THIS VALUE IS ALREADY CACHED, USE THAT
                if ((*cacheptr) && (!regptr->indexid)) {
                  regptr->id       = *cacheptr;
                  regptr->indirect = 0;
                }
                else {

                  // OTHERWISE, DETERMINE WHETHER WE WANT TO CACHE IT, BASED
                  // ON WHETHER IT IS USED AGAIN AND WHETHER AN INDEX IS
                  // BEING APPLIED
                  BOOL wanttocache = IsRegisterUsed(curr+1,checkid,0)
                                       && !regptr->indexid;

                  // IF WE DO WANT TO CACHE IT, FIND A PLACE TO DO SO
                  BYTE found = 0;
                  if (wanttocache || (holdreg.id == REG_WORK)) {
                    BYTE loop;

                    // LOOK FOR UNUSED REGISTERS, OR REGISTERS USED FOR
                    // INTERMEDIATE VALUES WHICH WON'T BE USED AGAIN
                    for (loop = NUMREGS-1; loop >= 1; --loop)
                      if ((loop != REG_WORK) &&
                          ((reguse[loop] == USE_UNUSED) ||
                           ((reguse[loop] == USE_INTER) &&
                            (!SStrChr(curr,regidtable[loop]))))) {
                        found = loop;
                        break;
                      }

                    // LOOK FOR REGISTERS USED FOR CACHING VALUES THAT
                    // NO LONGER NEED TO BE CACHED
                    if (!found)
                      for (loop = NUMREGS-1; loop >= 1; --loop)
                        if (reguse[loop] == USE_CACHE) {
                          BYTE findreg = 0;
                          for (int loop2 = 0; loop2 < NUMCACHE; ++loop2)
                            if (cache[loop2] == loop)
                              findreg = CHARTOID(cacheidtable[loop2]);
                          if (findreg)
                            found = IsRegisterUsed(curr,findreg,0) ? 0 : loop;
                        }

                  }

                  // IF WE DON'T WANT TO CACHE IT OR COULDN'T FIND A PLACE,
                  // PUT THE VALUE IN EITHER THE DESTINATION REGISTER OR
                  // THE WORK REGISTER
                  if (!found)
                    if (destreg.id &&
                        (!destreg.indexid) &&
                        (!destreg.portion) &&
                        (!destreg.indirect) &&
                        (reguse[destreg.id] == USE_INTER) &&
                        (holdreg.id != destreg.id) &&
                        !IsRegisterUsed(curr,destreg.id,1))
                      found = destreg.id;
                    else if (holdreg.id != REG_WORK)
                      found = REG_WORK;
                    else
                      FATALERROR(curr);

                  // ADD AN INSTRUCTION TO MOVE THE VALUE INTO THE CACHE OR
                  // WORK REGISTER
                  REG storereg = {found,0,0,0};
                  QueueInstruction(opsize,storereg,*regptr,s_nullreg,0);

                  // SAVE THE NEW LOCATION OF THE VALUE
                  if (wanttocache && (found != REG_WORK)) {
                    *cacheptr         = found;
                    reguse[*cacheptr] = USE_CACHE;
                  }
                  COPYREG(*regptr,storereg);

                }
            }
        }

        // DETERMINE WHETHER ONE OF THE OPERANDS OR THE DESTINATION REGISTER
        // CAN BE USED FOR THE RESULT OF THE OPERATION.  IF NOT, WE WILL USE
        // THE WORK REGISTER FOR THE RESULT.
        REG resultreg = {REG_WORK,0,0,0};
        if (retire) {
          COPYREG(resultreg,destreg);
          ZEROREG(destreg);
        }
        else {

          // THE DESTINATION REGISTER CAN BE USED FOR THE RESULT IF IT IS
          // NOT USED AGAIN IN THIS EQUATION, AND IT IS NOT A POINTER
          if ((!IsRegisterUsed(curr+1,destreg.id,1)) &&
              (reguse[destreg.id] != USE_POINTER) &&
              !(destreg.indexid || destreg.indirect))
            COPYREG(resultreg,destreg);

          // OTHERWISE, IF ONE OF THE OPERANDS IS THE WORK REGISTER, WE
          // CAN USE THAT FOR THE RESULT
          else if ((holdreg.id == REG_WORK) && !(holdreg.indexid || holdreg.indirect))
            COPYREG(resultreg,holdreg);
          else if ((reg.id == REG_WORK) && !(reg.indexid || reg.indirect))
            COPYREG(resultreg,reg);

          // OTHERWISE, WE CAN STILL USE ONE OF THE OPERANDS IF WE CAN FIND
          // ONE THAT IS NOT A POINTER, IS NOT USED AGAIN, AND IS USED ONLY
          // FOR INTERMEDIATE RESULTS OR AS A CACHE
          else {
            for (int operand = 0; operand <= 1; ++operand) {
              REG  *regptr = operand ? &reg : &holdreg;
              if ((!(regptr->indexid || regptr->indirect)) &&
                  ((reguse[regptr->id] == USE_INTER) ||
                   (reguse[regptr->id] == USE_CACHE))) {
                char findreg = regptr->id;
                if (reguse[regptr->id] == USE_CACHE) {
                  for (int loop = 0; loop < NUMCACHE; ++loop)
                    if (cache[loop] == regptr->id)
                      findreg = CHARTOID(cacheidtable[loop]);
                }
                if (!IsRegisterUsed(curr+1,findreg,0))
                  COPYREG(resultreg,*regptr);
              }
            }
          }

        }

        // ENCODE THE OPERATION
        if (retire) {
          if ((resultreg.id      != holdreg.id) ||
              (resultreg.portion != holdreg.portion))
            QueueInstruction(opsize,resultreg,holdreg,s_nullreg,0);
        }
        else
          QueueInstruction(opsize,resultreg,holdreg,reg,holdop);

        // SAVE THE RESULT REGISTER FOR USE BY THE NEXT OPERATION
        if (retire)
          ZEROREG(holdreg);
        else
          COPYREG(holdreg,resultreg);
        holdop = 0;

      }

    }

    // IF WE DIDN'T HIT ANY OF THE ABOVE, THEN REPORT AN ERROR IN THE
    // CODE STRING
    else
      FATALERROR(curr);

  } while (*curr++);
  if (s_instlist.IsEmpty())
    return FALSE;

  // SEARCH THE QUEUE FOR THE LARGEST UNINDEXED READ OR WRITE WE DID FOR EACH
  // OF THE POINTERS, THEN INCREMENT THE POINTER BY THAT AMOUNT
  {
    int largestsource = FindLargestUnindexedAccess(REG_SOURCE);
    int largesttable  = FindLargestUnindexedAccess(REG_TABLE);
    int largestdest   = FindLargestUnindexedAccess(REG_DEST);
    REG reg = {0,0,0,0};
    if (largestsource) {
      reg.id = REG_SOURCE;
      QueueInstruction(largestsource,reg,s_nullreg,s_nullreg,'+');
    }
    if (largesttable) {
      reg.id = REG_TABLE;
      QueueInstruction(largesttable,reg,s_nullreg,s_nullreg,'+');
    }
    if (largestdest) {
      reg.id = REG_DEST;
      QueueInstruction(largestdest,reg,s_nullreg,s_nullreg,'+');
    }
  }

  // ADD ROTATE INSTRUCTIONS AS NECESSARY TO ELIMINATE ACCESSES TO
  // INVIDIDUAL BYTES IN THE HIGH WORDS OF REGISTERS
  {
    ITERATELIST(INST,s_instlist,curr)
      for (int operandnum = 0; operandnum <= 1; ++operandnum) {
        REGPTR currreg  = operandnum ? &curr->dest : &curr->source;
        REGPTR otherreg = operandnum ? &curr->source : &curr->dest;
        if (currreg->portion > 2)
          if ((currreg->id == otherreg->indexid) ||
              ((currreg->id == otherreg->id) && (otherreg->portion <= 2)))
            FATALERROR(codestring+SStrLen(codestring));
          else {
            REG destreg = {currreg->id,0,0,0};
            for (BOOL after = FALSE; after <= TRUE; ++after)  {
              INSTPTR inst = s_instlist.NewNode(LIST_UNLINKED);
              COPYREG(inst->dest,destreg);
              COPYREG(inst->source,s_nullreg);
              inst->op     = OP_SWAP;
              inst->opsize = 4;
              s_instlist.LinkNode(inst,
                                  after ? LIST_LINK_AFTER
                                        : LIST_LINK_BEFORE,
                                  curr);
            }
            currreg->portion -= 2;
            if (otherreg->id == currreg->id)
              otherreg->portion -= 2;
          }
      }
  }

  // REMOVE DUPLICATE ROTATE INSTRUCTIONS
  {
    ITERATELIST(INST,s_instlist,curr)
      if (curr->op == OP_SWAP)
        ITERATEPARTIALLIST(INST,s_instlist,curr->Next(),search)
          if ((search->op == OP_SWAP) && (search->dest.id == curr->dest.id)) {
            s_instlist.DeleteNode(search);
            s_instlist.DeleteNode(curr);
            curr = s_instlist.Head();
            break;
          }
          else if ((search->dest.id        == curr->dest.id) ||
                   (search->source.id      == curr->dest.id) ||
                   (search->dest.indexid   == curr->dest.id) ||
                   (search->source.indexid == curr->dest.id))
            break;
  }

  // SIMPLIFY INSTRUCTIONS INVOLVING CONSTANT VALUES.  SOME EXAMPLES:
  // 1. "W&=1" IS REMOVED
  // 2. "W&=0" IS REPLACED WITH "W=0"
  // 3. "W^=1" IS REPLACED WITH "W~="
  {
    ITERATELIST(INST,s_instlist,curr) {
      BOOL remove = FALSE;
      BOOL setto0 = FALSE;
      BOOL setto1 = FALSE;
      BOOL usenot = FALSE;
      if (curr->source.id == REG_CONST0)
        switch (curr->op) {
          case OP_MOVE: setto0 = TRUE;  break;
          case OP_AND : setto0 = TRUE;  break;
          case OP_OR  : remove = TRUE;  break;
          case OP_XOR : remove = TRUE;  break;
          case OP_ADD : remove = TRUE;  break;
          case OP_SUB : remove = TRUE;  break;
        }
      else if (curr->source.id == REG_CONST1)
        switch (curr->op) {
          case OP_MOVE: remove = TRUE;  break;
          case OP_OR  : setto1 = TRUE;  break;
          case OP_XOR : usenot = TRUE;  break;
        }
      if (remove) {
        ITERATE_DELETE;
      }
      else if (setto0 || setto1) {
        curr->op = OP_MOVE;
        REG sourcereg = {setto1 ? REG_CONST1 : REG_CONST0,0,0,0};
        COPYREG(curr->source,sourcereg);
      }
      else if (usenot) {
        curr->op = OP_NOT;
        COPYREG(curr->source,s_nullreg);
      }
    }
  }

  // OPTIMIZE THE INSTRUCTION ORDERING.  TO DO THIS, WE FIND PAIRS OF
  // INSTRUCTIONS WHICH ARE UNPAIRABLE BECAUSE OF DATA DEPENDENCE, THEN
  // LOOK FOR OTHER INSTRUCTIONS WHICH CAN BE MOVED BETWEEN THEM.
  {
    INSTPTR last = s_instlist.Head();
    INSTPTR curr = last->Next();
    while (curr) {
      if ((curr->source.id      && (curr->source.id      == last->dest.id)) ||
          (curr->source.indexid && (curr->source.indexid == last->dest.id)) ||
          (curr->dest.id        && (curr->dest.id        == last->dest.id) && curr->op)) {

        // WE FOUND TWO UNPAIRABLE INSTRUCTIONS; NOW START SEARCHING FOR
        // SOMETHING TO SPLIT THEM UP
        BOOL set[NUMREGS];   ZeroMemory(set,NUMREGS*sizeof(BOOL));
        BOOL used[NUMREGS];  ZeroMemory(used,NUMREGS*sizeof(BOOL));
        ITERATEPARTIALLIST(INST,s_instlist,curr,searchcurr) {
          if ((searchcurr > curr) &&
              ((!searchcurr->source.id)      || (!set[searchcurr->source.id])) &&
              ((!searchcurr->source.indexid) || (!set[searchcurr->source.indexid])) &&
              ((!searchcurr->dest.id) ||
               ((!set[searchcurr->dest.id]) &&
                (!used[searchcurr->dest.id])))) {

            // WE FOUND AN INSTRUCTION WHICH CAN BE USED TO SPLIT THESE TWO
            // INSTRUCTIONS, SO MOVE IT BETWEEN THEM
            s_instlist.LinkNode(searchcurr,LIST_LINK_BEFORE,curr);

            break;
          }
          set[searchcurr->dest.id]         = TRUE;
          used[searchcurr->source.id]      = TRUE;
          used[searchcurr->source.indexid] = TRUE;
        }

      }
      last = curr;
      curr = curr->Next();
    }
  }

  // ALLOCATE OUTPUT BUFFERS IF THEY HAVEN'T ALREADY BEEN ALLOCATED
  if (!s_codebuf.data)
    s_codebuf.data = (LPBYTE)ALLOC(BUFFERSIZE);
  if (!s_retbuf.data)
    s_retbuf.data = (LPBYTE)ALLOC(BUFFERSIZE);

  // GET POINTERS TO THE PROCESSOR-SPECIFIC PORTION OF THE CODE GENERATOR
  LPBYTE (*addinst)(LPBYTE,INSTPTR) = NULL;
  LPBYTE (*addret )(LPBYTE)         = NULL;
  if (pseudocode) {
    addinst = TextGenerateCode;
    addret  = TextGenerateReturn;
  }
  else {
#ifdef _X86_
    addinst = IntelX86GenerateCode;
    addret  = IntelX86GenerateReturn;
#endif
  }
  if (!(addinst && addret))
    FATALERROR(codestring+SStrLen(codestring));

  // GENERATE PROCESSOR-SPECIFIC CODE INTO THE OUTPUT BUFFERS
  {
    LPBYTE dest = s_codebuf.data;
    ITERATELIST(INST,s_instlist,curr)
      dest = addinst(dest,curr);
    s_codebuf.bytes = dest-s_codebuf.data;
  }
  {
    LPBYTE dest = addret(s_retbuf.data);
    s_retbuf.bytes = dest-s_retbuf.data;
  }

  ClearQueue();
  return TRUE;
}

//===========================================================================
static inline BOOL IsRegisterUsed (LPCSTR codestring, BYTE regnum, BOOL singleequation) {
  char regid = regidtable[regnum];
  for (;;)
    if ((!*codestring) || ((*codestring == ' ') && singleequation))
      return FALSE;
    else if ((*codestring == regid) && (*(codestring+1) != '='))
      return TRUE;
    else
      ++codestring;
}

//===========================================================================
static void QueueInstruction (int opsize, REG dest, REG operand1, REG operand2, char operation) {

  // IF THE RESULT REGISTER OR EITHER OF THE OPERANDS CONTAIN A PORTION
  // IDENTIFIER, TEMPORARILY SET THE OPERATION SIZE TO A SINGLE BYTE
  if (dest.portion || operand1.portion || operand2.portion)
    opsize = 1;

  // IF WE ARE USING SINGLE BYTE OPERATIONS, THEN FORCE ALL NON-POINTER
  // OPERANDS TO USE PORTIONS
  if (opsize == 1) {
    if (!(dest.portion || dest.indexid || dest.indirect))
      if ((dest.id == operand1.id) && operand1.portion)
        dest.portion = operand1.portion;
      else if ((dest.id == operand2.id) && operand2.portion)
        dest.portion = operand2.portion;
      else
        dest.portion = 1;
    if (!(operand1.portion || operand1.indexid || operand1.indirect))
      operand1.portion = 1;
    if (operand2.id &&
        !(operand2.portion || operand2.indexid || operand2.indirect))
      operand2.portion = 1;
  }

  // IF THERE ARE TWO OPERANDS, NORMALIZE THE INSTRUCTION SO THAT:
  // 1. IF ONE OF THE OPERANDS IS THE SAME AS THE RESULT REGISTER, IT IS
  //    ON THE LEFT
  // 2. IF ONE OF THE OPERANDS IS A CONSTANT, IT IS ON THE RIGHT
  if (operand2.id && operation &&
      (COMPAREREGS(dest,operand2) && !COMPAREREGS(dest,operand1)) ||
      (((operand1.id == REG_CONST0) || (operand1.id == REG_CONST1)) &&
      !((operand2.id == REG_CONST0) || (operand2.id == REG_CONST1)))) {
    REG temp;
    COPYREG(temp,operand1);
    COPYREG(operand1,operand2);
    COPYREG(operand2,temp);
  }

  // SIMPLIFY THE INSTRUCTION SO THAT IT CAN BE EXPRESSED AS ONE OPERATION,
  // ONE RESULT OPERAND, AND ONE SOURCE OPERAND.  THIS MAY INVOLVE SPLITTING
  // IT INTO TWO INSTRUCTIONS.  SOME EXAMPLES:
  // 1. "W=W^A" BECOMES "W^=A"
  // 2. "W=A^B" BECOMES "W=A" FOLLOWED BY "W^=B"
  if (operand2.id && operation &&
      (!COMPAREREGS(dest,operand1)) && (!COMPAREREGS(dest,operand2))) {
    REG nullop = {0,0,0,0};
    QueueInstruction(opsize,dest,operand1,nullop,0);
    COPYREG(operand1,dest);
  }

  // QUEUE THE INSTRUCTION
  INSTPTR inst = s_instlist.NewNode();
  COPYREG(inst->dest  ,dest);
  COPYREG(inst->source,operand2.id ? operand2 : operand1);
  inst->op     = operation ? OPTOID(operation) : OP_MOVE;
  inst->opsize = opsize;

}

/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

//===========================================================================
BOOL APIENTRY SCodeCompile (LPCSTR        prologstring,
                            LPCSTR        loopstring,
                            LPCSTR       *firsterror,
                            DWORD         maxiterations,
                            DWORD         flags,
                            HSCODESTREAM *handle) {
  if (firsterror)
    *firsterror = NULL;

  VALIDATEBEGIN;
  VALIDATE(loopstring);
  VALIDATE(*loopstring);
  VALIDATE(maxiterations >= 1);
  VALIDATE(handle);
  VALIDATEEND;

  // CREATE NORMALIZED VERSIONS OF THE STRINGS
  BOOL  align                  = (flags & SCODE_CF_AUTOALIGNDWORD) != 0;
  char  localprologstring[256] = "W=0 ";
  char  localloopstring[256]   = "";
  char *userprologstring       = localprologstring;
  char *userloopstring         = localloopstring;
  if (prologstring)
    if (align)
      SStrCopy(localprologstring,prologstring);
    else {
      userprologstring += SStrLen(localprologstring);
      SStrPack(localprologstring,prologstring,256);
    }
  if (loopstring)
    SStrCopy(localloopstring,loopstring);
  if (align) {
    if (localprologstring[0] == '#')
      localprologstring[0] = '1';
    if (localloopstring[0] == '#')
      localloopstring[0] = '4';
  }
  _strupr(localprologstring);
  _strupr(localloopstring);

  // CREATE A NEW CODE STREAM BUFFER
  STREAMPTR stream = s_streamlist.NewNode(LIST_HEAD,maxiterations*sizeof(LPBYTE));
  stream->flags      = flags;
  stream->checkvalue = 0xFFFFFFFF;

  // COMPILE AND UNROLL THE PROLOG/EPILOG S-CODE STRING
  {
    BOOL result = GenerateCode(localprologstring,firsterror,0);
    if (firsterror && *firsterror)
      *firsterror = prologstring+(*firsterror-userprologstring);
    if (!result)
      return SCodeDelete((HSCODESTREAM)stream);
    DWORD iterations = align ? 3 : 1;
    stream->prologbytes      = s_codebuf.bytes*iterations+s_retbuf.bytes;
    stream->prologstreambase = (LPBYTE)ALLOC(stream->prologbytes);
    if (align) {
      stream->epilogbytes      = s_codebuf.bytes*iterations+s_retbuf.bytes;
      stream->epilogstreambase = (LPBYTE)ALLOC(stream->epilogbytes);
    }
    LPBYTE prologdest = stream->prologstreambase;
    LPBYTE epilogdest = stream->epilogstreambase;
    DWORD  loop;
    for (loop = 0; loop < iterations; ++loop) {
      CopyMemory(prologdest,s_codebuf.data,s_codebuf.bytes);
      prologdest += s_codebuf.bytes;
      if (align) {
        CopyMemory(epilogdest,s_codebuf.data,s_codebuf.bytes);
        epilogdest += s_codebuf.bytes;
      }
    }
    CopyMemory(prologdest,s_retbuf.data,s_retbuf.bytes);
    if (align) {
      CopyMemory(epilogdest,s_retbuf.data,s_retbuf.bytes);
      for (loop = 0; loop <= iterations; ++loop) {
        stream->prologstreamexec[loop] = stream->prologstreambase
                                           +(iterations-loop)*s_codebuf.bytes;
        if (align)
          stream->epilogstreamexec[loop] = stream->epilogstreambase
                                             +(iterations-loop)*s_codebuf.bytes;
      }
    }
  }

  // COMPILE AND UNROLL THE LOOP S-CODE STRING
  {
    BOOL result = GenerateCode(localloopstring,firsterror,0);
    if (firsterror && *firsterror)
      *firsterror = loopstring+(*firsterror-userloopstring);
    if (!result)
      return SCodeDelete((HSCODESTREAM)stream);
    stream->loopbytes      = s_codebuf.bytes*maxiterations+s_retbuf.bytes;
    stream->loopstreambase = (LPBYTE)ALLOC(stream->loopbytes);
    LPBYTE dest = stream->loopstreambase;
    DWORD  loop;
    for (loop = 0; loop < maxiterations; ++loop) {
      CopyMemory(dest,s_codebuf.data,s_codebuf.bytes);
      dest += s_codebuf.bytes;
    }
    CopyMemory(dest,s_retbuf.data,s_retbuf.bytes);
    for (loop = 0; loop <= maxiterations; ++loop)
      stream->loopstreamexec[loop] = stream->loopstreambase
                                       +(maxiterations-loop)*s_codebuf.bytes;
  }

  *handle = (HSCODESTREAM)stream;
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SCodeDelete (HSCODESTREAM handle) {
  VALIDATEBEGIN;
  VALIDATE(handle);
  VALIDATEEND;

  // FREE THE CODE BUFFERS
  STREAMPTR stream = (STREAMPTR)handle;
  if (stream->prologstreambase) {
    FREE(stream->prologstreambase);
    stream->prologstreambase = NULL;
  }
  if (stream->epilogstreambase) {
    FREE(stream->epilogstreambase);
    stream->epilogstreambase = NULL;
  }
  if (stream->loopstreambase) {
    FREE(stream->loopstreambase);
    stream->loopstreambase = NULL;
  }

  // UNLINK AND FREE THE NODE.  THE MEMORY IS FREED EVEN IF THE NODE IS NOT
  // FOUND IN THE LINKED LIST, WHICH WOULD BE THE CASE IF WE ARE BEING CALLED
  // BECAUSE OF AN ERROR DURING COMPILATION.
  s_streamlist.DeleteNode(stream);

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SCodeDestroy () {

  // DELETE ALL CODE STREAMS
  while (!s_streamlist.IsEmpty()) {
    REPORTRESOURCELEAK(HSCODESTREAM);
    SCodeDelete((HSCODESTREAM)s_streamlist.Head());
  }

  // FREE THE CODE BUFFER AND RETURN BUFFER IF NECESSARY
  if (s_codebuf.data)
    FREE(s_codebuf.data);
  ZeroMemory(&s_codebuf,sizeof(BUF));
  if (s_retbuf.data)
    FREE(s_retbuf.data);
  ZeroMemory(&s_retbuf,sizeof(BUF));

  // CLEAR THE INSTRUCTION QUEUE IF NECESSARY
  ClearQueue();

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SCodeExecute (HSCODESTREAM        handle,
                            SCODEEXECUTEDATAPTR executedata) {
  STREAMPTR stream      = (STREAMPTR)handle;
  DWORD     xiterations = executedata->xiterations;

  // IF WE ARE DOING AUTO-ALIGNMENT, FIX UP THE JUMP OFFSETS AS FOLLOWS:
  // 1. EXECUTE PROLOG CODE FOR BYTE OPERATIONS, UP TO A DWORD BOUNDARY
  // 2. EXECUTE LOOP CODE FOR WHOLE DWORD OPERATIONS
  // 3. EXECUTE EPILOG CODE FOR FINAL BYTE OPERATIONS
  // "XITERATIONS" REFERS TO THE TOTAL NUMBER OF BYTES TO PROCESS
  if (stream->flags & SCODE_CF_AUTOALIGNDWORD) {
    DWORD checkvalue = (xiterations << 2) ^ ((DWORD)executedata->dest & 3);
    if (stream->checkvalue != checkvalue) {
      stream->checkvalue = checkvalue;
      DWORD align1 = (4-((DWORD)executedata->dest & 3)) & 3;
      if (align1 > xiterations)
        align1 = xiterations;
      DWORD dwords = (xiterations -= align1) >> 2;
      DWORD align2 = xiterations & 3;
#ifdef _X86_
      LPBYTE retptr1, retptr2;
      __asm mov retptr1,OFFSET ex_loop1
      __asm mov retptr2,OFFSET ex_loop2
      if (align1 || align2) {
        stream->executeptr = stream->prologstreamexec[align1];
        *(LPDWORD)(stream->prologstreambase+stream->prologbytes-sizeof(DWORD))
          = stream->loopstreamexec[dwords]-(stream->prologstreambase+stream->prologbytes);
        *(LPDWORD)(stream->loopstreambase+stream->loopbytes-sizeof(DWORD))
          = stream->epilogstreamexec[align2]
              -(stream->loopstreambase+stream->loopbytes);
        *(LPDWORD)(stream->epilogstreambase+stream->epilogbytes-sizeof(DWORD))
          = ((stream->flags & SCODE_CF_USESALTADJUSTS) ? retptr2 : retptr1)
              -(stream->epilogstreambase+stream->epilogbytes);
      }
      else {
        stream->executeptr = stream->loopstreamexec[dwords];
        *(LPDWORD)(stream->loopstreambase+stream->loopbytes-sizeof(DWORD))
          = ((stream->flags & SCODE_CF_USESALTADJUSTS) ? retptr2 : retptr1)
              -(stream->loopstreambase+stream->loopbytes);
      }
#endif
    }
  }

  // OTHERWISE, FIX UP THE JUMP OFFSETS SO THAT WE CALL THE PROLOG CODE
  // ONCE, FOLLOWED BY "XITERATIONS" ITERATIONS OF THE LOOP CODE
  else {
    if (stream->checkvalue != xiterations) {
      stream->checkvalue = xiterations;
#ifdef _X86_
      LPBYTE retptr1, retptr2;
      __asm mov retptr1,OFFSET ex_loop1
      __asm mov retptr2,OFFSET ex_loop2
      stream->executeptr = stream->prologstreambase;
      *(LPDWORD)(stream->prologstreambase+stream->prologbytes-sizeof(DWORD))
        = stream->loopstreamexec[xiterations]
            -(stream->prologstreambase+stream->prologbytes);
      *(LPDWORD)(stream->loopstreambase+stream->loopbytes-sizeof(DWORD))
        = ((stream->flags & SCODE_CF_USESALTADJUSTS) ? retptr2 : retptr1)
            -(stream->loopstreambase+stream->loopbytes);
#endif
    }
  }

  // EXECUTE THE LOOPS FOR INTEL X86 PROCESSORS
#ifdef _X86_
#define  LOCAL_JUMPPTR       DWORD PTR [esp]
#define  LOCAL_YCOUNT        DWORD PTR [esp+4]
#define  LOCAL_EXECUTEDATA   DWORD PTR [esp+8]
#define  LOCAL_ADJUSTSOURCE  DWORD PTR [esp+12]
#define  LOCAL_ADJUSTDEST    DWORD PTR [esp+16]
  __asm {
                   push      edi
                   push      esi
                   push      ebp

                   // PREPARE OUR LOCAL DATA AREA ON THE STACK
                   mov       eax,executedata
                   mov       ebx,stream
                   sub       esp,20
                   mov       ecx,[eax]SCODEEXECUTEDATA.yiterations
                   mov       esi,[eax]SCODEEXECUTEDATA.adjustsource
                   mov       edi,[eax]SCODEEXECUTEDATA.adjustdest
                   mov       ebx,[ebx]STREAM.executeptr
                   mov       LOCAL_EXECUTEDATA,eax
                   mov       LOCAL_JUMPPTR,ebx
                   mov       LOCAL_YCOUNT,ecx
                   mov       LOCAL_ADJUSTSOURCE,esi
                   mov       LOCAL_ADJUSTDEST,edi

                   // PREPARE THE REGISTERS
                   mov       edi,[eax]SCODEEXECUTEDATA.dest
                   mov       esi,[eax]SCODEEXECUTEDATA.source
                   mov       ebp,[eax]SCODEEXECUTEDATA.table
                   mov       ecx,[eax]SCODEEXECUTEDATA.a
                   mov       edx,[eax]SCODEEXECUTEDATA.b
                   mov       ebx,[eax]SCODEEXECUTEDATA.c
  
                   // EXECUTE THE FIRST LOOP
                   mov       eax,LOCAL_JUMPPTR
                   jmp       eax

                   // EXECUTE THE NEXT LOOP FOR TYPE 1 (STANDARD)
                   align     16
    ex_loop1:      dec       LOCAL_YCOUNT
                   mov       eax,LOCAL_JUMPPTR
                   jz        ex_done
                   add       esi,LOCAL_ADJUSTSOURCE
                   add       edi,LOCAL_ADJUSTDEST
                   jmp       eax

                   // EXECUTE THE NEXT LOOP FOR TYPE 2 (ALTERNATING)
                   align     16
    ex_loop2:      test      LOCAL_YCOUNT,1
                   jnz       ex_loop2odd
                   dec       LOCAL_YCOUNT
                   mov       eax,LOCAL_JUMPPTR
                   add       esi,LOCAL_ADJUSTSOURCE
                   add       edi,LOCAL_ADJUSTDEST
                   jmp       eax
    ex_loop2odd:   dec       LOCAL_YCOUNT
                   jz        ex_done
                   mov       eax,LOCAL_EXECUTEDATA
                   add       esi,[eax]SCODEEXECUTEDATA.adjustsourcealt
                   add       edi,[eax]SCODEEXECUTEDATA.adjustdestalt
                   mov       eax,LOCAL_JUMPPTR
                   jmp       eax

                   // RESTORE THE STACK
    ex_done:       add       esp,20
                   pop       ebp
                   pop       esi
                   pop       edi

                   // SAVE THE VALUE OF EACH VARIABLE
                   mov       eax,executedata
                   mov       [eax]SCODEEXECUTEDATA.a,ecx
                   mov       [eax]SCODEEXECUTEDATA.b,edx
                   mov       [eax]SCODEEXECUTEDATA.c,ebx

  }
#undef  LOCAL_JUMPPTR
#undef  LOCAL_YCOUNT
#undef  LOCAL_ADJUSTSOURCE
#undef  LOCAL_ADJUSTDEST
#endif

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SCodeGetJumpTable (HSCODESTREAM   handle,
                                 LPBYTE       **jumptableptr,
                                 LPDWORD       *prologpatchlocation,
                                 LPDWORD       *looppatchlocation,
                                 LPDWORD       *epilogpatchlocation) {
  VALIDATEBEGIN;
  VALIDATE(handle);
  VALIDATEEND;

  STREAMPTR stream = (STREAMPTR)handle;

  if (jumptableptr)
    *jumptableptr = &stream->loopstreamexec[0];
  if (prologpatchlocation)
    *prologpatchlocation = (LPDWORD)(stream->prologstreambase+stream->prologbytes-sizeof(DWORD));
  if (looppatchlocation)
    *looppatchlocation = (LPDWORD)(stream->loopstreambase+stream->loopbytes-sizeof(DWORD));
  if (epilogpatchlocation)
    *epilogpatchlocation = (LPDWORD)(stream->epilogstreambase+stream->epilogbytes-sizeof(DWORD));

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SCodeGetPseudocode (LPCSTR scodestring,
                                  LPSTR  buffer,
                                  DWORD  buffersize) {
  VALIDATEBEGIN;
  VALIDATE(scodestring);
  VALIDATE(*scodestring);
  VALIDATE(buffer);
  VALIDATE(buffersize);
  VALIDATEEND;

  // CREATE A NORMALIZED VERSION OF THE STRING
  char localstring[256] = "";
  SStrCopy(localstring,scodestring,256);
  _strupr(localstring);

  // COMPILE THE S-CODE INTO PSEUDOCODE
  if (!GenerateCode(localstring,NULL,1)) {
    *buffer = 0;
    return FALSE;
  }
  *(s_codebuf.data+s_codebuf.bytes) = 0;

  // COPY THE PSEUDOCODE INTO THE BUFFER
  SStrCopy(buffer,(LPSTR)s_codebuf.data,buffersize);

  return (s_codebuf.bytes < buffersize);
}
