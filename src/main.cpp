#include "BluetoothSerial.h"
#include "robotka.h"
#include <thread>

#define DELAY 50
#define DELAY_BLINK 250
#define LED_COUNT 8

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
    cfg.name = "mojerobotka";
    cfg.motor_max_power_pct = 100; // limit výkonu motorů na xx %

    cfg.motor_enable_failsafe = false;
    cfg.rbcontroller_app_enable = false; // nepoužívám mobilní aplikaci (lze ji vypnout - kód se zrychlí, ale nelze ji odstranit z kódu -> kód se nezmenší)
    rkSetup(cfg);
    Serial.begin(115200);  // COM port 25 pocitac Burda
    if (!SerialBT.begin("TCA-BRobotka")) //Bluetooth device name; zapnutí BT musí být až za rkSetup(cfg); jinak to nebude fungovat a bude to tvořit reset ESP32
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
    fmt::print("{}'s Robotka '{}' with {} mV started!\n", cfg.owner, cfg.name, rkBatteryVoltageMv());
    rkLedYellow(true); // robot je připraven

    while(true) {
        if ( read_joystick() )
            {
                float axis_0 = (abs(axis[0]) < 10) ? 0 : -axis[0] /128.0; 
                //axis_0 = axis_0*axis_0*axis_0;
                float axis_3 = (abs(axis[3]) < 10) ? 0 : -axis[3] /128.0; 
                // axis_1 = axis_1*axis_1*axis_1;
                int levy_m = (axis_3 - (axis_0 /2 )) * speed_coef;  // hodnota pro levy motor
                int pravy_m = (axis_3 + (axis_0 /2 )) * speed_coef; // hodnota pro pravy motor 
                                
                if (BTworks) {
                    SerialBT.print(levy_m); SerialBT.print(" "); SerialBT.println(pravy_m);
                }
                else {
                    fmt::print("levy: {}, pravy: {} \n ", levy_m, pravy_m );
                }
                rkMotorsSetSpeed(levy_m, pravy_m);
            }
        delay(1);
    }

}

