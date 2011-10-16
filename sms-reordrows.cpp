/**
 * @file   sms-reord.cpp
 *
 * Permute rows and columns of the input matrix, in order to minimize
 * computation time of the rank by Gaussian Elimination algorithms.
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

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>


// matrix dimensions should fit into a `long` integer type
typedef long coord_t;

// always select the widest floating-point type available
# ifdef HAVE_LONG_DOUBLE
typedef long double val_t;
# else
typedef double val_t;
# endif 


class ReordRowsProgram : public FilterProgram, 
                         public SMSReader<val_t>,
                         public SMSWriter<val_t>
{
public:
  ReordRowsProgram() 
    : m(), r(), c(),
      a_(4.5), b_(2.0), c_(1.0), d_(2.0), e_(0.5)
  {
    std::ostringstream a_msg; a_msg << "Assign weight ARG (default: " << a_<< ") to criterion a.";
    this->add_option('a', "weight-a", required_argument, a_msg.str());
    std::ostringstream b_msg; b_msg << "Assign weight ARG (default: " << b_<< ") to criterion b.";
    this->add_option('b', "weight-b", required_argument, b_msg.str());
    std::ostringstream c_msg; c_msg << "Assign weight ARG (default: " << c_<< ") to criterion c.";
    this->add_option('c', "weight-c", required_argument, c_msg.str());
    std::ostringstream d_msg; d_msg << "Assign weight ARG (default: " << d_<< ") to criterion d.";
    this->add_option('d', "weight-d", required_argument, d_msg.str());
    std::ostringstream e_msg; e_msg << "Assign weight ARG (default: " << e_<< ") to criterion e.";
    this->add_option('e', "weight-e", required_argument, e_msg.str());
    this->description = 
      "Permute rows and columns of the input matrix, in order to minimize\n"
      "computation time of the rank by Gaussian Elimination algorithms.\n"
      "The algorithm chooses how to reorder the rows by using five criteria:\n"
      "   a. percentage of total nonzero entries in row\n"
      "   b. no. of nonzero entries in columns before the diagonal\n"
      "   c. no. of nonzero entries in columns after the diagonal\n"
      "   d. no. of nonzero entries such that no previous row has a nonzero entry\n"
      "      in the same column\n"
      "   e. distance between column `i` and the first nonzero in column > `i`\n"
      "The reordering favors (i.e., moves near the top) those rows that\n"
      "minimize criteria a., b., c., d., and maximize criterion e.\n"
      "The relative weight of each criterion can be changed with options '-a', -b',\n"
      "'-c', '-d', '-e', each of which takes a single floating-point argument.\n"
      ;
  };

  void process_option(const int opt, const char* argument)
  {
    if ('a' == opt)
      std::istringstream(argument) >> a_;
    else if ('b' == opt)
      std::istringstream(argument) >> b_;
    else if ('c' == opt)
      std::istringstream(argument) >> c_;
    else if ('d' == opt)
      std::istringstream(argument) >> d_;
    else if ('e' == opt)
      std::istringstream(argument) >> e_;
  };

  int run() { 
    // normalize weights
    double t = std::abs(a_) + std::abs(b_) + std::abs(c_) + std::abs(d_) + std::abs(e_);
    a_ /= t;
    b_ /= t;
    c_ /= t;
    d_ /= t;
    e_ /= t;

    SMSReader<val_t>::open(*FilterProgram::input_);
    const coord_t nrows = SMSReader<val_t>::rows();
    const coord_t ncols = SMSReader<val_t>::columns();

    // initialize row and column counts to zero; note indices are 1-based
    r.resize(nrows+1, 0); 
    c.resize(ncols+1, 0);

    // read and process matrix entries
    read();
    SMSReader<val_t>::close();

    // f[j] is `true` iff a non-zero has been already seen in column j
    std::vector<bool> f(ncols+1, false); 

    coord_t max_r = 0;
    for (coord_t i = 1; i <= nrows; ++i)
      if(r[i] > max_r)
        max_r = r[i];
    
    for (coord_t i = 1; i <= nrows; ++i) {
      // select and weight rows according to these criteria:
      //   0. they must have a non-zero in some column `j` >= `i`
      //   1. minimize number of nonzero entries in columns < `i`
      //   2. minimize number of nonzero entries in columns >= `i`
      //   3. minimize number of nonzero entries in columns `j` such that `f[j]` is `true`
      //   4. maximize the distance between column `i` and the first nonzero in column > `i`
      coord_t chosen_i = -1; // invalid index, initially
      coord_t chosen_j = -1; // invalid index, initially
      double badness = 1000.0; // max. attainable weight
      for (coord_t ii = i; ii <= nrows; ++ii) {
        // 0. ignore zero rows
        if (0 == r[ii])
          continue;

        // pivot column `j` is the one having minimal number of nozeroes
        coord_t j = -1;
        coord_t cj = nrows;
        for (row_t::const_iterator jj = m[ii].begin(); jj != m[ii].end(); ++jj) {
          if (jj->first < ii)
            continue;
          if ((c[jj->first] < cj)
              or (c[jj->first] == cj 
                  and (0 == m[ii].count(j) // i.e. m[ii][j] == 0
                       or jj->second < m[ii][j]))) 
            {
              j = jj->first;
              cj = c[jj->first];
            };
        }; // end for(jj = m[ii].begin(); ...)

        coord_t c1 = 0;
        coord_t c2 = 0;
        coord_t c3 = 0;
        coord_t l = ncols;
        for (row_t::const_iterator jj = m[ii].begin(); jj != m[ii].end(); ++jj) {
          // 1. count nonzero entries in columns < `i`
          if (jj->first < i)
            ++c1;
          else {
            // 2. count nonzero entries in columns > `i`
            ++c2;
            // 4. compute "distance" between `i` and first nonzero entry
            if (jj->first != j and (jj->first - i) < l)
              l = jj->first - i;
          };
          // 3. count nonzero entries such that `f[j]` is not `true`
          if (not f[jj->first])
            ++c3;
        };

        // compute row weight
        double badness_ =
          (100.0 * r[ii] / max_r) * a_ // percentage of nonzeroes
          + (100.0 * c1 / r[ii])  * b_ // fraction of nonzeroes in lower half
          + (100.0 * c2 / r[ii])  * c_ // fraction of nonzeroes in upper half
          + (100.0 * c3 / r[ii])  * d_ // fraction of "new" nonzeroes
          + (100.0 * exp(-1.0 * ncols / l)) * e_ // "distance" of first nonzero
          ;

        if (badness_ < badness) {
          chosen_i = ii;
          chosen_j = j;
          badness = badness_;
        };
      };

      // now do the swapping
      if(-1 == chosen_i) // all rows are zero
        break;
      assert(r[chosen_i] == m[chosen_i].size());
      std::swap(m[chosen_i], m[i]);
      std::swap(r[chosen_i], r[i]);
      if (-1 != chosen_j) {
        std::swap(c[chosen_j], c[i]);
        for (coord_t ii = 1; ii <= nrows; ++ii) {
          bool has_i = m[ii].count(i);
          bool has_chosen_j = m[ii].count(chosen_j);
          if (has_i and has_chosen_j)
            std::swap(m[ii][i], m[ii][chosen_j]);
          else if (has_i) {
            val_t value = m[ii][i];
            m[ii].erase(i);
            m[ii][chosen_j] = value;
          }
          else if (has_chosen_j) {
            val_t value = m[ii][chosen_j];
            m[ii].erase(chosen_j);
            m[ii][i] = value;
          };
        };
      };
      // update nonzero mask
      for (row_t::const_iterator j = m[i].begin(); j != m[i].end(); ++j)
        f[j->first] = true;
    };
  
    // output matrix
    SMSWriter<val_t>::open(*FilterProgram::output_, nrows, ncols);
    for(matrix_t::const_iterator r = m.begin(); r != m.end(); ++r)
      for(row_t::const_iterator c = r->second.begin(); c != r->second.end(); ++c)
        write_entry(r->first, c->first, c->second);
    SMSWriter<val_t>::close();

    return 0; 
  };

  void process_entry(const coord_t i, const coord_t j, const val_t& value)
  {
    m[i][j] = value;
    r[i] += 1;
    c[j] += 1;
  };


private:
  // weights for the various criteria
  double a_, b_, c_, d_, e_;

  // a sparse row is just a map from column index to value
  typedef std::map< coord_t, val_t > row_t;
  typedef std::map< coord_t, row_t > matrix_t;
  matrix_t m;

  /// number of elements on row `i` (old value)
  std::vector<coord_t> r;

  /// number of elements on column `j`(old value)
  std::vector<coord_t> c;
};


int main(int argc, char** argv)
{
  return ReordRowsProgram().main(argc, argv);
};
