#include "tap.h"

char char_offset(int off, char ch)
{
	return (ch + off);
}

int add(int a, int b, int c)
{
	return a + b + c;
}

int main () 
{
	int i;
	char ch = 'a';
	char ret = 'a';
	plan(10);
	ok(50 + 5, "foo %s", "bar");
	for (i = 0; i < 20; i++) {
		ok(++ret == char_offset(1,ch++), "off = %d  ch = %c", 1, ch);
	}
	ok( 6 == add(1,2,3), "%d %d %d", 1, 2, 3);
	done_testing();
}


