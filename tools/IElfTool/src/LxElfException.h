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

// Elf constants and structures

#ifndef ELFEXCEPTION_H
#define ELFEXCEPTION_H

#include <string>

struct LxException
{
  virtual ~LxException();

  virtual std::string GetMessage() const;
};

struct LxMessageException : public LxException
{
  LxMessageException(const std::string & m);

  virtual std::string GetMessage() const;

private:
  std::string mMessage;
};

struct LxFileException : public LxException
{
  enum Type
  {
    kFileOpenError,
    kFileReadError,
    kFileWriteError,
    kWrongElfType,
    kWrongElfClass,
    kParseError,
    kSymTableNotFound
  };

  LxFileException(const std::string & f, Type type);

  std::string GetMessage() const;

private:
  Type        mType;
  std::string mFileName;
};

struct LxSymbolException : public LxException
{
  enum Type
  {
    kStringNotFound,
    kSymbolNotFound
  };

  LxSymbolException(const std::string & symbol, Type type);

  std::string GetMessage() const;

private:
  Type        mType;
  std::string mSymbol;
};

struct LxSaveException : public LxException
{
  std::string GetMessage() const;
};

struct LxChecksumException : public LxException
{
  enum Type
  {
    kSymbolInSeveralSections,
    kWrongSymbolType,
    kWrongSymbolSize,
    kChecksumError,
    kChecksumRangeError,
    kChecksumOverlapError,
    kMultipleSectionsForSymbol,
    kNoSectionForAddress
  };

  LxChecksumException(Type type);

  std::string GetMessage() const;

private:
  Type mType;
};

struct LxFillException : public LxException
{
  enum Type
  {
    kPatternMissing0x,
    kOddNumberOfPatternChars,
    kMissingSemicolon,
    kRangeError    
  };

  LxFillException(Type type);

  std::string GetMessage() const;

private:
  Type mType;
};

struct LxStripException : public LxException
{
  std::string GetMessage() const;
};

#endif // ELFTYPES_H
