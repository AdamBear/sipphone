#ifndef ACTIONUR_COMMONDEFINE_H_
#define ACTIONUR_COMMONDEFINE_H_

namespace NS_AU
{

// Disallows copy and assignment.
#define DISALLOW_COPY_AND_ASSIGN(OBJ)  \
    OBJ(const OBJ&);  \
    void operator= (const OBJ&)

typedef int (*ACTIONURI_CONFIRM_PROC)();

}  // namespace NS_AU

// Code type

#endif  // ACTIONUR_COMMONDEFINE_H_
