/**
 * @file   sms-reordcols.cpp
 *
 * Permute columns of the input matrix, placing columns with more
 * nonzero elements towards the rightmost edge of the matrix.
 *
 * @author  riccardo.murri@gmail.com
 * @version $Revision$
 */
/*
 * Copyright (c) 2010, 2011 riccardo.murri@gmail.com.  All rights reserved.
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


class ReordColsProgram : public FilterProgram, 
                         public SMSReader<val_t>,
                         public SMSWriter<val_t>
{
public:
  ReordColsProgram() 
    : m(), r(), c(), new_col()
  {
    this->description = 
      "Permute columns of the input matrix, so that its block echelon\n"
      "form has the taller blocks towards the rightmost edge.\n"
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

    // initialize row and column counts to zero; note indices are 1-based
    r.resize(nrows+1, 0); 
    c.resize(ncols+1, 0);

    // read matrix entries and initialize r, c, new_col
    read();
    SMSReader<val_t>::close();

  
    // output matrix
    SMSWriter<val_t>::open(*FilterProgram::output_, nrows, ncols);
    for(matrix_t::const_iterator r = m.begin(); r != m.end(); ++r)
      for(row_t::const_iterator c = r->second.begin(); c != r->second.end(); ++c)
        write_entry(r->first, new_col[c->first], c->second);
    SMSWriter<val_t>::close();

    return 0; 
  };

  void process_entry(const coord_t i, const coord_t j, const val_t& value)
  {
    m[i][j] = value;
    r[i] += 1;
    c[j] += 1;
  };


private:
  // weights for the various criteria
  double a_, b_, c_, d_, e_;

  /// a sparse row is just a map from column index to value
  typedef std::map< coord_t, val_t > row_t;
  /// a sparse matrix is a map from row index to sparse row
  typedef std::map< coord_t, row_t > matrix_t;
  /// matrix data (as read from the stream)
  matrix_t m;

  /// number of nonzero elements on row `i` (old value)
  std::vector<coord_t> r;

  /// number of nonzero elements on column `j`(old value)
  std::vector<coord_t> c;

  /// map old column index to new one
  std::map< coord_t,coord_t > new_col;
};


int main(int argc, char** argv)
{
  return ReordColsProgram().main(argc, argv);
};
