#include <QtGui>
#include "window.h"

extern QHash<int,struct s_sdb_recode *> hash_sdb_recode;
extern QHash<int,int> h_ComboBoxKP_KP;

//
// ��������� ������������ ������ ������ �� �� ������ �� ComboBox.
// ---------------------------------------------------------------
void Window::KP_selected_dop(int index)
{
KP_selected(index, -1);
}

QListWidgetItem * Window::KP_selected(int index, int select)
{
QListWidgetItem *finded_item = 0;

qDebug()<< "ComboBox_selected"<<index;
int index_kp=0;
if ( index != 0 ) index_kp = h_ComboBoxKP_KP[index];
qDebug()<< "KP_selected"<<index_kp;

lwg->clear();         // �������� ������ ����������.
h_ListItem2index.clear();  //�������� ������� ������.

QListWidgetItem* pitem = 0;
QHash<int,struct s_sdb_recode *>::const_iterator i = hash_sdb_recode.constBegin();

 // ��������� ������ ������ lwg �� ������ �������  hash_sdb_recode.
 //
while (i != hash_sdb_recode.constEnd()) {
 struct s_sdb_recode *sdb = i.value();
 QString str = sdb->NOTE;
 int     key = i.key();
 if ( index != 0 && sdb->I_NKP != index_kp ) goto escape; // �������� �� ����������� ������� ��.
         // � ������ ������ ���������, � ������� ��������� ADAM'�.
  if (     ( (sdb->A_TYPE == 1 ) && (sdb->O_PORT_ADAM != 0 || sdb->I_PORT_ADAM != 0) )
        || ( (sdb->A_TYPE == 3 ) && sdb->O_PORT_ADAM != 0  )
        || ( (sdb->A_TYPE == 4 ) && sdb->I_PORT_ADAM != 0  )
      ) {
       pitem = new QListWidgetItem(str, lwg);
          // ��������� ������� h_ListItem2index, ������� ������
          // � ������������ �������� ������ ������ ����.
    h_ListItem2index[pitem] = key;
    if ( select != -1 && select == key){
      qDebug() << "key find" << key;
      finded_item = pitem;
    }
   }
  escape:
  ++i;
}
return finded_item;
}

