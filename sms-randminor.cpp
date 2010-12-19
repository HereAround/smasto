/**
 * @file   sms-randminor.cpp
 *
 * Output a random minor of the given input matrix, with specified dimensions.
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


class RandminorProgram : public FilterProgram, 
                         public SMSReader<val_t>,
                         public SMSWriter<val_t>
{
public:
  RandminorProgram() 
    : height_(0), width_(0),
      from_rows_(), to_rows_(),
      from_cols_(), to_cols_(),
      m()
  {
    this->add_option('C', "columns", required_argument, "Number of columns in the minor to extract.");
    this->add_option('R', "rows",    required_argument, "Number of rows in the minor to extract.");
    this->description = 
      "Extract a minor of the given input matrix (with dimensions specified.\n"
      "by the '-R' and '-C' options), obtained by randomly selecting rows\n"
      "and columns, and write it into OUTPUT.  Note that the minor rows and\n"
      "columns are selected from the whole allowable range, not just the rows\n"
      "and columns containing nonzero entries: therefore small minors have a\n"
      "likely chance of being entirely null.\n"
      ;
  };

  void process_option(const int opt, const char* argument)
  {
    if ('C' == opt)
      std::istringstream(argument) >> width_;
    else if ('R' == opt)
      std::istringstream(argument) >> height_;
  };

  int run() { 
    if (height_ < 1)
      throw std::runtime_error("Use the '-R' option to set a positive number of rows for the minor.");
    if (width_ < 1)
      throw std::runtime_error("Use the '-C' option to set a positive number of rows for the minor.");

    SMSReader<val_t>::open(*FilterProgram::input_);
    const coord_t nrows = SMSReader<val_t>::rows();
    const coord_t ncols = SMSReader<val_t>::columns();

    srand(time(NULL));
    // generate set of rows
    while (from_rows_.size() < height_) {
      const coord_t r = rand_coord();
      if (r <= nrows) {
        from_rows_.insert(r);
        to_rows_[r] = from_rows_.size();
      };
    };
    // generate set of columns
    while (from_cols_.size() < width_) {
      const coord_t c = rand_coord();
      if (c <= ncols) {
        from_cols_.insert(c);
        to_cols_[c] = from_cols_.size();
      };
    };
    
    // read and process matrix entries
    read();
    SMSReader<val_t>::close();

    // output minor
    SMSWriter<val_t>::open(*FilterProgram::output_, nrows, ncols);
    for(matrix_t::const_iterator r = m.begin(); r != m.end(); ++r)
      for(vector_t::const_iterator c = r->second.begin(); c != r->second.end(); ++c)
        write_entry(r->first, c->first, c->second);
    SMSWriter<val_t>::close();

    return 0; 
  };

  void process_entry(const coord_t i, const coord_t j, const val_t& value)
  {
    // if row i and col j are in the pre-selected set, copy triple
    // remapping row and col index
    if (from_rows_.count(i) and from_cols_.count(j))
      m[to_rows_[i]][to_cols_[j]] = value;
  };


private:
  coord_t height_;
  coord_t width_;

  std::set< coord_t > from_rows_;
  std::map< coord_t, coord_t > to_rows_;

  std::set< coord_t > from_cols_;
  std::map< coord_t, coord_t > to_cols_;

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
  return RandminorProgram().main(argc, argv);
};
