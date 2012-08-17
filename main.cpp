#include <QApplication>
#include <QtGui>
#include <QTextCodec>
#include <QtSql>
#include <QDebug>
#include <QStringList>
#include <iostream>

#include "sdb.h"
#include "window.h"
#include "rllib/rlserial.h"
#include "rllib/rlmodbus.h"

#define MAX_PORT    64
#define SpeedCOM    B115200 //4098//������� ��� ���� modbus
#define TimeoutMODBUS 200  //������� read modbus
#define COUNT_WRITE_MBUS 4
#define VERSION "0.81"

int OpenCom();
int ReadPort(int,int,int);
int WritePort(int,int,int,int);
int Read_KP(void);
void TestBound(int, int, int,int);

// ������ � ��� �������
struct sComADAM ComADAM;
char masCOMPort[MAX_PORT] = {0};

QHash<int,struct s_sdb_recode *> hash_sdb_recode;
QHash<int,struct s_kp *> h_KP;
QHash<int,int> h_ADAM_iConnections;
QHash<int,int> h_ADAM_oConnections;

 int main(int argc, char *argv[])
 {

  QApplication app(argc, argv);

        // ��������� ����� ��������� � �������.
  QTextCodec::setCodecForTr      (QTextCodec::codecForName("Windows-1251"));
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("Windows-1251"));
  QTextCodec::setCodecForLocale  (QTextCodec::codecForName("Windows-1251"));
  int status=-1;

  qWarning() << "Opening database" << endl;

        // ������ ������������� ��������� MySQL.
  QStringList driverList;
  driverList = QSqlDatabase::drivers();
  qDebug() << "Available db drivers: " ;
  QStringList::const_iterator it;
  for (it = driverList.constBegin();it != driverList.constEnd(); ++it)
       qDebug() << (*it).toLocal8Bit().constData() << "   ";
  qDebug() << endl;

        //  ��������� ���� ������ "base" ,��������� ������� QODBC.
        //
  QString namebase = "basedon";
  QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
  db.setDatabaseName(namebase);
         // �������� ������, ���� �� �������.
  if(!(status=db.open()))
      {qDebug() <<  "Cannot open database:" << db.lastError().driverText()
                                            << db.lastError().databaseText();
       messbox("���� ������ "+namebase+" �� �������",db.lastError().driverText());
       return -1; }

  qDebug() <<  "Opened database:" ;
  qDebug() << "database base:\n" << db.tables() << "\n";
            // �� ������� kp ���� ������ ������� ����������
            // ������ - ������� h_KP.
  if ( (status=Read_KP()) !=0 ) return status;

            // ������� sdb_recode �������� �����������  ���������� � ADAM.
            //
  QSqlQuery query;
  if(! query.exec("SELECT * FROM sdb_recode")) {
      qDebug() << "Unable to execute \" query.exec(\"SELECT * FROM sdb_recode\") \"\n ";
      messbox("Unable to execute  query.exec");
      return(-1);
  }
  struct s_sdb_recode *sdb_recode;
  QSqlRecord rec = query.record();
  int s_id;
  int iport, iaddress, ibit;
  int oport, oaddress, obit;
  int itestkey, otestkey;

  while(query.next()){
      sdb_recode= new struct s_sdb_recode;
      s_id=
      sdb_recode->S_ID=query.value(rec.indexOf("S_ID")).toInt();
      sdb_recode->PNAME=query.value(rec.indexOf("PNAME")).toString();
      sdb_recode->NOTE=query.value(rec.indexOf("NOTE")).toString();
      sdb_recode->I_NKP=query.value(rec.indexOf("I_NKP")).toInt();
      sdb_recode->I_NSKO=query.value(rec.indexOf("I_NSKO")).toInt();
      sdb_recode->I_IDGR=query.value(rec.indexOf("I_IDGR")).toInt();
      sdb_recode->I_NR=query.value(rec.indexOf("I_NR")).toInt();
      sdb_recode->O_GROUPL=query.value(rec.indexOf("O_GROUPL")).toInt();
      sdb_recode->O_NRLAMP=query.value(rec.indexOf("O_NRLAMP")).toInt();
      sdb_recode->O_GROUPK=query.value(rec.indexOf("O_GROUPK")).toInt();
      sdb_recode->O_NRKEY=query.value(rec.indexOf("O_NRKEY")).toInt();
      sdb_recode->O_KEY_INV=query.value(rec.indexOf("O_KEY_INV")).toInt();
      iport=
      sdb_recode->I_PORT_ADAM=query.value(rec.indexOf("I_PORT_ADAM")).toInt();
      iaddress =
      sdb_recode->I_ADR_ADAM=query.value(rec.indexOf("I_ADR_ADAM")).toInt();
      ibit =
      sdb_recode->I_NR_ADAM=query.value(rec.indexOf("I_NR_ADAM")).toInt();
      oport=
      sdb_recode->O_PORT_ADAM=query.value(rec.indexOf("O_PORT_ADAM")).toInt();
      oaddress=
      sdb_recode->O_ADR_ADAM=query.value(rec.indexOf("O_ADR_ADAM")).toInt();
      obit=
      sdb_recode->O_NR_ADAM=query.value(rec.indexOf("O_NR_ADAM")).toInt();
      sdb_recode->A_TYPE=query.value(rec.indexOf("A_TYPE")).toInt();
      sdb_recode->TAGE_LAMP=query.value(rec.indexOf("TAGE_LAMP")).toString();
      sdb_recode->TAGE_KEY=query.value(rec.indexOf("TAGE_KEY")).toString();
      sdb_recode->TAGE_TIT=query.value(rec.indexOf("TAGE_TIT")).toString();
      sdb_recode->state_key= -1;

      TestBound(iport, iaddress, ibit, s_id);     // ��������� �� ���������� ��������.
      TestBound(oport, oaddress, obit, s_id);
      
        // ��������� � ������� masCOMPort ������ ���������� ������ ADAM
        // ��� OpenCom().
      if (sdb_recode->O_PORT_ADAM != 0)
          masCOMPort[ sdb_recode->O_PORT_ADAM ] = 1;
      if (sdb_recode->I_PORT_ADAM != 0)
          masCOMPort[ sdb_recode->I_PORT_ADAM ] = 1;

        // ������ � ������� hash_sdb_recode ��������� ������ ���� ������.
      if (!hash_sdb_recode.contains(sdb_recode->S_ID))
          hash_sdb_recode[sdb_recode->S_ID] = sdb_recode;
      else { qDebug() << "base error: id dublicate\n"; return -1;}

      // ��������� ������� h_ADAM_iConnections : ADAM ������������(����) -> �������� ��.
      // � ��� ������� ���� ����������� �������������.

                    // ������� ������������ ���� �������.
      itestkey = iport | iaddress << 8 | ibit << (8*2);
      if ( iport != 0){
        if (!h_ADAM_iConnections.contains(itestkey))
            h_ADAM_iConnections[itestkey] = s_id;
        else { messbox( QString("����������� ����������� ����������\n �� � ��������� %1 %2")
                                .arg(s_id)
                                .arg(h_ADAM_iConnections[itestkey]));
             return -1;
             }
      }

      // ��������� ������� h_ADAM_oConnections : ADAM ������������(�����) -> �������� ��.
      // � ��� ������� ���� ����������� �������������.

      // ������� ������������ ���� �������.
      otestkey = oport | oaddress << 8 | obit << (8*2);
      if ( oport != 0){
          if (!h_ADAM_oConnections.contains(otestkey))
              h_ADAM_oConnections[otestkey] = s_id;
          else { messbox( QString("����������� ����������� ����������\n �� � ��������� %1 %2")
                          .arg(s_id)
                          .arg(h_ADAM_oConnections[otestkey]));
              return -1;
          }
      }
}

  // ������� �����  ADAM'��.
  //
  status=OpenCom();
  if (status){                        // �� ��� ����� �������.
    QString errports;                 // ���������� - �����.
    QString allports = "\n �� ���������� ";
    for (int i=0; i<MAX_PORT; i++){
          // "��������" ��� ���������� �����.
      if ( masCOMPort[i] != 0) allports += QString("   %1").arg(i);
          // �� �������� �� ������� ������ � masCOMPort[] ����� -1.
      if ( masCOMPort[i] < 0 ) errports+= QString("   %1").arg(i);
      else continue;
    }
    messbox("������ ��� �������� ������ ADAM: " + errports + allports);
  }

   Window window;
   window.setWindowTitle(QString("���(%1) ������ %2").arg(namebase).arg(VERSION));
   window.show();
   return app.exec();
 }
    // ��������� ��������� ��������� ADAM �� ����������.
 void TestBound(int port, int address, int bit,int s_id)
 {
  if ( port == 0 ) return;           //���� �� �������.
  if ( port <1 || port >64){
         messbox(QString("�������� ������ ADAM � ��������� 1-64. S_ID=%1").arg(s_id));
         abort();
     }
  if ( address <1 || address >255){
      messbox(QString("�������� ������� ADAM � ��������� 1-255. S_ID=%1").arg(s_id));
      abort();
     }
 if ( bit <0 || bit >7){
     messbox(QString("�������� ������ ������� ADAM � ��������� 0-7. S_ID=%1").arg(s_id));
     abort();
     }
}

 // ����� ���� � �������.
void messbox(QString str, QString extstr)
{
QMessageBox msgBox;
msgBox.setWindowTitle("Error!");
msgBox.setText(str);
if ( !extstr.isEmpty())
    msgBox.setDetailedText(extstr);
msgBox.exec();
}
        // ��������� �� ������ ������� KP ������� ������� h_KP.
        //
int Read_KP(void)
{
  QSqlQuery query;
  if(! query.exec("SELECT * FROM kp")) {
    qDebug() << "Unable to execute \" query.exec(\"SELECT * FROM kp\") \"\n ";
    messbox("Unable to execute  query.exe");
    return(-1);
  }
  struct s_kp *kp;
  QSqlRecord rec = query.record();
  while(query.next()){
    kp= new struct s_kp;
    kp->KPID=query.value(rec.indexOf("KPID")).toInt();
    kp->NKP=query.value(rec.indexOf("NKP")).toInt();
    kp->KPCIPHER=query.value(rec.indexOf("KPCIPHER")).toString();
    kp->KPNAME=query.value(rec.indexOf("KPNAME")).toString();
    kp->TU=query.value(rec.indexOf("TU")).toInt();
    kp->DKDS=query.value(rec.indexOf("DKDS")).toInt();

    if (!h_KP.contains(kp->KPID))
        h_KP[kp->KPID] = kp;
    else { qDebug() << "dubl kp error\n";
        messbox("dubl kp error");
        return -1;}
  }
  return 0;
}


 int OpenCom()
 {
     int k;
     char  *dev_namem=(char *)"\\\\.\\COM000";
     char chcom[4];
     char  dev_name[16];
     int rez;
     int flagerr;

        for(k = 0; k < 16; k++)
            dev_name[k]=dev_namem[k];
        flagerr=0;
        for(k = 1; k < MAX_PORT ; k++){
             if(masCOMPort[k]==1){
                 ComADAM.port[k] = new rlSerial();
                 itoa(k,chcom,10);
                 dev_name[7]=chcom[0];
                 if(k<10){
                    dev_name[8]=0;
                 }
                 else{
                    if(k<100){
                         dev_name[8]=chcom[1];
                         dev_name[9]=0;
                    }
                    else{ //k>100
                         dev_name[8]=chcom[1];
                         dev_name[9]=chcom[2];
                    }
                 }

                 rez = ComADAM.port[k]->openDevice(dev_name,SpeedCOM,1,1,8,1,rlSerial::NONE);
                 if(rez<0)
                 {
                     qDebug()<<"Error: open serial device(Start ADAM). Port = "<<dev_name;
                     masCOMPort[k]=-1;
                     flagerr=1;
                 }
                 else
                     qDebug()<<"Open serial device. Port = "<<dev_name;
             }
        }
        if(flagerr==0)
            return 0;
        else
            return 1;
 }
 //
 // ���� �� ���� �� MODBUS ����� ADAM ����������������(������)
 //
int ReadPort(int com_port, int adr_adam, int num_r)
{
    unsigned long int k;
    int rez;
    unsigned char bitb;
    char chcom[4];
    char *dev_namem=(char *)"\\\\.\\COM000";
    char dev_name[16]={0};
    unsigned char datr[16]={0};   // read buffer

      int func=rlModbus::ReadCoilStatus;
      rlModbus *mbus = new rlModbus(1024,rlModbus::MODBUS_RTU,0x0a);
      for(k = 0; k < 16; k++)
          dev_name[k]=dev_namem[k];
      k=com_port;
      itoa(k,chcom,10);
      dev_name[7]=chcom[0];
      if(k<10){
          dev_name[8]=0;
      }
      else{
          if(k<100){
            dev_name[8]=chcom[1];
            dev_name[9]=0;
          }
          else{ //k>100
            dev_name[8]=chcom[1];
            dev_name[9]=chcom[2];
          }
      }
      mbus->registerSerial(ComADAM.port[k]);
      // Modbus read
      rez=mbus->request(adr_adam ,rlModbus::ReadCoilStatus,0,32);
      if(rez<0)
      {
            qDebug()<<"Modbus_error: request (read ADAM) Port=COM"<<k;
            delete mbus;
            return 2;
      }
      else{
            //adr=adr_adam;
            rez= mbus->response(&adr_adam, &func, datr, TimeoutMODBUS);
            if(rez<0){
                qDebug()<<"Modbus_error: response read (read ADAM )Port=COM"<<k<<" AdrSlave="<<adr_adam<<" DATA="<<datr[0];
                delete mbus;
                return 2;
            }
            else{ //Modbus Okey
                qDebug()<<"Modbus_Okey: response read (read ADAM )Port=COM"<<k<<" AdrSlave="<<adr_adam<<" DATA="<<datr[0];
                delete mbus;
                bitb=0x1;
                bitb=bitb<<num_r;
                if((datr[0] & bitb) == 0)
                    return 0;
                else
                    return 1;
            }
     }
}
//
// ����� �� ��� �� MODBUS ����� ADAM ����������������(����)
//
int WritePort(int com_port, int adr_adam, int num_r, int value)
{
    unsigned long int k;
    int rez;
    char *dev_namem=(char *)"\\\\.\\COM000";
    char dev_name[16];
    char chcom[4];
    //unsigned char datw[16]={0,0x10,0,8,1,0};   // write buffer
    // 0,0x10 - adress write
    // 0,8       - count flags(bits)
    // 1          - write byte
    unsigned char datw[16]={0,0x10,0,0,0};   // write buffer
    unsigned char datr[16]={0};   // read buffer

    //int func=rlModbus::ReadCoilStatus;
    int func=rlModbus::ForceSingleCoil;
    rlModbus *mbus = new rlModbus(1024,rlModbus::MODBUS_RTU,0x0a);
    for(k = 0; k < 16; k++)
        dev_name[k]=dev_namem[k];
    k=com_port;
    itoa(k,chcom,10);
    dev_name[7]=chcom[0];
    if(k<10){
        dev_name[8]=0;
    }
    else{
        if(k<100){
            dev_name[8]=chcom[1];
            dev_name[9]=0;
        }
        else{ //k>100
            dev_name[8]=chcom[1];
            dev_name[9]=chcom[2];
        }
    }
    mbus->registerSerial(ComADAM.port[k]);
    // Modbus write
    //rez=mbus->write(adr_adam,rlModbus::ForceMultipleCoils ,&datw[0],COUNT_WRITE_MBUS,NULL); //bOUTADAM.OUTADAM[i].adr_ADAM
    datw[1]=0x10+num_r;
    if(value==0)
        datw[2]=0;
    else
        datw[2]=0xff;
    rez=mbus->write(adr_adam,rlModbus::ForceSingleCoil ,&datw[0],COUNT_WRITE_MBUS,NULL); //bOUTADAM.OUTADAM[i].adr_ADAM
    if(rez<0)
    {
        qDebug()<<"Modbus_error: write (write ADAM) Port=COM"<<k;
        delete mbus;
        return 1;
    }
    else{    //  write OK
        rez= mbus->response(&adr_adam,&func,datr, TimeoutMODBUS);
        if(rez<0){
            qDebug()<<"Modbus_error: response write (read ADAM) Port=COM"<<k<<" AdrSlave="<<adr_adam<<" DATA="<<datr[0];
            delete mbus;
            return 1;
        }
    }
    delete mbus;
    return 0;
}
