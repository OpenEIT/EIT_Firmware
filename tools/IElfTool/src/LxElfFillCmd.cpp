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

// Class that implements the command for filling an elf file

#include "LxElfFillCmd.h"
#include "LxElfFile.h"

#include <sstream>
#include <algorithm>

using namespace std;


LxElfFillCmd::
LxElfFillCmd(LxSymbolicRange      range,
             const FillPattern &  pattern,
             bool                 virtual_fill)
  : mRange(range), mPattern(pattern), mVirtual(virtual_fill)
{
}

namespace
{
  // Builds a list of ranges that spans over the given sections
  struct RangeCollector
  {
    RangeCollector(LxAddressRanges & ranges) : mRanges(ranges) {}

    void operator()(const LxElfSegment* segment)
    {
      LxAddressRange range(segment->GetRange());

      if (!mRanges.empty() && mRanges.back().GetEnd() + 1 == range.GetStart())
        mRanges.back().SetEnd(range.GetEnd());
      else
        mRanges.push_back(range);
    }
  private:
    LxAddressRanges & mRanges;
  };
}


void LxElfFillCmd::
Execute(LxElfFile & file, bool verbose)
{
  const LxAddressRange fillRange(LxGetAddressRange(mRange, file));
  LxElfConstSegments segments;
  file.GetSegmentsInAddrRange(fillRange, segments);

  // Find all ranges that span over the segments
  LxAddressRanges ranges;
  for_each(segments.begin(), segments.end(), RangeCollector(ranges));

  Elf32_Addr addr = fillRange.GetStart();
  while (addr < fillRange.GetEnd())
  {
    // Find the segment range that contains addr
    LxAddressRanges::iterator currentRange = find_if(ranges.begin(),
                                                     ranges.end(),
                         bind2nd(mem_fun_ref(&LxAddressRange::ContainsAddress),
                                 addr));

    if (currentRange == ranges.end())
    {
      // No section contains addr. We need to fill from addr to
      // either the next section or to the end of the given fill range.
      LxAddressRange range = LxAddressRange(addr, fillRange.GetEnd());

      // Find the segment range after addr
      LxAddressRanges::iterator next = find_if(ranges.begin(),
                                               ranges.end(),
                     bind2nd(mem_fun_ref(&LxAddressRange::StartsAfter), addr));

      if (next != ranges.end() && next->GetStart() <= fillRange.GetEnd())
      {
        range.SetEnd(next->GetStart() - 1);
      }

      if (mVirtual)
      {
        file.AddVirtualFill(LxElfVirtualFill(range, mPattern), verbose);        
      }
      else
      {
        AddFillScn(range, file, verbose);
      }
      addr = range.GetEnd() + 1;
    }
    else
    {
      // There is a segment at addr. Check if we need to fill after that
      // section.
      if (currentRange->GetEnd() == 0xFFFFFFFF)
      {
        // Handle special case where we have a segment that ends at 0xFFFFFFFF
        break;
      }
      else
        addr = currentRange->GetEnd() + 1;
    }
  }
}

void LxElfFillCmd::
AddFillScn(LxAddressRange range, LxElfFile & file, bool verbose)
{
  string sectionName = CreateFillSectionName(file);

  // Add the fill section last
  LxElfSection const * last = file.GetSection(file.GetNrOfSections() - 1);

  Elf32_Off pos = last->mHdr.sh_offset + last->mHdr.sh_size;
  LxElfSegment * fill = file.AddLoadSegment(range.GetLength(),
                                            range.GetLength(),
                                            /*align=*/0,
                                            PF_R | PF_X,
                                            range.GetStart(),
                                            pos,
                                            sectionName);

  // Fill the section with a pattern
  Elf32_Off patOffset = (fill->mHdr.p_vaddr - range.GetStart())
                                                             % mPattern.size();
  Fill(fill->mData, mPattern, patOffset);

  if (verbose)
  {
    cout << "Adding fill section over range "
         << hex << range.GetStart() << "-" << range.GetEnd() << endl;
  }
}


string LxElfFillCmd::
CreateFillSectionName(LxElfFile const & file) const
{
  unsigned long fillNr = 1;

  Elf32_Half labelIndex = file.GetSectionLabelsIdx();

  // Loop until we find a unique fill section name
  while (true)
  {
    ostringstream outputString;
    outputString << "Fill" << fillNr++;

    try
    {
      // GetStringIndex throws an exception if the string is not found
      file.GetStringIndex(outputString.str(), labelIndex);
    }
    catch(...)
    {
      return outputString.str();
    }
  }
}
