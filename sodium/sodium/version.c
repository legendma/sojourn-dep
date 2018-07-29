
//#include "version.h"

const char *
sodium_version_string(void)
{
    return 0;
}

int
sodium_library_version_major(void)
{
    return 0;
}

int
sodium_library_version_minor(void)
{
    return 0;
}

int
sodium_library_minimal(void)
{
#ifdef SODIUM_LIBRARY_MINIMAL
    return 1;
#else
    return 0;
#endif
}
