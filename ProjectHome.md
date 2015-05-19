SMaSTo (Sparse MAtrix Storage TOols) is a set of command-line
utilities to manipulate matrix files in the
[SMS format](http://www-ljk.imag.fr/membres/Jean-Guillaume.Dumas/simc.html)
(used for instance by the [LinBox](http://www.linalg.org)
linear algebra library).

Tools currently included in SMaSTo include:

  * `sms-norm`: compute matrix norm (choose between L<sup>1</sup>, L<sup>2</sup>, or L<sup>\infty</sup> metric).
  * `sms-info`: print matrix dimensions, number of nonzeroes, and fill-in percentage.
  * `sms-random`: generate a random sparse matrix of given density.
  * `sms-reord`: Permute matrix rows to speedup Gaussian Elimination.
  * `sms-rescale`: Copy matrix, multiplying all entries by a scale factor.
  * `sms-shrink`: Remove rows and columns consisting entirely of zeroes.
  * `sms-transpose`: Transpose matrix.

For more details, installation and usage instructions, visit the [Wiki](Home.md).