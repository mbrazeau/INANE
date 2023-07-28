#include <QApplication>
#include <QDialogButtonBox>
#include <QCryptographicHash>
#include <QDataWidgetMapper>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QtSql>
#include <QStyle>
#include <QTableView>
#include <QTextEdit>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

#include "taxaeditorwindow.h"

TaxaEditorWindow::TaxaEditorWindow(QWidget *parent)
    : QWidget{parent}
{
    setWindowModality(Qt::ApplicationModal);

    m_taxaTableView = new QTableView(this);
    m_taxaEditLayout = new QGridLayout(this);

    m_taxaTableModel = new QSqlRelationalTableModel(this, QSqlDatabase::database());
    m_taxaTableModel->setTable("taxa");
    m_taxaTableModel->select();
    m_taxaTableView->setModel(m_taxaTableModel);
    m_taxaEditLayout->addWidget(m_taxaTableView, 0, 0);
}
