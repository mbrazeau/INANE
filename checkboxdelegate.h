#ifndef CHECKBOXDELEGATE_H
#define CHECKBOXDELEGATE_H

#include <QItemDelegate>
#include <QObject>

class CheckboxDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit CheckboxDelegate(QObject *parent = nullptr);

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // CHECKBOXDELEGATE_H
