/**
 * @file   common.hpp
 *
 * Utility classes for manipulating SMS-format matrices.
 *
 * @author  riccardo.murri@gmail.com
 * @version $Revision$
 */
/*
 * Copyright (c) 2010-2012 riccardo.murri@gmail.com. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * See http://www.gnu.org/licenses/gpl.html for licence details.
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

#ifndef COMMON_HPP
#define COMMON_HPP


#include "config.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <errno.h>
#include <getopt.h>


/** Helper class to keep a pointer to std::cout or a std::ifstream
    instance, and still do the right thing when the object goes out of
    scope.  The destructor will delete the std::ifstream/std::ofstream
    (thereby closing it) but do nothing for std::cout/std::cerr. */
template< typename T >
class pointer
{
public:
  pointer(T& tref) { ptr_ = &tref; owned_ = false; };
  pointer(T* tptr) { ptr_ = tptr;  owned_ = true; };
  pointer() { ptr_ = NULL; owned_ = false; };
  ~pointer() { release(); };

  pointer<T>& operator=(T& tref) { ptr_ = &tref; owned_ = false; return *this; }
  pointer<T>& operator=(T* tptr) { ptr_ = tptr;  owned_ = true;  return *this; }

  T& operator*()  { return *ptr_; };
  T* operator->() { return ptr_; };

  bool owned()   { return (owned_ == true); };
  void disown()  { owned_ = false; };
  void release() { if(owned_) delete ptr_; disown(); ptr_ = NULL; };

private:
  T* ptr_;
  bool owned_;
};


/** Abstract base class for implementing an SMS-format file processor.
    Derived classes need implement the @c process_entry method, which
    is invoked once for each value read from the SMS stream. */
template< typename val_t, typename coord_t = long >
class SMSReader
{
public:
  /** Constructor. */
  SMSReader();
  /** Destructor: closes the given input stream. Override in derived classes. */
  virtual ~SMSReader();

  /** Read SMS header from the given input stream; a subsequent @ref
      read() will read matrix entries from this same stream, until
      @ref close() is called. */
  void open(std::istream& input);
  /** Read SMS header from the given input file; a subsequent @ref
      read() will read matrix entries from this same file, until
      @ref close() is called. */
  void open(const std::string& filename);

  /** Read and process all matrix entries in the opened stream. */
  void read();

  /** Return number of matrix rows. (As read from the most recently-opened stream.) */
  coord_t rows() const { return nrows_; };
  /** Return number of matrix columns. (As read from the most recently-opened stream.) */
  coord_t columns() const { return ncols_; };

  /** Finish reading matrix entries from the given stream. */
  void close();

protected:
  /** Process a single entry in the stream. */
  virtual void process_entry(const coord_t row, const coord_t column, const val_t& value) = 0;

  /** Called by @ref read() when it hits the end-of-stream marker. */
  virtual void done() { };

  pointer<std::istream> input_;
  coord_t nrows_;
  coord_t ncols_;
};


/** Helper class for writing out a stream of entries in SMS matrix format. */
template< typename val_t, typename coord_t = long >
class SMSWriter
{
public:
  /** Constructor, taking output stream. */
  SMSWriter();
  /** Destructor: closes the passed stream. */
  ~SMSWriter();

  /** Begin writing matrix stream to the given output stream. */
  void open(std::ostream& out, const coord_t nrows, const coord_t ncols);
  /** Begin writing matrix stream to the given file. */
  void open(const std::string& filename, const coord_t nrows, const coord_t ncols);

  /** Output a single matrix entry to the currently-open output stream. */
  void write_entry(const coord_t row, const coord_t col, const val_t& value);
  
  /** Finish writing matrix entries to the given stream.  Output
      end-of-data marker and close stream if necessary. */
  void close();

protected: 
  pointer<std::ostream> output_;
};


/** Common code for implementing a UNIX filter program.  Parses a
    command line invocation of the form "prog [options] [INPUT
    [OUTPUT]]" and then calls the @ref process method with open
    streams connected to INPUT and OUTPUT.  If omitted, INPUT defaults
    to @c std::cin and OUTPUT to @c std::cout. Common accepted options
    include @c --help, @c --verbose, @c --input and @c --output. */
class FilterProgram
{
public:
  /** Constructor. */
  FilterProgram();
  /** Destructor, override in derived classes. */
  virtual ~FilterProgram();

  /** Descriptive text, used in usage help text. */
  std::string description;

  /** Define a new option to be processed.  The @p short_name
      parameter must be a printable character; @p long_name has to be
      a nonempty string. */
  void add_option(const char short_name, const std::string& long_name, 
                  int has_arg, const std::string& description);

  /** Called when an option defined with @ref add_option has been
      found on the command-line. The @p short_name argument matches
      the like-named parameter used in @ref add_option; the @p
      argument parameter is @c NULL if the option has no argument (in
      case @c has_arg was @c no_argument in the call to @ref
      add_option, or it was @c optional_argument but no argument was
      given on the command line. Throw an exception if the option was
      not processed correctly; its @c what() message will be displayed
      to the user as an error. */
  virtual void process_option(const int short_name, const char* argument) = 0;

  /** Parse positional argument.  Called with what remains of the
      command-line arguments, after options and their arguments have
      been removed; `argc` is updated accordingly.  The default
      implementation looks for (optional) INPUT and OUTPUT arguments
      and sets the input and output streams based on that. */
  virtual void parse_args(int argc, char** argv);

  /** Process stream and return program exitcode.  When this is
      called, the instance members @c input_ and @c output_ are
      connected to the input and output streams as set from the
      command line. Override in subclasses. */
  virtual int run() = 0;

  /** Run filter program with command-line arguments. Return UNIX exit code. */
  int main(int argc, char** argv);

  // XXX: this should be migrated to SMSWriter!
  /** How to write matrix entries to the output stream. */
  typedef enum { DEFAULT_NOTATION, FIXED_NOTATION, SCIENTIFIC_NOTATION } entry_format;

  /** Use the specified format and precision for writing matrix
      entries to the output stream.  If this is never called, the std
      C++ formats apply. */
  void set_output_format(entry_format notation, const int precision);

protected:
  std::vector<struct option> options_;
  std::string optstring_;
  std::map<char, std::string> option_help_;

  pointer<std::istream> input_;
  void set_input(const std::string& filename);

  pointer<std::ostream> output_;
  void set_output(const std::string& filename);

  entry_format notation_;
  int precision_;
};



//
// implementation
//

// ---- SMSReader ----

template< typename val_t, typename coord_t >
SMSReader<val_t,coord_t>::SMSReader() 
  : input_() , nrows_(0), ncols_(0)
{
  // nothing to do
};


template< typename val_t, typename coord_t >
SMSReader<val_t,coord_t>::~SMSReader()
{
  input_.release();
};


template< typename val_t, typename coord_t >
void SMSReader<val_t,coord_t>::open(std::istream& input) 
{ 
  input_ = input;
  char M;
  (*input_) >> std::skipws >> nrows_ >> ncols_ >> M;
  if ('M' != M)
    throw std::runtime_error("Malformed SMS header");
};

template< typename val_t, typename coord_t >
void SMSReader<val_t,coord_t>::open(const std::string& filename) 
{ 
  errno = 0;
  std::ifstream input(filename); 
  if (input.good()) 
    input_ = input; 
  else { 
    std::ostringstream msg;
    msg << "Cannot open file '" << filename << "': " << strerror(errno);
    throw std::runtime_error(msg.str());
  };

  char M;
  (*input_) >> std::skipws >> nrows_ >> ncols_ >> M;
  if ('M' != M) {
    std::ostringstream msg;
    msg << "Malformed SMS header in file '" << filename << "'";
    throw std::runtime_error(msg.str());
  };
};


template< typename val_t, typename coord_t >
void SMSReader<val_t,coord_t>::read()
{
  while (not (*input_).eof()) {
    coord_t i, j; 
    double value;
    (*input_) >> i >> j >> value;
    assert(0 <= i and i <= nrows_);
    assert(0 <= j and j <= ncols_);
    // '0 0 0' is the end-of-stream marker
    if (0 == i and 0 == j and 0 == value) {
      this->done();
      break;
    };
    // process entry
    this->process_entry(i, j, value);
  };
};


template< typename val_t, typename coord_t >
void SMSReader<val_t,coord_t>::close() 
{
  input_.release();
};



// ---- SMSWriter ----

template< typename val_t, typename coord_t >
SMSWriter<val_t,coord_t>::SMSWriter()
  : output_()
{
  // nothing to do
};


template< typename val_t, typename coord_t >
SMSWriter<val_t,coord_t>::~SMSWriter()
{
  output_.release();
};


template< typename val_t, typename coord_t >
void SMSWriter<val_t,coord_t>::open(std::ostream& output, 
                                    const coord_t nrows, const coord_t ncols) 
{ 
  output_ = output;
  (*output_) << nrows <<" "<< ncols <<" M"<< std::endl;
  if (output_->bad())
    throw std::runtime_error("Error writing to stream");
};


template< typename val_t, typename coord_t >
void SMSWriter<val_t,coord_t>::open(const std::string& filename, 
                                    const coord_t nrows, const coord_t ncols) 
{ 
  errno = 0;
  std::ofstream output(filename); 
  if (output.good()) 
    output_ = output; 
  else { 
    std::ostringstream msg;
    msg << "Cannot open file '" << filename << "' for writing: " << strerror(errno);
    throw std::runtime_error(msg.str());
  };

  errno = 0;
  (*output_) << nrows <<" "<< ncols <<" M"<< std::endl;
  if (output_->bad()) { 
    std::ostringstream msg;
    msg << "Error writing to file '" << filename << "': " << strerror(errno);
    throw std::runtime_error(msg.str());
  };
};


template< typename val_t, typename coord_t >
void SMSWriter<val_t,coord_t>::write_entry(const coord_t row, const coord_t col, 
                                           const val_t& value)
{
  (*output_) << row <<" "<< col <<" "<< value << std::endl;
};


template< typename val_t, typename coord_t >
void SMSWriter<val_t,coord_t>::close()
{
  (*output_) << "0 0 0" << std::endl;
  output_.release();
};



// ---- FilterProgram ----

FilterProgram::FilterProgram()
  : description(),
    input_(std::cin), output_(std::cout),
    options_(), optstring_(),
    notation_(DEFAULT_NOTATION), precision_(-1)
{
  assert(options_.empty());

  // end marker for getopt_long
  struct option opt0;
  opt0.name = NULL;
  opt0.has_arg = 0;
  opt0.flag = NULL;
  opt0.val = 0;
  options_.push_back(opt0);

  // common options
  add_option('h', "help",    no_argument, "Print help text.");
  add_option('V', "version", no_argument, "Print version string.");
  add_option('i', "input",   required_argument, "Read input matrix from file ARG.");
  add_option('o', "output",  required_argument, "Write output matrix to file ARG.");
  add_option('p', "precision", required_argument, "Set number of significant digits for printing matrix entry values.");
  add_option('E', "scientific", no_argument, "Output matrix entry values using scientifc notation.");
  add_option('F', "fixed",   no_argument, "Output matrix entry values using fixed notation.");
  add_option('G', "default", no_argument, "Choose fixed or scientific notation based on how large a value is.");
};


FilterProgram::~FilterProgram()
{
  // free up memory used by the long options
        for (std::vector<struct option>::iterator it = options_.begin();
             it != options_.end();
             ++it)
          {
            free(const_cast<char*>(it->name));
          };  
};


void
FilterProgram::add_option(const char short_name,
                          const std::string& long_name,
                          int has_arg,
                          const std::string& description)
{
  struct option lopt;
  lopt.name = strdup(long_name.c_str());
  lopt.has_arg = has_arg;
  lopt.flag = NULL;
  lopt.val = short_name;
  options_.insert(options_.begin(), lopt);

  std::ostringstream sopt;
  sopt << short_name;
  if (required_argument == has_arg)
    sopt << ":";
  else if (optional_argument == has_arg)
    sopt << "::";
  optstring_ += sopt.str();

  option_help_[short_name] = description;
};


void 
FilterProgram::set_input(const std::string& filename)
{
  if (filename == "-") {
    input_ = std::cin;
    return;
  };
  errno = 0;
  std::ifstream* input = new std::ifstream(filename.c_str());
  if (not input->good()) {
    std::ostringstream msg;
    msg << "Cannot open input file '" << filename << "': " 
        << strerror(errno) << ".";
    throw std::runtime_error(msg.str());
  };
  input_ = input;
};


void 
FilterProgram::set_output(const std::string& filename)
{
  if (filename == "-") {
    output_ = std::cout;
    return;
  };
  errno = 0;
  std::ofstream* output = new std::ofstream(filename.c_str());
  if (not output->good()) {
    std::ostringstream msg;
    msg << "Cannot open output file '" << filename << "': " 
        << strerror(errno) << ".";
    throw std::runtime_error(msg.str());
  };
  output_ = output;
};


void 
FilterProgram::set_output_format(entry_format notation, const int precision)
{
  // set output format
  switch(notation) {
  case DEFAULT_NOTATION:    output_->unsetf(std::ios_base::floatfield); break;
  case FIXED_NOTATION:      output_->setf(std::ios_base::fixed); break;
  case SCIENTIFIC_NOTATION: output_->setf(std::ios_base::scientific); break;
  };
  if (precision >= 0)
    output_->precision(precision);
};


int
FilterProgram::main(int argc, char** argv)
{
  if (argc < 2) {
    std::cerr << "Type '" <<argv[0]<< " --help' to get usage help." << std::endl;
    return 1;
  };

  // program name is basename of argv[0]
  std::string invocation(argv[0]);
  std::size_t pos = invocation.rfind('/');
  std::string name;
  if (0 == pos)
    name = invocation;
  else 
    name = invocation.substr(pos+1);
  
  try {
    // parse command-line arguments
    int c;
    try {
      while (true) {
        c = getopt_long(argc, argv, optstring_.c_str(),
                        &(options_[0]), NULL);
        if (-1 == c)
          break;
        else if ('h' == c) {
          std::cout << "Usage: " << name << " [options] [INPUT [OUTPUT]]" << std::endl;
          std::cout << std::endl;
          std::cout << description << std::endl;
          std::cout << "Options:" << std::endl;
          for (std::vector<struct option>::const_iterator it = options_.begin();
               it != options_.end() - 1;
               ++it)
            {
              std::ostringstream optname;
              optname << "-" << static_cast<char>(it->val)
                      << ", --" << it->name;
              if (required_argument == it->has_arg)
                optname <<" ARG";
              else if (optional_argument == it->has_arg)
                optname << " [ARG]";

              std::cout <<" "<< std::setw(24) 
                        << std::setiosflags(std::ios::left) 
                        << optname.str();
              if (option_help_.find(it->val) != option_help_.end())
                std::cout << option_help_[it->val];
              std::cout << std::endl;
            };
          std::cout << std::endl;
          return 0;
        }
        else if ('V' == c) {
          // output conforms to GNU Coding Standards, but is kind of
          // overkill for such a small utility package...
          std::cout << name <<"(" PACKAGE_NAME ")" << PACKAGE_VERSION << std::endl;
          std::cout << 
            "\n"
            "Copyright (C) 2010-2012 Riccardo Murri <riccardo.murri@gmail.com>.\n"
            "\n"
            "License GPLv3+: GNU GPL version 3 or later; see http://gnu.org/licenses/gpl.html\n"
            "This is free software: you are free to change and redistribute it.\n"
            "There is NO WARRANTY, to the extent permitted by law.\n"
            "\n"
            "See " PACKAGE_URL " for more information.\n"
                    << std::endl;
          return 0;
        }
        else if ('i' == c) {
          set_input(optarg);
        }
        else if ('o' == c) {
          set_output(optarg);
        }
        else if ('p' == c) {
          std::istringstream(optarg) >> precision_;
        }
        else if ('E' == c) {
          notation_ = DEFAULT_NOTATION;
        }
        else if ('F' == c) {
          notation_ = FIXED_NOTATION;
        }
        else if ('G' == c) {
          notation_ = DEFAULT_NOTATION;
        }
        else if ('?' == c) {
          std::cerr << "Unknown option; type '" << argv[0] << " --help' to get usage help."
                    << std::endl;
          return 1;
        }
        else {
          process_option(c, optarg);
        };
      }; // while(true)
    }
    catch(std::exception& ex) {
      std::ostringstream msg;
      msg << "Error in option '-" << static_cast<char>(c) << "': " << ex.what()
          << " Type '" << argv[0] << " --help' to get usage help."
          << std::endl;
      throw std::runtime_error(msg.str());
    };

    // all option processing done, now parse positional arguments
    if (optind > 0)
      argv[optind-1] = argv[0];
    parse_args(argc - (optind-1), &(argv[optind-1]));

    // now do stuff
    return run();
  }
  catch (std::runtime_error& ex) {
    std::cerr << name << ": ERROR: " << ex.what() << std::endl;
    return 1;
  };
};


void
FilterProgram::parse_args(int argc, char** argv)
{
  // set INPUT, if any
  if (argc > 1) {
    set_input(argv[1]);
  };
  
  // set OUTPUT, if any
  if (argc > 2) {
    set_output(argv[2]);
  };
  set_output_format(notation_, precision_);
  
  // too many arguments
  if (argc > 3) {
    std::ostringstream msg;
    msg << "At most two positional arguments allowed."
        << " Type '" << argv[0] << " --help' to get usage help."
        << std::endl;
    throw std::runtime_error(msg.str());
  };
};


#endif // COMMON_HPP
