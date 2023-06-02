#include "BluetoothSerial.h"
#include "robotka.h"
#include <thread>
#include "SmartServoBus.hpp"
#include "arm_commands.h"

#define DELAY 50
#define DELAY_BLINK 250
#define LED_COUNT 8
int plus_or_minus = 1;
// podle potreby proved kalibraci 
// vyrobte klepeta a nahradni baterky 
// sezeneme krabici 

bool BTworks = true; // jede bluetooth a pouzivame ho? 
#include "joystick.h"  // na lorris je to COM5 - musi byt pripojeny analyzer i terminal 
void print() {
    while(true) {
        Serial.println(a);
        SerialBT.println(a);
        a++;
        delay(1000);
        
    }
}

void setup() {
    rkConfig cfg;
    cfg.owner = "vasek"; // Ujistěte se, že v aplikace RBController máte nastavené stejné
    cfg.name = "SokolSus";
    cfg.motor_max_power_pct = 100; // limit výkonu motorů na xx %

    cfg.motor_enable_failsafe = false;
    cfg.rbcontroller_app_enable = false; // nepoužívám mobilní aplikaci (lze ji vypnout - kód se zrychlí, ale nelze ji odstranit z kódu -> kód se nezmenší)
    rkSetup(cfg);
    Serial.begin(115200);  // COM port 25 pocitac Burda
    if (!SerialBT.begin("TCA-BSokolSus")) //Bluetooth device name; zapnutí BT musí být až za rkSetup(cfg); jinak to nebude fungovat a bude to tvořit reset ESP32
    {
        Serial.println("!!! Bluetooth initialization failed!");
    } else {
        SerialBT.println("!!! Bluetooth work!\n");
        Serial.println("!!! Bluetooth work!\n");
        rkLedBlue(true);
        delay(300);
        rkLedBlue(false);
    }

    std::thread t1(print);

    delay(300);
    fmt::print("{}'s SokolSus '{}' with {} mV started!\n", cfg.owner, cfg.name, rkBatteryVoltageMv());
    rkLedYellow(true); // robot je připraven

    servoBus.begin(3, UART_NUM_1, GPIO_NUM_27);


    
    
    


    // servoBus.setAutoStop(0, true);
    printf("Start\n");
    

    // while (true)
    // {
    //     // delay(1000);
    //     // servoBus.set(254, Angle::deg(110), 200.f, 1.f);
    //     // delay(2000);
    //     for(int i = 0; i < 255; i++) {
    //         // printf("S %d = %d\n", servoBus.getId(i));
    //         servoBus.set(i; Angle::deg(110), 200.f, 1.f)
    //         delay(100);
    //     }
    // }
    int PoziceDrapaku;
    int PoziceDole = 50;
    int PoziceNahore = 150;

    int Pozice_KlepetoJedna01;
    int Pozice_KlepetoJedna02;

    int Pozice_KlepetoJedna01_Zavrena = 21;
    int Pozice_KlepetoJedna01_Otevrena = 126;
   
    int Pozice_KlepetoJedna02_Zavrena = 79;
    int Pozice_KlepetoJedna02_Otevrena = 177;

    int OKolikPosouvatPozici = 1;

    while(true) {
        if ( read_joystick() ){

                float axis_0 = (abs(axis[0]) < 10) ? 0 : -axis[0] /128.0; 
                //axis_0 = axis_0*axis_0*axis_0;
                float axis_2 = (abs(axis[2]) < 10) ? 0 : -axis[2] /128.0; 
                // axis_1 = axis_1*axis_1*axis_1;

                int levy_m = ((axis_2 * -plus_or_minus) - (axis_0 /2 )) * speed_coef;  // hodnota pro levy motor
                int pravy_m = ((axis_2 * -plus_or_minus) + (axis_0 /2 )) * speed_coef; // hodnota pro pravy motor

            if(levy_m + pravy_m > 160 || levy_m + pravy_m < -160) {
                levy_m = ((axis_2 * -plus_or_minus) - (axis_0 /4 )) * speed_coef;
                pravy_m = ((axis_2 * -plus_or_minus) + (axis_0 /4 )) * speed_coef;
            }
            //.................................jizda..dopredu..jizda..dozadu................
            if (btn[0] == true) {  
                plus_or_minus = 1;
                KtereServo = 1;
            }
                
            if (btn[0] == false) { 
                plus_or_minus = -1;
                KtereServo = 0;
            }
            //................................ovladani..drapaku.............
            
            if(btn[1] == true){
                if(btn[4] == false || btn[3] == false){
                    PoziceNahore = PoziceNahore; // Ja vim ze je to blbost ale nech me byt
                }
                if(btn[4] == true){
                    PoziceNahore = PoziceNahore + OKolikPosouvatPozici;
                }
                if(btn[3] == true){
                    PoziceNahore = PoziceNahore - OKolikPosouvatPozici;
                }
                PoziceDrapaku = PoziceNahore;
            }
            
            if(btn[1] == false){
                if(btn[4] == false || btn[3] == false){
                    PoziceDole = PoziceDole; // Ja vim ze je to blbost ale nech me byt
                }
                if(btn[4] == true){
                    PoziceDole = PoziceDole + OKolikPosouvatPozici;
                }
                if(btn[3] == true){
                    PoziceDole = PoziceDole - OKolikPosouvatPozici;
                }
                PoziceDrapaku = PoziceDole;
            }
            
        if(btn[0] == true){
            Pozice_KlepetoJedna01 = Pozice_KlepetoJedna01_Otevrena;
            delay(300);
            Pozice_KlepetoJedna02 = Pozice_KlepetoJedna02_Otevrena;
        }
        if(btn[0] == false){
            Pozice_KlepetoJedna02 = Pozice_KlepetoJedna02_Zavrena;
            delay(300);
            Pozice_KlepetoJedna01 = Pozice_KlepetoJedna01_Zavrena;
        }
            servoBus.set(0, Angle::deg(Pozice_KlepetoJedna01))
            servoBus.set(1, Angle::deg(Pozice_KlepetoJedna02));

            servoBus.set(2, Angle::deg(PoziceDrapaku));
            servoBus.set(3, Angle::deg(PoziceDrapaku));

            if (BTworks) {
                SerialBT.print(levy_m); SerialBT.print(" "); SerialBT.println(PoziceKlepetoJedna);
                fmt::print("levy: {}, pravy: {} \n ", levy_m, PoziceKlepetoJedna );
            }
            else {
                fmt::print("levy: {}, pravy: {} \n ", levy_m, PoziceKlepetoJedna);
            }

            
            rkMotorsSetPower(levy_m, pravy_m); //rkMotorsSetSpeed jede sotva polovicni rychlosti !!
                
            }
        delay(1);
    }

}

