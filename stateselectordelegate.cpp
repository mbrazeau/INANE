#include <QAbstractItemView>
#include <QComboBox>
#include <QtSql>
#include <QDebug>
#include <QStringListModel>

#include "stateselectordelegate.h"

StateSelectorDelegate::StateSelectorDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{
}

QWidget *StateSelectorDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
    QComboBox *editor = new QComboBox(parent);

    return editor;
}

void StateSelectorDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    const QSqlQueryModel *m = qobject_cast<const QSqlQueryModel *>(index.model());
    QSqlRecord rec = m->record(index.row());
    int rowid = rec.field("id").value().toInt();

    QSqlQuery query;
    QStringList stateLabels;
    int charID;
    query.exec(QString("SELECT character FROM observations WHERE rowid = %1").arg(rowid));
    query.next();
    charID = query.value(0).toInt();

    query.exec(QString("SELECT label FROM states WHERE character = %1").arg(charID));

    QComboBox *comboBox = static_cast<QComboBox *>(editor);
    QString currentLabel = index.model()->data(index, Qt::EditRole).toString();
    stateLabels.push_back(currentLabel);

    while (query.next()) {
        if (currentLabel != query.value(0).toString()) {
            stateLabels.push_back(query.value(0).toString());
        }
    }
    stateLabels.push_back(QString("missing"));
    stateLabels.push_back(QString("inapplicable"));
    comboBox->addItems(stateLabels);
}

void StateSelectorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    Q_UNUSED(model);

    const QSqlQueryModel *m = qobject_cast<const QSqlQueryModel *>(index.model());
    QSqlRecord rec = m->record(index.row());
    int rowid = rec.field("id").value().toInt();

    int charID;
    QSqlQuery query;
    QComboBox *comboBox = static_cast<QComboBox *>(editor);
    QString key;
    key = comboBox->currentText();

    query.exec(QString("SELECT character FROM observations WHERE rowid = %1").arg(rowid));
    query.next();
    charID = query.value(0).toInt();

    if (key != "missing" && key != "inapplicable") {
        query.prepare("UPDATE observations SET state = (SELECT state_id FROM states WHERE character = :char_id AND label = :state) WHERE rowid = :rown");
        query.bindValue(":char_id", charID);
        query.bindValue(":state", key);
        query.bindValue(":rown", rowid);
    } else {
        query.prepare("UPDATE observations SET state = (SELECT state_id FROM states WHERE label = :state) WHERE rowid = :rown");
        query.bindValue(":state", key);
        query.bindValue(":rown", rowid);
    }

    if(!query.exec()){
        qDebug() << query.lastError().text();
    }
    QSqlRelationalTableModel *obsTable = static_cast<QSqlRelationalTableModel *>(model);
    obsTable->select();
}

void StateSelectorDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
    editor->setGeometry(option.rect);
}
