#include <QtGui>
#include "window.h"

extern QHash<int,struct s_sdb_recode *> hash_sdb_recode;
extern QHash<int,struct s_kp *> h_KP;

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
 m_Label          = new QLabel;
 m_Label_iPort    = new QLabel;
 m_Label_iAddress = new QLabel;
 m_Label_ibit     = new QLabel;
 m_LE_iPort       = new QLineEdit;
  m_LE_iPort->setReadOnly(true);
  m_LE_iPort->setFixedWidth(fontMetrics().width( "333" ));     // Фикс. длина 2+1
 m_LE_iAddress    = new QLineEdit;
  m_LE_iAddress->setReadOnly(true);
  m_LE_iAddress->setFixedWidth(fontMetrics().width( "3333" )); // Фикс. длина 3+1
 m_LE_iBit        = new QLineEdit;
  m_LE_iBit->setReadOnly(true);
  m_LE_iBit->setFixedWidth(fontMetrics().width( "33" ));        // Фикс. длина 1+1
 rb_input         = new QRadioButton;

 m_Label_o        = new QLabel;

 m_LineEdit     = new QLineEdit;
 m_LineEdit->setReadOnly(true);
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
    h_ComboBoxKP_KP[i_kp]=key_kp;   // Привязка индекс ComboBox к индекс h_KP;
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

 QHBoxLayout *hbxlayout = new QHBoxLayout;
 hbxlayout->addWidget(bn_read);
 hbxlayout->addWidget(bn_switch_on);
 hbxlayout->addWidget(bn_switch_off);

 QHBoxLayout *hbxlayout_i = new QHBoxLayout;
 hbxlayout_i->addWidget(m_Label_iPort);
 hbxlayout_i->addWidget(m_LE_iPort);
 hbxlayout_i->addWidget(m_Label_iAddress);
 hbxlayout_i->addWidget(m_LE_iAddress);
 hbxlayout_i->addWidget(m_Label_ibit);
 hbxlayout_i->addWidget(m_LE_iBit);
 hbxlayout_i->addWidget(rb_input);

 QVBoxLayout *vbxlayout = new QVBoxLayout;
 vbxlayout->addLayout(hbxlayout);
 vbxlayout->addWidget(m_Label);
 vbxlayout->addLayout(hbxlayout_i);
 vbxlayout->addWidget(m_Label_o);
 vbxlayout->addWidget(m_LineEdit);
 vbxlayout->addWidget(m_kp);
 vbxlayout->addWidget(lwg);
 this->setLayout(vbxlayout);

    // Определить связку сигнал - слот.
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
 connect(m_kp, SIGNAL(activated(int)),this, SLOT(KP_selected_dop(int)));
 connect(rb_input, SIGNAL(toggled(bool)),this, SLOT( Toggled_rb_input (bool)));

 lwg->item(0)->setSelected(true);
 lwg->setFocus();

}

void Window::sl_CurrentItemChanged( QListWidgetItem *, QListWidgetItem *)
{
  qDebug() << "Current item changed";
  InitStateOfObjects();
  m_SelectedItem = 0;
}

