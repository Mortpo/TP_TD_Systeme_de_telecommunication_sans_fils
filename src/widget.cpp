#pragma comment(lib,ODALID.lib)
#include "widget.h"
#include "ui_widget.h"
#include "ODALID.h"
#include "QDebug"
#include "string"
#include "stdio.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

ReaderName MonLecteur;
char pszHost[] = "192.168.1.4";

unsigned char key_ff[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
unsigned char key_A2[6]={0xA0 , 0xA1,0xA2,0xA3,0xA4,0xA5};
unsigned char key_B2[6]={0xB0 , 0xB1,0xB2,0xB3,0xB4,0xB5};
unsigned char key_C2[6]={0xC0, 0xC1,0xC2,0xC3,0xC4,0xC5};
unsigned char key_D2[6]={0xD0, 0xD1,0xD2,0xD3,0xD4,0xD5};
void Widget::on_Connect_clicked()
{

    uint16_t status = 0;
    MonLecteur.Type = ReaderCDC;
    MonLecteur.device = 0;
    status = OpenCOM(&MonLecteur);
    qDebug() << "OpendCOM1" << status;

    char version[30];
    uint8_t serial[4];
    char stackReader[20];

    status = Version(&MonLecteur,version,serial,stackReader);
    ui->Affichage->setText(version);
    ui->Affichage->update();
    std::string erreur_connexion = "64532";

    if(erreur_connexion.compare(version) < 0 ){
       lecture();
       LireCredit();
       LEDBuzzer(&MonLecteur,LED_ON);
    }
}


void Widget::on_Quitter_clicked()
{
    int16_t status =0;
    RF_Power_Control(&MonLecteur,FALSE,0);
    status= LEDBuzzer(&MonLecteur,LED_OFF);
    status = CloseCOM(&MonLecteur);
    qApp->quit();
}

void Widget::on_Ecriture_clicked()
{
    RF_Power_Control(&MonLecteur,TRUE,0);
    BYTE atq[2];
    BYTE sak[1];
    BYTE uid[12];
    uint16_t uid_len = 12;
    int status = ISO14443_3_A_PollCard(&MonLecteur,atq,sak,uid,&uid_len);
    if(status==0){
    QString stringdataNom = ui->Nom->toPlainText();
    QString stringdataPrenom = ui->Prenom->toPlainText();
    unsigned char dataNom[16];
    unsigned char dataPrenom[16];
    memcpy(dataNom,stringdataNom.toStdString().c_str(),16);
    memcpy(dataPrenom,stringdataPrenom.toStdString().c_str(),16);
    Mf_Classic_LoadKey(&MonLecteur,Auth_KeyA,key_A2,2);
    Mf_Classic_LoadKey(&MonLecteur,Auth_KeyB,key_B2,2);
    Mf_Classic_Write_Block(&MonLecteur,TRUE,10,dataNom,Auth_KeyB,2);
    Mf_Classic_Write_Block(&MonLecteur,TRUE,9,dataPrenom,Auth_KeyB,2);
    LEDBuzzer(&MonLecteur,BUZZER_OFF);
    }
}


void Widget::lecture() {
    RF_Power_Control(&MonLecteur,TRUE,0);
    BYTE atq[2];
    BYTE sak[1];
    BYTE uid[12];
    uint16_t uid_len = 12;
    int status;
    status = ISO14443_3_A_PollCard(&MonLecteur,atq,sak,uid,&uid_len);
    if(status == 0 ){
        //lecture nom prénom

        unsigned char data[16];
        //char result[16];
        //uint8_t block = 10;
        Mf_Classic_LoadKey(&MonLecteur,Auth_KeyA,key_A2,2);
        Mf_Classic_LoadKey(&MonLecteur,Auth_KeyB,key_B2,2);
        Mf_Classic_Read_Block(&MonLecteur,TRUE,10,data,Auth_KeyA,2);
        QString result = ""; //+ data[0];//QString::fromUtf8((char*)data);

        for (int i =0; i<16 ; i++) {
            result= result+data[i];
        }


        ui->Nom->setText(result);
        result = "";
        Mf_Classic_Read_Block(&MonLecteur,TRUE,9,data,Auth_KeyA,2);
        //sprintf( result, ui->Nom->toPlainText().toUtf8().data(),16);
        for (int i =0; i<16 ; i++) {
            result= result+data[i];
        }

        ui->Prenom->setText(result);
        ui->Nom->update();
        ui->Prenom->update();

        LEDBuzzer(&MonLecteur,LED_GREEN_OFF);

    }

}

void Widget::on_Lecture_clicked()
{
   lecture();
}

void Widget::LireCredit(){
    Mf_Classic_LoadKey(&MonLecteur,Auth_KeyA,key_C2,3);
    Mf_Classic_LoadKey(&MonLecteur,Auth_KeyB,key_D2,3);
       uint32_t data;
    Mf_Classic_Read_Value(&MonLecteur,TRUE,14,&data,Auth_KeyA,3);
    QString result = "Credit : "+ QString::number((int)data);//QString::fromUtf8((char*)data);

    ui->Credit->setText(result);
    LEDBuzzer(&MonLecteur,LED_GREEN_OFF);

}

void Widget::on_Ajouter_clicked()
{
   uint32_t add = ui->boxadd->value();
   uint32_t sub = ui->boxsub->value();
   ui->boxsub->setValue(0);
   ui->boxadd->setValue(0);
   int tot = add - sub;

   RF_Power_Control(&MonLecteur,TRUE,0);
   BYTE atq[2];
   BYTE sak[1];
   BYTE uid[12];
   uint16_t uid_len = 12;

   Mf_Classic_LoadKey(&MonLecteur,Auth_KeyA,key_C2,3);
   Mf_Classic_LoadKey(&MonLecteur,Auth_KeyB,key_D2,3);

   int status;
   status = ISO14443_3_A_PollCard(&MonLecteur,atq,sak,uid,&uid_len);
   if(status == 0){

       if(tot < 0){
        tot = tot*-1;
        uint32_t to=tot;
           Mf_Classic_Decrement_Value(&MonLecteur,TRUE,13,to,14,Auth_KeyB,3);
           Mf_Classic_Restore_Value(&MonLecteur,TRUE,14,13,Auth_KeyB,3);
       }else{ //pb
           uint32_t to=tot;
           Mf_Classic_Increment_Value(&MonLecteur,TRUE,13,to,14,Auth_KeyB,3);
           Mf_Classic_Restore_Value(&MonLecteur,TRUE,14,13,Auth_KeyB,3);
       }
       LireCredit();
       LEDBuzzer(&MonLecteur,BUZZER_OFF);
   }


}
