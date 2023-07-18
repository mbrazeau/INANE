#include <QComboBox>
#include <QSqlQuery>
#include <QSqlRelationalTableModel>

#include <QDebug>
#include <QSqlError>

#include "stateselectordelegate.h"

StateSelectorDelegate::StateSelectorDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{
}

QWidget *StateSelectorDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QComboBox *editor = new QComboBox(parent);

    return editor;
}

void StateSelectorDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QSqlQuery query;
    QStringList stateLabels;
    int charID;
    query.exec(QString("SELECT character FROM observations WHERE rowid = %1").arg(index.row() + 1));
    query.next();
    charID = query.value(0).toInt();

    query.exec(QString("SELECT label FROM states WHERE character = %1").arg(charID));
    while (query.next()) {
        stateLabels.push_back(query.value(0).toString());
    }

    QComboBox *comboBox = static_cast<QComboBox *>(editor);
    comboBox->addItems(stateLabels);
}

void StateSelectorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QSqlQuery query;
    QComboBox *comboBox = static_cast<QComboBox *>(editor);
    QString key;
    key = comboBox->currentText();
    qDebug() << "key: " << key;

    int charID;
    query.exec(QString("SELECT character FROM observations WHERE rowid = %1").arg(index.row() + 1));
    query.next();
    charID = query.value(0).toInt();
    qDebug() << "charID: " << charID;

    query.prepare("UPDATE observations SET state = (SELECT state_id FROM states WHERE character = :char_id AND label = :state) WHERE rowid = :rown");
    query.bindValue(":char_id", charID);
    query.bindValue(":state", key);
    query.bindValue(":rown", index.row() + 1);
    if(!query.exec()){
        qDebug() << query.lastError().text();
    }

    QSqlRelationalTableModel *obsTable = static_cast<QSqlRelationalTableModel *>(model);
    obsTable->select();
}

void StateSelectorDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
