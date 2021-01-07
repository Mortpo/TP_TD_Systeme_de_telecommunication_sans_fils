#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    void lecture();
    void LireCredit();
    ~Widget();

private slots:
    void on_Connect_clicked();

    void on_Quitter_clicked();

    void on_Ecriture_clicked();

    void on_Lecture_clicked();

    void on_Ajouter_clicked();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
