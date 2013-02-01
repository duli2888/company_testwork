#include <stdio.h>

enum hvg_eu_filetype_t {
	HVGBIN=100,
	HVGBINGZ,
	HVGTAR,
	HVGTARGZ,
	HVGUNKNOWN,
};

int main() {
	enum hvg_eu_filetype_t ft;

	ft = HVGBINGZ;

	printf("filetype %d\n", ft);
}
