/**
 * @file   sms-info.cpp
 *
 * Print summary information about a matrix stream.
 *
 * @author  riccardo.murri@gmail.com
 * @version $Revision$
 */
/*
 * Copyright (c) 2010-2012 riccardo.murri@gmail.com.  All rights reserved.
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
#include <sstream>


// matrix dimensions should fit into a `long` integer type
typedef long coord_t;

// always select the widest floating-point type available
# ifdef HAVE_LONG_DOUBLE
typedef long double val_t;
# else
typedef double val_t;
# endif 


class InfoProgram : public FilterProgram, 
                    public SMSReader<val_t>
{
public:
  InfoProgram() 
    : nnz_(0), short_(false)
  {
    this->add_option('s', "short",  no_argument, "One-line output format");
    this->description = 
      "Output information on the matrix given in the INPUT stream:\n"
      "number of rows and columns, number of nonzero values, density.\n"
      ;
  };

  void process_option(const int opt, const char* argument)
  {
    if ('s' == opt)
      short_ = true;
  };

  int run() { 
    SMSReader<val_t>::open(*FilterProgram::input_);
    coord_t nrows = SMSReader<val_t>::rows();
    coord_t ncols = SMSReader<val_t>::columns();
    read();
    SMSReader<val_t>::close();

    if (short_) {
      (*output_) << "rows:" << nrows 
                 << " columns:" << ncols 
                 << " nonzero:" << nnz_ 
                 << " density:" << (100.0 * nnz_ / nrows / ncols) << std::endl;
    }
    else {
      (*output_) << "Rows: " << nrows << std::endl;
      (*output_) << "Columns: " << ncols << std::endl;
      (*output_) << "Non-zeros: " << nnz_ << std::endl;
      (*output_) << "Density%: " << (100.0 * nnz_ / nrows / ncols) << std::endl;
    };
    return 0; 
  };

  void process_entry(const coord_t i, const coord_t j, const val_t& value)
  {
    if (0 != value)
      ++nnz_;
  };


private:
  coord_t nnz_;
  bool short_;
};


int main(int argc, char** argv)
{
  return InfoProgram().main(argc, argv);
};
