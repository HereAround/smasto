/**
 * @file   sms-random.cpp
 *
 * Write a random sparse matrix, of given dimensions and density.
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


class RandomSparseProgram : public FilterProgram,
                            public SMSWriter<val_t>
{
public:
  RandomSparseProgram() 
    : height_(0), width_(0), density_(0),
      N_(0)
  {
    this->add_option('I', "integer", required_argument, "Matrix has integer entries in the range 1 to ARG..");
    this->description = 
      "Generate a random sparse matrix of the given size and write it to OUTPUT.\n"
      "Each entry has a probability of being nonzero equal to the DENSITY.\n"
      "Entry values are uniformly distributed real numbers between 0 and 1;\n"
      "use the '-I N' option to generate integer entries in the range 1 to N.\n"
      ;
  };


  void process_option(const int opt, const char* argument)
  {
    if ('I' == opt)
      std::istringstream(argument) >> N_;
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

    // usage: DENSITY ROWS COLUMNS [OUTPUT]
    std::istringstream(argv[1]) >> density_;
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
    if (density_ <= 0 or density_ >= 1)
      throw std::runtime_error("First argument (density) must be strictly in the floating-point range 0 to 1.");

    // generate matrix and write it
    SMSWriter<val_t>::open(*FilterProgram::output_, height_, width_);
    boost::mt19937 rng;
    boost::uniform_01<double> nonzero_dist;
    boost::uniform_01<val_t> value_dist;
    for(coord_t i = 0; i < height_; ++i)
      for(coord_t j = 0; j < width_; ++j) {
        // make a nonzero entry if we get a random number less than
        // a certain threshold (the density)
        double p = nonzero_dist(rng);
        if (p >= density_)
          continue;

        val_t value = value_dist(rng);
        // round to int if `-I` was used
        if (N_ != 0)
          value = static_cast<long>(1 + N_ * value);

        if (0 == value)
          continue;

        // output entry
        write_entry(i, j, value);
      }
    SMSWriter<val_t>::close();

    return 0; 
  };


private:
  coord_t height_;
  coord_t width_;
  
  double density_;

  val_t N_;
};


int main(int argc, char** argv)
{
  return RandomSparseProgram().main(argc, argv);
};
