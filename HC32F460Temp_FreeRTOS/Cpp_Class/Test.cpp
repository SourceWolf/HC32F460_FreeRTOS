#include"Class_SPI.h"
SPI_Class myspi;
#ifdef __cplusplus
extern "C" {
#endif

void Testcpp(void)
{
	myspi.SPI_Open(0,0,Mode0);
}

#ifdef __cplusplus
};
#endif
