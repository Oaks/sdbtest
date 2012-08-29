#include <QtGui>
#include "window.h"

extern QHash<int,struct s_sdb_recode *> hash_sdb_recode;

//
// ��������� ����������� ������ (lwg, SIGNAL(itemActivated(QListWidgetItem *).
//
void Window::ListItemActivated(QListWidgetItem *item)
{
  ListItemSelected(item);
  int key = h_ListItem2index[m_SelectedItem];
  struct s_sdb_recode *sdb = hash_sdb_recode[key];
  if (sdb->A_TYPE == 1 || sdb->A_TYPE == 1 )
      full_Read_key();            // ����������� �������� ����.
}

//
// ��������� ���������� ������ ����������� ADAM �� �����.
void Window::SetInputStringtoEmpty()
{
  m_Label_iPort->setText(QString("<font color=silver>����  ����</font>"));
  m_LE_iPort->setText(QString(""));
   //m_LE_iPort->setVisible(false);
  m_Label_iAddress->setText(QString("<font color=silver>�����</font>"));
  m_LE_iAddress->setText(QString(""));
  //m_LE_iAddress->setVisible(false);
  m_Label_ibit->setText(QString("<font color=silver>������</font>"));
  m_LE_iBit->setText(QString(""));
  //m_LE_iBit->setVisible(false);
  //rb_input->setVisible(false);
}

// ��������� ����������� ������ (lwg, SIGNAL(itemClicked(QListWidgetItem *)
// ����� �������� ������ lwq �� �������� �����.
//
void Window::ListItemSelected(QListWidgetItem *item)
{
 m_SelectedItem = item;         // ��������� ��������� ������� ������.
 int index = h_ListItem2index[item]; // �� ������� ����� ������ ������ ���� ������
                                // sdb_recode ��������������� ���������� �������� ������.
        // ������� ����� ������ ������ �������� ���������� ��������.
 struct s_sdb_recode *sdb = hash_sdb_recode[index];
       // ���������� � ������ ������ ������������ ���������� ���������.
 QString str = sdb->NOTE+"   "+'\"'+sdb->PNAME+'\"';
 m_LineEdit->setText(str);
   qDebug()<< str;
                                // ���� ������ ��� ������.
 if (sdb->A_TYPE == 1 || sdb->A_TYPE == 4 ){
   m_Label_iPort->setText(QString("���� ����"));
   m_LE_iPort->setText(QString("%1").arg(sdb->I_PORT_ADAM));
   //m_LE_iPort->setInputMask("99");
    m_LE_iPort->setVisible(true);
   m_Label_iAddress->setText(QString("�����"));
   m_LE_iAddress->setText(QString("%1").arg(sdb->I_ADR_ADAM));
    m_LE_iAddress->setVisible(true);
   m_Label_ibit->setText(QString("������"));
   m_LE_iBit->setText(QString("%1").arg(sdb->I_NR_ADAM));
    m_LE_iBit->setVisible(true);
   rb_input->setVisible(true);

 }
 else SetInputStringtoEmpty();
                                // ����� ������ ��� ����.
 if (sdb->A_TYPE == 1 || sdb->A_TYPE == 3 ){
   m_Label_o->setText(QString("�����      ����=%1 �����=%2 ������=%3")
                      .arg(sdb->O_PORT_ADAM)
                      .arg(sdb->O_ADR_ADAM)
                      .arg(sdb->O_NR_ADAM));
 }
 else m_Label_o->setText("");

 // ������� ��� ���������, ������������/�������������� �����. ������ .
 switch (sdb->A_TYPE){
     case 1: m_Label->setText("�����/����");
             bn_switch_on->setDisabled(false);
             bn_switch_off->setDisabled(false);
             bn_read->setDisabled(false);
             break;
     case 3: m_Label->setText("�����");
             bn_switch_on->setDisabled(false);
             bn_switch_off->setDisabled(false);
             bn_read->setDisabled(true);
             break;
     case 4: m_Label->setText("����");
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
 sdb->str_type = m_Label->text();   // ��������� �������� ���� ���������.

    // ������� � ���. ������ ��������� �����,���� ������������.
 if (sdb->state_key != -1) {
   QString str =  sdb->str_type + "=" + (sdb->state_key == 0 ? "����.(0)":"���.(1)");
   m_Label->setText(str);
}
}




// ��������� ������������ ������ "��.�����" , "���.(1)", "����.(0)".
// � ������� ��������� ������.
void Window::InitStateOfObjects(void)
{
    qDebug()<< "������������� �����";
    // �������������� ������  , "���.(1)", "����.(0)"
  bn_switch_on->setDisabled(true);
  bn_switch_off->setDisabled(true);

    //���������� ��������� �������������� ������.
  m_Label->setText("");
  SetInputStringtoEmpty();
  m_Label_o->setText("");
  m_LineEdit->setText("");
  m_SelectedItem = 0;
}

