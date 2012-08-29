#include <QtGui>
#include "window.h"

extern QHash<int,struct s_sdb_recode *> hash_sdb_recode;
extern QHash<int,int> h_ComboBoxKP_KP;

//
// Процедура обрабатывает сигнал выбора КП из списка КП ComboBox.
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

lwg->clear();         // Очистить список параметров.
h_ListItem2index.clear();  //Очистить словарь списка.

QListWidgetItem* pitem = 0;
QHash<int,struct s_sdb_recode *>::const_iterator i = hash_sdb_recode.constBegin();

 // Формируем виджет списка lwg на основе словаря  hash_sdb_recode.
 //
while (i != hash_sdb_recode.constEnd()) {
 struct s_sdb_recode *sdb = i.value();
 QString str = sdb->NOTE;
 int     key = i.key();
 if ( index != 0 && sdb->I_NKP != index_kp ) goto escape; // параметр не принадлежит данному КП.
         // В список вносим параметры, к которым привязаны ADAM'ы.
  if (     ( (sdb->A_TYPE == 1 ) && (sdb->O_PORT_ADAM != 0 || sdb->I_PORT_ADAM != 0) )
        || ( (sdb->A_TYPE == 3 ) && sdb->O_PORT_ADAM != 0  )
        || ( (sdb->A_TYPE == 4 ) && sdb->I_PORT_ADAM != 0  )
      ) {
       pitem = new QListWidgetItem(str, lwg);
          // Формируем словарь h_ListItem2index, который ставит
          // в соответствие элементу списка индекс базы.
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

