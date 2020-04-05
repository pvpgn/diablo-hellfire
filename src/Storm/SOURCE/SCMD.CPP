/****************************************************************************
*
*  SCMD.CPP
*  Storm command line parsing functions
*
*  By Michael O'Brien (4/14/97)
*  Based on cmdline.cpp by Patrick Wyatt (1/21/93)
*
***/

#include "pch.h"
#pragma  hdrstop

#define  ARGVAL(f)               ((f) & SCMD_ARG_MASK)
#define  BOOLVAL(f)              ((f) & SCMD_BOOL_MASK)
#define  NUMVAL(f)               ((f) & SCMD_NUM_MASK)
#define  TYPEVAL(f)              ((f) & SCMD_TYPE_MASK)

#define  MAXNAMELENGTH           16
#define  FLAGCHARS               "-/"
#define  WHITESPACE              " ,;\"\t\n\r\x1A"

#define  STR_BADARGUMENT         0
#define  STR_NOTENOUGHARGUMENTS  1
#define  STR_OPENFAILED          2
#define  STRINGS                 3

static const LPCTSTR s_errorstr[STRINGS] =
                       {"Invalid argument: %s",
                        "The syntax of the command is incorrect.",
                        "Unable to open response file: %s"};

NODEDECL(CMDDEF) {
    DWORD        flags;
    DWORD        id;
    char         name[MAXNAMELENGTH];
    int          namelength;
    DWORD        setvalue;
    DWORD        setmask;
    LPVOID       variableptr;
    DWORD        variablebytes;
    SCMDCALLBACK callback;
    BOOL         found;
    union {
      DWORD  currvalue;
      LPTSTR currvaluestr;
    };
} *CMDDEFPTR;

typedef struct _PROCESSING {
    CMDDEFPTR ptr;
    char      name[MAXNAMELENGTH];
    int       namelength;
} PROCESSING, *PROCESSINGPTR;

static BOOL         s_addedoptional = FALSE;
static LIST(CMDDEF) s_arglist;
static LIST(CMDDEF) s_flaglist;

static BOOL ProcessString (LPCTSTR           *stringptr,
                           PROCESSINGPTR      processing,
                           CMDDEFPTR         *nextarg,
                           SCMDEXTRACALLBACK  extracallback,
                           SCMDERRORCALLBACK  errorcallback);
static BOOL ProcessToken (LPCTSTR            string,
                          BOOL               quoted,
                          PROCESSINGPTR      processing,
                          CMDDEFPTR         *nextarg,
                          SCMDEXTRACALLBACK  extracallback,
                          SCMDERRORCALLBACK  errorcallback);

//===========================================================================
static void ConvertBool (CMDDEFPTR  ptr,
                         LPCTSTR    string,
                         int       *datachars) {

  // DETERMINE WHETHER THE FLAG WILL BE SET OR CLEARED
  BOOL set;
  if (*string == '-') {
    set        = FALSE;
    *datachars = 1;
  }
  else if (*string == '+') {
    set        = TRUE;
    *datachars = 1;
  }
  else if (BOOLVAL(ptr->flags) == SCMD_BOOL_CLEAR)
    set = FALSE;
  else
    set = TRUE;

  // MODIFY IT LOCALLY
  ptr->currvalue &= ~ptr->setmask;
  if (set)
    ptr->currvalue |= ptr->setvalue;

  // MODIFY IT IN THE APPLICATION'S MEMORY
  if (ptr->variableptr) {
    *(LPDWORD)ptr->variableptr &= ~ptr->setmask;
    if (set)
      *(LPDWORD)ptr->variableptr |= ptr->setvalue;
  }

}

//===========================================================================
static void ConvertNumber (CMDDEFPTR  ptr,
                           LPCTSTR    string,
                           int       *datachars) {

  // DETERMINE THE NUMERIC VALUE
  LPTSTR endptr = NULL;
  if (NUMVAL(ptr->flags) == SCMD_NUM_SIGNED)
    ptr->currvalue = (DWORD)strtol(string,&endptr,0);
  else
    ptr->currvalue = strtoul(string,&endptr,0);
  if (endptr)
    *datachars = endptr-string;
  else
    *datachars = SStrLen(string);

  // WRITE IT INTO THE APPLICATION'S MEMORY
  if (ptr->variableptr)
    CopyMemory(ptr->variableptr,
               &ptr->currvalue,
               min(sizeof(ptr->currvalue),ptr->variablebytes));

}

//===========================================================================
static void ConvertString (CMDDEFPTR  ptr,
                           LPCTSTR    string,
                           int       *datachars) {
  *datachars = SStrLen(string);

  // SAVE THE STRING
  if (ptr->currvaluestr)
    FREE(ptr->currvaluestr);
  ptr->currvaluestr = (LPTSTR)ALLOC(SStrLen(string)+1);
  SStrCopy(ptr->currvaluestr,string);

  // COPY THE STRING INTO THE APPLICATION'S MEMORY
  if (ptr->variableptr)
    SStrCopy((LPTSTR)ptr->variableptr,
             string,
             ptr->variablebytes);

}

//===========================================================================
static CMDDEFPTR FindFlagDef (LPCTSTR   string,
                              CMDDEFPTR firstdef,
                              int       minlength) {
  int       strlength  = SStrLen(string);
  int       bestlength = minlength-1;
  CMDDEFPTR bestptr    = NULL;
  for (CMDDEFPTR curr = firstdef;
       curr;
       curr = curr->Next())
    if ((curr->namelength >  bestlength) &&
        (curr->namelength <= strlength)) {
      BOOL match = FALSE;
      if (curr->flags & SCMD_CASESENSITIVE)
        match = !strncmp(curr->name,string,curr->namelength);
      else
        match = !_strnicmp(curr->name,string,curr->namelength);
      if (match) {
        bestlength = curr->namelength;
        bestptr    = curr;
      }
    }
  return bestptr;
}

//===========================================================================
static void GenerateError (SCMDERRORCALLBACK errorcallback,
                           DWORD             errorcode,
                           LPCTSTR           itemstring) {

  // GENERATE THE COMPLETE ERROR MESSAGE
  char errorstr[256] = "";
  {
    DWORD strid;
    UINT  resid;
    switch (errorcode)  {

      case SCMD_ERROR_BAD_ARGUMENT:
        strid = STR_BADARGUMENT;
        resid = IDS_BADARGUMENT;
      break;

      case SCMD_ERROR_NOT_ENOUGH_ARGUMENTS:
        strid = STR_NOTENOUGHARGUMENTS;
        resid = IDS_NOTENOUGHARGUMENTS;
      break;

      case SCMD_ERROR_OPEN_FAILED:
        strid = STR_OPENFAILED;
        resid = IDS_OPENFAILED;
      break;

      default:
      return;

    }
    char buffer[256] = "";
    LoadString(StormGetInstance(),resid,buffer,256);
    if (!buffer[0])
      SStrCopy(buffer,s_errorstr[strid],256);
    if (strstr(buffer,"%s"))
      wsprintf(errorstr,buffer,itemstring);
    else
      SStrCopy(errorstr,buffer,256);
    if (errorstr[0])
      SStrPack(errorstr,"\n",256);
  }

  // SET THE LAST ERROR CODE
  SErrSetLastError(errorcode);

  // CALL THE APPLICATION'S CALLBACK FUNCTION
  CMDERROR data;
  data.errorcode = errorcode;
  data.itemstr   = itemstring;
  data.errorstr  = errorstr;
  errorcallback(&data);

}

//===========================================================================
static BOOL PerformConversion (CMDDEFPTR  ptr,
                               LPCTSTR    string,
                               int       *datachars) {

  // PERFORM THE CONVERSION
  *datachars = 0;
  switch (TYPEVAL(ptr->flags)) {

    case SCMD_TYPE_BOOL:
      ConvertBool(ptr,string,datachars);
    break;

    case SCMD_TYPE_NUMERIC:
      ConvertNumber(ptr,string,datachars);
    break;

    case SCMD_TYPE_STRING:
      ConvertString(ptr,string,datachars);
    break;

    default:
    return FALSE;

  }

  // MARK THE PARAMETER AS FOUND
  ptr->found = TRUE;

  // CALL THE APPLICATION'S CALLBACK FUNCTION
  if (ptr->callback) {
    CMDPARAMS params;
    params.flags         = ptr->flags;
    params.id            = ptr->id;
    params.name          = ptr->name;
    params.variable      = ptr->variableptr;
    params.setvalue      = ptr->setvalue;
    params.setmask       = ptr->setmask;
    params.unsignedvalue = ptr->currvalue;
    if (!ptr->callback(&params,string))
      return FALSE;
  }

  // PROPAGATE THE CURRENT VALUE TO ANY OTHER ARGUMENTS WITH THE SAME ID,
  // SO THAT IF THE USER QUERIES THE VALUE BY ID THEN THE QUERY FUNCTION
  // CAN SIMPLY RETURN THE FIRST HIT
  for (BOOL flaglist = FALSE; flaglist <= TRUE; ++flaglist)
    ITERATELIST(CMDDEF,
                flaglist ? s_flaglist : s_arglist,
                currptr)
      if ((currptr->id == ptr->id) &&
          (TYPEVAL(currptr->flags) == TYPEVAL(ptr->flags)) &&
          (currptr != ptr)) {
        currptr->found = TRUE;
        if (TYPEVAL(currptr->flags) == SCMD_TYPE_STRING) {
          if (currptr->currvaluestr)
            FREE(currptr->currvaluestr);
          currptr->currvaluestr = (LPTSTR)ALLOC(SStrLen(ptr->currvaluestr)+1);
          SStrCopy(currptr->currvaluestr,ptr->currvaluestr);
        }
        else
          currptr->currvalue = ptr->currvalue;
      }

  return TRUE;
}

//===========================================================================
static BOOL ProcessCurrentFlag (LPCTSTR        string,
                                PROCESSINGPTR  processing,
                                int           *datachars) {
  *datachars = 0;
  CMDDEFPTR ptr = processing->ptr;
  processing->ptr = NULL;
  while (ptr) {
    int currdatachars;
    if (!PerformConversion(ptr,string,&currdatachars))
      return FALSE;
    *datachars = max(*datachars,currdatachars);
    ptr = FindFlagDef(processing->name,ptr->Next(),processing->namelength);
  }
  return TRUE;
}

//===========================================================================
static BOOL ProcessFile (LPCTSTR            filename,
                         PROCESSINGPTR      processing,
                         CMDDEFPTR         *nextarg,
                         SCMDEXTRACALLBACK  extracallback,
                         SCMDERRORCALLBACK  errorcallback) {

  // OPEN THE FILE
  HANDLE file = CreateFile(filename,
                           GENERIC_READ,
                           FILE_SHARE_READ,
                           NULL,
                           OPEN_EXISTING,
                           FILE_FLAG_SEQUENTIAL_SCAN,
                           NULL);
  if (!file) {
    if (errorcallback)
      GenerateError(errorcallback,SCMD_ERROR_OPEN_FAILED,filename);
    return FALSE;
  }

  // READ IT INTO MEMORY
  DWORD  size   = GetFileSize(file,NULL);
  LPVOID buffer = ALLOC(size+1);
  DWORD  bytesread;
  ReadFile(file,buffer,size,&bytesread,NULL);
  CloseHandle(file);

  // NULL TERMINATE THE ENTIRE FILE
  *((LPBYTE)buffer+bytesread) = 0;

  // PROCESS IT AS ONE LARGE STRING
  LPCTSTR curr = (LPCTSTR)buffer;
  BOOL success = ProcessString(&curr,
                               processing,
                               nextarg,
                               extracallback,
                               errorcallback);

  // FREE THE BUFFER
  FREE(buffer);

  return success;
}

//===========================================================================
static BOOL ProcessFlags (LPCTSTR           string,
                          PROCESSINGPTR     processing,
                          SCMDERRORCALLBACK errorcallback) {
  char lastflag[256] = "";
  while (*string) {

    // FIND THE BEST MATCHED FLAG DEFINITION, COMBINING THIS FLAG WITH THE
    // HEADER CHARACTERS FROM THE LAST FLAG IN THIS TOKEN IF POSSIBLE
    int       strlength      = SStrLen(string);
    int       lastflaglength = max(1,SStrLen(lastflag));
    CMDDEFPTR ptr            = NULL;
    while (lastflaglength--)
      if (strlength+lastflaglength < 256) {
        SStrCopy(lastflag+lastflaglength,string,256);
        if ((ptr = FindFlagDef(lastflag,s_flaglist.Head(),0)) != NULL) {
          lastflaglength = ptr->namelength;
          lastflag[lastflaglength] = 0;
          break;
        }
      }
    if (!ptr) {
      if (errorcallback)
        GenerateError(errorcallback,SCMD_ERROR_BAD_ARGUMENT,string);
      return FALSE;
    }

    // MOVE THE STRING POINTER PAST THE FLAG NAME
    string += lastflaglength;

    // SETUP THE PROCESSING STRUCTURE
    processing->ptr        = ptr;
    processing->namelength = lastflaglength;
    SStrCopy(processing->name,lastflag);

    // IF THERE IS NO DATA PAST THE FLAG NAME AND THIS IS A NON-BOOLEAN
    // FLAG, DELAY PROCESSING UNTIL WE GET THE DATA FROM THE NEXT TOKEN
    if ((!*string) &&
        (TYPEVAL(ptr->flags) != SCMD_TYPE_BOOL))
      return TRUE;

    // PROCESS ALL INSTANCES OF THE BEST MATCHED FLAG
    int datachars;
    if (!ProcessCurrentFlag(string,processing,&datachars))
      return FALSE;

    // MOVE THE STRING POINTER PAST THE FLAG DATA
    string += datachars;

  }
  return TRUE;
}

//===========================================================================
static BOOL ProcessString (LPCTSTR           *stringptr,
                           PROCESSINGPTR      processing,
                           CMDDEFPTR         *nextarg,
                           SCMDEXTRACALLBACK  extracallback,
                           SCMDERRORCALLBACK  errorcallback) {

  // PROCESS EACH TOKEN IN THE STRING
  while (**stringptr) {
    LPCTSTR nextptr     = *stringptr;
    char    buffer[256] = "";
    BOOL    quoted      = FALSE;
    SStrTokenize(&nextptr,
                 buffer,
                 256,
                 WHITESPACE,
                 &quoted);
    if (!ProcessToken(buffer,
                      quoted,
                      processing,
                      nextarg,
                      extracallback,
                      errorcallback))
      break;
    *stringptr = nextptr;
  }

  // RETURN SUCCESS IF WE PROCESSED THE ENTIRE STRING
  return !**stringptr;

}

//===========================================================================
static BOOL ProcessToken (LPCTSTR            string,
                          BOOL               quoted,
                          PROCESSINGPTR      processing,
                          CMDDEFPTR         *nextarg,
                          SCMDEXTRACALLBACK  extracallback,
                          SCMDERRORCALLBACK  errorcallback) {

  // IF THIS IS A RESPONSE FILE, PROCESS IT RECURSIVELY
  if ((*string == '@') && !quoted)
    return ProcessFile(string+1,
                       processing,
                       nextarg,
                       extracallback,
                       errorcallback);

  // IF THIS TOKEN CONTAINS FLAGS, PROCESS THEM
  if (SStrChr(FLAGCHARS,*string) && !quoted) {
    processing->ptr = NULL;
    return ProcessFlags(string+1,processing,errorcallback);
  }

  // IF WE ARE STILL PROCESSING A PREVIOUS ARGUMENT, USE THIS TOKEN
  // AS DATA FOR THAT ARGUMENT
  if (processing->ptr) {
    int datachars;
    return ProcessCurrentFlag(string,processing,&datachars);
  }

  // IF THERE ARE ADDITIONAL REQUIRED/OPTIONAL ARGUMENTS TO FILL,
  // USE THIS TOKEN AS ONE
  if (*nextarg) {
    int datachars;
    if (!PerformConversion(*nextarg,string,&datachars))
      return FALSE;
    *nextarg = (*nextarg)->Next();
    return TRUE;
  }

  // IF THERE IS AN EXTRA CALLBACK DEFINED, PASS THIS TOKEN TO THE
  // EXTRA CALLBACK
  if (extracallback)
    return extracallback(string);

  // OTHERWISE, DISPLAY AN ERROR AND RETURN FALSE TO STOP THE COMMAND
  // LINE PARSING BECAUSE WE ARE UNABLE TO IDENTIFY OR PROCESS THE TOKEN
  if (errorcallback)
    GenerateError(errorcallback,SCMD_ERROR_BAD_ARGUMENT,string);
  return FALSE;

}

/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

//===========================================================================
BOOL APIENTRY SCmdCheckId (DWORD id) {
  for (BOOL flaglist = FALSE; flaglist <= TRUE; ++flaglist)
    ITERATELIST(CMDDEF,
                flaglist ? s_flaglist : s_arglist,
                currptr)
      if (currptr->id == id)
        return currptr->found;
  return FALSE;
}

//===========================================================================
BOOL APIENTRY SCmdDestroy () {
  for (BOOL flaglist = FALSE; flaglist <= TRUE; ++flaglist)
    ITERATELIST(CMDDEF,
                flaglist ? s_flaglist : s_arglist,
                currptr)
      if ((TYPEVAL(currptr->flags) == SCMD_TYPE_STRING) &&
          currptr->currvaluestr) {
        FREE(currptr->currvaluestr);
        currptr->currvaluestr = NULL;
      }
  s_arglist.Clear();
  s_flaglist.Clear();
  s_addedoptional = FALSE;
  return TRUE;
}

//===========================================================================
BOOL APIENTRY SCmdGetBool (DWORD id) {
  return (SCmdGetNum(id) != 0);
}

//===========================================================================
DWORD APIENTRY SCmdGetNum (DWORD id) {
  for (BOOL flaglist = FALSE; flaglist <= TRUE; ++flaglist)
    ITERATELIST(CMDDEF,
                flaglist ? s_flaglist : s_arglist,
                currptr)
      if (currptr->id == id)
        return currptr->currvalue;
  return 0;
}

//===========================================================================
BOOL APIENTRY SCmdGetString (DWORD  id,
                             LPTSTR buffer,
                             DWORD  bufferchars) {
  if (buffer)
    *buffer = 0;

  VALIDATEBEGIN;
  VALIDATE(buffer);
  VALIDATE(bufferchars);
  VALIDATEEND;

  for (BOOL flaglist = FALSE; flaglist <= TRUE; ++flaglist)
    ITERATELIST(CMDDEF,
                flaglist ? s_flaglist : s_arglist,
                currptr)
      if (currptr->id == id) {
        if (currptr->currvaluestr)
          SStrCopy(buffer,currptr->currvaluestr,bufferchars);
        return TRUE;
      }
  return FALSE;
}

//===========================================================================
BOOL APIENTRY SCmdProcess (LPCTSTR           cmdline,
                           BOOL              skipprogname,
                           SCMDEXTRACALLBACK extracallback,
                           SCMDERRORCALLBACK errorcallback) {
  VALIDATEBEGIN;
  VALIDATE(cmdline);
  VALIDATEEND;

  // IF REQUESTED, SKIP PAST THE PROGRAM NAME AT THE START OF THE COMMAND
  // LINE
  if (skipprogname)
    SStrTokenize(&cmdline,
                 NULL,
                 0,
                 WHITESPACE,
                 NULL);

  // PROCESS EACH ARGUMENT
  PROCESSING processing;
  ZeroMemory(&processing,sizeof(PROCESSING));
  CMDDEFPTR nextarg = s_arglist.Head();
  if (!ProcessString(&cmdline,
                     &processing,
                     &nextarg,
                     extracallback,
                     errorcallback))
    return FALSE;

  // DETERMINE WHETHER ALL REQUIRED ARGUMENTS WERE FILLED IN
  BOOL allfilled = TRUE;
  while (nextarg && allfilled)
    if (ARGVAL(nextarg->flags) == SCMD_ARG_REQUIRED)
      allfilled = FALSE;
    else
      nextarg = nextarg->Next();
  if (errorcallback && !allfilled)
    GenerateError(errorcallback,SCMD_ERROR_NOT_ENOUGH_ARGUMENTS,"");

  return allfilled;
}

//===========================================================================
BOOL APIENTRY SCmdRegisterArgList (const ARGLIST *listptr,
                                   DWORD          numargs) {
  VALIDATEBEGIN;
  VALIDATE(listptr);
  VALIDATEEND;

  for (DWORD loop = 0; loop < numargs; ++loop) {
    if (!SCmdRegisterArgument(listptr->flags,
                              listptr->id,
                              listptr->name,
                              NULL,
                              0,
                              TRUE,
                              0xFFFFFFFF,
                              listptr->callback))
      return FALSE;
    ++listptr;
  }

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SCmdRegisterArgument (DWORD        flags,
                                    DWORD        id,
                                    LPCTSTR      name,
                                    LPVOID       variableptr,
                                    DWORD        variablebytes,
                                    DWORD        setvalue,
                                    DWORD        setmask,
                                    SCMDCALLBACK callback) {
  if (!name)
    name = "";
  int namelength = SStrLen(name);

  VALIDATEBEGIN;
  VALIDATE(namelength < MAXNAMELENGTH);
  VALIDATE((!variablebytes) || variableptr);
  VALIDATE((ARGVAL(flags) != SCMD_ARG_REQUIRED) || (!s_addedoptional));
  VALIDATE((ARGVAL(flags) != SCMD_ARG_FLAGGED) || (namelength > 0));
  VALIDATE((TYPEVAL(flags) != SCMD_TYPE_BOOL) || (!variableptr) || (variablebytes == sizeof(DWORD)));
  VALIDATEEND;

  // ADD THE ARGUMENT TO THE END OF THE APPROPRIATE LINKED LIST
  CMDDEFPTR newptr;
  if (ARGVAL(flags) == SCMD_ARG_FLAGGED)
    newptr = s_flaglist.NewNode();
  else
    newptr = s_arglist.NewNode();
  SStrCopy(newptr->name,name,MAXNAMELENGTH);
  newptr->id            = id;
  newptr->namelength    = namelength;
  newptr->flags         = flags;
  newptr->variableptr   = variableptr;
  newptr->variablebytes = variablebytes;
  newptr->setvalue      = setvalue;
  newptr->setmask       = setmask;
  newptr->callback      = callback;

  // SET THE INITIAL VALUE
  if ((TYPEVAL(flags) == SCMD_TYPE_BOOL) &&
      (BOOLVAL(flags) == SCMD_BOOL_CLEAR))
    newptr->currvalue = setvalue;
  else
    newptr->currvalue = 0;

  // IF THIS IS AN OPTIONAL ARGUMENT, KEEP TRACK OF THE FACT THAT WE'VE
  // ADDED AT LEAST ONE OPTIONAL ARGUMENT SO THAT WE WILL REFUSE TO ADD
  // ANY MORE REQUIRED ARGUMENTS IN THE FUTURE
  if (ARGVAL(flags) == SCMD_ARG_OPTIONAL)
    s_addedoptional = TRUE;

  return TRUE;
}
