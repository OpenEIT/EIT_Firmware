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

// Class that implements the command for calculating a checksum

#include "LxElfChecksumCmd.h"
#include "LxElfParityCmd.h"
#include "LxElfException.h"
#include "LxElfFile.h"
#include "LxMain.h"
#include <algorithm>
#include <functional>

#include <memory>
#include <stdexcept>
#include <sstream>

#define ELEMENTS(x) sizeof(x)/sizeof(x[0])

using namespace std;

ChecksumLog LxElfChecksumCmd::mLog;
ChecksumLog LxElfParityCmd::mLog;

namespace
{
  static
  uint64_t
  mirror(uint64_t orig, uint8_t size)
  {
    uint64_t one = 1, d, r = 0;
    for (d = 0; d < (8 * size); d++)
    {
      if (orig & (one << d))
        r |= ((one << (8 * size - 1)) >> d);
    }
    return r;
  }

  class Mirror
  {
  public:
    Mirror()
    {
      for (int index = 0; index < 256; index++)
        mMirrorByte[index] = (uint8_t)mirror((uint8_t)index, 1);
    }

    uint8_t operator [] (uint32_t index)
    {
      return mMirrorByte[index];
    }

  private:
    uint8_t mMirrorByte[256];
  };


  struct AlgorithmSettings
  {
    AlgorithmSettings(uint64_t       polynomial,
                      uint64_t       startValue,
                      StartValueType startValueType,
                      LxCompl        complement,
                      uint8_t        unitSize,
                      bool           mirror,
                      bool           reverse,
                      uint8_t        symbolSize,
                      bool           rSign = false,
                      bool           even = true,
                      uint32_t       flashBase = 0u,
					  uint32_t       forceAlign = 1u)
      : mPolynomial(polynomial),
        mStartValue(startValue),
        mStartValueType(startValueType),
        mComplement(complement),
        mMirror(mirror),
        mReverse(reverse),
        mUnitSize(unitSize),
        mIndex(0),
        mSymbolSize(symbolSize),
        mRSIGN(rSign),
        mEven(even),
        mFlashBase(flashBase),
		mForceAlign(forceAlign)
    {
    }

    uint64_t       mPolynomial;
    uint64_t       mStartValue;
    StartValueType mStartValueType;
    LxCompl        mComplement;
    uint8_t        mUnitSize;
    uint8_t        mIndex;
    bool           mMirror;
    bool           mReverse;
    uint8_t        mSymbolSize;
    bool           mRSIGN;
    bool           mEven;
    uint32_t       mFlashBase;
	uint32_t       mForceAlign;
  };

  class Algorithm : public LxByteVisitor
  {
  public:

    Algorithm(const AlgorithmSettings & settings);
    virtual ~Algorithm();

    void Calculate(LxAddressRanges const & ranges,
                   LxByteVisitor* pVisitor,
                   LxElfFile & file);

    virtual uint64_t GetSum() const {return mSum;}

  protected:
    virtual void Initialize() = 0;
    virtual void Finalize() = 0;

    virtual void ComplementAndMirror(uint8_t len);

  protected:
    const AlgorithmSettings mSettings;

    uint64_t mSum;
    bool     mBigEndian;
  };

  class CRCAlgo : public Algorithm
  {
  public:
    CRCAlgo(const AlgorithmSettings & settings);

  protected:
    void CalcCRCTable(int size);
    uint8_t PushByte(uint8_t);
    uint8_t PopByte(void);

  protected:
    uint64_t mCRCTable[256];
  private:
    uint8_t  mBuffer[4];
    uint8_t  mIndex;
  };

  class CRCSize1Algo : public CRCAlgo
  {
  public:
    CRCSize1Algo(const AlgorithmSettings & settings);

    virtual void VisitByte(uint8_t b);

  protected:
    virtual void Initialize();
    virtual void Finalize();
  };

  class CRCSize2Algo : public CRCAlgo
  {
  public:
    CRCSize2Algo(const AlgorithmSettings & settings);

    virtual void VisitByte(uint8_t b);

  protected:
    virtual void Initialize();
    virtual void Finalize();
  };

  class CRCSize4Algo : public CRCAlgo
  {
  public:
    CRCSize4Algo(const AlgorithmSettings & settings);

    virtual void VisitByte(uint8_t b);

  protected:
    virtual void Initialize();
    virtual void Finalize();
  };

  class CRCSize8Algo : public CRCAlgo
  {
  public:
    CRCSize8Algo(const AlgorithmSettings & settings);

    virtual void VisitByte(uint8_t b);

  private:
    virtual void Initialize();
    virtual void Finalize();
  };


  class SumWideAlgo : public Algorithm
  {
  public:
    SumWideAlgo(const AlgorithmSettings & settings);

    virtual void VisitByte(uint8_t b);

  protected:
    virtual void Initialize();
    virtual void Finalize();
  };

  class SumTruncAlgo : public SumWideAlgo
  {
  public:
    SumTruncAlgo(const AlgorithmSettings & settings);

  protected:
    virtual void Finalize();
  };


  class Sum32Algo : public Algorithm
  {
  public:
    Sum32Algo(const AlgorithmSettings & settings);

    virtual void VisitByte(uint8_t b);

  protected:
    virtual void Initialize();
    virtual void Finalize();

  private:
    uint32_t mVal;
    uint8_t  mShift;
  };
  

class ParityAlgorithm : public Algorithm
  {
  public:
    ParityAlgorithm(uint32_t size, const AlgorithmSettings & settings)
    : Algorithm(settings), mParityWords(size, 0xFFFFFFFF)
  {
  }

    std::vector<uint32_t> GetParityWords () const { return mParityWords; }

  protected:
    uint8_t PushByte(uint8_t);
    uint8_t PopByte(void);

  protected:
    virtual void SetVisitRange(const LxAddressRange & currentRange, bool reverse);
    virtual void VisitByte(uint8_t b);
    virtual void Initialize()
    {
    }
    virtual void Finalize()
    {
    }

  private:
    uint8_t  mBuffer[4];
    uint32_t mByteIndex;
    std::vector<uint32_t> mParityWords;
    uint32_t mWordIndex;
    uint32_t mBitIndex;
  };

void ParityAlgorithm::SetVisitRange(const LxAddressRange & range, bool reverse)
{
    if (reverse)
    {
        // reverse == true is associated with checksums only, and is invalid for parity.
        throw LxMessageException("Internal error, parity ranges must be traversed in forward direction.");
    }

    if (0 != (range.GetStart() % mSettings.mUnitSize))
    {
        // not aligned
        ostringstream os;
        os << "The parity range start address 0x"
		   << hex << range.GetStart()
		   << " must be aligned on a "
           << (int)(mSettings.mUnitSize)
           << " byte boundary.";
        throw LxMessageException(os.str());
    }


    uint32_t wordNum = (range.GetStart() - mSettings.mFlashBase) / mSettings.mUnitSize;

    // Initialise the byte buffer
    mBuffer[0] = 0;
    mBuffer[1] = 0;
    mBuffer[2] = 0;
    mBuffer[3] = 0;
    mByteIndex = 0;
    mWordIndex = wordNum / 32;
    mBitIndex  = wordNum % 32;
}

void ParityAlgorithm::VisitByte(uint8_t b)
{
    mBuffer[mByteIndex++] = b;

    if (mByteIndex >= mSettings.mUnitSize)
    {
        /* Process word into a parity bit. The bit and byte orders within the word do not
         * matter to the result, and neither do unused bytes in mBuffer (i.e. for byte or
         * halfword parity) as they are set to zero.
         */
        uint32_t dataWord = (mBuffer[0] | (mBuffer[1] << 8) | (mBuffer[2] << 16) | (mBuffer[3] << 24));
        uint32_t parityBit = mSettings.mEven; // 1 (true) for even parity, 0 (false) for odd parity
           
        mBuffer[0] = 0;
        mBuffer[1] = 0;
        mBuffer[2] = 0;
        mBuffer[3] = 0;
        mByteIndex = 0;

       /* For every set bit in dataWord, toggle the parity bit */
        while(0u != dataWord)
        {
            parityBit ^= (1u & dataWord); /* no-op if bottom bit is zero */
            dataWord >>= 1;               /* shift the next bit down */
        }
            
        /* Now use the parity bit to toggle the appropriate bit in the parity word.
        * The parity word starts out as all-1s (so that unchanged bits have no effect
        * when written) and gets toggled to zero if parityBit is 1. This is why
        * parityBit is initialised to 1 for even parity, above. If there are an even
        * number of set bits in the dataWord then parityBit will still be 1 and will
        * toggle the bit in nParityWord to zero, i.e. the hardware expects *even* parity.
        */
        if (mSettings.mReverse)
            mParityWords[mWordIndex] ^= (parityBit << (31u - mBitIndex));
        else
            mParityWords[mWordIndex] ^= (parityBit << mBitIndex);

        ++mBitIndex;

        if (mBitIndex > 31)
        {
            uint32_t nParityWord = mParityWords[mWordIndex];
            mBitIndex = 0;
            ++mWordIndex;
        }
    }
}

Algorithm::
  Algorithm(const AlgorithmSettings & settings)
    : mSum(0), mSettings(settings)
  {
  }

  Algorithm::
  ~Algorithm()
  {
  }

  void Algorithm::
  Calculate(LxAddressRanges const & ranges,
            LxByteVisitor* pVisitor,
            LxElfFile & file)
  {
    mBigEndian = file.IsBigEndian();

    // Initialize algorithm
    Initialize();

    // Process all data
    file.VisitSegmentRanges(*pVisitor, ranges, mSettings.mRSIGN, mSettings.mForceAlign);

    // Do final processing of the checksum value
    Finalize();
  }

  void Algorithm::
  ComplementAndMirror(uint8_t len)
  {
    switch (mSettings.mComplement)
    {
    case k1sCompl: mSum = ~mSum;     break;
    case k2sCompl: mSum = ~mSum + 1; break;
    default: /* Do nothing */        break;
    }

    if (mSettings.mMirror)
      mSum = mirror(mSum, len);
  }

  /** CRCAlgo ***************************************************************/
  CRCAlgo::
  CRCAlgo(const AlgorithmSettings & settings)
    : Algorithm(settings), mIndex(settings.mIndex)
  {
  }

  void CRCAlgo::
  CalcCRCTable(int size)
  {
    uint64_t index = 0, mask = 1;
    mask <<= (size * 8 - 1);
    int left = (size - 1) * 8;

    for (index = 0; index < ELEMENTS(mCRCTable); ++index)
    {
      uint64_t r = index << left;
      for (int i = 0; i < 8; ++i)
      {
        if (r & mask)
          r = (r << 1) ^ mSettings.mPolynomial;
        else
          r <<= 1;
      }
      mCRCTable[index] = r;
    }
  }

  uint8_t CRCAlgo::
  PushByte(uint8_t byte)
  {
    mBuffer[mIndex++] = byte;
    return mIndex;
  }

  uint8_t CRCAlgo::
  PopByte(void)
  {
    return mBuffer[--mIndex];
  }

  /** CRCSize1Algo **********************************************************/
  CRCSize1Algo::
  CRCSize1Algo(const AlgorithmSettings & settings)
    : CRCAlgo(settings)
  {
  }

  void CRCSize1Algo::
  Initialize()
  {
    CalcCRCTable(1);

    if (mSettings.mStartValueType == kPrepended)
    {
      VisitByte(static_cast<uint8_t>(mSettings.mStartValue & 0xFF));
    }
    else
    {
      mSum = static_cast<uint8_t>(mSettings.mStartValue);
    }
  }

  void CRCSize1Algo::
  VisitByte(uint8_t b)
  {
    if (mSettings.mUnitSize == 1)
    {
      uint8_t i = static_cast<uint8_t>(mSum ^ b);
      mSum = mCRCTable[i];
    }
    else
    {
      uint8_t storedBytes = PushByte(b);
      if (storedBytes == mSettings.mUnitSize)
      {
        while (storedBytes--)
        {
          uint8_t i = static_cast<uint8_t>(mSum ^ PopByte());
          mSum = mCRCTable[i];
        }
      }
    }
  }

  void CRCSize1Algo::
  Finalize()
  {
    ComplementAndMirror(1);
    mSum &= 0x00FF;
  }


  /** CRCSize2Algo **********************************************************/
  CRCSize2Algo::
  CRCSize2Algo(const AlgorithmSettings & settings)
  : CRCAlgo(settings)
  {
  }

  void CRCSize2Algo::
  Initialize()
  {
    CalcCRCTable(2);

    if (mSettings.mStartValueType == kPrepended)
    {
      VisitByte(static_cast<uint8_t>((mSettings.mStartValue & 0xFF00) >> 8));
      VisitByte(static_cast<uint8_t>(mSettings.mStartValue & 0xFF));
    }
    else
    {
      mSum = static_cast<uint16_t>(mSettings.mStartValue);
    }
  }

  void CRCSize2Algo::
  VisitByte(uint8_t b)
  {
    if (mSettings.mUnitSize == 1)
    {
      uint8_t i = static_cast<uint8_t>(mSum >> 8);
      mSum = mCRCTable[i ^ b] ^ (mSum << 8);
    }
    else
    {
      uint8_t storedBytes = PushByte(b);
      if (storedBytes == mSettings.mUnitSize)
      {
        while (storedBytes--)
        {
          uint8_t i = static_cast<uint8_t>(mSum >> 8);
          mSum = mCRCTable[i ^ PopByte()] ^ (mSum << 8);
        }
      }

    }
  }

  void CRCSize2Algo::
  Finalize()
  {
    ComplementAndMirror(2);
    mSum &= 0xFFFF;
  }

  /** CRCSize4Algo **********************************************************/
  CRCSize4Algo::
  CRCSize4Algo(const AlgorithmSettings & settings)
    : CRCAlgo(settings)
  {
  }

  void CRCSize4Algo::
  Initialize()
  {
    CalcCRCTable(4);

    if (mSettings.mStartValueType == kPrepended)
    {
      uint32_t data = static_cast<uint32_t>(mSettings.mStartValue);

      for (uint32_t i = 0, mask = 0xFF000000, shift = 24 ; 
           i < 4 ; 
           ++i, mask >>= 8, shift -= 8)
      {
        VisitByte((data & mask) >> shift);
      }
    }
    else // kInitial or kNone
    {
      mSum = static_cast<uint32_t>(mSettings.mStartValue);
    }
  }

  void CRCSize4Algo::
  VisitByte(uint8_t b)
  {
    if (mSettings.mUnitSize == 1)
    {
      uint8_t i = static_cast<uint8_t>(mSum >> 24);
      mSum = mCRCTable[i ^ b] ^ (mSum << 8);
    }
    else
    {
      uint8_t storedBytes = PushByte(b);
      if (storedBytes == mSettings.mUnitSize)
      {
        while (storedBytes--)
        {
          uint8_t i = static_cast<uint8_t>(mSum >> 24);
          mSum = mCRCTable[i ^ PopByte()] ^ (mSum << 8);
        }
      }      
    }
  }

  void CRCSize4Algo::
  Finalize()
  {    
    ComplementAndMirror(4);
    mSum &= 0xFFFFFFFF;
  }


  /** CRCSize8Algo **********************************************************/
  CRCSize8Algo::
  CRCSize8Algo(const AlgorithmSettings & settings)
    : CRCAlgo(settings)
  {
  }

  void CRCSize8Algo::
  Initialize()
  {
    CalcCRCTable(8);

    if (mSettings.mStartValueType == kPrepended)
    {
      uint32_t data = mSettings.mStartValue >> 32;
      uint32_t lo   = mSettings.mStartValue & 0xFFFFFFFF;

      for (uint32_t i = 0; i < 2 ; ++i)
      {
        for (uint32_t j = 0, mask = 0xFF000000, shift = 24 ; 
             j < 4 ; 
             ++j, mask >>= 8, shift -= 8)
        {
          VisitByte((data & mask) >> shift);
        }
        data = lo;
      }
    }
    else // kInitial or kNone
    {
      mSum = mSettings.mStartValue;
    }
  }

  void CRCSize8Algo::
  VisitByte(uint8_t b)
  {
    if (mSettings.mUnitSize == 1)
    {
      uint8_t i = mSum >> 56;
      mSum = mCRCTable[i ^ b] ^ (mSum << 8);
    }
    else
    {
      uint8_t storedBytes = PushByte(b);
      if (storedBytes == mSettings.mUnitSize)
      {
        while (storedBytes--)
        {
          uint8_t i =  mSum >> 56;
          mSum = mCRCTable[i ^ PopByte()] ^ (mSum << 8);
        }
      }

    }
  }

  void CRCSize8Algo::
  Finalize()
  {
    ComplementAndMirror(8);
  }

  /** SumWideAlgo ***********************************************************/
  SumWideAlgo::
  SumWideAlgo(const AlgorithmSettings & settings)
    : Algorithm(settings)
  {
  }

  void SumWideAlgo::
  Initialize()
  {
    mSum = mSettings.mStartValue;
  }

  void SumWideAlgo::
  VisitByte(uint8_t b)
  {
    mSum += b;
  }

  void SumWideAlgo::
  Finalize()
  {
    ComplementAndMirror(mSettings.mSymbolSize);
    mSum &= 0xFFFFFFFF;
  }

  /** SumTruncAlgo **********************************************************/
  SumTruncAlgo::
  SumTruncAlgo(const AlgorithmSettings & settings)
    : SumWideAlgo(settings)
  {
  }

  void SumTruncAlgo::
  Finalize()
  {
    ComplementAndMirror(1);
    mSum &= 0x00FF;
  }


  /** Sum32Algo *************************************************************/
  Sum32Algo::
  Sum32Algo(const AlgorithmSettings & settings)
    : Algorithm(settings), mVal(0), mShift(0)
  {
  }

  void Sum32Algo::
  Initialize()
  {
    mVal   = 0;
    mShift = 0;
    mSum   = 0;
  }

  void Sum32Algo::
  VisitByte(uint8_t b)
  {
    if (mBigEndian)
      mVal |= (((uint32_t) b) << (24 - mShift));
    else
      mVal |= (((uint32_t) b) << mShift);
    mShift += 8;

    if (mShift > 24)
    {
      mSum  += mVal;
      mShift = 0;
      mVal   = 0;
    }
  }

  void Sum32Algo::
  Finalize()
  {
    if (mShift != 0)
      cout << "ielftool warning: " << (mShift/8) << " bytes skipped in calculation\n";

    ComplementAndMirror(4);
    mSum &= 0xFFFFFFFF;
  }


  static
  LxByteVisitor*
  createByteVisitor(uint8_t size,
                    Algorithm & a,
                    const AlgorithmSettings & settings);


  static
  Algorithm*
  createCRCAlgorithm(LxAlgo         algorithm,
                     uint8_t        size,
                     const AlgorithmSettings & settings)
  {
    switch (algorithm)
    {
    case kCrc16:
    case kCrc32:
    case kCrc64iso:
    case kCrc64ecma:
    case kCrcPoly:
      switch (size)
      {
      case 1: return new CRCSize1Algo(settings);
      case 2: return new CRCSize2Algo(settings);
      case 4: return new CRCSize4Algo(settings);
      case 8: return new CRCSize8Algo(settings);
      }
      break;

    case kCrcSimple:
      return new SumTruncAlgo(settings);

    case kCrcSimpleWide:
      return new SumWideAlgo(settings);

    case kCrcSimple32:
      return new Sum32Algo(settings);
    }

    return NULL;
  }
}


static
  ParityAlgorithm*
  createParityAlgorithm(uint32_t        size,
                     const AlgorithmSettings & settings)
{
    return new ParityAlgorithm(size, settings);
}



// LxElfChecksumCmd //////////////////////////////////////////////////////
LxElfChecksumCmd::
LxElfChecksumCmd(uint8_t                   symSize,
                 LxAlgo                    algorithm,
                 LxCompl                   complement,
                 bool                      mirror,
                 bool                      reverse,
                 bool                      rSign,
                 uint64_t                  polynomial,
                 LxSymbolicRanges const &  ranges,
                 LxSymbolicAddress const & symbol,
                 uint64_t                  startValue,
                 StartValueType            startValueType,
                 uint8_t                   unitSize)
 : mSize(symSize),
   mUnitSize(unitSize),
   mAlgorithm(algorithm),
   mComplement(complement),
   mMirror(mirror),
   mReverse(reverse),
   mRSIGN(rSign),
   mPolynomial(polynomial),
   mRanges(ranges),
   mSymbol(symbol),
   mStartValue(startValue),
   mStartValueType(startValueType)
{
}


void LxElfChecksumCmd::
Execute(LxElfFile & file, bool verbose)
{
  mVerbose = verbose;

  CalcAndStoreCRC(file);
}


void LxElfChecksumCmd::
RemoveChecksumFromRanges(LxAddressRanges & ranges, Elf32_Addr symAddr)
{
  // Exclude the checksum symbol area
  typedef LxAddressRanges::iterator Iter;
  Iter i = find_if<Iter>(ranges.begin(),
                         ranges.end(),
                         bind2nd(mem_fun_ref(&LxAddressRange::ContainsAddress),
                                             symAddr));

  // TODO: Checksum could be covered by several ranges!
  if (i != ranges.end())
  {
    if (i->GetStart() == symAddr)
    {
      // If overlap at start, just move the range start forward
      i->SetStart(symAddr + mSize);
    }
    else
    {
      // We might need to split the range into two.
      Elf32_Addr endAddr = i->GetEnd();
      i->SetEnd(symAddr - 1);
      if (endAddr >= symAddr + mSize)
        ranges.insert(i + 1, LxAddressRange(symAddr + mSize, endAddr));
    }
  }
}

namespace
{
  Elf32_Word GetRangesLength(const LxAddressRanges & ranges)
  {
    Elf32_Word length = 0;
    for (LxAddressRanges::const_iterator i = ranges.begin(), e = ranges.end();
         i != e; ++i)
    {
      length += i->GetLength();
    }
    return length;
  }
}

uint64_t LxElfChecksumCmd::
CalcChecksum(LxAddressRanges const & ranges,
             LxElfFile & file)
{
  AlgorithmSettings settings(mPolynomial,
                             mStartValue,
                             mStartValueType,
                             mComplement,
                             mUnitSize,
                             mMirror,
                             mReverse,
                             mSize,
                             mRSIGN);

  Algorithm* pAlgorithm = createCRCAlgorithm(mAlgorithm,
                                             mSize,
                                             settings);

  // The length of the ranges must be a multiple of the unit size
  if (GetRangesLength(ranges) % mUnitSize != 0)
  {
    ostringstream os;
    os << "The checksum range must be divisable by "
       << (int) (mUnitSize)
       << " in order to match the checksum unit size.";
    throw LxMessageException(os.str());
  }


  LxByteVisitor* pVisitor = createByteVisitor(mSize,
                                              *pAlgorithm,
                                              settings);

  pAlgorithm->Calculate(ranges, pVisitor, file);

  uint64_t sum = pAlgorithm->GetSum();

  if (pVisitor != pAlgorithm)
    delete pVisitor;
  delete pAlgorithm;
  
  return sum;
}


Elf32_Sym LxElfChecksumCmd::
FindSymbol(LxElfFile const & file) const
{
  // Find the string in the string table
  LxElfSection* symScn = file.GetSymbolSection();

  Elf32_Word strIndex = file.GetStringIndex(mSymbol.GetLabel(),
                                            symScn->mHdr.sh_link);

  // Use the string index to find the symbol in the symbol table
  try
  {
    pair<Elf32_Sym, Elf32_Word> sw = file.GetSymbol(strIndex);

    //if (sym.st_size > 4 || sym.st_size < 1)
      //return kWrongSymbolSize;

    // Symbol must be global
    if ((sw.first.st_info >> 4) != STB_GLOBAL)
    {
      throw LxChecksumException(LxChecksumException::kWrongSymbolType);
    }

    return sw.first;

  }
  catch(...)
  {
    throw LxSymbolException(mSymbol.GetLabel(),
                            LxSymbolException::kSymbolNotFound);
  }
}

// Stores the crc value for the given symbolName
void LxElfChecksumCmd::
CalcAndStoreCRC(LxElfFile & file)
{
  Elf32_Addr symAddr(0);

  // Get absolute address ranges
  LxAddressRanges ranges(LxGetAddressRanges(mRanges, file));

  // Check that the ranges do not overlap
  if (LxRangesOverlap(ranges))
  {
    throw LxChecksumException(LxChecksumException::kChecksumOverlapError);
  }

  // Check if the ranges include a checksum from another cmd
  if (mLog.RangesIncludeOldChecksum(ranges))
  {
    throw LxChecksumException(LxChecksumException::kChecksumOverlapError);
  }

  if (!mSymbol.IsAbsolute())
  {
    // We have been given a symbol for the checksum
    // Find the symbol in the symbol table
    Elf32_Sym sym = FindSymbol(file);

    symAddr = sym.st_value;
  }

  symAddr += mSymbol.GetOffset();

  // Find the section that contains the data for the symbol
  LxElfSection* section = file.GetSectionAtAddr(symAddr);
  if (!section)
  {
    throw LxChecksumException(LxChecksumException::kNoSectionForAddress);
  }

  // Check that the address is contained in a progbits section
  if (!section->IsProgBits())
  {
    throw LxChecksumException(LxChecksumException::kWrongSymbolType);
  }

  LxAddressRange symbolRange(symAddr, symAddr + mSize - 1);

  // Make sure that the checksum fits in the section
  if (!section->GetRange().Contains(symbolRange))
  {
    throw LxChecksumException(LxChecksumException::kSymbolInSeveralSections);
  }

  // Check that the address is not included in other checksum ranges
  if (mLog.ChecksumIsIncludedInOldRanges(symbolRange))
  {
    throw LxChecksumException(LxChecksumException::kChecksumOverlapError);
  }

  // Exclude the checksum symbol area from the ranges
  RemoveChecksumFromRanges(ranges, symAddr);

  // Store this in the cmd parameter history
  mLog.AddSym(symbolRange);
  mLog.AddRanges(ranges);

  // Calculate the checksum
  uint64_t sum = CalcChecksum(ranges, file);

  if (mVerbose)
  {
    cout << "Calculated the checksum for ";
    cout << hex << "0x"<< symAddr;

    if (!mSymbol.IsAbsolute())
    {
      cout << " (" << mSymbol.GetLabel();
      if (mSymbol.GetOffset() != 0)
        cout << " + " << mSymbol.GetOffset();
      cout << ")";
    }

    cout << ": " << hex << "0x" << sum << endl;
  }

  // Store the checksum value
  StoreChecksum(symAddr - section->mHdr.sh_addr, section, sum);

  if (!mSymbol.IsAbsolute())
  {
    AddDebugSymbol(mSymbol.GetLabel() + "_value", sum, file);
  }
}


void LxElfChecksumCmd::
StoreChecksum(Elf32_Off scnOffset,
              LxElfSection* scn,
              uint64_t sum)
{
  // Store the checksum value
  LxElfWriter wi(scn->mData, scnOffset);
  switch (mSize)
  {
  case 8:  wi.PutDoubleWord(sum);                    break;
  case 4:  wi.PutWord(static_cast<Elf32_Word>(sum)); break;
  case 2:  wi.PutHalf(static_cast<Elf32_Half>(sum)); break;
  case 1:  wi.PutByte(static_cast<uint8_t>(sum));    break;
  default: throw LxChecksumException(LxChecksumException::kWrongSymbolSize);
  }
}

void LxElfChecksumCmd::
AddDebugSymbol(string const & symbolName,
               uint64_t value,
               LxElfFile & file)
{
  // There are two cases here, a 32 (or fewer) bit checksum or a
  // checksum with more than 32 bits. The 33+ bit checksum will be
  // output as two symbols, one containing the 32 least significant
  // bits (named as a normal checksum symbol) and one (named as the
  // original symbol with the suffix "_high") containing the most
  // significant bits

  // Add debug string with suffix "_value"
  //string dbgStr = mSymbolName + "_value";
  LxElfSection* symScn = file.GetSymbolSection();

  bool extraSymbol = value > 0xFFFFFFFF;

  Elf32_Word strIdx = file.AddStringToStringTable(symbolName,
                                                  symScn->mHdr.sh_link);
  Elf32_Word strIdx2 = 0;
  if (extraSymbol)
    strIdx2 = file.AddStringToStringTable(symbolName + "_high",
                                          symScn->mHdr.sh_link);

  // Add a symbol for the value
  Elf32_Sym dbgSym;
  dbgSym.st_name  = strIdx;
  dbgSym.st_value = static_cast<Elf32_Word>(value);
  dbgSym.st_size  = 0;
  dbgSym.st_info  = (STB_GLOBAL<<4) | (STT_OBJECT);
  dbgSym.st_other = 0;
  dbgSym.st_shndx = SHN_ABS;

  file.AddSymbol(dbgSym);

  if (extraSymbol)
  {
    Elf32_Sym dbgSym2;
    dbgSym2.st_name  = strIdx2;
    dbgSym2.st_value = value >> 32;
    dbgSym2.st_size  = 0;
    dbgSym2.st_info  = (STB_GLOBAL<<4) | (STT_OBJECT);
    dbgSym2.st_other = 0;
    dbgSym2.st_shndx = SHN_ABS;

    file.AddSymbol(dbgSym2);
  }
}



// LxElfParityCmd //////////////////////////////////////////////////////
LxElfParityCmd::
LxElfParityCmd(uint32_t                   symSize,
                 bool                      even,
                 bool                      reverse,
                 LxSymbolicRanges const &  ranges,
                 LxSymbolicAddress const & symbol,
                 uint32_t                  unitSize,
                 LxSymbolicAddress const & flashBase)
 : mSize(symSize),
   mUnitSize(unitSize),
   mEven(even),
   mReverse(reverse),
   mRanges(ranges),
   mSymbol(symbol),
   mFlashBase(flashBase)
{
}


void LxElfParityCmd::
Execute(LxElfFile & file, bool verbose)
{
  mVerbose = verbose;

  CalcAndStoreParity(file);
}


void LxElfParityCmd::
RemoveParityFromRanges(LxAddressRanges & ranges, Elf32_Addr symAddr)
{
  // Exclude the Parity symbol area
  typedef LxAddressRanges::iterator Iter;
  Iter i = find_if<Iter>(ranges.begin(),
                         ranges.end(),
                         bind2nd(mem_fun_ref(&LxAddressRange::ContainsAddress),
                                             symAddr));

  // TODO: Parity could be covered by several ranges!
  if (i != ranges.end())
  {
    if (i->GetStart() == symAddr)
    {
      // If overlap at start, just move the range start forward
      i->SetStart(symAddr + mSize);
    }
    else
    {
      // We might need to split the range into two.
      Elf32_Addr endAddr = i->GetEnd();
      i->SetEnd(symAddr - 1);
      if (endAddr >= symAddr + mSize)
        ranges.insert(i + 1, LxAddressRange(symAddr + mSize, endAddr));
    }
  }
}

std::vector<uint32_t> LxElfParityCmd::
CalcParity(LxAddressRanges const & ranges,
             LxElfFile & file)
{
  AlgorithmSettings settings(0u,
                             0u,
                             kNone,
                             kNoCompl,
                             mUnitSize,
                             false,
                             mReverse,
                             mSize,
                             false,
                             mEven,
                             LxGetAddress(mFlashBase, file),
							 4u);

  ParityAlgorithm* pAlgorithm = createParityAlgorithm(mSize,
                                             settings);

  // The length of the ranges must be a multiple of the input size for the
  // Parity algorithm.
  if (GetRangesLength(ranges) % mUnitSize != 0)
  {
    ostringstream os;
    os << "The Parity range must be divisable by "
       << (int) (mUnitSize)
       << " in order to match the Parity unit size.";
    throw LxMessageException(os.str());
  }


  LxByteVisitor* pVisitor = createByteVisitor(mSize,
                                              *pAlgorithm,
                                              settings);

  pAlgorithm->Calculate(ranges, pVisitor, file);

  std::vector<uint32_t> parityWords = pAlgorithm->GetParityWords();

  if (pVisitor != pAlgorithm)
    delete pVisitor;
  delete pAlgorithm;
  
  return parityWords;
}


Elf32_Sym LxElfParityCmd::
FindSymbol(LxElfFile const & file) const
{
  // Find the string in the string table
  LxElfSection* symScn = file.GetSymbolSection();

  Elf32_Word strIndex = file.GetStringIndex(mSymbol.GetLabel(),
                                            symScn->mHdr.sh_link);

  // Use the string index to find the symbol in the symbol table
  try
  {
    pair<Elf32_Sym, Elf32_Word> sw = file.GetSymbol(strIndex);

    // Symbol must be global
    if ((sw.first.st_info >> 4) != STB_GLOBAL)
    {
      throw LxChecksumException(LxChecksumException::kWrongSymbolType);
    }

    return sw.first;

  }
  catch(...)
  {
    throw LxSymbolException(mSymbol.GetLabel(),
                            LxSymbolException::kSymbolNotFound);
  }
}

// Stores the parity value for the given symbolName
void LxElfParityCmd::
CalcAndStoreParity(LxElfFile & file)
{
  Elf32_Addr symAddr(0);

  // Get absolute address ranges
  LxAddressRanges ranges(LxGetAddressRanges(mRanges, file));

  // Check that the ranges do not overlap
  if (LxRangesOverlap(ranges))
  {
    throw LxChecksumException(LxChecksumException::kChecksumOverlapError);
  }

  // Check if the ranges include a checksum from another cmd
  if (mLog.RangesIncludeOldChecksum(ranges))
  {
    throw LxChecksumException(LxChecksumException::kChecksumOverlapError);
  }

  if (!mSymbol.IsAbsolute())
  {
    // We have been given a symbol for the checksum
    // Find the symbol in the symbol table
    Elf32_Sym sym = FindSymbol(file);

    symAddr = sym.st_value;
  }

  symAddr += mSymbol.GetOffset();

  // Find the section that contains the data for the symbol
  LxElfSection* section = file.GetSectionAtAddr(symAddr);
  if (!section)
  {
    throw LxChecksumException(LxChecksumException::kNoSectionForAddress);
  }

  // Check that the address is contained in a progbits section
  if (!section->IsProgBits())
  {
    throw LxChecksumException(LxChecksumException::kWrongSymbolType);
  }

  LxAddressRange symbolRange(symAddr, symAddr + mSize - 1);

  // Make sure that the checksum fits in the section
  if (!section->GetRange().Contains(symbolRange))
  {
    throw LxChecksumException(LxChecksumException::kSymbolInSeveralSections);
  }

  // Check that the address is not included in other checksum ranges
  if (mLog.ChecksumIsIncludedInOldRanges(symbolRange))
  {
    throw LxChecksumException(LxChecksumException::kChecksumOverlapError);
  }

  // Exclude the Parity symbol area from the ranges
  RemoveParityFromRanges(ranges, symAddr);

  // Store this in the cmd parameter history
  mLog.AddSym(symbolRange);
  mLog.AddRanges(ranges);

  // Calculate the Parity
  std::vector<uint32_t> parityWords = CalcParity(ranges, file);

  if (mVerbose)
  {
    cout << "Calculated the Parity for ";
    cout << hex << "0x"<< symAddr;

    if (!mSymbol.IsAbsolute())
    {
      cout << " (" << mSymbol.GetLabel();
      if (mSymbol.GetOffset() != 0)
        cout << " + " << mSymbol.GetOffset();
      cout << ")";
    }

    cout << endl;
  }

  // Store the Parity value
  StoreParity(symAddr - section->mHdr.sh_addr, section, parityWords);
}


void LxElfParityCmd::
StoreParity(Elf32_Off scnOffset,
              LxElfSection* scn,
              const std::vector<uint32_t> &parityWords)
{
  // Store the Parity value
  LxElfWriter wi(scn->mData, scnOffset);
  const uint32_t wordCount = mSize/4u;

  for (uint32_t i = 0;
       i < wordCount;
       ++i)
  {
      wi.PutWord(static_cast<Elf32_Word>(parityWords[i]));
  }
}

namespace
{
  class FilterByteVisitor : public LxByteVisitor
  {
  public:
    FilterByteVisitor(LxByteVisitor & next) : mNext(next) {}

  protected:
    LxByteVisitor & mNext;
  };

  class MirroredByteVisitor : public FilterByteVisitor
  {
  public:
    MirroredByteVisitor(LxByteVisitor & next) : FilterByteVisitor(next) {}

    virtual void VisitByte(uint8_t b)
    {
      mNext.VisitByte(mByteMirror[b]);
    }

  private:
    Mirror mByteMirror;
  };

  class ReversedByteVisitor : public FilterByteVisitor
  {
  public:
    ReversedByteVisitor(int size, LxByteVisitor & next)
      : FilterByteVisitor(next), mSize(size), mBufLength(0) {}

    virtual void VisitByte(uint8_t b)
    {
      if (mBufLength < mSize)
        mBuffer[mBufLength++] = b;

      if (mBufLength == mSize)
      {
        while (mBufLength > 0)
        {
          mNext.VisitByte(mBuffer[--mBufLength]);
        }
      }
    }

    virtual void VisitEnd()
    {
      if (mBufLength > 0)
      {
        cout << "ielftool warning: "
             << mBufLength << " bytes skipped in calculation\n";
      }
    }

    virtual Elf32_Word getNrOfBufferBytes() const
    {
      return mBufLength;
    }

  private:
    const int mSize;
    uint8_t   mBuffer[8];
    uint8_t   mBufLength;
  };

  class ReversedMirroredByteVisitor : public ReversedByteVisitor
  {
  public:
    ReversedMirroredByteVisitor(int size, LxByteVisitor & next)
      : ReversedByteVisitor(size, next) {}

    virtual void VisitByte(uint8_t b)
    {
      ReversedByteVisitor::VisitByte(mByteMirror[b]);
    }

  private:
    Mirror mByteMirror;
  };

  static
  LxByteVisitor*
  createByteVisitor(uint8_t size,
                    Algorithm & a,
                    const AlgorithmSettings & settings)
  {
    if (settings.mReverse && size > 1)
    {
      if (settings.mMirror)
        return new ReversedMirroredByteVisitor(size, a);
      else
        return new ReversedByteVisitor(size, a);
    }
    else
    {
      if (settings.mMirror)
        return new MirroredByteVisitor(a);
      else
        return &a;
    }
  }
}

void ChecksumLog::
AddSym(const LxAddressRange & symRange)
{
  mSyms.push_back(symRange);
}

void ChecksumLog::
AddRanges(const LxAddressRanges & ranges)
{
  mRanges.push_back(ranges);
}


namespace
{
  bool Intersects(LxAddressRange a, LxAddressRange b)
  {
    return a.Intersects(b);
  }

  bool RangesIntersectRange(LxAddressRanges a, LxAddressRange b)
  {
    return find_if(a.begin(),
                   a.end(),
                   bind2nd(ptr_fun(Intersects), b)) != a.end();
  }

  bool RangeIntersectRanges(LxAddressRange a, LxAddressRanges b)
  {
    return find_if(b.begin(),
                   b.end(),
                   bind2nd(ptr_fun(Intersects), a)) != b.end();
  }
}

bool ChecksumLog::
RangesIncludeOldChecksum(LxAddressRanges const & ranges) const
{
  // Check if the ranges include old checksum symbols
  return find_if(mSyms.begin(),
                 mSyms.end(),
                 bind2nd(ptr_fun(RangeIntersectRanges),
                         ranges)) != mSyms.end();
}

bool ChecksumLog::
ChecksumIsIncludedInOldRanges(const LxAddressRange & symbolRange) const
{
  // Check if this symbol is included in older ranges
  return find_if(mRanges.begin(),
                 mRanges.end(),
                 bind2nd(ptr_fun(RangesIntersectRange),
                         symbolRange)) != mRanges.end();
}
