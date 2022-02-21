/*
 *	Square root by abacus algorithm, Martin Guy @ UKC, June 1985.
 *	From a book on programming abaci by Mr C. Woo.
 *	Argument is a positive integer, as is result.
 *
 *	I have formally proved that on exit:
 *		   2		   2		   2
 *		res  <= x < (res+1)	and	res  + op == x
 *
 *	This is also nine times faster than the library routine (-lm).
 */

int isqrt(x)
int x;
{
  /*
   *	Logically, these are unsigned. We need the sign bit to test
   *	whether (op - res - one) underflowed.
   */

  register int op, res, one;

  op = x;
  res = 0;

  /* "one" starts at the highest power of four <= than the argument. */

  one = 1 << 30; /* second-to-top bit set */
  while (one > op) one >>= 2;

  while (one != 0) {
    if (op >= res + one) {
      op = op - (res + one);
      res = res + 2 * one;
    }
    res /= 2;
    one /= 4;
  }
  return (res);
}
