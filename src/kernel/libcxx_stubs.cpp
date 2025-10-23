extern "C"
{
    int __cxa_guard_acquire(unsigned long long *guard) { return 1; }
    void __cxa_guard_release(unsigned long long *guard) {}
    void __cxa_guard_abort(unsigned long long *guard) {}
    void *__dso_handle = (void*)0;
    int __cxa_atexit(void (*f)(void *), void *obj, void *dso) { return 0; }
    void __cxa_finalize(void *f) {}
}