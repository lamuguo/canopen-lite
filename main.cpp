#include <stdio.h>
#include "glog/logging.h"

int main(int argc, char* argv[]) {
    google::InitGoogleLogging(argv[0]);
    LOG(INFO) << "Hello, glog!";
    printf("Hello world!\n");
    return 0;
}
