#include <stdio.h>
#include <time.h>

#define LOG(...) do{ \
    time_t rawtime; time(&rawtime); struct tm * timeinfo  = localtime(&rawtime); \
    printf("%02d:%02d:%02d ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec); \
    printf(__VA_ARGS__);printf("\n"); \
  } while(0);

int main() {
  LOG("Hello world!\n");
  return 0;
}
