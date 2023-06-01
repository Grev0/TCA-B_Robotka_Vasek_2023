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

    rkSetup(cfg);
    servoBus.begin(4, UART_NUM_2, GPIO_NUM_27);
    servoBus.set


    servoBus.setAutoStop(0, true);
    printf("Start\n");
    

    while (true)
    {
        // delay(1000);
        // servoBus.set(254, Angle::deg(110), 200.f, 1.f);
        // delay(2000);
        for(int i = 0; i < 255; i++) {
            // printf("S %d = %d\n", servoBus.getId(i));
            servoBus.set(i; Angle::deg(110), 200.f, 1.f)
            delay(100);
        }
    }
    

    while(true) {
        if ( read_joystick() )
            {
                float axis_0 = (abs(axis[0]) < 10) ? 0 : -axis[0] /128.0; 
                //axis_0 = axis_0*axis_0*axis_0;
                float axis_2 = (abs(axis[2]) < 10) ? 0 : -axis[2] /128.0; 
                // axis_1 = axis_1*axis_1*axis_1;


                int levy_m = ((axis_2 * -plus_or_minus) - (axis_0 /2 )) * speed_coef;  // hodnota pro levy motor
                int pravy_m = ((axis_2 * -plus_or_minus) + (axis_0 /2 )) * speed_coef; // hodnota pro pravy motor

                if(levy_m + pravy_m > 160 || levy_m + pravy_m < -160)
                {
                    levy_m = ((axis_2 * -plus_or_minus) - (axis_0 /4 )) * speed_coef;
                    pravy_m = ((axis_2 * -plus_or_minus) + (axis_0 /4 )) * speed_coef;
                }

        if (btn[0] == true)
                { 
                    plus_or_minus = 1;
            }
                Serial.println(levy_m);
                
        if (btn[0] == false)
            { 
                plus_or_minus = -1;
            }
                Serial.println(levy_m);

                if (BTworks) {
                    SerialBT.print(levy_m); SerialBT.print(" "); SerialBT.println(pravy_m);
                    fmt::print("levy: {}, pravy: {} \n ", levy_m, pravy_m );
                }
                else {
                    fmt::print("levy: {}, pravy: {} \n ", levy_m, pravy_m );
                }
                
                rkMotorsSetPower(levy_m, pravy_m); //rkMotorsSetSpeed jede sotva polovicni rychlosti !!
                
            }
        delay(1);
    }

}

