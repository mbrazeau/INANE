#ifndef TAXAEDITORWINDOW_H
#define TAXAEDITORWINDOW_H

#include <QWidget>

class QDialogButtonBox;
class QDialogButtonBox;
class QGridLayout;
class QHBoxLayout;
class QTableView;
class QSqlRelationalTableModel;
class QSqlDatabase;
class QLabel;
class QLineEdit;
class QTextEdit;
class QSqlRelationalTableModel;
class QDataWidgetMapper;
class QPushButton;
class QModelIndex;
class QToolBar;
class QToolButton;
class QVBoxLayout;

class TaxaEditorWindow : public QWidget
{
    Q_OBJECT

    QSqlRelationalTableModel *m_taxaTableModel;
    QTableView               *m_taxaTableView;
    QGridLayout              *m_taxaEditLayout;

public:
    explicit TaxaEditorWindow(QWidget *parent = nullptr);

    void getNewTaxon();
    void deleteTaxon();

signals:

};

#endif // TAXAEDITORWINDOW_H
