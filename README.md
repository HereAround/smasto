SMaSTo (Sparse MAtrix Storage TOols) is a set of command-line
utilities to manipulate matrix files in the
[SMS format](http://www-ljk.imag.fr/membres/Jean-Guillaume.Dumas/simc.html)
(used for instance by the [LinBox](http://www.linalg.org)
linear algebra library).

Tools currently included in SMaSTo include:

* `sms-adjoin`: stack matrices or adjoin them side-by-side
* `sms-info`: print matrix dimensions, number of nonzeroes, and fill-in percentage.
* `sms-norm`: compute matrix norm (choose between L<sup>1</sup>, L<sup>2</sup>, or L<sup>\infty</sup> metric).
* `sms-random`: generate a random sparse matrix of given density.
* `sms-reord`: Permute matrix rows to speedup Gaussian Elimination.
* `sms-rescale`: Copy matrix, multiplying all entries by a scale factor.
* `sms-shrink`: Remove rows and columns consisting entirely of zeroes.
* `sms-transpose`: Transpose matrix.

For more details, installation and usage instructions, visit the [docs](docs/).
Comments and patches are welcome!


## Copyright and license

Copyright (c) 2010-2015 Riccardo Murri <mailto:riccardo.murri@gmail.com>

*smasto* is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

-- Riccardo Murri <riccardo.murri@gmail.com>
