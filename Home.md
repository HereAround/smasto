# SMaSTo #

SMaSTo (Sparse MAtrix Storage TOols) is a set of command-line
utilities to manipulate matrix files in the
[SMS format](http://www-ljk.imag.fr/membres/Jean-Guillaume.Dumas/simc.html)
(used for instance by the [LinBox](http://www.linalg.org)
linear algebra library).

SMaSTo utilities accept an extended SMS format: namely, matrix values
can be floating-point numbers, not just integers (and indeed the
**sms-rescale** command will output matrices with fractional entries,
if a non-unity divide factor is specified).  By default, integer
values are still printed out as integer values, but this can be changed with
the `--fixed`/`--scientific` options, together with `--precision=...`
to set the number of significant digits after the decimal comma.




## Installation ##

SMaSTo uses [GNU Autotools](http://en.wikipedia.org/wiki/GNU_build_system)
for compilation; you should be able to build a working version of
SMaSTo on every system with a standard C++ compiler and a working
installation of GNU [autoconf](http://www.gnu.org/software/autoconf/)
and [automake](http://www.gnu.org/software/automake/).

There is no packaged version of SMaSTo at the moment, but you should
be able to get it working by following these steps:

0. Check out the SMaSTo sources with [SubVersion](http://subversion.tigris.org/):
```
    svn checkout http://smasto.googlecode.com/svn/trunk smasto
    cd smasto
```

1. Run autotools to generate the `configure` script and the `Makefile`:
```
    autoreconf -i
```

2. From here on, installation proceeds as with any package using GNU autotools:
```
    ./configure --prefix=/path/to/root/install/directory
    make
    make install
```



## Usage ##

All SMaSTo utilities accept the same invocation format:
```
    sms-program [options] INPUT OUTPUT
```

`INPUT` must be a path to a file containing matrices in J.-G. Dumas' SMS format.
`OUTPUT` is the name of a file where the program output will be stored;
for most SMaSTo commands, the output will be a transformed matrix.
Each program accepts different options, explained in the sections below,
although a great deal of the options are common.

You can leave out just `INPUT` or both `INPUT` and `OUTPUT`.  If
`OUTPUT` is omitted, the program output is directed to the standard
output stream (normally, the terminal display); if `INPUT` is omitted,
the input matrix is read from the standard input stream.

If `INPUT` (resp. `OUTPUT`) is `-`, then the standard input stream
(resp. standard output stream) is used.

This allows composing SMaSTo utilities using the shell's pipelining mechanism:
```
    # transpose a matrix and rescale it
    sms-transpose inputfile.sms | sms-rescale -m 1.5 - outputfile.sms
```

Options `-i` and `-o` (common to all utilities) provide an alternate
way of specifying input and output streams.  If _both_ the INPUT
argument and the `-i` option are provided, only the INPUT argument is
retained.  Similarly for simultaneous specification of OUTPUT and `-o`.


### sms-info ###

Usage: sms-info _options_ _INPUT_ _OUTPUT_

Output information on the matrix given in the INPUT stream: number of
rows and columns, number of nonzero values, fill-in percentage.

Note that OUTPUT is not a matrix here, rather a UNIX stream
where the norm of the matrix will be written to; leave it
out to have the norm printed to your terminal screen.

Options:
|-G, --default      |Choose fixed or scientific notation based on how large a value is.|
|:------------------|:-----------------------------------------------------------------|
|-F, --fixed        |Output matrix entry values using fixed notation.                  |
|-E, --scientific   |Output matrix entry values using scientifc notation.              |
|-p, --precision ARG|Set number of significant digits for printing matrix entry values.|
|-o, --output ARG   |Write output matrix to file ARG.                                  |
|-i, --input ARG    |Read input matrix from file ARG.                                  |
|-V, --version      |Print version string.                                             |
|-h, --help         |Print help text.                                                  |


### sms-norm ###

Usage: sms-norm _options_ _INPUT_ _OUTPUT_

Output the norm of the matrix given in the INPUT stream.
Note that OUTPUT is not a matrix here, rather a UNIX stream
where the norm of the matrix will be written to; leave it
out to have the norm printed to your terminal screen.

Options allow to choose whether the L<sup>1, L</sup>2 or L^\infty
norm should be computed.  The INPUT matrix stream should be
in J.-G. Dumas' SMS format.

Options:
|-m, --max          |Compute L<sup>\infty</sup> norm                                            |
|:------------------|:--------------------------------------------------------------------------|
|-2, --l2           |Compute L<sup>2</sup> norm                                                 |
|-1, --l1           |Compute L<sup>1</sup> norm                                                 |
|-G, --default      |Choose fixed or scientific notation based on how large a value is.         |
|-F, --fixed        |Output matrix entry values using fixed notation.                           |
|-E, --scientific   |Output matrix entry values using scientifc notation.                       |
|-p, --precision ARG|Set number of significant digits for printing matrix entry values.         |
|-o, --output ARG   |Write output matrix to file ARG.                                           |
|-i, --input ARG    |Read input matrix from file ARG.                                           |
|-V, --version      |Print version string.                                                      |
|-h, --help         |Print help text.                                                           |


### sms-randminor ###

Usage: sms-randminor _options_ _INPUT_ _OUTPUT_

Extract a minor of the given input matrix (with dimensions specified.
by the '-R' and '-C' options), obtained by randomly selecting rows
and columns, and write it into OUTPUT.  Note that the minor rows and
columns are selected from the whole allowable range, not just the rows
and columns containing nonzero entries: therefore small minors have a
likely chance of being entirely null.

Options:
|-R, --rows ARG     |Number of rows in the minor to extract.                           |
|:------------------|:-----------------------------------------------------------------|
|-C, --columns ARG  |Number of columns in the minor to extract.                        |
|-G, --default      |Choose fixed or scientific notation based on how large a value is.|
|-F, --fixed        |Output matrix entry values using fixed notation.                  |
|-E, --scientific   |Output matrix entry values using scientifc notation.              |
|-p, --precision ARG|Set number of significant digits for printing matrix entry values.|
|-o, --output ARG   |Write output matrix to file ARG.                                  |
|-i, --input ARG    |Read input matrix from file ARG.                                  |
|-V, --version      |Print version string.                                             |
|-h, --help         |Print help text.                                                  |


### sms-random ###

Usage: sms-random [options](options.md) [[OUTPUT](INPUT.md)]

Generate a random sparse matrix of the given size and write it to OUTPUT.
Each entry has a probability of being nonzero equal to the DENSITY.
Entry values are uniformly distributed real numbers between 0 and 1;
use the '-I N' option to generate integer entries in the range 1 to N.

Options:
|-I, --integer ARG  |Matrix has integer entries in the range 1 to ARG.                 |
|:------------------|:-----------------------------------------------------------------|
|-G, --default      |Choose fixed or scientific notation based on how large a value is.|
|-F, --fixed        |Output matrix entry values using fixed notation.                  |
|-E, --scientific   |Output matrix entry values using scientifc notation.              |
|-p, --precision ARG|Set number of significant digits for printing matrix entry values.|
|-o, --output ARG   |Write output matrix to file ARG.                                  |
|-i, --input ARG    |Read input matrix from file ARG.                                  |
|-V, --version      |Print version string.                                             |
|-h, --help         |Print help text.                                                  |


### sms-reord ###

Usage: sms-reord _options_ _INPUT_ _OUTPUT_

Permute rows and columns of the input matrix, in order to minimize
computation time of the rank by Gaussian Elimination algorithms.
The algorithm chooses how to reorder the rows by using five criteria:
  * _a._ percentage of total nonzero entries in row
  * _b._ no. of nonzero entries in columns before the diagonal
  * _c._ no. of nonzero entries in columns after the diagonal
  * _d._ no. of nonzero entries such that no previous row has a nonzero entry in the same column
  * _e._ distance between column `i` and the first nonzero in column > `i`

The reordering favors (i.e., moves near the top) those rows that
minimize criteria _a._, _b._, _c._, _d._, and maximize criterion e.
The relative weight of each criterion can be changed with options `-a`, `-b`,
`-c`, `-d`, `-e`, each of which takes a single floating-point argument.

Note that the reordering algorithm is still experimental and quite
slow; it may well be the case that the total time taken by reordering
the matrix and computing the rank is greater than directly computing
the rank of the initial matrix.

Options:
|-e, --weight-e ARG |Assign weight ARG (default: 0.5) to criterion e.                  |
|:------------------|:-----------------------------------------------------------------|
|-d, --weight-d ARG |Assign weight ARG (default: 2) to criterion d.                    |
|-c, --weight-c ARG |Assign weight ARG (default: 1) to criterion c.                    |
|-b, --weight-b ARG |Assign weight ARG (default: 2) to criterion b.                    |
|-a, --weight-a ARG |Assign weight ARG (default: 4.5) to criterion a.                  |
|-G, --default      |Choose fixed or scientific notation based on how large a value is.|
|-F, --fixed        |Output matrix entry values using fixed notation.                  |
|-E, --scientific   |Output matrix entry values using scientifc notation.              |
|-p, --precision ARG|Set number of significant digits for printing matrix entry values.|
|-o, --output ARG   |Write output matrix to file ARG.                                  |
|-i, --input ARG    |Read input matrix from file ARG.                                  |
|-V, --version      |Print version string.                                             |
|-h, --help         |Print help text.                                                  |


### sms-rescale ###

Usage: sms-rescale _options_ _INPUT_ _OUTPUT_

Output a copy of the matrix given in the INPUT stream,
optionally multiplying each entry by a constant factor.

Both the INPUT and the OUTPUT matrix streams are in J.-G.
Dumas' SMS format.

Options:
|-r, --divide ARG   |Divide each entry by ARG.                                         |
|:------------------|:-----------------------------------------------------------------|
|-m, --multiply ARG |Multiply each entry by ARG.                                       |
|-G, --default      |Choose fixed or scientific notation based on how large a value is.|
|-F, --fixed        |Output matrix entry values using fixed notation.                  |
|-E, --scientific   |Output matrix entry values using scientifc notation.              |
|-p, --precision ARG|Set number of significant digits for printing matrix entry values.|
|-o, --output ARG   |Write output matrix to file ARG.                                  |
|-i, --input ARG    |Read input matrix from file ARG.                                  |
|-V, --version      |Print version string.                                             |
|-h, --help         |Print help text.                                                  |


### sms-shrink ###

Usage: sms-shrink _options_ _INPUT_ _OUTPUT_

Copy INPUT matrix to OUTPUT, removing rows and columns consisting
entirely of zeroes.  Rows and columns are renumbered to preserve the
relative order in which they appear in the INPUT matrix.

Options:
|-G, --default       |Choose fixed or scientific notation based on how large a value is.|
|:-------------------|:-----------------------------------------------------------------|
|-F, --fixed         |Output matrix entry values using fixed notation.                  |
|-E, --scientific    |Output matrix entry values using scientifc notation.              |
|-p, --precision ARG |Set number of significant digits for printing matrix entry values.|
|-o, --output ARG    |Write output matrix to file ARG.                                  |
|-i, --input ARG     |Read input matrix from file ARG.                                  |
|-V, --version       |Print version string.                                             |
|-h, --help          |Print help text.                                                  |


### sms-transpose ###

Usage: sms-transpose _options_ _INPUT_ _OUTPUT_

Output the transpose of the INPUT stream matrix.
If the '-R' or '-C' options are given, the OUTPUT
matrix is a transpose of INPUT only if it matches
the requested condition; it is an exact copy otherwise.

Both the INPUT and the OUTPUT matrix streams are in J.-G.
Dumas' SMS format.

Options:
|-R, --tall          |Only transpose if the output matrix has more rows than columns.   |
|:-------------------|:-----------------------------------------------------------------|
|-C, --wide          |Only transpose if the output matrix has more columns than rows.   |
|-G, --default       |Choose fixed or scientific notation based on how large a value is.|
|-F, --fixed         |Output matrix entry values using fixed notation.                  |
|-E, --scientific    |Output matrix entry values using scientifc notation.              |
|-p, --precision ARG |Set number of significant digits for printing matrix entry values.|
|-o, --output ARG    |Write output matrix to file ARG.                                  |
|-i, --input ARG     |Read input matrix from file ARG.                                  |
|-V, --version       |Print version string.                                             |
|-h, --help          |Print help text.                                                  |


### sms-to-svg ###

Usage sms-to-svg _options_ _INPUT_[OUTPUT](OUTPUT.md)

Create an image (SVG format) of nonzero pattern of the INPUT matrix.
The OUTPUT image has a square block for every nonzero in the matrix;
a box encloses the matrix entries and axes are drawn every 120 entries
to help reading the coordinates on large matrices.

The size of the nonzero squares, their color, as well as the color of
the frame and the axes can be chosen with options.  Colors may be
specified in any format that is valid in an SVG file, e.g., `black` or
`rgb(0,0,0)`.

|-b, --block-size    |Set edge length of nonzero blocks (in pixels).                    |
|:-------------------|:-----------------------------------------------------------------|
|-c, --color C       |Set color of nonzero blocks.                                      |
|-g, --grid NUM      |Draw axes every NUM entries; if NUM is 0, disable drawing axes.   |
|-j, --frame-color C |Set color of the enclosing frame.                                 |
|-k, --grid-color  C |Set color of the axes (if drawn).                                 |
|-o, --output ARG    |Write output to file ARG.                                         |
|-i, --input ARG     |Read input matrix from file ARG.                                  |
|-V, --version       |Print version string.                                             |
|-h, --help          |Print help text.                                                  |