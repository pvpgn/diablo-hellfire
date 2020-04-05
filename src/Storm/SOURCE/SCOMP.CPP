/****************************************************************************
*
*  SCOMP.CPP
*  Storm compression functions
*
*  By Michael O'Brien (10/17/97)
*
***/

#include "pch.h"
#pragma  hdrstop

#define  SOURCESYMBOLS    256
#define  EOS              256
#define  ESCAPE           257
#define  SYMBOLS          258

static const BYTE s_probability[SCOMP_HINTS][SYMBOLS] =
  {

    // BASIC PROBABILITY TABLE
    {10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},

    // BINARY PROBABILITY TABLE
    {84,22,22,13,12,8,6,5,6,5,6,3,4,4,3,5,
     14,11,20,19,19,9,11,6,5,4,3,2,3,2,2,2,
     13,7,9,6,6,4,3,2,4,3,3,3,3,3,2,2,
     9,6,4,4,4,4,3,2,3,2,2,2,2,3,2,4,
     8,3,4,7,9,5,3,3,3,3,2,2,2,3,2,2,
     3,2,2,2,2,2,2,2,2,1,1,1,2,1,2,2,
     6,10,8,8,6,7,4,3,4,4,2,2,4,2,3,3,
     4,3,7,7,9,6,4,3,3,2,1,2,2,2,2,2,
     10,2,2,3,2,2,1,1,2,2,2,6,3,5,2,3,
     2,1,1,1,1,1,1,1,1,1,1,2,3,1,1,1,
     2,1,1,1,1,1,1,2,4,4,4,7,9,8,12,2,
     1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,3,
     4,1,2,4,5,1,1,1,1,1,1,1,2,1,1,1,
     4,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,
     2,1,1,1,1,1,1,1,3,1,1,1,1,1,1,1,
     2,1,1,1,1,1,1,2,2,1,1,2,2,2,6,75},

    // TEXT PROBABILITY TABLE
    {0,0,0,0,0,0,0,0,0,3,39,0,0,35,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     255,1,1,1,1,1,1,1,2,2,1,1,6,14,16,4,
     6,8,5,4,4,3,3,2,2,3,3,1,1,2,1,1,
     1,4,2,4,2,2,2,1,1,4,1,1,2,3,3,2,
     3,1,3,6,4,1,1,1,1,1,1,2,1,2,1,1,
     1,41,7,22,18,64,10,10,17,37,1,3,23,16,38,42,
     16,1,35,35,47,16,6,7,2,9,1,1,1,1,1,0},

    // EXECUTABLE PROBABILITY TABLE
    {255,11,7,5,11,2,2,2,6,2,2,1,4,2,1,3,
     9,1,1,1,3,4,1,1,2,1,1,1,2,1,1,1,
     5,1,1,1,13,1,1,1,1,1,1,1,1,1,1,1,
     2,1,1,3,1,1,1,1,1,1,1,2,1,1,1,1,
     10,4,2,1,6,3,2,1,1,1,1,1,3,1,1,1,
     5,2,3,4,3,3,3,2,1,1,1,2,1,2,3,3,
     1,3,1,1,2,5,1,1,4,3,5,1,3,1,3,3,
     2,1,4,3,10,6,1,1,1,1,1,1,1,1,1,1,
     2,2,1,10,2,5,1,1,2,7,2,23,1,5,1,1,
     14,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
     1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
     1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
     6,2,1,4,5,1,1,2,1,1,1,1,2,1,1,1,
     1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
     1,1,1,1,1,1,1,1,7,1,1,2,1,1,1,1,
     2,1,1,1,1,1,1,1,2,1,1,1,1,1,1,17},

    // ADPCM4 PROBABILITY TABLE
    {255,251,152,154,132,133,99,100,62,62,34,34,19,19,24,23},

    // ADPCM6 PROBABILITY TABLE
    {255,241,157,158,154,155,154,151,147,147,140,142,134,136,128,130,
     124,124,114,115,105,107,95,96,85,86,74,75,64,65,55,55,
     47,47,39,39,33,33,27,28,23,23,19,19,16,16,13,13,
     11,11,9,9,8,8,7,7,6,5,5,4,4,4,25,24}

  };

/****************************************************************************
*
*  BITWISE I/O SUPPORT
*
***/

namespace {

class CBitInput {

  private:
    const DWORD *m_currsource;
    DWORD        m_rack;
    DWORD        m_rackbits;

  public:
    CBitInput (LPCVOID source,
               DWORD   sourcebytes);
    inline DWORD InputBit ();
    inline DWORD InputBits (DWORD count,
                            DWORD mask);
    inline DWORD PeekBits (DWORD count,
                           DWORD mask);
    inline void  RemoveBits (DWORD count);

};

class CBitOutput {

  private:
    LPBYTE m_basedest;
    DWORD  m_bytesleft;
    LPBYTE m_currdest;
    DWORD  m_rack;
    DWORD  m_rackbits;

  public:
    CBitOutput (LPVOID dest,
                DWORD  destsize);
    DWORD GetTotalBytes ();
    inline void OutputBits (DWORD value,
                            DWORD count);
    void Pad ();

};

//===========================================================================
CBitInput::CBitInput (LPCVOID source,
                      DWORD   sourcebytes) {
  m_currsource = (const DWORD *)source;
  m_rack       = *m_currsource++;
  m_rackbits   = 32;
}

//===========================================================================
DWORD CBitInput::InputBit () {
  DWORD result = (m_rack & 1);
  m_rack >>= 1;
  if (!--m_rackbits) {
    m_rack     = *m_currsource++;
    m_rackbits = 32;
  }
  return result;
}

//===========================================================================
DWORD CBitInput::InputBits (DWORD count,
                            DWORD mask) {
  DWORD result = PeekBits(count,mask);
  RemoveBits(count);
  return result;
}

//===========================================================================
DWORD CBitInput::PeekBits (DWORD count,
                           DWORD mask) {
  while (m_rackbits <= count) {
    m_rack       |= *(LPBYTE)m_currsource << m_rackbits;
    m_rackbits   += 8;
    m_currsource  = (LPDWORD)((LPBYTE)m_currsource+1);
  }
  return m_rack & mask;
}

//===========================================================================
void CBitInput::RemoveBits (DWORD count) {
  m_rack     >>= count;
  m_rackbits  -= count;
}

//===========================================================================
CBitOutput::CBitOutput (LPVOID dest,
                        DWORD  destsize) {
  m_basedest  = m_currdest = (LPBYTE)dest;
  m_bytesleft = destsize;
  m_rack      = 0;
  m_rackbits  = 0;
}

//===========================================================================
DWORD CBitOutput::GetTotalBytes () {
  return m_currdest-m_basedest;
}

//===========================================================================
void CBitOutput::OutputBits (DWORD value,
                             DWORD count) {
  m_rack     |= (value << m_rackbits);
  m_rackbits += count;
  while (m_rackbits >= 8) {
    if (m_bytesleft) {
      *m_currdest++ = (BYTE)m_rack;
      --m_bytesleft;
    }
    m_rack     >>= 8;
    m_rackbits  -= 8;
  }
}

//===========================================================================
void CBitOutput::Pad () {
  while (m_rackbits) {
    if (m_bytesleft) {
      *m_currdest++ = (BYTE)m_rack;
      --m_bytesleft;
    }
    m_rack     >>= 8;
    m_rackbits  -= min(8,m_rackbits);
  }
}

} // end of namespace

/****************************************************************************
*
*  HUFFMAN ENCODER
*
***/

namespace {

NODEDECL(HUFFNODE) {
    int       symbol;
    DWORD     weight;
    HUFFNODE *parent;
    HUFFNODE *child;
} *HUFFNODEPTR;

class CHuffman {

  protected:
    BOOL           m_adaptive;
    DWORD          m_changesequence;
    LIST(HUFFNODE) m_nodelist;
    HUFFNODEPTR    m_symbol[SYMBOLS];

    void AddSymbol (int symbol);
    void BuildTree (BYTE hint);
    inline void IncrementWeight (HUFFNODEPTR node);

  public:
    CHuffman ();
    ~CHuffman ();
 
};

class CHuffmanDecoder : public CHuffman {

  private:
    DWORD m_cachebits[SYMBOLS];
    DWORD m_cachesequence[SYMBOLS];
    int   m_cachesymbol[SYMBOLS];

    inline int DecodeSymbol (CBitInput *input);

  public:
    CHuffmanDecoder ();
    DWORD Decompress (LPVOID     dest,
                      CBitInput *input);

};

class CHuffmanEncoder : public CHuffman {

  private:
    inline void EncodeSymbol (CBitOutput *output,
                              int         symbol);

  public:
    DWORD Compress (CBitOutput *output,
                    LPCVOID     source,
                    DWORD       sourcesize,
                    BYTE        hint);

};

//===========================================================================
CHuffman::CHuffman () {
  m_changesequence = 1;
}

//===========================================================================
CHuffman::~CHuffman () {
  m_nodelist.Clear();
}

//===========================================================================
void CHuffman::AddSymbol (int symbol) {

  // CONVERT THE LIGHTEST WEIGHT LEAF NODE INTO A BRANCH
  HUFFNODEPTR parent   = m_nodelist.Head();
  HUFFNODEPTR oldchild = m_nodelist.NewNode(LIST_HEAD);
  oldchild->symbol = parent->symbol;
  oldchild->weight = parent->weight;
  oldchild->parent = parent;
  m_symbol[oldchild->symbol] = oldchild;

  // ALLOCATE A NEW LEAF NODE FOR THIS SYMBOL
  HUFFNODEPTR newchild = m_nodelist.NewNode(LIST_HEAD);
  newchild->symbol = symbol;
  newchild->weight = 0;
  newchild->parent = parent;
  m_symbol[symbol] = newchild;

  // ATTACH THE PARENT NODE TO THESE TWO LEAF NODES
  parent->child = newchild;

  // INCREMENT THE NEW LEAF NODE'S WEIGHT
  IncrementWeight(newchild);

}

//===========================================================================
void CHuffman::BuildTree (BYTE hint) {

  // REMOVE ALL NODES FROM THE EXISTING NODE LIST
  m_nodelist.Clear();

  // ADD LEAF NODES FOR EACH SYMBOL
  ZeroMemory(m_symbol,SYMBOLS*sizeof(HUFFNODEPTR));
  DWORD maxweight = 0;
  int   symbol;
  for (symbol = 0; symbol < SOURCESYMBOLS; ++symbol)
    if (s_probability[hint][symbol]) {

      // CREATE A NEW NODE FOR THIS SYMBOL
      HUFFNODEPTR newnode = m_symbol[symbol] = m_nodelist.NewNode(LIST_TAIL);
      newnode->symbol = symbol;
      newnode->weight = s_probability[hint][symbol];

      // LINK IT INTO THE NODE LIST IN SORTED ORDER
      if (newnode->weight >= maxweight)
        maxweight = newnode->weight;
      else {
        HUFFNODEPTR checknode = m_nodelist.Head();
        while (checknode && (checknode->weight < newnode->weight))
          checknode = checknode->Next();
        m_nodelist.LinkNode(newnode,LIST_LINK_BEFORE,checknode);
      }

    }
  for (; symbol < SYMBOLS; ++symbol) {
    HUFFNODEPTR newnode = m_symbol[symbol] = m_nodelist.NewNode(LIST_HEAD);
    newnode->symbol = symbol;
    newnode->weight = 1;
  }

  // BUILD THE NODE TREE UP FROM THE LEAF NODES
  for (HUFFNODEPTR currnode = m_nodelist.Head(), nextnode;
       currnode && (nextnode = currnode->Next());
       currnode = nextnode->Next()) {

    // CREATE A NEW NODE TO BE THE PARENT OF THESE TWO NODES
    HUFFNODEPTR newnode = m_nodelist.NewNode(LIST_TAIL);
    newnode->weight = currnode->weight+nextnode->weight;
    newnode->child  = currnode;

    // SET EACH NODE'S PARENT TO THE NEW NODE
    currnode->parent = newnode;
    nextnode->parent = newnode;

    // LINK THE NEW NODE INTO THE LIST, SORTED BY WEIGHT
    if (newnode->weight >= maxweight)
      maxweight = newnode->weight;
    else {
      HUFFNODEPTR checknode = nextnode->Next();
      while (checknode && (checknode->weight < newnode->weight))
        checknode = checknode->Next();
      m_nodelist.LinkNode(newnode,LIST_LINK_BEFORE,checknode);
    }

  }

  // INITIALIZE THE SEQUENCE NUMBER
  m_changesequence = 1;

}

//===========================================================================
void CHuffman::IncrementWeight (HUFFNODEPTR node) {

  // PROPAGATE THE WEIGHT CHANGE UP THE TREE
  for (HUFFNODEPTR currnode = node;
       currnode;
       currnode = currnode->parent) {

    // UPDATE THIS NODE'S WEIGHT
    ++currnode->weight;

    // CHECK TO SEE IF THIS NODE NEEDS TO BE MOVED UP THE TREE
    HUFFNODEPTR checknode = currnode;
    HUFFNODEPTR nextchecknode;
    while ((nextchecknode = checknode->Next()) != NULL)
      if (nextchecknode->weight < currnode->weight)
        checknode = nextchecknode;
      else
        break;

    // IF IT DOES, THEN SWAP IT WITH THE NODE THAT HAS THE NEXT
    // HIGHER WEIGHT
    if (checknode != currnode) {

      // SWAP THE TWO NODES' POSITIONS IN THE LIST
      m_nodelist.LinkNode(checknode,LIST_LINK_BEFORE,currnode);
      m_nodelist.LinkNode(currnode,LIST_LINK_BEFORE,nextchecknode);

      // SWAP THE TWO NODES' PARENTS
      HUFFNODEPTR currnodefirstchild  = currnode->parent->child;
      HUFFNODEPTR checknodefirstchild = checknode->parent->child;
      if (currnodefirstchild == currnode)
        currnode->parent->child = checknode;
      if (checknodefirstchild == checknode)
        checknode->parent->child = currnode;
      SWAP(currnode->parent,checknode->parent);

      // INVALIDATE THE CACHE
      ++m_changesequence;

    }

  }

}

//===========================================================================
CHuffmanDecoder::CHuffmanDecoder () {
  ZeroMemory(m_cachesequence,SYMBOLS*sizeof(DWORD));
}

//===========================================================================
int CHuffmanDecoder::DecodeSymbol (CBitInput *input) {

  // CHECK TO SEE WHETHER THE DECODING OF THE NEXT SYMBOL IS CACHED
  DWORD nextbyte = input->PeekBits(8,0xFF);
  if (m_cachesequence[nextbyte] == m_changesequence) {
    input->RemoveBits(m_cachebits[nextbyte]);
    return m_cachesymbol[nextbyte];;
  }

  // IF NOT, DECODE THE SYMBOL BY WALKING THE TREE, THEN ADD IT TO THE CACHE
  DWORD bits = 0;
  for (HUFFNODEPTR currnode = m_nodelist.Tail();
       currnode->child;
       ++bits) {
    currnode = currnode->child;
    if (input->InputBit())
      currnode = currnode->Next();
  }
  int symbol = currnode->symbol;
  if (bits <= 8) {
    m_cachebits[nextbyte]     = bits;
    m_cachesequence[nextbyte] = m_changesequence;
    m_cachesymbol[nextbyte]   = symbol;
  }
  return symbol;

}

//===========================================================================
DWORD CHuffmanDecoder::Decompress (LPVOID     dest,
                                   CBitInput *input) {

  // INITIALIZE THE DECOMPRESSION TREE
  BYTE hint = (BYTE)input->InputBits(8,0xFF);
  BuildTree(hint);
  m_adaptive = (hint == SCOMP_HINT_NONE);

  // DECOMPRESS THE DATA
  LPBYTE currdest = (LPBYTE)dest;
  for (;;) {

    // DECODE THE NEXT SYMBOL
    int symbol = DecodeSymbol(input);

    // PROCESS THE SYMBOL
    if (symbol == ESCAPE) {
      symbol = (int)input->InputBits(8,0xFF);
      AddSymbol(symbol);
      if (!m_adaptive)
        IncrementWeight(m_symbol[symbol]);
    }
    if (symbol == EOS)
      break;
    *currdest++ = (BYTE)symbol;

    // UPDATE SYMBOL'S WEIGHT
    if (m_adaptive)
      IncrementWeight(m_symbol[symbol]);

  }

  return currdest-(LPBYTE)dest;
}

//===========================================================================
DWORD CHuffmanEncoder::Compress (CBitOutput *output,
                                 LPCVOID     source,
                                 DWORD       sourcesize,
                                 BYTE        hint) {

  // INITIALIZE THE COMPRESSION TREE USING THE HINT, AND SAVE THE HINT
  // TO THE DESTINATION BUFFER
  BuildTree(hint);
  m_adaptive = (hint == SCOMP_HINT_NONE);
  output->OutputBits(hint,8);

  // COMPRESS THE DATA
  const BYTE *currsource = (const BYTE *)source;
  while (sourcesize--) {
    int symbol = *currsource++;

    // IF THIS IS THE FIRST USE OF THIS SYMBOL, ENCODE AN ESCAPE SEQUENCE
    if (!m_symbol[symbol]) {
      EncodeSymbol(output,ESCAPE);
      output->OutputBits(symbol,8);
      AddSymbol(symbol);
      if (!m_adaptive)
        IncrementWeight(m_symbol[symbol]);
    }

    // OTHERWISE, ENCODE THE SYMBOL
    else
      EncodeSymbol(output,symbol);

    // UPDATE SYMBOL'S WEIGHT
    if (m_adaptive)
      IncrementWeight(m_symbol[symbol]);

  }

  // ENCODE AN END-OF-STREAM SEQUENCE
  EncodeSymbol(output,EOS);
  output->Pad();

  return output->GetTotalBytes();
}

//===========================================================================
void CHuffmanEncoder::EncodeSymbol (CBitOutput *output,
                                    int         symbol) {
  DWORD encoding     = 0;
  DWORD encodingbits = 0;
  for (HUFFNODEPTR currnode = m_symbol[symbol], parent;
       parent = currnode->parent;
       currnode = parent) {
    encoding <<= 1;
    encoding  |= (parent->child != currnode);
    ++encodingbits;
  }
  output->OutputBits(encoding,
                     encodingbits);
}

} // end of namespace

/****************************************************************************
*
*  ADPCM ENCODER
*
***/

namespace {

static const DWORD s_adpcm_2bit[2]  = {51,102};
static const DWORD s_adpcm_3bit[4]  = {58,58,80,112};
static const DWORD s_adpcm_4bit[8]  = {58,58,58,58,77,102,128,154};
static const DWORD s_adpcm_6bit[32] = {58,58,58,58,58,58,58,58,
                                       58,58,58,58,58,58,58,58,
                                       70,83,96,109,122,134,147,160,
                                       173,186,198,211,224,237,250,262};

class CAdpcm {

  protected:
    const DWORD *m_adapttable;
    DWORD        m_bitspersample;
    DWORD        m_minadapt;
    DWORD        m_maxadapt;
    DWORD        m_steps;

    inline void AdjustValue (int   *last,
                             DWORD *adapt,
                             DWORD  scaleddelta,
                             BOOL   negative);
    void Initialize (DWORD bitspersample);
    inline void PredictNextValue (int *last);

};

class CAdpcmDecoder : public CAdpcm {

  public:
    DWORD Decompress (short      *dest,
                      const BYTE *source,
                      DWORD       sourcesize,
                      DWORD       channels);

};

class CAdpcmEncoder : public CAdpcm {

  public:
    DWORD Compress (LPBYTE       dest,
                    const short *source,
                    DWORD        sourcesize,
                    DWORD        channels,
                    DWORD        bitspersample);

};

//===========================================================================
void CAdpcm::AdjustValue (int   *last,
                          DWORD *adapt,
                          DWORD  scaleddelta,
                          BOOL   negative) {

  // SAVE THE NEW VALUE, ADJUSTED BY THE SCALED DELTA, AS THE VALUE TO
  // BE USED AS THE LAST VALUE FOR THE NEXT ITERATION OF THE LOOP
  int quantdiff = (int)((scaleddelta+1)*(*adapt)+m_steps) >> m_bitspersample;
  if (negative) {
    *last -= quantdiff;
    if (*last < -32768)
      *last = -32768;
  }
  else {
    *last += quantdiff;
    if (*last > 32767)
      *last = 32767;
  }

  // ADJUST THE ADAPTATION FACTOR
  *adapt = ((*adapt)*m_adapttable[scaleddelta]+128) >> 6;
  if (*adapt < m_minadapt)
    *adapt = m_minadapt;
  if (*adapt > m_maxadapt)
    *adapt = m_maxadapt;

}

//===========================================================================
void CAdpcm::Initialize (DWORD bitspersample) {

  // SELECT THE ADAPTATION TABLE TO USE
  switch (bitspersample) {
    case 2:  m_adapttable = s_adpcm_2bit;  break;
    case 3:  m_adapttable = s_adpcm_3bit;  break;
    case 4:  m_adapttable = s_adpcm_4bit;  break;
    case 6:  m_adapttable = s_adpcm_6bit;  break;
    default: m_adapttable = NULL;          break;
  }
  ASSERT(m_adapttable);

  // SAVE THE NUMBER OF BITS PER SAMPLE
  m_bitspersample = bitspersample;

  // COMPUTE THE NUMBER OF STEPS IN THE ADAPTATION TABLE, AND THE MINIMUM
  // AND MAXIMUM ADAPTATION VALUES
  m_steps    = (1 << m_bitspersample) / 2;
  m_minadapt = 1 << m_bitspersample;
  m_maxadapt = 131072;

}

//===========================================================================
void CAdpcm::PredictNextValue (int *last) {
  *last = (230*(*last)+128) >> 8;
}

//===========================================================================
DWORD CAdpcmDecoder::Decompress (short      *dest,
                                 const BYTE *source,
                                 DWORD       sourcesize,
                                 DWORD       channels) {
  const BYTE *basesource = source;
  short      *basedest   = dest;
  DWORD loop;

  // READ THE BITRATE AND INITIALIZE ADPCM DECOMPRESSION
  Initialize(*source++);

  // READ THE ACCUMULATED ERROR FOR THIS BLOCK FOR EACH CHANNEL
  DWORD absaccumerror[2];
  DWORD negaccumerror[2];
  for (loop = 0; loop < channels; ++loop) {
    BYTE code = *source++;
    absaccumerror[loop] = code >> 1;
    negaccumerror[loop] = code & 1;
  }

  // READ THE STARTING ADAPTATION VALUE FOR EACH CHANNEL
  DWORD adapt[2];
  for (loop = 0; loop < channels; ++loop) {
    adapt[loop] = ((DWORD)(*(LPWORD)source)) << m_bitspersample;
    source += 2;
  }

  // READ THE INITIAL SAMPLE FOR EACH CHANNEL
  int last[2];
  for (loop = 0; loop < channels; ++loop) {
    last[loop] = *(const short *)source;
    source += 2;
    *dest++ = (short)last[loop];
  }

  // PROCESS ALL THE COMPRESSED SAMPLES IN THIS BLOCK
  DWORD channel = 0;
  for (loop = source-basesource; loop < sourcesize; ++loop) {

    // ADJUST THE LAST VALUE BY A CONSTANT SCALING FACTOR
    PredictNextValue(&last[channel]);

    // READ THE NEXT COMPRESSED SAMPLE
    BYTE  code   = *source++;
    DWORD scaled = code >> 1;
    BOOL  neg    = code & 1;

    // GENERATE THE VALUE OF THE UNCOMPRESSED SAMPLE, AND ADJUST THE
    // ADAPTATION VALUE
    AdjustValue(&last[channel],
                &adapt[channel],
                scaled,
                neg);

    // IF WE ARE NEARING THE END OF THE BLOCK, ADD IN A LITTLE BIT OF
    // THE ERROR TERM INTO EACH SAMPLE, SO THAT THE BLOCK WILL END ON
    // EXACTLY THE CORRECT VALUE
    int   output      = last[channel];
    DWORD samplesleft = (sourcesize-(loop+1)) >> (channels-1);
    if (samplesleft < absaccumerror[channel])
      if (negaccumerror[channel]) {
        output += absaccumerror[channel]-samplesleft;
        if (output > 32767)
          output = 32767;
      }
      else {
        output -= absaccumerror[channel]-samplesleft;
        if (output < -32768)
          output = -32768;
      }
    *dest++ = (short)output;

    // ALTERNATE CHANNELS
    if (channels == 2)
      channel = !channel;

  }

  return (LPBYTE)dest-(LPBYTE)basedest;
}

//===========================================================================
DWORD CAdpcmEncoder::Compress (LPBYTE       dest,
                               const short *source,
                               DWORD        sourcesize,
                               DWORD        channels,
                               DWORD        bitspersample) {
  LPBYTE basedest = dest;
  DWORD  loop;

  // INITIALIZE ADPCM COMPRESSION
  Initialize(bitspersample);

  // SAVE THE NUMBER OF BITS PER SAMPLE
  *dest++ = (BYTE)bitspersample;

  // RESERVE SPACE FOR THE ACCUMULATED ERROR FOR EACH CHANNEL
  LPBYTE accumerrorptr[2];
  for (loop = 0; loop < channels; ++loop)
    accumerrorptr[loop] = dest++;

  // COMPUTE AND SAVE THE STARTING ADAPTATION VALUE FOR EACH CHANNEL
  DWORD adapt[2];
  for (loop = 0; loop < channels; ++loop) {
    int   last      = source[loop];
    PredictNextValue(&last);
    DWORD firstdiff = abs(source[loop+channels]-last) << m_bitspersample;
    DWORD bestadapt = firstdiff*2/m_steps;
    bestadapt = max(bestadapt,m_minadapt);
    bestadapt = min(bestadapt,m_maxadapt);
    DWORD savevalue = bestadapt >> m_bitspersample;
    *(LPWORD)dest = (WORD)savevalue;
    dest += 2;
    adapt[loop] = savevalue << m_bitspersample;
  }

  // SAVE THE VALUE OF THE FIRST SAMPLE FOR EACH CHANNEL
  int last[2];
  for (loop = 0; loop < channels; ++loop) {
    last[loop] = *source++;
    *(short *)dest = (short)last[loop];
    dest += 2;
  }

  // PROCESS EACH OF THE REMAINING SAMPLES
  int   val[2];
  DWORD channel = 0;
  DWORD samples = sourcesize/2;
  for (loop = channels; loop < samples; ++loop) {

    // ADJUST THE LAST VALUE BY A CONSTANT SCALING FACTOR
    PredictNextValue(&last[channel]);

    // READ THE NEXT SAMPLE
    val[channel] = *source++;
    DWORD diff = abs(val[channel]-last[channel]);
    BOOL  neg  = last[channel] > val[channel];

    // PRODUCE A DELTA WHICH IS SCALED BY THE ADAPTATION FACTOR
    DWORD unscaled = diff << bitspersample;
    DWORD scaled   = 0;
    if (unscaled > adapt[channel])
      scaled = min((unscaled - (adapt[channel] >> 1)) / adapt[channel],
                   m_steps-1);

    // PACK THE SCALED NUMBER AND THE SIGN INTO THE DESIRED NUMBER OF BITS
    *dest++ = (BYTE)((scaled << 1) | neg);

    // ADJUST THE ADAPTATION FACTOR, AND GENERATE THE VALUE OF THE
    // COMPRESSED AND DECOMPRESSED SAMPLE, WHICH WE WILL USE AS THE
    // VALUE OF THE LAST SAMPLE IN THE NEXT ITERATION OF THE LOOP
    AdjustValue(&last[channel],
                &adapt[channel],
                scaled,
                neg);

    // ALTERNATE CHANNELS
    if (channels == 2)
      channel = !channel;

  }

  // COMPUTE AND SAVE THE ACCUMULATED ERROR FOR EACH CHANNEL
  for (loop = 0; loop < channels; ++loop) {
    int   accumerror    = last[loop]-val[loop];
    DWORD absaccumerror = abs(accumerror);
    BOOL  negaccumerror = accumerror < 0;
    absaccumerror = min(127,absaccumerror);
    *accumerrorptr[loop] = (BYTE)(absaccumerror << 1) | negaccumerror;
  }

  return dest-basedest;
}

} // end of namespace

/****************************************************************************
*
*  PKWARE ENCODER
*
***/

typedef struct _PKWAREINFO {
    LPVOID  dest;
    DWORD   destpos;
    DWORD   destsize;
    LPCVOID source;
    DWORD   sourcepos;
    DWORD   sourcesize;
} PKWAREINFO, *PKWAREINFOPTR;

//===========================================================================
static UINT __cdecl PkwareBufferRead (LPSTR   buffer,
                                      UINT   *size,
                                      LPVOID  param) {
  PKWAREINFOPTR infoptr = (PKWAREINFOPTR)param;
  UINT bytes = min(*size,infoptr->sourcesize-infoptr->sourcepos);
  CopyMemory(buffer,
             (const BYTE *)infoptr->source+infoptr->sourcepos,
             bytes);
  infoptr->sourcepos += bytes;
  return bytes;
}

//===========================================================================
static void __cdecl PkwareBufferWrite (LPSTR   buffer,
                                       UINT   *size,
                                       LPVOID  param) {
  PKWAREINFOPTR infoptr = (PKWAREINFOPTR)param;
  UINT bytes = min(*size,infoptr->destsize-infoptr->destpos);
  CopyMemory((LPBYTE)infoptr->dest+infoptr->destpos,
             buffer,
             bytes);
  infoptr->destpos += bytes;
}

//===========================================================================
static void PkwareCompress (LPVOID   dest,
                            DWORD   *destsize,
                            LPCVOID  source,
                            DWORD    sourcesize,
                            DWORD   *hint,
                            DWORD    optimization) {

  // CREATE A COMPRESSION BUFFER
  LPSTR implodebuffer = (LPSTR)ALLOC(CMP_BUFFER_SIZE);

  // CREATE AN INFORMATION RECORD
  PKWAREINFO info;
  info.dest       = dest;
  info.destpos    = 0;
  info.destsize   = *destsize;
  info.source     = source;
  info.sourcepos  = 0;
  info.sourcesize = sourcesize;

  // DETERMINE THE SOURCE TYPE
  unsigned type     = (*hint == SCOMP_HINT_TEXT)
                        ? CMP_ASCII
                        : CMP_BINARY;
  unsigned dictsize = (sourcesize >= 3072)
                        ? 4096
                        : (sourcesize >= 1536)
                          ? 2048
                          : 1024;

  // PERFORM THE DECOMPRESSION
  implode(PkwareBufferRead,
          PkwareBufferWrite,
          (LPSTR)implodebuffer,
          &info,
          &type,
          &dictsize);

  // FREE THE DECOMPRESSION BUFFER
  FREE(implodebuffer);

  *destsize = info.destpos;
  *hint     = SCOMP_HINT_NONE;
}

//===========================================================================
static void PkwareDecompress (LPVOID   dest,
                              DWORD   *destsize,
                              LPCVOID  source,
                              DWORD    sourcesize) {

  // CREATE A DECOMPRESSION BUFFER
  LPSTR explodebuffer = (LPSTR)ALLOC(EXP_BUFFER_SIZE);

  // CREATE AN INFORMATION RECORD
  PKWAREINFO info;
  info.dest       = dest;
  info.destpos    = 0;
  info.destsize   = *destsize;
  info.source     = source;
  info.sourcepos  = 0;
  info.sourcesize = sourcesize;

  // PERFORM THE DECOMPRESSION
  explode(PkwareBufferRead,
          PkwareBufferWrite,
          (LPSTR)explodebuffer,
          &info);

  // FREE THE DECOMPRESSION BUFFER
  FREE(explodebuffer);

  *destsize = info.destpos;
}

/****************************************************************************
*
*  WRAPPER FUNCTIONS
*
***/

//===========================================================================
static void AdpcmMonoCompress (LPVOID   dest,
                               DWORD   *destsize,
                               LPCVOID  source,
                               DWORD    sourcesize,
                               DWORD   *hint,
                               DWORD    optimization) {
  CAdpcmEncoder adpcm;
  *destsize = adpcm.Compress((LPBYTE)dest,
                             (const short *)source,
                             sourcesize,
                             1,
                             (optimization == SCOMP_OPT_COMPRESSION)
                               ? 4
                               : 6);
  *hint = (optimization == SCOMP_OPT_COMPRESSION)
            ? SCOMP_HINT_ADPCM4
            : SCOMP_HINT_ADPCM6;
}

//===========================================================================
static void AdpcmMonoDecompress (LPVOID   dest,
                                 DWORD   *destsize,
                                 LPCVOID  source,
                                 DWORD    sourcesize) {
  CAdpcmDecoder adpcm;
  *destsize = adpcm.Decompress((short *)dest,
                               (const BYTE *)source,
                               sourcesize,
                               1);
}

//===========================================================================
static void AdpcmStereoCompress (LPVOID   dest,
                                 DWORD   *destsize,
                                 LPCVOID  source,
                                 DWORD    sourcesize,
                                 DWORD   *hint,
                                 DWORD    optimization) {
  CAdpcmEncoder adpcm;
  *destsize = adpcm.Compress((LPBYTE)dest,
                             (const short *)source,
                             sourcesize,
                             2,
                             (optimization == SCOMP_OPT_COMPRESSION)
                               ? 4
                               : 6);
  *hint = SCOMP_HINT_ADPCM6;
}

//===========================================================================
static void AdpcmStereoDecompress (LPVOID   dest,
                                   DWORD   *destsize,
                                   LPCVOID  source,
                                   DWORD    sourcesize) {
  CAdpcmDecoder adpcm;
  *destsize = adpcm.Decompress((short *)dest,
                               (const BYTE *)source,
                               sourcesize,
                               2);
}

//===========================================================================
static void HuffmanCompress (LPVOID   dest,
                             DWORD   *destsize,
                             LPCVOID  source,
                             DWORD    sourcesize,
                             DWORD   *hint,
                             DWORD    optimization) {
  CBitOutput      output(dest,*destsize);
  CHuffmanEncoder huff;
  *destsize = huff.Compress(&output,
                            source,
                            sourcesize,
                            (BYTE)*hint);
}

//===========================================================================
static void HuffmanDecompress (LPVOID   dest,
                               DWORD   *destsize,
                               LPCVOID  source,
                               DWORD    sourcesize) {
  CBitInput       input(source,sourcesize);
  CHuffmanDecoder huff;
  *destsize = huff.Decompress(dest,&input);
}

/****************************************************************************
*
*  UTILITY FUNCTIONS
*
***/

//===========================================================================
static inline BOOL BuffersOverlap (LPCVOID buf1,
                                   LPCVOID buf2,
                                   DWORD   length) {
  return (((LPBYTE)buf1+length > (LPBYTE)buf2) &&
          ((LPBYTE)buf2+length > (LPBYTE)buf1));
}

/****************************************************************************
*
*  EXPORTED FUNCTIONS
*
***/

#define  ALGORITHMS  4

typedef void (*COMPRESSFUNC)(LPVOID,DWORD *,LPCVOID,DWORD,DWORD *,DWORD);
typedef void (*DECOMPRESSFUNC)(LPVOID,DWORD *,LPCVOID,DWORD);

typedef struct _COMPRESSALGORITHM {
    DWORD        id;
    COMPRESSFUNC func;
} COMPRESSALGORITHM;

typedef struct _DECOMPRESSALGORITHM {
    DWORD          id;
    DECOMPRESSFUNC func;
} DECOMPRESSALGORITHM;

static const COMPRESSALGORITHM s_compressalgorithm[ALGORITHMS] =
    {{SCOMP_TYPE_LOSSY_ADPCM_MONO  ,AdpcmMonoCompress},
     {SCOMP_TYPE_LOSSY_ADPCM_STEREO,AdpcmStereoCompress},
     {SCOMP_TYPE_HUFFMAN           ,HuffmanCompress},
     {SCOMP_TYPE_PKWARE            ,PkwareCompress}};
static const DECOMPRESSALGORITHM s_decompressalgorithm[ALGORITHMS] =
    {{SCOMP_TYPE_LOSSY_ADPCM_MONO  ,AdpcmMonoDecompress},
     {SCOMP_TYPE_LOSSY_ADPCM_STEREO,AdpcmStereoDecompress},
     {SCOMP_TYPE_HUFFMAN           ,HuffmanDecompress},
     {SCOMP_TYPE_PKWARE            ,PkwareDecompress}};

//===========================================================================
BOOL APIENTRY SCompCompress (LPVOID   dest,
                             DWORD   *destsize,
                             LPCVOID  source,
                             DWORD    sourcesize,
                             DWORD    compressiontypes,
                             DWORD    hint,
                             DWORD    optimization) {
  VALIDATEBEGIN;
  VALIDATE(dest);
  VALIDATE(destsize);
  VALIDATE(*destsize >= sourcesize);
  VALIDATE(source);
  VALIDATEEND;

  // COUNT THE NUMBER OF OPERATIONS
  DWORD operations = 0;
  int   loop;
  for (loop = 0; loop < ALGORITHMS; ++loop)
    if (compressiontypes & s_compressalgorithm[loop].id)
      ++operations;

  // ALLOCATE A WORK BUFFER IF NECESSARY
  LPVOID work = NULL;
  if ((operations >= 2) ||
      (BuffersOverlap(dest,source,sourcesize) && operations))
    work = ALLOC(sourcesize);

  // APPLY EACH OPERATION
  LPCVOID curr = source;
  DWORD   size = sourcesize;
  for (loop = 0; loop < ALGORITHMS; ++loop)
    if (compressiontypes & s_compressalgorithm[loop].id) {
      --operations;

      // DETERMINE WHICH BUFFER WE WILL COMPRESS INTO FOR THIS OPERATION
      LPVOID target = (operations & 1) ? work : ((LPBYTE)dest+1);
      if (BuffersOverlap(target,curr,size))
        target = BuffersOverlap(target,work,size) ? ((LPBYTE)dest+1) : work;

      // PERFORM THE COMPRESSION OPERATION
      DWORD targetsize = size-1;
      s_compressalgorithm[loop].func(target,
                                     &targetsize,
                                     curr,
                                     size,
                                     &hint,
                                     optimization);

      // IF THE OPERATION FAILED TO COMPRESS THE DATA, THEN DISCARD THE
      // RESULT
      if (targetsize+1 < size) {
        curr = target;
        size = targetsize;
      }
      else
        compressiontypes &= ~s_compressalgorithm[loop].id;

    }

  // COPY THE FINAL RESULT TO THE DESTINATION BUFFER IF NECESSARY, AND
  // SAVE THE COMPRESSION TYPES USED
  if (curr != dest)
    if (curr == (LPBYTE)dest+1) {
      *(LPBYTE)dest = (BYTE)compressiontypes;
      ++size;
    }
    else if (compressiontypes) {
      CopyMemory((LPBYTE)dest+1,curr,size);
      *(LPBYTE)dest = (BYTE)compressiontypes;
      ++size;
    }
    else
      CopyMemory(dest,curr,size);
  *destsize = size;

  // FREE THE WORK BUFFER
  FREEIFUSED(work);

  return TRUE;
}

//===========================================================================
BOOL APIENTRY SCompDecompress (LPVOID   dest,
                               DWORD   *destsize,
                               LPCVOID  source,
                               DWORD    sourcesize) {
  VALIDATEBEGIN;
  VALIDATE(dest);
  VALIDATE(destsize);
  VALIDATE(*destsize >= sourcesize);
  VALIDATE(source);
  VALIDATEEND;

  // IF THE DATA IS NOT COMPRESSED, JUST COPY IT TO THE DESTINATION BUFFER
  // AND RETURN
  if (sourcesize == *destsize) {
    if (dest != source)
      CopyMemory(dest,source,sourcesize);
    return TRUE;
  }

  // EXTRACT THE COMPRESSION TYPES
  DWORD compressiontypes = *(LPBYTE)source;
  source = (LPBYTE)source+1;
  --sourcesize;

  // COUNT THE NUMBER OF OPERATIONS
  DWORD operations = 0;
  int   loop;
  for (loop = ALGORITHMS-1; loop >= 0; --loop)
    if (compressiontypes & s_decompressalgorithm[loop].id)
      ++operations;

  // ALLOCATE A WORK BUFFER IF NECESSARY
  LPVOID work = NULL;
  if ((operations >= 2) ||
      (BuffersOverlap(dest,source,sourcesize) && operations))
    work = ALLOC(*destsize);

  // APPLY EACH OPERATION
  LPCVOID curr = source;
  DWORD   size = sourcesize;
  for (loop = ALGORITHMS-1; loop >= 0; --loop)
    if (compressiontypes & s_decompressalgorithm[loop].id) {
      --operations;

      // DETERMINE WHICH BUFFER WE WILL DECOMPRESS INTO FOR THIS OPERATION
      LPVOID target = (operations & 1) ? work : dest;
      if (BuffersOverlap(target,curr,size))
        target = BuffersOverlap(target,work,size) ? dest : work;

      // PERFORM THE DECOMPRESSION OPERATION
      DWORD targetsize = *destsize;
      s_decompressalgorithm[loop].func(target,
                                       &targetsize,
                                       curr,
                                       size);
      curr = target;
      size = targetsize;

    }

  // COPY THE FINAL RESULT TO THE DESTINATION BUFFER IF NECESSARY
  if (curr != dest)
    CopyMemory(dest,curr,size);
  *destsize = size;

  // FREE THE WORK BUFFER
  FREEIFUSED(work);

  return TRUE;
}
