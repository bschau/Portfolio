/*
 * This RNG is a simple and fast XOR-shift based pseudo random number generator.
 * The algorithm is specified in the paper:
 *
 * 	Marsaglia, George (2003) Xorshift RNGS:
 *
 * 	http://www.jstatsoft.org/v08/i14/xorshift.pdf
 *
 * Tests have shown that ca. 3% of the generated numbers are duplicates.
 */
#include "Include.h"

static UInt32 y=842502087U, z=3579807591U, w=273326509U, x;

/*
 * Random
 *
 * Return a random number.
 *
 *  -> seed		Use this seed. If seed is 0, no seed is used.
 */
UInt32
Random(UInt32 seed)
{
	UInt32 t;
	
	if (seed)
		x=seed;
	
	t=(x^(x<<11));
	x=y;
	y=z;
	z=w;
	w=(w^(w>>19))^(t^(t>>8));

	return w;
}
