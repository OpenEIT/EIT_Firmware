/*
 * Copyright (C) 2007-2009 IAR Systems AB.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* $Rev: 25169 $ */

// The entry point for the application. Parses the command line and creates
// the commands.

#include "LxMain.h"

#include "LxElfCmdFactory.h"
#include "LxElfException.h"
#include "LxElfFile.h"
#include "BuildTxt.h"
#include "Version.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <errno.h>
#include <functional>

using namespace std;

// Visual studio does not call strtoull by its proper name
#ifdef _MSC_VER 
#define strtoull _strtoui64
#endif

namespace
{
  typedef vector<LxElfCmd*> ElfCmds;
  typedef vector<string>    Strings;

  bool silent(false);
  bool signOnPrinted(false);
}

// Exits the program with return value 1
static void
Terminate(const string & str);

std::ostream &
operator << (std::ostream & o, LxSymbolicAddress const & x)
{
  if (x.IsAbsolute())
    o << hex << "0x" << x.GetOffset() << dec;
  else
  {
    o << x.GetLabel();
    if (x.GetOffset() != 0)
      o << "+" << hex << "0x" << x.GetOffset() << dec;
  }
  return o;
}

std::ostream &
operator << (std::ostream & o, LxSymbolicRange const & x)
{
  return o << x.GetStart() << "-" << x.GetEnd();
}

namespace
{
  template<typename T>
  std::string
  ToString(T const & x)
  {
    std::ostringstream os;
    os << x;
    return os.str();
  }
}


Elf32_Addr
LxGetAddress(LxSymbolicAddress const & addr, LxElfFile const & ef)
{
  Elf32_Addr base = 0;
  if (!addr.IsAbsolute())
    base = ef.GetSymbolAddress(addr.GetLabel());
  return base + addr.GetOffset();
}

LxAddressRange
LxGetAddressRange(LxSymbolicRange const & r, LxElfFile const & ef)
{
  Elf32_Addr startAddr = LxGetAddress(r.GetStart(), ef);
  Elf32_Addr endAddr   = LxGetAddress(r.GetEnd(),   ef);
  LxAddressRange range(startAddr, endAddr);

  if (range.IsReversed())
    throw LxMessageException("Range ends before it starts: " + ToString(r));

  return range;
}

LxAddressRanges
LxGetAddressRanges(LxSymbolicRanges const & ranges, LxElfFile const & file)
{
  LxAddressRanges result;
  typedef LxSymbolicRanges::const_iterator Iter;
  for (Iter i = ranges.begin(), n = ranges.end(); i != n; ++i)
  {
    result.push_back(LxGetAddressRange(*i, file));
  }
  return result;
}

static
bool
StartsWith(std::string const & str, std::string const & sub)
{
  return str.compare(0, sub.size(), sub) == 0;
}

// Returns a string containing the arguments given after --checksum or --fill
// progArgs is the vectorized argv array
// title is "--checksum" or "--fill"
// idx is the current index in progArgs (can be incremented by this function).
static
string
GetParams(Strings const      & progArgs,
          string const       & title,
          unsigned int       & idx)
{
  string params;
  string arg = progArgs[idx];

  if (StartsWith(arg, title))
  {
    if (arg.length() == title.length() && idx < progArgs.size()-1)
      params = string(progArgs[++idx]);
    else if (arg[title.length()] == '=' && arg.length() > title.length()+1)
      params = arg.substr(title.length() + 1);
  }

  return params;
}

namespace
{
  template<typename T>
  bool
  IsUnsigned(std::string const & str, T * valp)
  {
    char * endptr;
    uint64_t val = strtoull(str.c_str(), &endptr, 0);
    if (errno == ERANGE || *endptr != '\0' || static_cast<T>(val) != val)
      return false;
    else if (valp != NULL)
      *valp = static_cast<T>(val);
    return true;
  }
}

// Returns true if ranges overlap
bool
LxRangesOverlap(const LxAddressRanges & ranges)
{
  LxAddressRanges sortedRanges( ranges.begin(), ranges.end());
  std::sort(sortedRanges.begin(), sortedRanges.end(), LxRangeSort());

  return adjacent_find(sortedRanges.begin(),
                       sortedRanges.end(),
                       mem_fun_ref(&LxAddressRange::Intersects)) != sortedRanges.end();
}

class CmdParser
{
public:
  CmdParser(const string & cmdLine)
    : mCmdLine(cmdLine)
  {
  }

  virtual ~CmdParser() {}

  virtual void Parse(LxElfCmdFactory & cmdFactory) = 0;

protected:
  Elf32_Addr ParseOffset(std::string const & str)
  {
    Elf32_Addr addr;
    if (!IsUnsigned(str, &addr))
      throw LxMessageException("Invalid address or offset: " + str);

    return addr;
  }

  LxSymbolicAddress ParseAddress(string str)
  {
    std::string label;
    Elf32_Addr offset = 0;
    if (!str.empty() && !isdigit(static_cast<unsigned char>(str[0])))
    {
      string::size_type pos = str.find('+');
      label = str.substr(0, pos);
      if (pos == string::npos)
        str = "";
      else
        str = str.substr(label.size() + 1);
    }
    if (!str.empty())
      offset = ParseOffset(str);
    return LxSymbolicAddress(label, offset);
  }

  bool ParseRanges(LxSymbolicRanges & ranges)
  {
    string::size_type pos = 0;

    while (!mCmdLine.empty())
    {
      pos = mCmdLine.find('-');
      if (pos == 0 || pos == string::npos)
        return false;

      LxSymbolicAddress startAddr = ParseAddress(mCmdLine.substr(0, pos));
      mCmdLine = mCmdLine.substr(pos + 1);
      pos = mCmdLine.find(';');

      LxSymbolicAddress endAddr = ParseAddress(mCmdLine.substr(0, pos));
      if (pos == string::npos)
        mCmdLine.clear();
      else
        mCmdLine  = mCmdLine.substr(pos + 1);

      ranges.push_back(LxSymbolicRange(startAddr, endAddr));
    }

    return true;
  }

  string Consume(const string & separators, bool consumeSeparator, char & foundSeparator)
  {
    string::size_type pos = mCmdLine.find_first_of(separators);
    string retVal;

    if (pos == string::npos || pos >= mCmdLine.length())
      throw LxMessageException("Invalid --checksum/--parity arguments");
    else
    {
      foundSeparator = mCmdLine[pos];
      if (pos != 0)
        retVal = mCmdLine.substr(0, pos);
      if (consumeSeparator)
      {
        if (mCmdLine.length() > pos + 1)
          mCmdLine = mCmdLine.substr(pos + 1);
        else
          mCmdLine = "";
      }
      else
      {
        mCmdLine = mCmdLine.substr(pos);
      }
    }
    return retVal;
  }

  string Consume(const string & separators, bool consumeSeparator)
  {
    char foundSeparator;
    return Consume(separators, consumeSeparator, foundSeparator);
  }

  char Peek()
  {
    if (mCmdLine.empty())
      return 0;
    else
      return mCmdLine[0];
  }

private:
  string mCmdLine;
};


class ChecksumCmdParser : public CmdParser
{
public:
  ChecksumCmdParser(const string & cmdLine, ElfCmds & cmds)
    : CmdParser(cmdLine),
      mCmds(cmds),
      mAlgorithm(kCrcSimple),
      mPolynomial(0),
      mComplement(kNoCompl),
      mMirror(false),
      mReverse(false),
      mStartValue(0),
      mStartValueType(kNone),
      mSymSize(0),
      mUnitSize(1),
      mRSIGN(false)
  {
  }

  virtual void Parse(LxElfCmdFactory & cmdFactory)
  {
    // Syntax is
    // "--checksum sym:size,algo[:flags][,start];range[;range...]"
    // where flags is [1|2][m][r][R][i|p][L][W]

    ParseSymbol();
    ParseSize();
    ParseAlgorithm();
    
    if (Peek() == ':')
    {
      Consume(":", true);
      ParseFlags();
    }

    if (Peek() == ',')
    {
      Consume(",", true);
      ParseStartValue();
    }

    if (Peek() != ';')
      throw LxMessageException("Invalid --checksum arguments!");
    
    Consume(";", true);

    ParseRanges();

    CreateCmd(cmdFactory);
  }

private:
  void ParseSymbol()
  {
    string token = Consume(":", true);

    mSymbolName = token;
  }

  void ParseSize()
  {
    string token = Consume(",", true);

    mSymSize = (uint8_t) strtoull(token.c_str(), NULL, 0);

    if (mSymSize != 1 && mSymSize != 2 && mSymSize != 4 && mSymSize != 8)
      throw LxMessageException("Invalid symbol size argument in --checksum!");
  }

  void ParseAlgorithm()
  {
    string token = Consume(":,;", false);

    if (token == "sum")
    {
      mAlgorithm = kCrcSimple;
    }
    else if (token == "sum8wide")
    {
      mAlgorithm = kCrcSimpleWide;
    }
    else if (token == "sum32")
    {
      mAlgorithm = kCrcSimple32;
      mUnitSize  = 4;
    }
    else if (token == "crc16")
    {
      mAlgorithm  = kCrc16;
      mPolynomial = 0x11021;
    }
    else if (token == "crc32")
    {
      mAlgorithm  = kCrc32;
      mPolynomial = 0x4C11DB7;
    }
    else if (token == "crc64iso")
    {
      mAlgorithm  = kCrc64iso;
      mPolynomial = 0x1b;
    }
    else if (token == "crc64ecma")
    {
      mAlgorithm  = kCrc64ecma;
      mPolynomial = 0x42F0E1EBA9EA3693ULL;
    }
    else if (token.substr(0,4) == "crc=")
    {
      mAlgorithm = kCrcPoly;
      mPolynomial = strtoull(token.substr(4).c_str(), NULL, 0);
    }
    else
      throw LxMessageException("Invalid checksum algorithm!");
  }

  void ParseFlags()
  {
    string token = Consume(",;", false);

    for (string::size_type i=0; i< token.length(); i++)
    {
      switch (token[i])
      {
        case '1': mComplement = k1sCompl;       break;
        case '2': mComplement = k2sCompl;       break;
        case 'm': mMirror = true;               break;
        case 'r': mReverse = true;              break;
        case 'R': mRSIGN = true;                break;
        case 'i': mStartValueType = kInitial;   break;
        case 'p': mStartValueType = kPrepended; break;
        case 'L': mUnitSize = 4;                break;
        case 'W': mUnitSize = 2;                break;
        default: 
          throw LxMessageException(string("Invalid flag: ") + token[i]);
      }
    }

    if (mUnitSize != 4 && mAlgorithm == kCrcSimple32)
      LxMessageException(string("sum32 must use a 4-byte unit size"));

    if (mRSIGN)
    {
        mReverse = !mReverse;
    }
  }

  void ParseStartValue()
  {
    string token = Consume(";", false);

    mStartValue = strtoull(token.c_str(), NULL, 0);
  }

  void ParseRanges()
  {
    if (!CmdParser::ParseRanges(mRanges))
      throw LxChecksumException(LxChecksumException::kChecksumRangeError);
  }

  void CreateCmd(LxElfCmdFactory & cmdFactory)
  {
    LxSymbolicAddress symAddr = ParseAddress(mSymbolName);

    LxElfCmd* cksumCmd = cmdFactory.CreateChecksumCmd(mSymSize,
                                                      mAlgorithm,
                                                      mComplement,
                                                      mMirror,
                                                      mReverse,
                                                      mRSIGN,
                                                      mPolynomial,
                                                      mRanges,
                                                      symAddr,
                                                      mStartValue,
                                                      mStartValueType,
                                                      mUnitSize);

    if (cksumCmd != NULL)
      mCmds.push_back(cksumCmd);
    else
      throw LxMessageException("Invalid --checksum arguments!");
  }

private:
  ElfCmds & mCmds;

  LxAlgo           mAlgorithm;
  uint64_t         mPolynomial;
  LxCompl          mComplement;
  bool             mMirror;
  bool             mReverse;
  bool             mRSIGN;
  uint64_t         mStartValue;
  StartValueType   mStartValueType;
  string           mSymbolName;
  uint8_t          mSymSize;
  uint8_t          mUnitSize;
  LxSymbolicRanges mRanges;
};

class FillCmdParser : public CmdParser
{
public:
  FillCmdParser(const string & cmdLine, ElfCmds & cmds)
    : CmdParser(cmdLine), mCmds(cmds)
  {
  }

  virtual void Parse(LxElfCmdFactory & cmdFactory)
  {
    // Syntax is
    // "--fill pattern;range[;range...]\n"

    bool is_virtual = PossiblyParseVirtuality();
    ParsePattern();
    ParseRanges();

    CreateCmds(cmdFactory, is_virtual);
  }

  void GetRanges(LxSymbolicRanges & ranges)
  {
    ranges.clear();
    copy(mRanges.begin(), mRanges.end(), back_inserter(ranges));
  }

private:
  bool PossiblyParseVirtuality()
  {
    if (Peek() == 'v')
    {
      string token = Consume(";", true);
      if (token.substr(0,1) != "v")
        throw LxFillException(LxFillException::kMissingSemicolon);
      return true;
    }
    return false;
  }


  void ParsePattern()
  {
    string token = Consume(";", true);

    if (token.substr(0,2) != "0x")
      throw LxFillException(LxFillException::kPatternMissing0x);

    token = token.substr(2);

    if (token.length()%2 != 0)
      throw LxFillException(LxFillException::kOddNumberOfPatternChars);

    unsigned int fillLength  = (unsigned int) token.length()/2;
    mPattern.clear();
    mPattern.reserve(fillLength);

    for (string::size_type i = 0; i < token.length(); i += 2)
    {
      string byteStr = token.substr(i, 2);
      mPattern.push_back((char) strtol(byteStr.c_str(), NULL, 16));
    }
  }

  void ParseRanges()
  {
    if (!CmdParser::ParseRanges(mRanges))
      throw LxFillException(LxFillException::kRangeError);
  }

  void CreateCmds(LxElfCmdFactory & cmdFactory, bool virtual_fill)
  {
    for (LxSymbolicRanges::const_iterator i = mRanges.begin(),
                                          e = mRanges.end(); i != e; ++i)
    {
      LxElfCmd* fillCmd = cmdFactory.CreateFillCmd(*i, mPattern, virtual_fill);
      if (fillCmd == NULL)
        throw LxMessageException("Invalid fill arguments!");

      mCmds.push_back(fillCmd);
    }
  }

private:
  ElfCmds &   mCmds;
  LxSymbolicRanges mRanges;
  FillPattern      mPattern;
};

class ParityCmdParser : public CmdParser
{
public:
  ParityCmdParser(const string & cmdLine, ElfCmds & cmds)
    : CmdParser(cmdLine),
      mCmds(cmds),
      mEven(false),
      mReverse(false),
      mSymSize(0),
      mUnitSize(4u)
  {
  }

  virtual void Parse(LxElfCmdFactory & cmdFactory)
  {
    // Syntax is
    // "--Parity sym:size,algo[:flags];range[;range...]"
    // where flags is [r][L][W][B]

    ParseSymbol();
    ParseSize();
    ParseAlgorithm();
    ParseFlashBase();
    
    if (Peek() == ':')
    {
      Consume(":", true);
      ParseFlags();
    }

    if (Peek() != ';')
      throw LxMessageException("Invalid --Parity arguments!");
    
    Consume(";", true);

    ParseRanges();

    CreateCmd(cmdFactory);
  }

private:
  void ParseSymbol()
  {
    string token = Consume(":", true);

    mSymbolName = token;
  }

  void ParseSize()
  {
    string token = Consume(",", true);

    mSymSize = (uint32_t)strtoull(token.c_str(), NULL, 0);
#if 0
    if (mSymSize != 1 && mSymSize != 2 && mSymSize != 4 && mSymSize != 8)
      throw LxMessageException("Invalid symbol size argument in --Parity!");
#endif
  }

  void ParseAlgorithm()
  {
    string token = Consume(":,;", false);

    if (token == "odd")
    {
      mEven = false;
    }
    else if (token == "even")
    {
      mEven = true;
    }
    else
      throw LxMessageException("Invalid Parity algorithm!");
  }

  void ParseFlashBase()
  {
    Consume(":", true);
    string token = Consume(":", false);

    mFlashBaseName = token;
  }

 void ParseFlags()
  {
    string token = Consume(",;", false);

    for (string::size_type i=0; i< token.length(); i++)
    {
      switch (token[i])
      {
        case 'r': mReverse = true;              break;
        case 'L': mUnitSize = 4; break;
        case 'W': mUnitSize = 2; break;
        case 'B': mUnitSize = 1; break;
        default: throw LxMessageException(string("Invalid flag: ") + token[i]);
      }
    }
  }

  void ParseRanges()
  {
    if (!CmdParser::ParseRanges(mRanges))
      throw LxChecksumException(LxChecksumException::kChecksumRangeError);
  }

  void CreateCmd(LxElfCmdFactory & cmdFactory)
  {
    LxSymbolicAddress symAddr = ParseAddress(mSymbolName);
    LxSymbolicAddress flashBase = ParseAddress(mFlashBaseName);

    LxElfCmd* cksumCmd = cmdFactory.CreateParityCmd(mSymSize,
                                                      mEven,
                                                      mReverse,
                                                      mRanges,
                                                      symAddr,
                                                      mUnitSize,
                                                      flashBase);

    if (cksumCmd != NULL)
      mCmds.push_back(cksumCmd);
    else
      throw LxMessageException("Invalid --Parity arguments!");
  }

private:
  ElfCmds & mCmds;

  bool             mEven;
  bool             mReverse;
  string           mSymbolName;
  string           mFlashBaseName;
  uint32_t         mSymSize;
  uint32_t         mUnitSize;
  LxSymbolicRanges mRanges;
};

static
bool
ReadOptions(const Strings      & progArgs,
            LxElfCmdFactory    & cmdFactory,
            ElfCmds            & cmds,
            std::string        * filename)
{
  const string     kChksumOpt  ("--checksum");
  const string     kFillOpt    ("--fill");
  const string     kParityOpt  ("--parity");
  const string     kStripOpt   ("--strip");
  const string     kSilentOpt  ("--silent");
  const string     kVerboseOpt ("--verbose");
  const string     kSrecOpt    ("--srec");
  const string     kSrecLen    ("--srec-len");
  const string     kS3OnlyOpt  ("--srec-s3only");
  const string     kIHexOpt    ("--ihex");
  const string     kTiTxtOpt   ("--titxt");
  const string     kSimpleOpt  ("--simple");
#ifdef _DEBUG
  const string     kSimpleNEOpt  ("--simple-ne");
#endif
  const string     kBinOpt       ("--bin");
  const string     kRelocOpt     ("--self-reloc");
  LxElfCmd*        saveCmd(NULL);
  LxElfCmd*        stripCmd(NULL);
  LxElfCmd*        relocCmd(NULL);
  ElfCmds          cksumCmds;
  ElfCmds          fillCmds;
  ElfCmds          parityCmds;
  LxSymbolicRanges fillRanges;
  string           outFilename;
  enum             OutputFormat
  {
    kElf,
    kSRec,
    kIHex,
    kTiTxt,
    kBin,
    kSimpleCode
  };
  OutputFormat     format(kElf);
  bool             s3Only(false);
  bool	           simpleEntryRec(true);
  unsigned char    sRecLen(16);
  int              posN(0);

  for (unsigned int i = 0; i < progArgs.size(); ++i)
  {
    string arg = progArgs[i];

    if (StartsWith(arg, kChksumOpt))
    {
      string chksumargs = GetParams(progArgs, kChksumOpt, i);

      ChecksumCmdParser parser(chksumargs, cksumCmds);
      parser.Parse(cmdFactory);
    }
    else if (StartsWith(arg, kFillOpt))
    {
      string fillargs = GetParams(progArgs, kFillOpt, i);
      FillCmdParser parser(fillargs, fillCmds);
      parser.Parse(cmdFactory);

      parser.GetRanges(fillRanges);
    }
    else if (StartsWith(arg, kParityOpt))
    {
      string parityargs = GetParams(progArgs, kParityOpt, i);
      ParityCmdParser parser(parityargs, parityCmds);
      parser.Parse(cmdFactory);
    }
    else if (kStripOpt == arg && stripCmd == NULL)
    {
      stripCmd = cmdFactory.CreateStripCmd();
    }
    else if (kRelocOpt == arg)
    {
      if (relocCmd != NULL)
        throw LxMessageException("More than one --self-reloc option: '" + arg + "'");
      string relocargs = GetParams(progArgs, kRelocOpt, i);
      // Handle debug flag
      bool withDebug = false;
      if (relocargs.substr(relocargs.length() - 2) == ";d")
      {
        relocargs = relocargs.substr(0, relocargs.length() - 2);
        withDebug = true;
      }
      // Handle jump table entry count
      unsigned long nJumpTableEntries = 0;
      string::size_type pos = relocargs.find(",");
      if (pos != string::npos)
      {
        string count = relocargs.substr(pos + 1);
        relocargs = relocargs.substr(0, pos);
        if (!IsUnsigned(count, &nJumpTableEntries))
        {
          throw LxMessageException("Parse error in --self-reloc jump table count: '"
                                   + count + "'");
        }
      }
      relocCmd = cmdFactory.CreateRelocCmd(relocargs,
                                           nJumpTableEntries,
                                           withDebug);
    }
    else if (kVerboseOpt == arg)
    {
      silent = false;
    }
    else if (kSilentOpt == arg)
    {
      silent = true;
    }
    else if (kS3OnlyOpt == arg)
    {
      s3Only = true;
    }
    else if (StartsWith(arg, kSrecLen))
    {
      string sRecLenArgs = GetParams(progArgs, kSrecLen, i);
      int len = atoi(sRecLenArgs.c_str());
      sRecLen = (unsigned char) (len < 1 ? 1 : (len > 255 ? 255 : len));
    }
    else if (kSrecOpt == arg)
    {
      format = kSRec;
    }
    else if (kIHexOpt == arg)
    {
      format = kIHex;
    }
    else if (kTiTxtOpt == arg)
    {
      format = kTiTxt;
    }
    else if (kSimpleOpt == arg)
    {
      format = kSimpleCode;
    }
#ifdef _DEBUG
    else if (kSimpleNEOpt == arg)
    {
      format = kSimpleCode;
      simpleEntryRec = false;
    }
#endif
    else if (kBinOpt == arg)
    {
      format = kBin;
    }
    else if (StartsWith(arg, "-"))
    {
      throw LxMessageException("Unknown option: '" + arg + "'");
    }
    else
    {
      // Positional argument
      switch (posN)
      {
      case 0:
        *filename = arg;
        break;

      case 1:
        outFilename = arg;
        break;

      default:
        throw LxMessageException("Too many filenames: '" + arg + "'");
      }
      ++posN;
    }
  }

  // Insert the commands in correct order
  if (posN == 0)
    throw LxMessageException("Input file name missing");

  if (!fillRanges.empty())
    cmds.push_back(cmdFactory.CreateFillValidateCmd(fillRanges));

  // Fill commands should be executed before checksum and parity commands,
  // and parity coommands before checksum
  copy(fillCmds.begin(),  fillCmds.end(),  back_inserter(cmds));
  copy(parityCmds.begin(), parityCmds.end(), back_inserter(cmds));
  copy(cksumCmds.begin(), cksumCmds.end(), back_inserter(cmds));

  if (relocCmd)
    cmds.push_back(relocCmd);

  // Strip and save must be performed after all other commands
  if (stripCmd)
    cmds.push_back(stripCmd);

  if (outFilename.empty())
    throw LxMessageException("Output file name missing");
  switch(format)
  {
  case kElf:
    saveCmd = cmdFactory.CreateSaveCmd(outFilename);
    break;

  case kSRec:
    saveCmd = cmdFactory.CreateSaveSRecCmd(outFilename,
                                           sRecLen,
                                           s3Only ? kS37 : kAdaptiveVariant);
    break;

  case kIHex:
    saveCmd = cmdFactory.CreateSaveIHexCmd(outFilename);
	break;

  case kTiTxt:
    saveCmd = cmdFactory.CreateSaveTiTxtCmd(outFilename);
    break;

  case kSimpleCode:
    saveCmd = cmdFactory.CreateSaveSimpleCodeCmd(outFilename, simpleEntryRec);
  break;

  case kBin:
    saveCmd = cmdFactory.CreateSaveBinCmd(outFilename);
    break;
  }

  cmds.push_back(saveCmd);

  return true;
}


static
void
PrintSignOn()
{
  if (!signOnPrinted)
  {
    std::string signon(
      "\n"
      "   " GEN_SIGNON BUILDTEXT "\n"
      "   " GEN_COPYRIGHT "\n");

    if (GEN_USE_COMMENT)
    {
      signon += "   " GEN_COMMENT "\n";
    }
    signon += "\n";

    cout << signon;

    signOnPrinted = true;
  }
}

static
void
PrintHelp()
{
  PrintSignOn();

  cout <<
  "Usage:       ielftool input_file output_file\n"
  "\n"
  "Available command line options:\n"
  "--checksum sym:size,algo[:[1|2][m][r][i|p][W|L]][,start];range[;range...]\n"
  "                Generate checksum\n"
  "                   sym       Checksum symbol\n"
  "                   size      Length of the symbol in bytes\n"
  "                   algo      Algorithm: sum, sum8wide, sum32, crc16, crc32\n"
  "                             crc64iso, crc64ecma or crc=poly\n"
  "                   1|2       Complement: 1 or 2\n"
  "                   m         Reverse the bit order within each byte\n"
  "                   r         Reverse the byte order within each word\n"
  "                   i|p       Use initial value normally: i\n"
  "                             Prefix input data with the start value: p\n" 
  "                   W|L       Use a checksum unit length of 2 bytes: W\n"
  "                             Use a checksum unit length of 4 bytes: L\n"
  "                   start     Initial checksum value (defaults to 0)\n"
  "                   range     Do checksum of bytes in range\n"
  "--fill [v;]pattern;range[;range...]\n"
  "                Specify fill\n"
  "                   v         Virtual fill, do not generate actual filler bytes.\n"
  "                             This can be used for checksums.\n"
  "                   pattern   Sequence of filler bytes\n"
  "                   range     Fill range\n"
  "--strip         Remove all section headers and non-program sections\n"
  "--self_reloc relocator[,jtc]\n"
  "                Create self-relocating image with relocator\n"
  "                   jtc       Number of jump table entries\n"
  "--srec          Save as Motorola S-records\n"
  "--srec-len length\n"
  "                Restrict the length of S-records\n"
  "--srec-s3only   Restrict the type of S-records to S3 (and S7)\n"
  "--ihex          Save as Intel hex\n"
  "--titxt         Save as TI-txt format\n"
  "--simple        Save as SimpleCode\n"
#ifdef _DEBUG
  "--simple-ne     Save as SimpleCode without entry record\n"
#endif
  "--bin           Save as raw binary\n"
  "--silent        Silent operation\n"
  "--verbose       Print all performed operations\n";
}

static
void
Terminate(const string & str)
{
  PrintSignOn();
  cerr << "ielftool error: " << str << endl;
  exit(1);
}

static
void
AddToCommentSection(LxElfFile & file, Strings const & args)
{
  // String to add to beginning of comment section
  std::string prefix;
  prefix = "IAR ielftool " BUILDTEXT;
  prefix += '\0';
  for (size_t i = 0, n = args.size(); i != n; ++i)
  {
    if (i != 0)
      prefix += ' ';
    prefix += args[i];
  }
  prefix += '\0';
  prefix += '\0';
  // Create comment section if none present
  LxElfSection * csec = NULL;
  for (Elf32_Word i = 1; i < file.GetNrOfSections(); ++i)
  {
    LxElfSection * sec = file.GetSection(i);
    std::string secName = file.GetString(sec->mHdr.sh_name,
                                         file.GetSectionLabelsIdx());
    if (sec->IsProgBits() && secName == ".comment")
    {
      csec = sec;
      break;
    }
  }
  if (csec == NULL)
  {
    csec = file.AddSection(".comment",
                           SHT_PROGBITS,
                           /*flags=*/0,
                           /*addr=*/0,
                           /*size=*/prefix.size(),
                           /*link=*/0,
                           /*info=*/0,
                           /*align=*/0,
                           /*entSize=*/0,
                           file.GetElfHeader().e_shoff);
  }
  else
  {
    file.ExpandSection(csec, prefix.size());
  }

  LxElfDataBuffer old(csec->mData);
  old.Shrink(prefix.size());
  LxElfWriter wi(csec->mData);
  for (size_t ci = 0; ci != prefix.size(); ++ci)
  {
    wi.PutByte(prefix[ci]);
  }
  wi.Put(old);
}

int
main(int argc, char* argv[])
{
  setlocale(LC_CTYPE, "");

  bool success(false);

  try
  {
    if (argc == 1)
    {
      // No args => print help text
      PrintHelp();
      return 0;
    }

    LxElfCmdFactory cmdFactory;
    ElfCmds         cmds;
    std::string     filename;
    // Put the program arguments in a string vector
    Strings         progArgs(&argv[1], &argv[argc]);

    // Parse the program arguments and create the vector of commands
    success = ReadOptions(progArgs,
                          cmdFactory,
                          cmds,
                          &filename);

    if (!silent)
      PrintSignOn();

    // Execute all commands
    if (success)
    {
      if (!silent)
        cout << "Loading " << filename << endl;

      LxElfFile infile(filename);
      AddToCommentSection(infile, progArgs);
      typedef ElfCmds::const_iterator CIter;
      for (CIter i = cmds.begin(), n = cmds.end(); i != n; ++i)
      {
        (*i)->Execute(infile, !silent);
        delete *i;
      }
    }
  }
  catch (std::bad_alloc const &)
  {
    Terminate("Out of memory");  
  }
  catch (std::exception const & exc)
  {
    Terminate(exc.what());
  }
  catch (const LxException & error)
  {
    Terminate(error.GetMessage());
  }
  catch (...)
  {
    Terminate("Unexpected exception");  
  }

  return (success ? 0 : 1);
}
