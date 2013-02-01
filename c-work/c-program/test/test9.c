#include <stdio.h>

enum{
	HVGBIN=100,
	HVGBINGZ,
	HVGTAR,
	HVGTARGZ,
	HVGUNKNOWN,
};  


int get_package_type(char *file_name)
{
	char *current = file_name;
	int type;

	while (*current && *current != '.')
		++current;

	++current;

	if (strcmp(current,"bin") == 0) {
		type = HVGBIN;
	} else if (strcmp(current, "bin.gz") == 0) {
		type = HVGBINGZ;
	} else if (strcmp(current, "tar.gz") == 0) {
		type = HVGTARGZ;
	} else if (strcmp(current, "tar") == 0) {
		type = HVGTAR;
	} else {
		type = HVGUNKNOWN;
	}   

	return type;
}


int main()
{
	int ret;
	ret = get_package_type("pack.tar.gz");
	printf("%d\n", ret);

}
