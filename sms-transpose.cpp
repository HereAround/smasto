/**
 * @file   sms-transpose.cpp
 *
 * Transpose a matrix stream.
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
#include <sstream>
#include <stdexcept>


// matrix dimensions should fit into a `long` integer type
typedef long coord_t;

// always select the widest floating-point type available
# ifdef HAVE_LONG_DOUBLE
typedef long double val_t;
# else
typedef double val_t;
# endif 


class TransposeProgram : public FilterProgram, 
                    public SMSReader<val_t>, 
                    public SMSWriter<val_t>
{
public:
  TransposeProgram() 
    : m(), tall_(false), wide_(false), transpose_(true)
  {
    this->add_option('C', "wide", no_argument, "Only transpose if the output matrix has more columns than rows.");
    this->add_option('R', "tall", no_argument, "Only transpose if the output matrix has more rows than columns.");
    this->description = 
      "Output the transpose of the INPUT stream matrix.\n"
      "If the '-R' or '-C' options are given, the OUTPUT\n"
      "matrix is a transpose of INPUT only if it matches\n"
      "the requested condition; it is an exact copy otherwise.\n"
      "\n"
      "Both the INPUT and the OUTPUT matrix streams are in J.-G.\n" 
      "Dumas' SMS format.\n"
      ;
  };

  void process_option(const int opt, const char* argument)
  {
    if ('C' == opt)
      wide_ = true;
    else if ('R' == opt)
      tall_ = true;
  };

  int run() { 
    if (tall_ and wide_)
      throw std::runtime_error("Only one of the options '-R' and '-C' can be specified at a time.");

    SMSReader<val_t>::open(*FilterProgram::input_);
    coord_t nrows = SMSReader<val_t>::rows();
    coord_t ncols = SMSReader<val_t>::columns();

    if (tall_ and nrows > ncols)
      transpose_ = false;
    if (wide_ and ncols > nrows)
      transpose_ = false;

    if (transpose_)
      std::swap(nrows, ncols);
    SMSWriter<val_t>::open(*FilterProgram::output_, nrows, ncols);
                           
    read();

    for(matrix_t::const_iterator r = m.begin(); r != m.end(); ++r)
      for(vector_t::const_iterator c = r->second.begin(); c != r->second.end(); ++c)
        write_entry(r->first, c->first, c->second);

    SMSWriter<val_t>::close();
    SMSReader<val_t>::close();
    return 0; 
  };

  void process_entry(const coord_t i, const coord_t j, const val_t& value)
  {
    if (transpose_)
      m[j][i] = value;
    else
      m[i][j] = value;
  };


private:
  typedef std::map< coord_t, val_t > vector_t;
  typedef std::map< coord_t, vector_t > matrix_t;
  matrix_t m;

  bool tall_;
  bool wide_;
  bool transpose_;
};


int main(int argc, char** argv)
{
  return TransposeProgram().main(argc, argv);
};
