/**
 * @file   sms-info.cpp
 *
 * Print summary information about a matrix stream.
 *
 * @author  riccardo.murri@gmail.com
 * @version $Revision$
 */
/*
 * Copyright (c) 2010, 2011, 2012 riccardo.murri@gmail.com.  All rights reserved.
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

// do not care about the type of the entries, just check if they are zero/nonzero
typedef std::string val_t;


class SvgProgram : public FilterProgram,
                   public SMSReader<val_t>
{
public:
  SvgProgram()
    : size_(5)
    , ticks_(120)
    , entry_color_("blue")
    , frame_color_("black")
    , grid_color_("silver")
  {
    this->add_option('b', "block-size",  required_argument, "Size (in pixels) of each square blocks representing matrix entries.");
    this->add_option('c', "color",       required_argument, "Color of the matrix entries in the output SVG file. Any color spec that is defined in the SVG standard is allowed.");
    this->add_option('g', "grid",        required_argument, "Draw axes every NUM entries (default 120); disable if NUM is 0.");
    this->add_option('j', "grid-color",  required_argument, "Color of the grid axes (if any).");
    this->add_option('k', "frame-color", required_argument, "Color of the enclosing box.");
    this->description =
      "Write to OUTPUT a representation of the nonzero pattern of the INPUT matrix.\n"
      ;
  };

  void process_option(const int opt, const char* argument)
  {
    if ('b' == opt)
      std::istringstream(argument) >> size_;
    else if ('c' == opt)
      entry_color_ = argument;
    else if ('g' == opt)
      std::istringstream(argument) >> ticks_;
    else if ('j' == opt)
      grid_color_ = argument;
    else if ('k' == opt)
      frame_color_ = argument;
  };

  int run() {
    // write SVG header
    (*output_)
      << "<?xml version=\"1.0\" standalone=\"no\"?>\n"
      << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n"
      << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n"
      << "<style type=\"text/css\"><![CDATA[\n"
      << "  .MatrixEntry { fill:" << entry_color_ << "; stroke-width:0; }\n"
      << "  .MatrixFrame { fill:none; stroke-width:1; stroke:" << frame_color_ << "; }\n"
      << "  .MatrixSeparator { fill:none; stroke-width:1; stroke:" << grid_color_ << "; }\n"
      << "]]></style>\n"
      << "<g>\n"
      << std::endl;


    SMSReader<val_t>::open(*FilterProgram::input_);
    // draw frame around matrix
    coord_t nrows = SMSReader<val_t>::rows();
    coord_t ncols = SMSReader<val_t>::columns();
    (*output_)
      << "<rect class=\"MatrixFrame\" x=\"0\" y=\"0\""
      << " height=\"" << (size_ * nrows) << "\""
      << " width=\"" << (size_ * ncols) << "\""
      << " />"
      << std::endl;

    // draw ticks marks
    if (0 != ticks_)
      {
        ticks_ *= size_;
        int max_digits = (1 + floor(log10(std::max(nrows, ncols))));
        coord_t dy = (ticks_ / 3);
        coord_t pt = (ticks_ / (2 * max_digits));
        for (coord_t j = ticks_; j < ncols*size_; j += ticks_) {
          (*output_)
            << "<line class=\"MatrixSeparator\""
            << " x1=\"" << j << "\""
            << " y1=\"" << 0 - dy << "\""
            << " x2=\"" << j << "\""
            << " y2=\"" << nrows*size_ + dy << "\""
            << " />"
            << std::endl;
          (*output_)
            << "<text font-size=\"" << pt << "\""
            << " x=\"" << j << "\""
            << " y=\"" << 0 - dy << "\">\n"
            << "<tspan>\n"
            << (j / size_)
            << "</tspan>\n"
            << "</text>\n"
            << std::endl;
          (*output_)
            << "<text font-size=\"" << pt << "\""
            << " x=\"" << j << "\""
            << " y=\"" << nrows*size_ + dy << "\">\n"
            << "<tspan>\n"
            << (j / size_)
            << "</tspan>\n"
            << "</text>\n"
            << std::endl;
        };
        for (coord_t i = ticks_; i < (nrows * size_); i += ticks_) {
          (*output_)
            << "<line class=\"MatrixSeparator\""
            << " x1=\"" << 0 << "\""
            << " y1=\"" << i << "\""
            << " x2=\"" << ncols*size_ << "\""
            << " y2=\"" << i << "\""
            << " />"
            << std::endl;
          (*output_)
            << "<text font-size=\"" << pt << "\""
            << " y=\"" << i << "\""
            << " x=\"" << 0 - dy - max_digits*pt << "\">\n"
            << "<tspan>\n"
            << (i / size_)
            << "</tspan>\n"
            << "</text>\n"
            << std::endl;
          (*output_)
            << "<text font-size=\"" << pt << "\""
            << " y=\"" << i << "\""
            << " x=\"" << ncols*size_ + dy << "\">\n"
            << "<tspan>\n"
            << (i / size_)
            << "</tspan>\n"
            << "</text>\n"
            << std::endl;
        };
      };

    // one rect per matrix entry
    read();

    // write closing XML elements
    (*output_)
      << "</g>\n"
      << "</svg>\n"
      << std::endl;

    SMSReader<val_t>::close();
    return 0;
  };

  void process_entry(const coord_t i, const coord_t j, const val_t& value)
  {
    if (not is_zero(value))
      (*output_)
        << "<rect class=\"MatrixEntry\""
        << " width=\"" << size_ << "\" height=\"" << size_ << "\""
        << " x=\"" << ((j-1)*size_) << "\""
        << " y=\"" << ((i-1)*size_) << "\""
        << " />"
        << std::endl;
  };


private:
  coord_t      size_;
  coord_t      ticks_;
  std::string  entry_color_;
  std::string  frame_color_;
  std::string  grid_color_;
};


int main(int argc, char** argv)
{
  return SvgProgram().main(argc, argv);
};
