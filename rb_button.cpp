#include <QtGui>
#include "window.h"

extern QHash<int,struct s_sdb_recode *> hash_sdb_recode;

// ��������� ������������ ������ toggled �� �����������  rb_input.
//
void Window::Toggled_rb_input(bool b)
{
if ( b ) {
    qDebug() << "Radio button is checked";
    rb_input_checked();
}
else {
     qDebug() << "Radio button is unchecked";
     rb_input_unchecked();
 }
}
//
// ��������� ������������ ��������� "checked" ������ rb_button.
// ------------------------------------------------------------
void Window::rb_input_checked()
{
// ��������� ������������� ���� iPort, iAddress, iBit.
m_LE_iPort->setReadOnly(false);
m_LE_iPort->setText("");
m_LE_iAddress->setReadOnly(false);
m_LE_iAddress->setText("");
m_LE_iBit->setReadOnly(false);
m_LE_iBit->setText("");
m_LE_iPort->setFocus();
}
//
// ��������� ������������ ��������� "unchecked" ������ rb_button.
// --------------------------------------------------------------
void Window::rb_input_unchecked()
{
// ������� � ���������� ������ ��������� ������������� iport, iaddress, ibit.
int iport = m_LE_iPort->text().toUInt();
int iaddress = m_LE_iAddress->text().toUInt();
int ibit = m_LE_iBit->text().toUInt();
qDebug() << iport << iaddress << ibit;

// ���� � ������� hash_sdb_recode �������� ��� � ����� ������������.
QHash<int,struct s_sdb_recode *>::const_iterator i = hash_sdb_recode.constBegin();
int key;
while (i != hash_sdb_recode.constEnd()) {
 struct s_sdb_recode *sdb = i.value();
 key = i.key();
 if ( sdb->I_PORT_ADAM !=0 &&
      (sdb->I_PORT_ADAM == iport  && sdb->I_ADR_ADAM == iaddress  && sdb->I_NR_ADAM == ibit )) {
    qDebug() << "Find param" << sdb->NOTE;
    goto l_finded;
 }
  ++i;
}
qDebug() << "Do not find param";
messbox(QString("����������� %1 %2 %3 �� �������").arg(iport).arg(iaddress).arg(ibit));
goto l_return;

l_finded:           // ����� �������� � ����� ������������.
QListWidgetItem *pitem;
m_kp->setCurrentIndex(0);     // ����������  "��� ��".
pitem=KP_selected(0, key);
pitem->setSelected(true);     // �������� ��������� �������.
lwg->setCurrentItem(pitem);   // � �� ���� �������.
l_return:
lwg->setFocus();
    // ���������  ������������� ���� iPort, iAddress, iBit.
m_LE_iPort->setReadOnly(true);
m_LE_iPort->setText("");
m_LE_iAddress->setReadOnly(true);
m_LE_iAddress->setText("");
m_LE_iBit->setReadOnly(true);
m_LE_iBit->setText("");
}

