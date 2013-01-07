# ifndef DLDEBUG
#define DLPRINTF(fmt, ...) do { } while (0)
#else
#define DLPRINTF(fmt, ...)\
		do {\
			printf("%s@:%d ", __func__, __LINE__);\
				printf(fmt, ##__VA_ARGS__); \
		} while (0)
#endif

