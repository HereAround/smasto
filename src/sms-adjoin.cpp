/**
 * @file   sms-adjoin.cpp
 *
 * Adjoin two matrices, either side-by-side or one on top of the other
 *
 * @author  riccardo.murri@gmail.com
 * @version $Revision$
 */
/*
 * Copyright (c) 2015 riccardo.murri@gmail.com.  All rights reserved.
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

// adjoining does not care about the type of the entries
typedef std::string val_t;

// what direction to adjoin matrices
typedef enum { side_by_side, stacked } direction_t;


class AdjoinProgram : public FilterProgram,
                      public SMSReader<val_t>,
                      public SMSWriter<val_t>
{
public:
  AdjoinProgram()
    : m(), base_i(0), base_j(0), direction_(side_by_side)
  {
    this->add_option('R', "side-by_side", no_argument,
                     "Concatenate matrix rows (default)."
                     " All matrices should have the same nr of rows.");
    this->add_option('C', "stacked", no_argument,
                     "Concatenated matrix columns."
                     " All matrices should have the same nr of columns.");
    this->description =
      "Adjoin all the INPUT stream matrices.\n"
      "Matrices can be joined by concatenating the rows to form a new\n"
      "row (i.e., side-by-side; default), or by concatenating columns to form the output\n"
      "column (i.e., stacking matrices one on top of the other; `-C` option)\n"
      "\n"
      "Both the INPUT and the OUTPUT matrix streams are in J.-G.\n"
      "Dumas' SMS format.\n"
      ;
  };

  void process_option(const int opt, const char* argument)
  {
    if ('C' == opt)
      direction_ = stacked;
    else if ('R' == opt)
      direction_ = side_by_side;
  };

  void parse_args(int argc, char** argv)
  {
    // too few arguments
    if (argc < 3) {
      std::ostringstream msg;
      msg << "At least two positional arguments required."
          << " Type '" << argv[0] << " --help' to get usage help."
          << std::endl;
      throw std::runtime_error(msg.str());
    };

    // set OUTPUT, if any
    if (argc > 3) {
      set_output(argv[argc-1]);
    };
    set_output_format(notation_, precision_);
  };

  int run() {
    coord_t nrows = 0;
    coord_t ncols = 0;

    for (int i = 1; i < argc_; ++i)
      {
        set_input(argv_[i]);
        SMSReader<val_t>::open(*FilterProgram::input_);

        const coord_t this_nrows = SMSReader<val_t>::rows();
        const coord_t this_ncols = SMSReader<val_t>::columns();

        switch(direction_)
          {
          case side_by_side:
            base_j = ncols;
            ncols += this_ncols;
            if (nrows > 0)
              assert (this_nrows == nrows);
            else
              nrows = this_nrows;
            break;
          case stacked:
            base_i = nrows;
            nrows += this_nrows;
            if (ncols > 0)
              assert (this_ncols == ncols);
            else
              ncols = this_ncols;
            break;
          default:
            assert(false); // BUG: unhandled case!
          };

        read();

        SMSReader<val_t>::close();
      };

    SMSWriter<val_t>::open(*FilterProgram::output_, nrows, ncols);
    for(matrix_t::const_iterator r = m.begin(); r != m.end(); ++r)
      for(vector_t::const_iterator c = r->second.begin(); c != r->second.end(); ++c)
        write_entry(r->first, c->first, c->second);

    SMSWriter<val_t>::close();
    return 0;
  };

  void process_entry(const coord_t i, const coord_t j, const val_t& value)
  {
    m[base_i + i][base_j + j] = value;
  };


private:
  typedef std::map< coord_t, val_t > vector_t;
  typedef std::map< coord_t, vector_t > matrix_t;
  matrix_t m;

  coord_t base_i, base_j;

  direction_t direction_;
};


int main(int argc, char** argv)
{
  return AdjoinProgram().main(argc, argv);
};
