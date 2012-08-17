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
            // ���������� ������ "��.�����" , "���.(1)", "����.(0)"
 bn_switch_on  = new QPushButton("���.(1)");
 bn_switch_off = new QPushButton("����.(0)");
 bn_read = new QPushButton("��.������");
  bn_read->setDisabled(false);
            //���������� �������������� ������.
 m_Label       = new QLabel;
 m_Label_i     = new QLabel;
 m_Label_o     = new QLabel;

 m_LineEdit     = new QLineEdit;
        // ��������� ComboBox �� ������� ��.
 m_kp           = new QComboBox();
 m_kp->addItem("��� ��");
                //������ ��������� �� ������� h_KP.
 QHash<int,struct s_kp *>::const_iterator j = h_KP.constBegin();
 int i_kp=1;
 while (j != h_KP.constEnd()) {
    struct s_kp *kp = j.value();
    int key_kp = j.key();
    m_kp->addItem(kp->KPNAME);
    h_ComboBoxKP_KP[i_kp]=key_kp;   // �������� ������ ComboBox � ������ h_KP;
    ++i_kp;
    ++j;
 }

            // ���������� ��������� ��������� ���������������� ��������.
 InitStateOfObjects();

            // ���������� ������ ������ ����������.
 lwg    = new QListWidget;
 lwg->setResizeMode(QListWidget::Adjust);
 QListWidgetItem* pitem = 0;
 QStringList      lst;
 QHash<int,struct s_sdb_recode *>::const_iterator i = hash_sdb_recode.constBegin();

    // ��������� ������ ������ lwg �� ������ �������  hash_sdb_recode.
    //
 while (i != hash_sdb_recode.constEnd()) {
    struct s_sdb_recode *sdb = i.value();
    QString str = sdb->NOTE;
    int     key = i.key();
            // � ������ ������ ���������, � ������� ��������� ADAM'�.
     if (     ( (sdb->A_TYPE == 1 ) && (sdb->O_PORT_ADAM != 0 || sdb->I_PORT_ADAM != 0) )
           || ( (sdb->A_TYPE == 3 ) && sdb->O_PORT_ADAM != 0  )
           || ( (sdb->A_TYPE == 4 ) && sdb->I_PORT_ADAM != 0  )
         ) {
          pitem = new QListWidgetItem(str, lwg);
             // ��������� ������� h_ListItem2index, ������� ������
             // � ������������ �������� ������ ������ ����.
       h_ListItem2index[pitem] = key;
      }
   ++i;
 }

    // ���������� ����� ������������ ��������.
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

    // ���������� ������ ������ - ����.
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
// ��������� ����������� ������ (lwg, SIGNAL(itemClicked(QListWidgetItem *)
// ����� �������� ������ lwq �� �������� �����.
//
void Window::ListItemSelected(QListWidgetItem *item)
{
 QString str=item->text();      // ���������� � ������ ������
 m_LineEdit->setText(str);      // ������������ ���������� ���������.
   qDebug()<< str;
 m_SelectedItem = item;         // ��������� ��������� ������� ������.
 int index = h_ListItem2index[item]; // �� ������� ����� ������ ������ ���� ������
                                // sdb_recode ��������������� ���������� �������� ������.
        // ������� ����� ������ ������ �������� ���������� ��������.
 struct s_sdb_recode *sdb = hash_sdb_recode[index];
                                // ���� ������ ��� ������.
 if (sdb->A_TYPE == 1 || sdb->A_TYPE == 4 ){
   m_Label_i->setText(QString("����        ����=%1 �����=%2 ������=%3")
                      .arg(sdb->I_PORT_ADAM)
                      .arg(sdb->I_ADR_ADAM)
                      .arg(sdb->I_NR_ADAM));
 }
 else m_Label_i->setText("");
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
             m_Label_i->setText("");
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

//
// ��������� ������������ ������� ������ "��. ������".
//
void Window::bn_read_Clicked()
{
 qDebug()<< "��.������";
 m_Label->setText("");
 m_Label_i->setText("");
 m_Label_o->setText("");
 m_LineEdit->setText("");

 bn_read->setDisabled(true);
 bn_switch_on->setDisabled(true);
 bn_switch_off->setDisabled(true);
 m_Label_i->setAlignment( Qt::AlignCenter );

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
    //QTest::qWait(1000) ;
    m_Label_i->setText(QString("�����  %1/%2").arg(row+1).arg(lwg->count()));
}
 bn_read->setDisabled(false);
 m_Label_i->setAlignment( Qt::AlignLeft );
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
// ��������� ������������ ������� ������ "���.(1)".
//
void Window::bn_switch_on_Clicked()
{
        // ����� ������ ���� ������, ��������������� ���������� ��������
        // � ��������� � sdb.
 int key = h_ListItem2index[m_SelectedItem];
 struct s_sdb_recode *sdb = hash_sdb_recode[key];
 int port =  sdb->O_PORT_ADAM;
 int addr =  sdb->O_ADR_ADAM;
 int nr  =  sdb->O_NR_ADAM;

 qDebug() << "���.(1) " << m_SelectedItem->text()
          << "    ADAM PORT=" << port
          <<           "ADR="  << addr
          <<            "NR="   << nr;

 int state=WritePort( port, addr, nr, 1 );
 if ( state!=0) {
   qDebug() << "Bad";
   messbox("������ ������");
   lwg->setFocus( Qt::TabFocusReason );   // ���������� ����� �� ������.
   return;
}
else         qDebug() << "Ok";
lwg->setFocus( Qt::TabFocusReason );   // ���������� ����� �� ������.
}
    //
    // ��������� ������������ ������� ������ "����.(0)".
    //
void Window::bn_switch_off_Clicked()
{
 int key = h_ListItem2index[m_SelectedItem];
 struct s_sdb_recode *sdb = hash_sdb_recode[key];
 int port =  sdb->O_PORT_ADAM;
 int addr =  sdb->O_ADR_ADAM;
 int nr  =  sdb->O_NR_ADAM;

 qDebug() << "����.(0)  " << m_SelectedItem->text()
          << "    ADAM PORT=" << port
          <<           "ADR=" << addr
          <<            "NR="   << nr;
 int state=WritePort( port, addr, nr, 0 );
 if ( state!=0) {
   qDebug() << "Bad";
   messbox("������ ������");
   lwg->setFocus( Qt::TabFocusReason );   // ���������� ����� �� ������.
   return;
}
 else         qDebug() << "Ok";
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
  m_Label_i->setText("");
  m_Label_o->setText("");
  m_LineEdit->setText("");
  m_SelectedItem = 0;
}
