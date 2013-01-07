/* this .h file for debug
 * [2012-10-18:DuLi]
 */

#ifndef DLDEBUG
#   define DLPRINTF(fmt, ...) do { } while (0)
#else
#   define DLPRINTF(fmt, ...) \
	do {\
		printf("[DLdebug]");\
		printf("%s@:%d ", __func__, __LINE__);\
		printf(fmt, ##__VA_ARGS__);\
	} while (0)
#endif
