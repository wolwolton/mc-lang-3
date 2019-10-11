extern "C" int MyMain(unsigned long (*edk_print)(const char16_t* format, ...) __attribute__((ms_abi))){
    int f(int);
    edk_print(u"f()= %lu\n",f(5));
    return 0;   
}
