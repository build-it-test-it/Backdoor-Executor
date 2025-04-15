// objc/runtime.h stub for CI builds
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Types
typedef void* id;
typedef void* Class;
typedef void* SEL;
typedef void* IMP;
typedef void* Method;
typedef void* Protocol;
typedef void* objc_property_t;

// Functions
SEL sel_registerName(const char* name);
Class objc_getClass(const char* name);
Class objc_getMetaClass(const char* name);
IMP class_getMethodImplementation(Class cls, SEL name);
Method class_getInstanceMethod(Class cls, SEL name);
Method class_getClassMethod(Class cls, SEL name);
IMP method_getImplementation(Method m);
void method_exchangeImplementations(Method m1, Method m2);
IMP method_setImplementation(Method m, IMP imp);
const char* class_getName(Class cls);
BOOL class_addMethod(Class cls, SEL name, IMP imp, const char* types);

#ifdef __cplusplus
}
#endif
