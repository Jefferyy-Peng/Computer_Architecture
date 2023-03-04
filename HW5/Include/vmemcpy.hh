#ifndef _VMEMCPY_HH_
#define _VMEMCPY_HH_

namespace pipelined
{
    void *vmemcpy(void *dest, const void *src, size_t n);
};

#endif
