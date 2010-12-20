/**
 * @file   sms-shrink.cpp
 *
 * Copy a matrix, removing rows and columns consisting entirely of zeroes.
 *
 * @author  riccardo.murri@gmail.com
 * @version $Revision$
 */
/*
 * Copyright (c) 2010 riccardo.murri@gmail.com.  All rights reserved.
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

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <map>
#include <set>
#include <sstream>


// matrix dimensions should fit into a `long` integer type
typedef long coord_t;

// always select the widest floating-point type available
# ifdef HAVE_LONG_DOUBLE
typedef long double val_t;
# else
typedef double val_t;
# endif 


class ShrinkProgram : public FilterProgram, 
                         public SMSReader<val_t>,
                         public SMSWriter<val_t>
{
public:
  ShrinkProgram() 
    : seen_row_(), seen_col_(),
      m()
  {
    this->description = 
      "Copy INPUT matrix to OUTPUT, removing rows and columns consisting\n"
      "entirely of zeroes.  Rows and columns are renumbered to preserve the\n"
      "relative order in which they appear in the INPUT matrix.\n"
      ;
  };

  void process_option(const int opt, const char* argument)
  {
    // no options
  };

  int run() { 
    SMSReader<val_t>::open(*FilterProgram::input_);
    const coord_t nrows = SMSReader<val_t>::rows();
    const coord_t ncols = SMSReader<val_t>::columns();

    // read and process input matrix entries
    read();
    SMSReader<val_t>::close();

    // renumber rows and columns
    coord_t skipped_rows = 0;
    std::map<size_t, size_t> new_row_number;
    for (coord_t i = 1; i <= nrows; ++i)
      if (seen_row_[i])
        new_row_number[i] = i - skipped_rows;
      else // seen_row_[i] == false
        skipped_rows++;
    
    std::map<size_t, size_t> new_col_number;
    size_t skipped_cols = 0;
    for (coord_t j = 0; j <= ncols; ++j)
      if (seen_col_[j])
        new_col_number[j] = j - skipped_cols;
      else // seen_col_[j] == false
        skipped_cols++;

    // output
    SMSWriter<val_t>::open(*FilterProgram::output_, nrows, ncols);
    for(matrix_t::const_iterator r = m.begin(); r != m.end(); ++r)
      for(vector_t::const_iterator c = r->second.begin(); c != r->second.end(); ++c)
        write_entry(new_row_number[r->first], new_col_number[c->first], c->second);
    SMSWriter<val_t>::close();

    return 0; 
  };

  void process_entry(const coord_t i, const coord_t j, const val_t& value)
  {
    // if row i and col j are in the pre-selected set, copy triple
    // remapping row and col index
    seen_row_[i] = true;
    seen_col_[j] = true;
    m[i][j] = value;
  };


private:
  std::vector< bool > seen_row_;
  std::vector< bool > seen_col_;
  
  typedef std::map< coord_t, val_t > vector_t;
  typedef std::map< coord_t, vector_t > matrix_t;
  matrix_t m;

  // just in case rand() does not provide enough random bits
  coord_t rand_coord()
  {
#if RAND_MAX < 2147483647 /* 2^31 */
    return (rand() ^ (rand() << 16));
#else
    return rand();
#endif
  };
};


int main(int argc, char** argv)
{
  return ShrinkProgram().main(argc, argv);
};
