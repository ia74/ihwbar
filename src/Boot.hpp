#ifndef BOOT_HPP
#define BOOT_HPP

namespace Boot
{
    bool Init(
        int &r,
        int &g,
        int &b,
        int &h,
        int &sc,
        int &tcr,
        int &tcg,
        int &tcb,
        int &bx,
        int &by,
        int &bw
    );

    void Welcome();
}

#endif 
