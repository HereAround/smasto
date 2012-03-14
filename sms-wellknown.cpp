/**
 * @file   sms-random.cpp
 *
 * Write a random sparse matrix, of given dimensions and density.
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

#include <boost/random/mersenne_twister.hpp>
//#include <boost/random/random_device.hpp>
#include <boost/random/uniform_01.hpp>

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


class WellKnownProgram : public FilterProgram,
                            public SMSWriter<val_t>
{
public:
  WellKnownProgram() 
    : height_(0), width_(0), kind_(ZERO_MATRIX)
  {
    this->description = 
      "Generate a matrix of the given size and kind, then write it to OUTPUT.\n"
      "First argument KIND specifies what matrix is to be generated: currently\n"
      "allowed values are:\n"
      "  - `identity`: the identity matrix (aliases: `1` or `I`);\n"
      "  - `zero`: the null matrix (aliases: `0` or `O`).\n"
      ;
  };


  void process_option(const int opt, const char* argument)
  {
    // this is required by the base class, but there's nothing to do
    // for `process_option` here.
  };


  void parse_args(int argc, char** argv)
  {
    // too few arguments
    if (argc < 4) {
      std::ostringstream msg;
      msg << "Not all required arguments present."
          << " Type '" << argv[0] << " --help' to get usage help."
          << std::endl;
      throw std::runtime_error(msg.str());
    };

    // usage: KIND ROWS COLUMNS [OUTPUT]
    switch (argv[1][0]) {
    case 'I':
    case 'i':
    case '1':
      kind_ = IDENTITY_MATRIX;
      break;
    case 'Z':
    case 'z':
    case 'O':
    case '0':
      kind_ = ZERO_MATRIX;
      break;
    };
    std::istringstream(argv[2]) >> height_;
    std::istringstream(argv[3]) >> width_;
    if (argc > 4)
      set_output(argv[4]);
    else
      set_output("-");
    set_output_format(notation_, precision_);

    // too many arguments
    if (argc > 5) {
      std::ostringstream msg;
      msg << "At most four positional arguments allowed."
          << " Type '" << argv[0] << " --help' to get usage help."
          << std::endl;
      throw std::runtime_error(msg.str());
    };
  };


  int run() { 
    if (height_ < 1)
      throw std::runtime_error("Second argument (number of rows) must be positive!");
    if (width_ < 1)
      throw std::runtime_error("Third argument (number of columns) must be positive!");

    // generate matrix and write it
    SMSWriter<val_t>::open(*FilterProgram::output_, height_, width_);
    switch (kind_) {
    case ZERO_MATRIX: 
      {
        // nothing to do: the null matrix just has header and footer
        break;
      };
    case IDENTITY_MATRIX: 
      {
        for(coord_t i = 0; i < height_; ++i)
          // output entry
          write_entry(i+1, i+1, 1);
        break;
      };
    };
    SMSWriter<val_t>::close();

    return 0; 
  };


private:
  coord_t height_;
  coord_t width_;
  
  enum { ZERO_MATRIX=0, IDENTITY_MATRIX=1 } kind_;
};


int main(int argc, char** argv)
{
  return WellKnownProgram().main(argc, argv);
};
