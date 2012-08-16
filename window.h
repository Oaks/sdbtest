 #ifndef WINDOW_H
 #define WINDOW_H


#include <QWidget>
#include <QtGui>

 class QComboBox;
 class QLineEdit;

 class Window : public QWidget
 {
     Q_OBJECT

 public:
     Window();

 protected:
    int  Read_key(QListWidgetItem *);
    void InitStateOfObjects(void);
    void full_Read_key();

 public slots:
    void sl_CurrentItemChanged( QListWidgetItem *, QListWidgetItem *);
    void ListItemSelected(QListWidgetItem *);
    void ListItemActivated(QListWidgetItem *);
    void bn_switch_off_Clicked();
    void bn_switch_on_Clicked();
    void bn_read_Clicked();
    void KP_selected(int index);


 private:
     QListWidget *lwg;
     QLineEdit *m_LineEdit;
     QListWidgetItem *m_SelectedItem;
     QLabel *m_Label;
     QLabel *m_Label_i;
     QLabel *m_Label_o;
     QComboBox *m_kp;
     QHash<QListWidgetItem *, int> h_ListItem2index;
     QPushButton *bn_switch_on;
     QPushButton *bn_switch_off;
     QPushButton *bn_read;

 };

 void messbox(QString str, QString extstr="");

 #endif
