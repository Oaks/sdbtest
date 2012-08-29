#include <QtGui>
#include "window.h"

extern QHash<int,struct s_sdb_recode *> hash_sdb_recode;

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
// Процедура сбрасывает строку подключения ADAM по вводу.
void Window::SetInputStringtoEmpty()
{
  m_Label_iPort->setText(QString("<font color=silver>ВВОД  порт</font>"));
  m_LE_iPort->setText(QString(""));
   //m_LE_iPort->setVisible(false);
  m_Label_iAddress->setText(QString("<font color=silver>адрес</font>"));
  m_LE_iAddress->setText(QString(""));
  //m_LE_iAddress->setVisible(false);
  m_Label_ibit->setText(QString("<font color=silver>разряд</font>"));
  m_LE_iBit->setText(QString(""));
  //m_LE_iBit->setVisible(false);
  //rb_input->setVisible(false);
}

// Процедура обслуживает сигнал (lwg, SIGNAL(itemClicked(QListWidgetItem *)
// выбор элемента списка lwq по двойному клику.
//
void Window::ListItemSelected(QListWidgetItem *item)
{
 m_SelectedItem = item;         // Запомнить выбранный элемент списка.
 int index = h_ListItem2index[item]; // По словарю найти индекс строки бавы данных
                                // sdb_recode соответствующую выбранному элементу списка.
        // Вывести порты адреса номера разрядов выбранного элемента.
 struct s_sdb_recode *sdb = hash_sdb_recode[index];
       // Установить в строке выбора наименование выбранного параметра.
 QString str = sdb->NOTE+"   "+'\"'+sdb->PNAME+'\"';
 m_LineEdit->setText(str);
   qDebug()<< str;
                                // Ввод только для ключей.
 if (sdb->A_TYPE == 1 || sdb->A_TYPE == 4 ){
   m_Label_iPort->setText(QString("ВВОД порт"));
   m_LE_iPort->setText(QString("%1").arg(sdb->I_PORT_ADAM));
   //m_LE_iPort->setInputMask("99");
    m_LE_iPort->setVisible(true);
   m_Label_iAddress->setText(QString("адрес"));
   m_LE_iAddress->setText(QString("%1").arg(sdb->I_ADR_ADAM));
    m_LE_iAddress->setVisible(true);
   m_Label_ibit->setText(QString("разряд"));
   m_LE_iBit->setText(QString("%1").arg(sdb->I_NR_ADAM));
    m_LE_iBit->setVisible(true);
   rb_input->setVisible(true);

 }
 else SetInputStringtoEmpty();
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
             SetInputStringtoEmpty();
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
  SetInputStringtoEmpty();
  m_Label_o->setText("");
  m_LineEdit->setText("");
  m_SelectedItem = 0;
}

