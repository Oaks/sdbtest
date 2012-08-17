#include <QtGui>
#include "window.h"
#include "sdb.h"

extern QHash<int,struct s_sdb_recode *> hash_sdb_recode;
extern QHash<int,struct s_kp *> h_KP;
extern int WritePort(int, int, int, int);
extern int ReadPort(int, int, int);

QHash<int,int> h_ComboBoxKP_KP;

Window::Window()
{
 m_SelectedItem = 0;
            // Определить кнопки "Чт.ключа" , "Вкл.(1)", "Выкл.(0)"
 bn_switch_on  = new QPushButton("Вкл.(1)");
 bn_switch_off = new QPushButton("Откл.(0)");
 bn_read = new QPushButton("Чт.ключей");
  bn_read->setDisabled(false);
            //Определить информационные строки.
 m_Label       = new QLabel;
 m_Label_i     = new QLabel;
 m_Label_o     = new QLabel;

 m_LineEdit     = new QLineEdit;
        // Формируем ComboBox со списком КП.
 m_kp           = new QComboBox();
 m_kp->addItem("Все КП");
                //Список формируем из словаря h_KP.
 QHash<int,struct s_kp *>::const_iterator j = h_KP.constBegin();
 int i_kp=1;
 while (j != h_KP.constEnd()) {
    struct s_kp *kp = j.value();
    int key_kp = j.key();
    m_kp->addItem(kp->KPNAME);
    h_ComboBoxKP_KP[i_kp]=key_kp;   // Привязка индекс ComboBox к игдукс h_KP;
    ++i_kp;
    ++j;
 }

            // Определить начальное состояние вышеопределенных объектов.
 InitStateOfObjects();

            // Определить виджет списка параметров.
 lwg    = new QListWidget;
 lwg->setResizeMode(QListWidget::Adjust);
 QListWidgetItem* pitem = 0;
 QStringList      lst;
 QHash<int,struct s_sdb_recode *>::const_iterator i = hash_sdb_recode.constBegin();

    // Формируем виджет списка lwg на основе словаря  hash_sdb_recode.
    //
 while (i != hash_sdb_recode.constEnd()) {
    struct s_sdb_recode *sdb = i.value();
    QString str = sdb->NOTE;
    int     key = i.key();
            // В список вносим параметры, к которым привязаны ADAM'ы.
     if (     ( (sdb->A_TYPE == 1 ) && (sdb->O_PORT_ADAM != 0 || sdb->I_PORT_ADAM != 0) )
           || ( (sdb->A_TYPE == 3 ) && sdb->O_PORT_ADAM != 0  )
           || ( (sdb->A_TYPE == 4 ) && sdb->I_PORT_ADAM != 0  )
         ) {
          pitem = new QListWidgetItem(str, lwg);
             // Формируем словарь h_ListItem2index, который ставит
             // в соответствие элементу списка индекс базы.
       h_ListItem2index[pitem] = key;
      }
   ++i;
 }

    // Определить схему расположения виджетов.
 QVBoxLayout *vbxlayout = new QVBoxLayout;
 QHBoxLayout *hbxlayout = new QHBoxLayout;

 hbxlayout->addWidget(bn_read);
 hbxlayout->addWidget(bn_switch_on);
 hbxlayout->addWidget(bn_switch_off);

 vbxlayout->addLayout(hbxlayout);
 vbxlayout->addWidget(m_Label);
 vbxlayout->addWidget(m_Label_i);
 vbxlayout->addWidget(m_Label_o);
 vbxlayout->addWidget(m_LineEdit);
 vbxlayout->addWidget(m_kp);
 vbxlayout->addWidget(lwg);
 this->setLayout(vbxlayout);

    // Определить связку сигнал - слот.
 //connect(lwg, SIGNAL(itemClicked(QListWidgetItem *)),
 //            this, SLOT(ListItemSelected(QListWidgetItem *)));
 connect(lwg, SIGNAL(currentItemChanged ( QListWidgetItem *, QListWidgetItem *)),
         this, SLOT(sl_CurrentItemChanged( QListWidgetItem *, QListWidgetItem *)));
 connect(lwg, SIGNAL(itemActivated(QListWidgetItem *)),
             this, SLOT(ListItemActivated(QListWidgetItem *)));
 connect(bn_switch_on, SIGNAL(clicked()),
                     this, SLOT(bn_switch_on_Clicked()) );
 connect(bn_switch_off, SIGNAL(clicked()),
                     this, SLOT(bn_switch_off_Clicked()) );
 connect(bn_read, SIGNAL(clicked()),
                     this, SLOT(bn_read_Clicked()) );
 connect(m_kp, SIGNAL(activated(int)),this, SLOT(KP_selected(int)));
}

void Window::KP_selected(int index)
{
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
       }
      escape:
      ++i;
  }
}

void Window::sl_CurrentItemChanged( QListWidgetItem *, QListWidgetItem *)
{
  qDebug() << "Current item changed";
  InitStateOfObjects();
  m_SelectedItem = 0;
}

//
// Процедура обслуживает сигнал (lwg, SIGNAL(itemActivated(QListWidgetItem *).
//
void Window::ListItemActivated(QListWidgetItem *item)
{
  ListItemSelected(item);
  int key = h_ListItem2index[m_SelectedItem];
  struct s_sdb_recode *sdb = hash_sdb_recode[key];
  if (sdb->A_TYPE == 1 || sdb->A_TYPE == 1 )
      full_Read_key();            // Активирован параметр ключ.
}

//
// Процедура обслуживает сигнал (lwg, SIGNAL(itemClicked(QListWidgetItem *)
// выбор элемента списка lwq по двойному клику.
//
void Window::ListItemSelected(QListWidgetItem *item)
{
 QString str=item->text();      // Установить в строке выбора
 m_LineEdit->setText(str);      // наименование выбранного параметра.
   qDebug()<< str;
 m_SelectedItem = item;         // Запомнить выбранный элемент списка.
 int index = h_ListItem2index[item]; // По словарю найти индекс строки бавы данных
                                // sdb_recode соответствующую выбранному элементу списка.
        // Вывести порты адреса номера разрядов выбранного элемента.
 struct s_sdb_recode *sdb = hash_sdb_recode[index];
                                // Ввод только для ключей.
 if (sdb->A_TYPE == 1 || sdb->A_TYPE == 4 ){
   m_Label_i->setText(QString("ВВОД        порт=%1 адрес=%2 разряд=%3")
                      .arg(sdb->I_PORT_ADAM)
                      .arg(sdb->I_ADR_ADAM)
                      .arg(sdb->I_NR_ADAM));
 }
 else m_Label_i->setText("");
                                // Вывод только для ламп.
 if (sdb->A_TYPE == 1 || sdb->A_TYPE == 3 ){
   m_Label_o->setText(QString("ВЫВОД      порт=%1 адрес=%2 разряд=%3")
                      .arg(sdb->O_PORT_ADAM)
                      .arg(sdb->O_ADR_ADAM)
                      .arg(sdb->O_NR_ADAM));
 }
 else m_Label_o->setText("");

 // Вывести тип параметра, активировать/деактивировать соотв. кнопки .
 switch (sdb->A_TYPE){
     case 1: m_Label->setText("Лампа/Ключ");
             bn_switch_on->setDisabled(false);
             bn_switch_off->setDisabled(false);
             bn_read->setDisabled(false);
             break;
     case 3: m_Label->setText("Лампа");
             bn_switch_on->setDisabled(false);
             bn_switch_off->setDisabled(false);
             bn_read->setDisabled(true);
             break;
     case 4: m_Label->setText("Ключ");
             bn_switch_on->setDisabled(true);
             bn_switch_off->setDisabled(true);
              bn_read->setDisabled(false);
             break;
     default:m_Label->setText("");
             m_Label_i->setText("");
             bn_switch_on->setDisabled(true);
             bn_switch_off->setDisabled(true);
             bn_read->setDisabled(true);
 }
 sdb->str_type = m_Label->text();   // Запомнить название типа параметра.

    // Вывести в инф. строку состояние ключа,если присутствует.
 if (sdb->state_key != -1) {
   QString str =  sdb->str_type + "=" + (sdb->state_key == 0 ? "Откл.(0)":"Вкл.(1)");
   m_Label->setText(str);
}
}

//
// Процедура отрабатывает нажатие кнопки "Чт. ключей".
//
void Window::bn_read_Clicked()
{
 qDebug()<< "Чт.ключей";
 m_Label->setText("");
 m_Label_i->setText("");
 m_Label_o->setText("");
 m_LineEdit->setText("");

 bn_read->setDisabled(true);
 bn_switch_on->setDisabled(true);
 bn_switch_off->setDisabled(true);
 m_Label_i->setAlignment( Qt::AlignCenter );

       // Формируем цикл по списку lwg для чтения ключей.
       //
 for(int row = 0; row < lwg->count(); row++)
   {
    QListWidgetItem *pitem = lwg->item(row);
    int key = h_ListItem2index[pitem];
    struct s_sdb_recode *sdb   = hash_sdb_recode[key];

    // Если текущий параметр ключ , то определяем его состояние
    // с выводом графического изображения.
    if ( sdb->A_TYPE == 1 || sdb->A_TYPE == 4 ) {
     Read_key(pitem);
    }
    //QTest::qWait(1000) ;
    m_Label_i->setText(QString("Ждите  %1/%2").arg(row+1).arg(lwg->count()));
}
 bn_read->setDisabled(false);
 m_Label_i->setAlignment( Qt::AlignLeft );
 InitStateOfObjects();
 lwg->setFocus( Qt::TabFocusReason );   // Установить фокус на список.
}



void Window::full_Read_key()
{
 int key = h_ListItem2index[m_SelectedItem];
 struct s_sdb_recode *sdb = hash_sdb_recode[key];
 int state = Read_key(m_SelectedItem);
 if ( state == -1) {
   messbox("Ошибка чтения");
   return;
 }
    // Вывести в инф. строку состояние ключа.
 QString str =  sdb->str_type + "=" + (sdb->state_key == 0 ? "Откл.(0)":"Вкл.(1)");
 m_Label->setText(str);
 lwg->setFocus( Qt::TabFocusReason );   // Установить фокус на список.

}

//
// Процедура обрабатывает нажатие кнопки "Вкл.(1)".
//
void Window::bn_switch_on_Clicked()
{
        // Найти запись базы данных, соответствующую выбранному элементу
        // и поместить в sdb.
 int key = h_ListItem2index[m_SelectedItem];
 struct s_sdb_recode *sdb = hash_sdb_recode[key];
 int port =  sdb->O_PORT_ADAM;
 int addr =  sdb->O_ADR_ADAM;
 int nr  =  sdb->O_NR_ADAM;

 qDebug() << "Вкл.(1) " << m_SelectedItem->text()
          << "    ADAM PORT=" << port
          <<           "ADR="  << addr
          <<            "NR="   << nr;

 int state=WritePort( port, addr, nr, 1 );
 if ( state!=0) {
   qDebug() << "Bad";
   messbox("Ошибка записи");
   lwg->setFocus( Qt::TabFocusReason );   // Установить фокус на список.
   return;
}
else         qDebug() << "Ok";
lwg->setFocus( Qt::TabFocusReason );   // Установить фокус на список.
}
    //
    // Процедура обрабатывает нажатие кнопки "Откл.(0)".
    //
void Window::bn_switch_off_Clicked()
{
 int key = h_ListItem2index[m_SelectedItem];
 struct s_sdb_recode *sdb = hash_sdb_recode[key];
 int port =  sdb->O_PORT_ADAM;
 int addr =  sdb->O_ADR_ADAM;
 int nr  =  sdb->O_NR_ADAM;

 qDebug() << "Откл.(0)  " << m_SelectedItem->text()
          << "    ADAM PORT=" << port
          <<           "ADR=" << addr
          <<            "NR="   << nr;
 int state=WritePort( port, addr, nr, 0 );
 if ( state!=0) {
   qDebug() << "Bad";
   messbox("Ошибка записи");
   lwg->setFocus( Qt::TabFocusReason );   // Установить фокус на список.
   return;
}
 else         qDebug() << "Ok";
 lwg->setFocus( Qt::TabFocusReason );   // Установить фокус на список.

}

//
// Процедура выполняет чтение ключей.
//
int Window::Read_key(QListWidgetItem *pitem)
{
 int key = h_ListItem2index[pitem];
 struct s_sdb_recode *sdb = hash_sdb_recode[key];
 int port =  sdb->I_PORT_ADAM;
 int addr =  sdb->I_ADR_ADAM;
 int nr  =  sdb->I_NR_ADAM;

    qDebug() << "Чт. ключа " << sdb->NOTE
             << "    ADAM PORT=" << port
             <<           "ADR=" << addr
             <<            "NR=" << nr;

int state=ReadPort( port, addr, nr );
if ( state!=0 && state!= 1){ qDebug() << "Bad" << state;
                             pitem->setIcon(QPixmap("white.jpg"));
                             return -1;
                           }
else                         qDebug() << "Ok" << state;
sdb->state_key = state;

if ( sdb->state_key == 0 )
     pitem->setIcon(QPixmap("green.jpg"));
else
     pitem->setIcon(QPixmap("red.jpg"));

return state;
}


// Процедура деактивирует кнопки "Чт.ключа" , "Вкл.(1)", "Выкл.(0)".
// и выводит служебные строки.
void Window::InitStateOfObjects(void)
{
    qDebug()<< "Инициализация строк";
    // Деактивировать кнопки  , "Вкл.(1)", "Выкл.(0)"
  bn_switch_on->setDisabled(true);
  bn_switch_off->setDisabled(true);

    //Определить начальные информационные строки.
  m_Label->setText("");
  m_Label_i->setText("");
  m_Label_o->setText("");
  m_LineEdit->setText("");
  m_SelectedItem = 0;
}
