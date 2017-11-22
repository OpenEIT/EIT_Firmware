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

/* $Rev: 21495 $ */

// Class that encapsulates a data buffer used for section data

#include "LxElfDataBuffer.h"

#include <assert.h>
#include <string.h>
#include <stdexcept>

using namespace std;

namespace
{
  uint64_t
  CreateULL(uint8_t v0,
            uint8_t v1,
            uint8_t v2,
            uint8_t v3,
            uint8_t v4,
            uint8_t v5,
            uint8_t v6,
            uint8_t v7, 
            bool bigEndian)
  {
    uint64_t retVal;
    if (bigEndian)
    {
      uint32_t lo = (v4 << 24) | (v5 << 16) | (v6 << 8) | v7;
      retVal      = (v0 << 24) | (v1 << 16) | (v2 << 8) | v3;
      retVal <<= 32;
      retVal |= lo;
    }
    else
    {
      uint32_t lo = (v3 << 24) | (v2 << 16) | (v1 << 8) | v0;
      retVal      = (v7 << 24) | (v6 << 16) | (v5 << 8) | v4;
      retVal <<= 32;
      retVal |= lo;
    }
    return retVal;
  }

  unsigned long
  CreateUL(uint8_t v0,
           uint8_t v1,
           uint8_t v2,
           uint8_t v3,
           bool bigEndian)
  {
    unsigned long retVal(0);

    if (bigEndian)
      retVal = v3 | (v2 << 8) | (v1 << 16) | (v0 << 24);
    else
      retVal = v0 | (v1 << 8) | (v2 << 16) | (v3 << 24);

    return retVal;
  }


  unsigned short
  CreateUS(uint8_t v0,
           uint8_t v1,
           bool bigEndian)
  {
    unsigned short retVal(0);

    if (bigEndian)
      retVal = v1 | (v0 << 8);
    else
      retVal = v0 | (v1 << 8);

    return retVal;
  }
}

static const size_t kOwner = static_cast<size_t>(-1);

LxElfDataBuffer::
LxElfDataBuffer()
  : mOffset(kOwner),
    mOwnedBuf(NULL),
    mBufSize(0),
    mElfBigEndian(false)
{
}

LxElfDataBuffer::
LxElfDataBuffer(bool elfBigEndian)
  : mOffset(kOwner),
    mOwnedBuf(NULL),
    mBufSize(0),
    mElfBigEndian(elfBigEndian)
{
}

LxElfDataBuffer::
LxElfDataBuffer(Elf32_Off bufSize, bool elfBigEndian)
 : mOffset(kOwner),
   mOwnedBuf(NULL),
   mBufSize(bufSize),
   mElfBigEndian(elfBigEndian)
{
  Allocate(bufSize);
}

LxElfDataBuffer::
LxElfDataBuffer(LxElfDataBuffer const & x)
  : mOffset(x.mOffset),
    mBufSize(x.mBufSize),
    mElfBigEndian(x.mElfBigEndian)
{
  if (IsOwner())
  {
    if (mBufSize == 0)
      mOwnedBuf = NULL;
    else
    {
      mOwnedBuf = new uint8_t[mBufSize];
      memcpy(mOwnedBuf, x.mOwnedBuf, mBufSize);
    }
  }
  else
    mBase = x.mBase;
}

LxElfDataBuffer & LxElfDataBuffer::
operator = (LxElfDataBuffer const & x)
{
  LxElfDataBuffer tmp(x);
  swap(tmp);
  return *this;
}

LxElfDataBuffer::
~LxElfDataBuffer()
{
  if (IsOwner())
    delete [] mOwnedBuf;
}

void LxElfDataBuffer::
swap(LxElfDataBuffer & x)
{
  uint8_t *         tob = IsOwner() ? mOwnedBuf : NULL;
  LxElfDataBuffer * tb  = IsOwner() ? NULL      : mBase;
  if (x.IsOwner())
    mOwnedBuf = x.mOwnedBuf;
  else
    mBase     = x.mBase;
  if (IsOwner())
    x.mOwnedBuf = tob;
  else
    x.mBase     = tb;
  using std::swap;
  swap(mOffset,       x.mOffset);
  swap(mBufSize,      x.mBufSize);
  swap(mElfBigEndian, x.mElfBigEndian);
}

bool LxElfDataBuffer::
IsOwner() const
{
  return mOffset == kOwner;
}

void LxElfDataBuffer::
Reset()
{
  if (IsOwner())
    delete [] mOwnedBuf;
  mOffset = kOwner;
  mOwnedBuf = NULL;
}

void LxElfDataBuffer::
SetWindow(LxElfDataBuffer & base,
          Elf32_Off offset,
          Elf32_Off size)
{
  Reset();
  mOffset = offset;
  mBase = &base;
  mBufSize = size;
}

void LxElfDataBuffer::
Allocate(Elf32_Off bufSize)
{
  // Deallocate old data
  Reset();

  if (bufSize != 0)
  {
    // Allocate buffer for elf data
    mOwnedBuf = new uint8_t[bufSize];
  }

  mBufSize = bufSize;
}


void LxElfDataBuffer::
Expand(Elf32_Off expSize)
{
  assert(IsOwner());

  unsigned long newBufSize = mBufSize + expSize;

  uint8_t* newBuf = new uint8_t[newBufSize];

  memcpy(newBuf, mOwnedBuf, mBufSize);
  mBufSize = newBufSize;

  // Deallocate old data
  delete [] mOwnedBuf;

  mOwnedBuf = newBuf;
}


void LxElfDataBuffer::
Shrink(Elf32_Off delta)
{
  mBufSize -= delta;
}


bool
LxLoad(LxElfDataBuffer & buf,
       istream &  inFile,
       Elf32_Off  offset,
       Elf32_Off  length)
{
  buf.Allocate(length);

  // Read the elf data section into the buffer
  inFile.seekg(offset, ios::beg);
  inFile.read((char*) &*buf.begin(), length);

  return true;
}


bool
LxSave(LxElfDataBuffer const & buf, ostream & outFile)
{
  outFile.write((char const *)(&*buf.begin()), buf.GetBufLen());

  return true;
}

LxElfDataBuffer::const_iterator LxElfDataBuffer::
begin() const
{
  return IsOwner() ? mOwnedBuf : mBase->begin() + mOffset;
}

LxElfDataBuffer::iterator LxElfDataBuffer::
begin()
{
  return IsOwner() ? mOwnedBuf : mBase->begin() + mOffset;
}

LxElfDataBuffer::const_iterator LxElfDataBuffer::
end() const
{
  return begin() + mBufSize;
}

LxElfDataBuffer::iterator LxElfDataBuffer::
end()
{
  return begin() + mBufSize;
}

bool LxElfReader::
AtEnd() const
{
  return mOffset == mEnd;
}

Elf64_Word LxElfReader::
GetDoubleWord()
{
  assert(mOffset + 8 <= mEnd);
  LxElfDataBuffer::const_iterator p = mBuf->begin() + mOffset;
  mOffset += 8;
  return CreateULL(p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], 
                  mBuf->IsBigEndian());

}

Elf32_Off LxElfReader::
GetOff()
{
  assert(mOffset + 4 <= mEnd);
  LxElfDataBuffer::const_iterator p = mBuf->begin() + mOffset;
  mOffset += 4;
  return CreateUL(p[0], p[1], p[2], p[3], mBuf->IsBigEndian());
}

Elf32_Addr LxElfReader::
GetAddr()
{
  return GetOff();
}

Elf32_Word LxElfReader::
GetWord()
{
  return GetOff();
}

Elf32_Sword LxElfReader::
GetSword()
{
  return static_cast<Elf32_Sword>(GetOff());
}

Elf32_Half LxElfReader::
GetHalf()
{
  assert(mOffset + 2 <= mEnd);
  LxElfDataBuffer::const_iterator p = mBuf->begin() + mOffset;
  mOffset += 2;
  return CreateUS(p[0], p[1], mBuf->IsBigEndian());
}

uint8_t LxElfReader::
GetByte()
{
  assert(mOffset < mEnd);
  LxElfDataBuffer::const_iterator p = mBuf->begin() + mOffset;
  ++mOffset;
  return *p;
}

uint8_t const * LxElfReader::
Ptr()
{
  assert(mOffset < mEnd);
  LxElfDataBuffer::const_iterator p = mBuf->begin() + mOffset;
  return &*p;
}

void LxElfWriter::
PutOff(Elf32_Off val)
{
  LxElfDataBuffer::iterator p = mBuf->begin() + mOffset;
  if (p + 4 > mBuf->end())
    throw std::runtime_error("write outside of buffer");

  if (mBuf->IsBigEndian())
  {
    p[0] = (uint8_t) (val >> 24);
    p[1] = (uint8_t) (val >> 16);
    p[2] = (uint8_t) (val >> 8);
    p[3] = (uint8_t)  val;
  }
  else
  {
    p[0] = (uint8_t)  val;
    p[1] = (uint8_t) (val >> 8);
    p[2] = (uint8_t) (val >> 16);
    p[3] = (uint8_t) (val >> 24);
  }
  mOffset += 4;
}

void LxElfWriter::
PutDoubleWord(Elf64_Word val)
{
  LxElfDataBuffer::iterator p = mBuf->begin() + mOffset;
  if (p + 8 > mBuf->end())
    throw std::runtime_error("write outside of buffer");

  if (mBuf->IsBigEndian())
  {
    p[0] = (uint8_t) (val >> 56);
    p[1] = (uint8_t) (val >> 48);
    p[2] = (uint8_t) (val >> 40);
    p[3] = (uint8_t) (val >> 32);
    p[4] = (uint8_t) (val >> 24);
    p[5] = (uint8_t) (val >> 16);
    p[6] = (uint8_t) (val >> 8);
    p[7] = (uint8_t)  val;
  }
  else
  {
    p[0] = (uint8_t)  val;
    p[1] = (uint8_t) (val >> 8);
    p[2] = (uint8_t) (val >> 16);
    p[3] = (uint8_t) (val >> 24);
    p[4] = (uint8_t) (val >> 32);
    p[5] = (uint8_t) (val >> 40);
    p[6] = (uint8_t) (val >> 48);
    p[7] = (uint8_t) (val >> 56);
  }
  mOffset += 8;
}


void LxElfWriter::
PutAddr(Elf32_Addr val)
{
  PutOff(val);
}

void LxElfWriter::
PutWord(Elf32_Word val)
{
  PutOff(val);
}

void LxElfWriter::
PutHalf(Elf32_Half val)
{
  LxElfDataBuffer::iterator p = mBuf->begin() + mOffset;
  if (p + 2 > mBuf->end())
    throw std::runtime_error("write outside of buffer");

  if (mBuf->IsBigEndian())
  {
    p[0] = (uint8_t) (val >> 8);
    p[1] = (uint8_t)  val;
  }
  else
  {
    p[0] = (uint8_t)  val;
    p[1] = (uint8_t) (val >> 8);
  }
  mOffset += 2;
}

void LxElfWriter::
PutByte(uint8_t val)
{
  LxElfDataBuffer::iterator p = mBuf->begin() + mOffset;
  if (p + 1 > mBuf->end())
    throw std::runtime_error("write outside of buffer");
  p[0] = val;
  ++mOffset;
}

void LxElfWriter::
Put(LxElfDataBuffer const & x)
{
  LxElfDataBuffer::iterator p = mBuf->begin() + mOffset;
  if (p + x.GetBufLen() > mBuf->end())
    throw std::runtime_error("write outside of buffer");
  LxElfDataBuffer::const_iterator f = x.begin();
  memcpy(&*p, &*f, x.GetBufLen());
  mOffset += x.GetBufLen();
}


void
Fill(LxElfDataBuffer & buf,
     FillPattern const & pattern,
     Elf32_Off           patOffset)
{
  typedef LxElfDataBuffer::iterator Iter;
  size_t patLen = pattern.size();
  for (Iter p = buf.begin(), e = buf.end(); p != e; ++p)
  {
    *p = pattern[(p - buf.begin() + patOffset) % patLen];
  }
}
