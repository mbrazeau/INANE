#include <QApplication>
#include <QDialogButtonBox>
#include <QCryptographicHash>
#include <QDataWidgetMapper>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
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
#include "checkboxdelegate.h"
#include "mdatabasemanager.h"

TaxaEditorWindow::TaxaEditorWindow(QWidget *parent)
    : QWidget{parent}
{
    setWindowModality(Qt::ApplicationModal);

    m_taxaTableView = new QTableView(this);
    m_taxaEditLayout = new QGridLayout(this);

    QWidget *taxaToolbar = new QWidget(this);
    QHBoxLayout *toolsLayout = new QHBoxLayout(taxaToolbar);

    QPushButton *addTaxonBtn = new QPushButton(tr("New taxon"), this);
    QPushButton *deleteTaxonBtn = new QPushButton(tr("Delete taxon"), this);

    toolsLayout->addWidget(addTaxonBtn);
    toolsLayout->addWidget(deleteTaxonBtn);
    toolsLayout->addStretch();
    connect(addTaxonBtn, &QPushButton::released, this, &TaxaEditorWindow::getNewTaxon);
    connect(deleteTaxonBtn, &QPushButton::released, this, &TaxaEditorWindow::deleteTaxon);

    m_taxaTableModel = new QSqlRelationalTableModel(this, QSqlDatabase::database());
    m_taxaTableModel->setTable("taxa");
    m_taxaTableModel->setRelation(4, QSqlRelation("taxongroups", "group_id", "groupname"));
    m_taxaTableModel->select();
    m_taxaTableView->setModel(m_taxaTableModel);
    m_taxaEditLayout->addWidget(taxaToolbar, 0, 0);
    m_taxaEditLayout->addWidget(m_taxaTableView, 1, 0);


    m_taxaTableModel->setHeaderData(5, Qt::Horizontal, tr("Inc."));

//    StateSelectorDelegate *obsDelegate = new StateSelectorDelegate(observationsTable);
//    obsTableView->setItemDelegateForColumn(3, obsDelegate);

    m_taxaTableView->horizontalHeader()->moveSection(5, 0);
    CheckboxDelegate *inclTaxonCheckbox = new CheckboxDelegate(m_taxaTableModel);
    m_taxaTableView->setItemDelegateForColumn(5, inclTaxonCheckbox);

    m_taxaTableView->setColumnHidden(0, true);
    m_taxaTableView->setColumnHidden(1, true);

    m_taxaTableView->resizeColumnsToContents();
//    m_taxaTableView->resizeRowsToContents();
    m_taxaTableView->horizontalHeader()->setStretchLastSection(true);
}

void TaxaEditorWindow::getNewTaxon()
{
    bool ok;

    if (!QSqlDatabase::database().defaultConnection) {
        qDebug() << "No database open!";
//        writeToConsole("No database open");
        return;
    }

    QString taxonName = QInputDialog::getText(nullptr,
                                              tr("New taxon"),
                                              tr("Input a new taxon name"),
                                              QLineEdit::Normal,
                                              "",
                                              &ok);

    if (ok && !taxonName.isEmpty()) {
        MDatabaseManager::addTaxon(taxonName);
    } else {
        return;
    }

    m_taxaTableModel->select();
}

void TaxaEditorWindow::deleteTaxon()
{
    QMessageBox checkDelete;
    checkDelete.setIcon(QMessageBox::Critical);
    checkDelete.setText("Are you sure you want to delete this taxon?");
    checkDelete.setInformativeText("Deleting taxa is not best practice. If you want to exclude a taxon from analyses, you should toggle its inclusion status.");
    checkDelete.setStandardButtons(QMessageBox::Abort | QMessageBox::Ignore);
    checkDelete.setDefaultButton(QMessageBox::Abort);

    int ret = checkDelete.exec();

    int row;
    switch (ret) {
    case QMessageBox::Abort:
        break;
    case QMessageBox::Ignore:
        row = m_taxaTableView->currentIndex().row();
        m_taxaTableModel->removeRow(row);
        m_taxaTableModel->select();
        if (row < m_taxaTableModel->rowCount()) {
            m_taxaTableModel->selectRow(row);
        } else {
            m_taxaTableModel->selectRow(row - 1);
        }

        break;
    default:
        break;
    }

}
