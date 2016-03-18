#include <bcm/error.h>

#ifndef AREFHELPER_H
#define AREFHELPER_H


#define APICALLCHK(cmd, arg) { int rv; if ( (rv = (cmd arg)) < 0 ) { \
                               printk("Error %d : %s %s at file %s line %d\n", rv, #cmd, bcm_errmsg(rv), __FILE__, __LINE__); \
                               return rv; } }

#define APICALLCHK_NR(cmd, arg) { int rv; if ( (rv = (cmd arg)) < 0 ) { \
                               printk("Error %d : %s at file %s line %d\n", rv, #cmd,__FILE__, __LINE__); return; } }



#endif
