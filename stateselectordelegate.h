#ifndef STATESELECTORDELEGATE_H
#define STATESELECTORDELEGATE_H

#include <QStyledItemDelegate>
#include <QObject>
#include <QWidget>

class StateSelectorDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit StateSelectorDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override;
};

#endif // STATESELECTORDELEGATE_H
