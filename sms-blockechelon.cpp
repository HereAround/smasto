/**
 * @file   sms-blockechelon.cpp
 *
 * Put matrix in block echelon form.
 *
 * @author  riccardo.murri@gmail.com
 * @version $Revision$
 */
/*
 * Copyright (c) 2010-2013 riccardo.murri@gmail.com.  All rights reserved.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#include "common.hpp"

#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>


// matrix dimensions should fit into a `long` integer type
typedef long coord_t;

// always select the widest floating-point type available
# ifdef HAVE_LONG_DOUBLE
typedef long double val_t;
# else
typedef double val_t;
# endif


class BlockEchelonProgram : public FilterProgram,
                         public SMSReader<val_t>,
                         public SMSWriter<val_t>
{
public:
  BlockEchelonProgram()
    : m(), blk()
  {
    this->description =
      "Put INPUT matrix in block echelon form.\n"
      ;
  };

  void process_option(const int opt, const char* argument)
  {
    // no options to process
  };

  int run() {
    SMSReader<val_t>::open(*FilterProgram::input_);
    const coord_t nrows = SMSReader<val_t>::rows();
    const coord_t ncols = SMSReader<val_t>::columns();

    // read matrix entries and group them into `block`
    read();
    SMSReader<val_t>::close();

    // compute list of blocks
    //
    // The SMS format mandates that entries are written to the file in
    // lexicographic order, so we could actually build the `blk` map
    // as we read the entries.  However, "be liberal in what you
    // accept, and be strict in what you produce" so let's build
    // matrix and blocks in two separate steps, which allows for
    // out-of-order lines.
    for(matrix_t::const_iterator r = m.begin(); r != m.end(); ++r) {
      // find the row item with the minimum column index; this works
      // as lexicographic comparison is used on `std::pair` and the
      // column index is the first item in the pair
      row_t::const_iterator minpair(std::min_element(r->second.begin(), r->second.end()));
      const coord_t first_nonzero_col = minpair->first;
      // TODO: add sorting to rows
      blk[first_nonzero_col].push_back(r->first);
    };

    // output matrix
    SMSWriter<val_t>::open(*FilterProgram::output_, nrows, ncols);
    coord_t new_row_idx = 0;
    for(reblock_t::const_iterator b = blk.begin(); b != blk.end(); ++b) {
      // output all rows in the block
      for(block_t::const_iterator i = b->second.begin(); i != b->second.end(); ++i) {
        ++new_row_idx;
        const coord_t old_row_idx = *i;
        row_t const& r = m[old_row_idx];
        for(row_t::const_iterator c = r.begin(); c != r.end(); ++c)
          write_entry(new_row_idx, c->first, c->second);
      };
    };
    SMSWriter<val_t>::close();

    return 0;
  };

  void process_entry(const coord_t i, const coord_t j, const val_t& value)
  {
    m[i][j] = value;
  };


private:
  /// a sparse row is just a map from column index to value
  typedef std::map< coord_t, val_t > row_t;
  /// a sparse matrix is a map from row index to sparse row
  typedef std::map< coord_t, row_t > matrix_t;
  /// matrix data (as read from the stream)
  matrix_t m;

  /// a block is a sequence of row numbers
  typedef std::vector< coord_t > block_t;
  typedef std::map< coord_t,block_t > reblock_t;
  /// map column index to set of rows that start at that column
  reblock_t blk;
};


int main(int argc, char** argv)
{
  return BlockEchelonProgram().main(argc, argv);
};
