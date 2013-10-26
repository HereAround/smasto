/**
 * @file   sms-info.cpp
 *
 * Print summary information about a matrix stream.
 *
 * @author  riccardo.murri@gmail.com
 * @version $Revision$
 */
/*
 * Copyright (c) 2010-2013 riccardo.murri@gmail.com.  All rights reserved.
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

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <map>
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
    , shrink_(1)
    , max_width_(0)
    , max_height_(0)
    , darken_(1.0)
    , m_()
    , xticks_(0)
    , yticks_(0)
    , entry_color_("blue")
    , frame_color_("black")
    , grid_color_("silver")
  {
    this->add_option('b', "block-size",  required_argument, "Size (in pixels) of each square dot representing matrix entries.");
    this->add_option('c', "color",       required_argument, "Color of the matrix entries in the output SVG file. Any color spec that is defined in the SVG standard is allowed.");
    this->add_option('d', "darken",       required_argument, "Overcount nonzero elements in matrix tiles. This option has effect only when shrinking.");
    this->add_option('g', "grid",        required_argument, "Draw axes every NUM entries; disable if NUM is 0 (default). Mutually incompatible with `-x` and `-y`.");
    this->add_option('j', "grid-color",  required_argument, "Color of the grid axes (if any).");
    this->add_option('k', "frame-color", required_argument, "Color of the enclosing box.");
    this->add_option('s', "shrink",      required_argument, "One dot in the SVG output corresponds to a NUM by NUM square in the INPUT matrix. Default: dots in SVG OUTPUT correspond 1-1 to matrix entries in INPUT.");
    this->add_option('t', "shrink-to-height", required_argument, "Scale the output image so that the drawing area is at most NUM pixels tall. Mutually exclusive with options `-s` and `-w`.");
    this->add_option('w', "shrink-to-width", required_argument, "Scale the output image so that the drawing area is at most NUM pixels wide.  Mutually exclusive with options `-s` and `-t`.");
    this->add_option('x', "num-vert-axes",  required_argument, "Draw NUM vertical axes, equally spaced across the entire picture width. Disable if NUM is 0 (default). Mutually incompatible with `-g`.");
    this->add_option('y', "num-horiz-axes", required_argument, "Draw NUM horizontal axes, equally spaced across the entire picture height. Disable if NUM is 0 (default). Mutually incompatible with `-g`.");
    this->description =
      "Draw a picture of the nonzero pattern of the INPUT matrix into the\n"
      "OUTPUT stream in SVG format.\n"
      "\n"
      "Normally, one entry in the INPUT matrix corresponds to one single\n"
      "square 'dot' in the OUTPUT picture.  The size of the 'dot' in pixels\n"
      "can be set with the `--block-size` option.\n"
      "\n"
      "For large matrices, it is possible to shrink the OUTPUT picture, by\n"
      "mapping a square NxN tile of the matrix into a single dot.  Option\n"
      "`--shrink` specifies the size N of input tiles. Alternatively, options\n"
      "`--shrink-to-width` (resp. `--shrink-to-height`) allow setting the\n"
      "tile size so that the OUTPUT picture does not exceed the specified\n"
      "width (resp. height), expressed in pixels.  The `--shrink`,\n"
      "`--shrink-to-width` and `--shrink-to-height` options are mutually\n"
      "conflicting; if more than one is specified, the last takes precedence.\n"
      "\n"
      "When shrinking the matrix, pixel color intensity is proportional to\n"
      "the number of nonzeroes in each INPUT tile.  For very sparse matrices,\n"
      "option `--darken` allows to enhance the contrast: given a\n"
      "floating-point number BETA, the number of nonzero INPUT entries found\n"
      "in a tile is raised to the power BETA before computing the intensity.\n"
      "\n"
      "Optionally, grid axes can be drawn on the OUTPUT picture.  Option\n"
      "`--grid` draws a square grid, with axes spaced NUM pixels apart.\n"
      "Option `--num-vert-axes` requires that the specified number of\n"
      "vertical axes are drawn, equally spaced apart.  Option\n"
      "`--num-horiz-axes` does the same for horizontal axes.  Passing an\n"
      "argument 0 to each of these options turns off drawing axes.\n"
      ;
  };

  void process_option(const int opt, const char* argument)
  {
    if ('b' == opt)
      std::istringstream(argument) >> size_;
    else if ('c' == opt)
      entry_color_ = argument;
    else if ('d' == opt)
      std::istringstream(argument) >> darken_;
    else if ('g' == opt) {
      std::istringstream(argument) >> xticks_;
      yticks_ = xticks_;
    }
    else if ('j' == opt)
      grid_color_ = argument;
    else if ('k' == opt)
      frame_color_ = argument;
    else if ('s' == opt)
      std::istringstream(argument) >> shrink_;
    else if ('t' == opt)
      std::istringstream(argument) >> max_height_;
    else if ('w' == opt)
      std::istringstream(argument) >> max_width_;
    else if ('x' == opt) {
      // a negative value for `xticks_` is interpreted as a request to
      // create that many equally-spaced axes
      std::istringstream(argument) >> xticks_;
      xticks_ = -xticks_;
    }
    else if ('y' == opt) {
      // a negative value for `yticks_` is interpreted as a request to
      // create that many equally-spaced ayes
      std::istringstream(argument) >> yticks_;
      yticks_ = -yticks_;
    };
  };

  int run() {
    // open INPUT file early, so we can detect format errors and abort
    // before printing anything to the output
    SMSReader<val_t>::open(*FilterProgram::input_);

    // size computations for options -t, -w, -x, -y
    coord_t nrows = SMSReader<val_t>::rows();
    coord_t ncols = SMSReader<val_t>::columns();

    if (0 != max_width_)
      shrink_ = ncols / (max_width_ / size_);

    if (0 != max_height_)
      shrink_ = nrows / (max_height_ / size_);

    if (xticks_ < 0) {
      const coord_t divs = -xticks_;
      xticks_ = (ncols/shrink_/divs);
    };

    if (yticks_ < 0) {
      const coord_t divs = -yticks_;
      yticks_ = (nrows/shrink_/divs);
    };

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

    // draw frame around matrix
    (*output_)
      << "<rect class=\"MatrixFrame\" x=\"0\" y=\"0\""
      << " height=\"" << size_ * (1 + nrows / shrink_) << "\""
      << " width=\"" << size_ * (1 + ncols / shrink_) << "\""
      << " />"
      << std::endl;

    // draw ticks marks
    //

    // vertical axes (x-ticks)
    xticks_ *= size_;
    yticks_ *= size_;
    int const max_digits = (1 + floor(log10(std::max(nrows, ncols) / shrink_)));
    const coord_t pt = (std::min(xticks_, yticks_) / (2 * max_digits));

    if (0 != xticks_)
      {
        const coord_t dy = (xticks_ / 3);
        for (coord_t j = xticks_; j < ncols*size_/shrink_; j += xticks_) {
          // vertical axis
          (*output_)
            << "<line class=\"MatrixSeparator\""
            << " x1=\"" << j << "\""
            << " y1=\"" << 0 - dy << "\""
            << " x2=\"" << j << "\""
            << " y2=\"" << nrows*size_/shrink_ + dy << "\""
            << " />"
            << std::endl;
          // top label
          (*output_)
            << "<text font-size=\"" << pt << "\""
            << " x=\"" << j << "\""
            << " y=\"" << 0 - dy << "\">\n"
            << "<tspan>\n"
            << (j*shrink_ / size_)
            << "</tspan>\n"
            << "</text>\n"
            << std::endl;
          // bottom label
          (*output_)
            << "<text font-size=\"" << pt << "\""
            << " x=\"" << j << "\""
            << " y=\"" << nrows*size_/shrink_ + dy << "\">\n"
            << "<tspan>\n"
            << (j*shrink_ / size_)
            << "</tspan>\n"
            << "</text>\n"
            << std::endl;
        };
      }; // if (0 != xticks_)

    // draw horizontal axes (y-ticks)
    if (0 != yticks_)
      {
        const coord_t dx = (yticks_ / 3);
        for (coord_t i = yticks_; i < (nrows * size_ / shrink_); i += yticks_) {
          (*output_)
            << "<line class=\"MatrixSeparator\""
            << " x1=\"" << 0 << "\""
            << " y1=\"" << i << "\""
            << " x2=\"" << ncols*size_/shrink_ << "\""
            << " y2=\"" << i << "\""
            << " />"
            << std::endl;
          (*output_)
            << "<text font-size=\"" << pt << "\""
            << " y=\"" << i << "\""
            << " x=\"" << 0 - dx - max_digits*pt << "\">\n"
            << "<tspan>\n"
            << (i*shrink_ / size_)
            << "</tspan>\n"
            << "</text>\n"
            << std::endl;
          (*output_)
            << "<text font-size=\"" << pt << "\""
            << " y=\"" << i << "\""
            << " x=\"" << ncols*size_/shrink_ + dx << "\">\n"
            << "<tspan>\n"
            << (i*shrink_ / size_)
            << "</tspan>\n"
            << "</text>\n"
            << std::endl;
        };
      };

    // one rect per matrix entry
    read();
    for(matrix_density_t::const_iterator r = m_.begin(); r != m_.end(); ++r)
      for(row_density_t::const_iterator c = r->second.begin(); c != r->second.end(); ++c) {
        const coord_t i = r->first;
        const coord_t j = c->first;
        (*output_)
          << "<rect class=\"MatrixEntry\""
          << " style='opacity:" << std::pow(m_[i][j], darken_) / (shrink_ * shrink_) << "'"
          << " width=\"" << size_ << "\" height=\"" << size_ << "\""
          << " x=\"" << (j*size_) << "\""
          << " y=\"" << (i*size_) << "\""
          << " />"
          << std::endl;
      };

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
      m_[i / shrink_][j / shrink_] += 1;
  };


private:
  coord_t      size_;
  coord_t      shrink_;
  coord_t      max_width_;
  coord_t      max_height_;
  double       darken_;
  coord_t      xticks_;
  coord_t      yticks_;
  std::string  entry_color_;
  std::string  frame_color_;
  std::string  grid_color_;

  /// a sparse row is just a map from column index to value
  typedef std::map< coord_t, double > row_density_t;
  /// a sparse matrix is a map from row index to sparse row
  typedef std::map< coord_t, row_density_t > matrix_density_t;
  /// matrix data (as read from the stream)
  matrix_density_t m_;
};


int main(int argc, char** argv)
{
  return SvgProgram().main(argc, argv);
};
