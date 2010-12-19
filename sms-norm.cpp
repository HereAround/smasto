/**
 * @file   sms-norm.cpp
 *
 * Compute the norm of a matrix.
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
#include <sstream>


// matrix dimensions should fit into a `long` integer type
typedef long coord_t;

// always select the widest floating-point type available
# ifdef HAVE_LONG_DOUBLE
typedef long double val_t;
# else
typedef double val_t;
# endif 


class ComputeNormProgram : public FilterProgram
{
public:
  ComputeNormProgram() 
    : metric_(L2_METRIC)
  {
    this->add_option('1', "l1",  no_argument, "Compute L1 norm");
    this->add_option('2', "l2",  no_argument, "Compute L2 norm");
    this->add_option('m', "max", no_argument, "Compute L^\\infty norm");
    this->description = 
      "Output the norm of the matrix given in the INPUT stream.\n"
      "\n"
      "Options allow to choose whether the L^1, L^2 or L^\\infty\n"
      "norm should be computed.  The INPUT matrix stream should be\n"
      "in J.-G. Dumas' SMS format.\n"
      ;
  };

  void process_option(const int opt, const char* argument)
  {
    if ('1' == opt)
      metric_ = L1_METRIC;
    else if ('2' == opt)
      metric_ = L2_METRIC;
    else if ('m' == opt)
      metric_ = LINFTY_METRIC;
  };

  int run() { 
    StreamNormComputer<val_t>* processor;
    switch (metric_) {
    case L1_METRIC: processor = new L1NormComputer<val_t>(); break;
    case L2_METRIC: processor = new L2NormComputer<val_t>(); break;
    case LINFTY_METRIC: processor = new LInftyNormComputer<val_t>(); break;
    };
    processor->open(*input_);
    processor->read();
    (*output_) <<  processor->get_norm() << std::endl;
    return 0; 
  };

  /** Base class for the metric-dependent norm computation. */
  template< typename val_t >
  class StreamNormComputer : public SMSReader<val_t> {
  public:
    virtual val_t get_norm() const = 0;
  };

  template< typename val_t >
  class L1NormComputer : public StreamNormComputer<val_t> {
  public:
    val_t get_norm() const { return norm_; };
  private:
    void process_entry(const coord_t, const coord_t, const val_t& value) {
      norm_ += std::abs(value);
    };
    val_t norm_;
  };

  template< typename val_t >
  class L2NormComputer : public StreamNormComputer<val_t> {
  public:
    val_t get_norm() const { return norm_; };
  private:
    void process_entry(const coord_t, const coord_t, const val_t& value) {
      const val_t absval = std::abs(value);
      norm_ = std::sqrt(norm_ * norm_ + absval * absval);
    };
    val_t norm_;
  };

  template< typename val_t >
  class LInftyNormComputer : public StreamNormComputer<val_t> {
  public:
    val_t get_norm() const { return norm_; };
  private:
    void process_entry(const coord_t, const coord_t, const val_t& value) {
      const val_t absval = std::abs(value);
      if (absval > norm_)
        norm_ = absval;
    };
    val_t norm_;
  };

private:
  enum { 
    L1_METRIC, 
    L2_METRIC, 
    LINFTY_METRIC 
  } metric_;
};


int main(int argc, char** argv)
{
  return ComputeNormProgram().main(argc, argv);
};
