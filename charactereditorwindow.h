#ifndef CHARACTEREDITORWINDOW_H
#define CHARACTEREDITORWINDOW_H

#include <QWidget>

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

class CharacterEditorWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CharacterEditorWindow(QWidget *parent = nullptr);
    void setCharTable(QSqlRelationalTableModel *charTable, QSqlRelationalTableModel *statesTable);

private:
    QGridLayout *charWindLayout;
    QTableView  *charTableView;
    QSqlRelationalTableModel *charTable_p;
    QSqlRelationalTableModel *statesTable_p;
    QWidget *editorArea;

    QDataWidgetMapper *mapper;

    QLabel *labelLabel;
    QLineEdit *labelField;
    QLabel *statesLabel;
    QTableView *statesTable;
    QLabel *sourceLabel;
    QLineEdit  *sourceField;
    QLabel *descripLabel;
    QTextEdit  *descripField;

    QPushButton *newChar;
    QPushButton *deleteChar;
    QPushButton *submitButton;

    void initEditorArea();
    void populateEditor();

    void newCharacterAction();
    void commitCharChange();

//private slots:

    void onCharacterClicked(const QModelIndex &index);

signals:

};

#endif // CHARACTEREDITORWINDOW_H
