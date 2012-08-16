#ifndef SDB_H
#define SDB_H

#endif // SDB_H
#include "rllib/rlserial.h"

struct s_sdb_recode{
 
    int     S_ID;       //  int(32) unsigned
    QString PNAME;      // char(255)
    QString NOTE;       // char(255)
    int     I_NKP,      //  int(16) unsigned
            I_NSKO,     //  int(16) unsigned
            I_IDGR,     //  int(16) unsigned
            I_NR,       //  int(16) unsigned
            O_GROUPL,   // int(16) unsigned
            O_NRLAMP,   //  int(16) unsigned
            O_GROUPK,   //  int(16) unsigned
            O_NRKEY,    //  int(16) unsigned
            O_KEY_INV,  //  tinyint(1) unsigned
            I_PORT_ADAM,//  int(16) unsigned
            I_ADR_ADAM, //  int(16) unsigned
            I_NR_ADAM,  //  int(16) unsigned
            O_PORT_ADAM,//  int(16) unsigned
            O_ADR_ADAM, //  int(16) unsigned
            O_NR_ADAM,  //  int(16) unsigned
            A_TYPE;     //  int(16) unsigned
    QString TAGE_LAMP,  //  char(255)
            TAGE_KEY,   //  char(255)
            TAGE_TIT;    // char(255)
    int     state_key;
    QString str_type;
    
};

struct s_kp{
	
    int     KPID,
            NKP;
    QString KPCIPHER,
            KPNAME;
    int     TU,
            DKDS;
};

// Работа с сом портами
struct sComADAM{
       rlSerial *port[64];
};



