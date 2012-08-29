#include "window.h"

extern QHash<int,struct s_sdb_recode *> hash_sdb_recode;
extern int ReadPort(int, int, int);


//
// ��������� ������������ ������� ������ "��. ������".
//
void Window::bn_read_Clicked()
{
 qDebug()<< "��.������";
 m_Label->setText("");
 SetInputStringtoEmpty();
 m_Label_o->setText("");
 m_LineEdit->setText("");

 bn_read->setDisabled(true);
 bn_switch_on->setDisabled(true);
 bn_switch_off->setDisabled(true);
 m_Label->setAlignment( Qt::AlignCenter );

       // ��������� ���� �� ������ lwg ��� ������ ������.
       //
 for(int row = 0; row < lwg->count(); row++)
   {
    QListWidgetItem *pitem = lwg->item(row);
    int key = h_ListItem2index[pitem];
    struct s_sdb_recode *sdb   = hash_sdb_recode[key];

    // ���� ������� �������� ���� , �� ���������� ��� ���������
    // � ������� ������������ �����������.
    if ( sdb->A_TYPE == 1 || sdb->A_TYPE == 4 ) {
     Read_key(pitem);
    }
    m_Label->setText(QString("�����  %1/%2").arg(row+1).arg(lwg->count()));
}
 bn_read->setDisabled(false);
 m_Label->setAlignment( Qt::AlignLeft );
 InitStateOfObjects();
 lwg->setFocus( Qt::TabFocusReason );   // ���������� ����� �� ������.
}

void Window::full_Read_key()
{
 int key = h_ListItem2index[m_SelectedItem];
 struct s_sdb_recode *sdb = hash_sdb_recode[key];
 int state = Read_key(m_SelectedItem);
 if ( state == -1) {
   messbox("������ ������");
   return;
 }
    // ������� � ���. ������ ��������� �����.
 QString str =  sdb->str_type + "=" + (sdb->state_key == 0 ? "����.(0)":"���.(1)");
 m_Label->setText(str);
 lwg->setFocus( Qt::TabFocusReason );   // ���������� ����� �� ������.

}

//
// ��������� ��������� ������ ������.
//
int Window::Read_key(QListWidgetItem *pitem)
{
 int key = h_ListItem2index[pitem];
 struct s_sdb_recode *sdb = hash_sdb_recode[key];
 int port =  sdb->I_PORT_ADAM;
 int addr =  sdb->I_ADR_ADAM;
 int nr  =  sdb->I_NR_ADAM;

    qDebug() << "��. ����� " << sdb->NOTE
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


