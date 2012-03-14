/**
 * @file   sms-copy.cpp
 *
 * Copy a matrix stream, optionally rescaling the values by a constant factor.
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


class RescaleProgram : public FilterProgram, 
                       public SMSReader<val_t>, 
                       public SMSWriter<val_t>
{
public:
  RescaleProgram() 
    : multiply_(1), divide_(1)
  {
    this->add_option('m', "multiply", required_argument, "Multiply each entry by ARG.");
    this->add_option('r', "divide",   required_argument, "Divide each entry by ARG.");
    this->description = 
      "Output a copy of the matrix given in the INPUT stream,\n"
      "optionally multiplying each entry by a constant factor.\n"
      "\n"
      "Both the INPUT and the OUTPUT matrix streams are in J.-G.\n" 
      "Dumas' SMS format.\n"
      ;
  };

  void process_option(const int opt, const char* argument)
  {
    if ('m' == opt)
      std::istringstream(argument) >> multiply_;
    else if ('r' == opt)
      std::istringstream(argument) >> divide_;
  };

  int run() { 
    SMSReader<val_t>::open(*FilterProgram::input_);
    SMSWriter<val_t>::open(*FilterProgram::output_, 
                           SMSReader<val_t>::rows(), SMSReader<val_t>::columns());
    read();
    SMSWriter<val_t>::close();
    SMSReader<val_t>::close();
    return 0; 
  };

  void process_entry(const coord_t i, const coord_t j, const val_t& value)
  {
    write_entry(i, j, value * multiply_ / divide_);
  };


private:
  val_t multiply_;
  val_t divide_;
};


int main(int argc, char** argv)
{
  return RescaleProgram().main(argc, argv);
};
