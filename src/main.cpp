// #include "BluetoothSerial.h"
#include "WiFi.h"
#include "robotka.h"
#include <thread>
#include "SmartServoBus.hpp"
#include "arm_commands.h"

#define DELAY 50
#define DELAY_BLINK 250
#define LED_COUNT 8

constexpr std::size_t bufferSize = 64;

constexpr size_t axisOpCode = 0x80;
constexpr size_t axisCount = 5;
// cislo os je o dve vyssi, nez pise Lorris, protoze prvni byte je hlavicka a druhy je pocet os a az potom jsou hodnoty os
constexpr size_t xAxisPosition = 2; // pravy nahoru dolu
constexpr size_t yAxisPosition = 4; // levy doprava doleva
constexpr size_t armAxisPosition = 5;

constexpr size_t buttonOpCode = 0x81;
constexpr size_t buttonCount = 14;
constexpr size_t buttonIdPosition = 1;
constexpr size_t buttonStatePosition = 2;

int Servo_Hodnota_Nula = 0;
int Servo_Hodnota_Jedna = 0;
int Servo_Hodnota_Dva = 0;
int Servo_Hodnota_Tri = 0;

void handleAxes(const char buffer[bufferSize]);
void handleButton(const char btn[bufferSize], WiFiUDP &udp);

//---------Drapak-L
int Servo_Leve_Nahore = 100;
int Servo_Leve_Dole = 160;
//---------Drapak-P
int Servo_Prave_Nahore = 208;
int Servo_Prave_Dole = 152;

//----------Klepeto-L
int Klepeto_Leve_Otevrene = 157;
int Klepeto_Leve_Zavrene = 63;
//----------Klepeto-P
int Klepeto_Prave_Otevrene;
int Klepeto_Prave_Zavrene;


int HodnotaKlepeta;

int OKolikPosouvatPozici = 5;

int Pozice_Klepeta_Nastavovani = 0;


int plus_or_minus = 1;
bool btn[8];
// podle potreby proved kalibraci
// vyrobte klepeta a nahradni baterky
// sezeneme krabici

bool BTworks = true; // jede bluetooth a pouzivame ho?
#include "joystick.h"  // na lorris je to COM5 - musi byt pripojeny analyzer i terminal
void print() {
    while(true) {
        // fmt::print("levy: {}, pravy: {} , drap{}, \n ", levy_m, pravy_m, PoziceDrapaku);
        // printf("Pozici: %d\n", PoziceDrapaku);
        //Serial.println(a);
        // udp.println(a);
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

    WiFi.softAP("vasek", "sokolska");
    WiFiUDP udp;
    printf("Zapnute DUP:%d\n", udp.begin(88));


    // if (!SerialBT.begin("TCA-BSokolSus")) //Bluetooth device name; zapnutí BT musí být až za rkSetup(cfg); jinak to nebude fungovat a bude to tvořit reset ESP32
    // {
    //     Serial.println("!!! Bluetooth initialization failed!");
    // } else {
    //     SerialBT.println("!!! Bluetooth work!\n");
    //     Serial.println("!!! Bluetooth work!\n");
    //     rkLedBlue(true);
    //     delay(300);
    //     rkLedBlue(false);
    // }

    std::thread t1(print);

    delay(300);
    fmt::print("{}'s SokolSus '{}' with {} mV started!\n", cfg.owner, cfg.name, rkBatteryVoltageMv());
    rkLedYellow(true); // robot je připraven

    servoBus.begin(1, UART_NUM_1, GPIO_NUM_27);

    // // Set servo Id (must be only one servo connected to the bus)
     servoBus.setId(3);
     int a = 1;
     while (true) {
        if(a > 200){
            a = a - 199;
        }
        a = a + 1;
         printf(" %d\n", a);
         printf("GetId: %d\n", servoBus.getId());
         delay(100);
         servoBus.set(3, Angle::deg(a));
     }

    // servoBus.setAutoStop(0, true);
    printf("Start\n");



    int count = 0;

    while (true) {
        char buffer[bufferSize] = { 0 };
        memset(buffer, 0, bufferSize);

        udp.parsePacket();
        if ((count = udp.read(buffer, bufferSize - 1)) > 0) {
            // fwrite(buffer, 1, count, stdout);
            // fflush(stdout);

            if (buffer[0] == axisOpCode) {
                handleAxes(buffer); // pohyb robota
            } else if (buffer[0] == buttonOpCode) {
                handleButton(buffer, udp); // tlacitka
            } else {
                ////// tady zastav robota
                rkMotorsSetPower(0, 0); //rkMotorsSetSpeed jede sotva polovicni rychlosti !!
            }
        }

        // vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    // stary while
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
                printf("Zmena smeru 1\n");
            }

            if (btn[0] == false) {
                plus_or_minus = -1;
                printf("Zmena smeru -1\n");
            }
            //................................ovladani..drapaku.............

            

            // if (BTworks) {
            //     SerialBT.print(levy_m); SerialBT.print(" "); SerialBT.println(pravy_m);
            //     fmt::print("levy: {}, pravy: {} \n ", levy_m, pravy_m );
            // }
            // else {
            //     fmt::print("levy: {}, pravy: {} \n ", levy_m, pravy_m);
            // }


            rkMotorsSetPower(levy_m, pravy_m); //rkMotorsSetSpeed jede sotva polovicni rychlosti !!

            }
        delay(5);
    }

}


void handleAxes(const char buffer[bufferSize]) {
    // auto& man = rb::Manager::get();

    if (buffer[1] != axisCount) {
        ESP_LOGE("UDP Parser", "Wrong axis count");
    }

    // printf("Axes:");
    // for(int i = 0; i < axisCount; i++) {
    //     int axis = static_cast<int8_t>(buffer[i]);
    //     printf("[%d]: %d, ", i, axis);
    // }
    // printf("\n");

    // printf("orig x: %i\ty: %i\n", buffer[xAxisPosition], buffer[yAxisPosition]);
    int x = static_cast<int8_t>(buffer[xAxisPosition]);
    int y = -static_cast<int8_t>(buffer[yAxisPosition]);

    // int r = ((y - (x / 1.5f)));
    // int l = ((y + (x / 1.5f)));

    // r = rb::clamp(r, -50, 50);
    // l = rb::clamp(l, -50, 50);
    // printf("calc x: %i\ty: %i\n", x, y);

    // if (r < 0 && l < 0) {
    //     std::swap(r, l);
    // }

    float axis_0 = (abs(x) < 10) ? 0 : -x /128.0;
    //axis_0 = axis_0*axis_0*axis_0;
    float axis_2 = (abs(y) < 10) ? 0 : -y /128.0;
    // axis_1 = axis_1*axis_1*axis_1;

    int levy_m = ((axis_2 * -plus_or_minus) - (axis_0 /2 )) * speed_coef;  // hodnota pro levy motor
    int pravy_m = ((axis_2 * -plus_or_minus) + (axis_0 /2 )) * speed_coef; // hodnota pro pravy motor

    // if(levy_m + pravy_m > 160 || levy_m + pravy_m < -160) {
    //     levy_m = ((axis_2 * -plus_or_minus) - (axis_0 /4 )) * speed_coef;
    //     pravy_m = ((axis_2 * -plus_or_minus) + (axis_0 /4 )) * speed_coef;
    // }

    rkMotorsSetPower(levy_m, pravy_m); //rkMotorsSetSpeed jede sotva polovicni rychlosti !!
}

void handleButton(const char buffer[bufferSize], WiFiUDP &udp) {
    size_t id = buffer[buttonIdPosition];
    uint8_t state = buffer[buttonStatePosition];

    if (id >= buttonCount) {
        ESP_LOGE("UDP Parser", "Button id out of bounds");
    }

    // ESP_LOGI("UDP Parser", "Button %u changed to %u\n", id, state);
    udp.printf("Button %u changed to %u\n", id, state);
    printf("Button %u changed to %u\n", id, state);

    btn[id] = (bool)state;


/*/////////////////////////////////////

    if (btn[0] == true) { //Drapaky nahoru
        Servo_Hodnota_Jedna = Servo_Leve_Nahore;
        Servo_Hodnota_Dva = Servo_Prave_Nahore;
        //plus_or_minus = 1;
        //printf("Zmena smeru 1\n");
    }

    if (btn[0] == false) {//Drapaky dolu
        Servo_Hodnota_Jedna = Servo_Leve_Dole;
        Servo_Hodnota_Dva = Servo_Prave_Dole;
        //plus_or_minus = -1;
        //printf("Zmena smeru -1\n");
    }
*/////////////////////////////////////////////////

        if(btn[4] == false || btn[3] == false){
            Pozice_Klepeta_Nastavovani = Pozice_Klepeta_Nastavovani; // Ja vim ze je to blbost ale nech me byt
        }
        if(btn[4] == true){
            Pozice_Klepeta_Nastavovani = Pozice_Klepeta_Nastavovani + 1;
            }
        if(btn[3] == true){
            Pozice_Klepeta_Nastavovani = Pozice_Klepeta_Nastavovani - 1;
    
} 

    printf("Pozici: %d\n", Pozice_Klepeta_Nastavovani);

    // if(btn[0] == true){
    //     Pozice_KlepetoJedna01 = Pozice_KlepetoJedna01_Otevrena;
    //     delay(300);
    //     Pozice_KlepetoJedna02 = Pozice_KlepetoJedna02_Otevrena;
    // }
    // if(btn[0] == false){
    //     Pozice_KlepetoJedna02 = Pozice_KlepetoJedna02_Zavrena;
    //     delay(300);
    //     Pozice_KlepetoJedna01 = Pozice_KlepetoJedna01_Zavrena;
    // }
    // servoBus.set(0, Angle::deg(Pozice_KlepetoJedna01));
    // servoBus.set(1, Angle::deg(Pozice_KlepetoJedna02));

    //servoBus.set(1, Angle::deg(Servo_Hodnota_Jedna));
    //servoBus.set(2, Angle::deg(Servo_Hodnota_Dva));

    servoBus.set(3, Angle::deg(Pozice_Klepeta_Nastavovani));


    // if(id == 0){
    //     if(state == 1) {

    //     }
    //     if (state == 0) {

    //     }

    // switch (id) {
    // case 0:
    //     if (state) {
    //         printf("plus minus 1\n");
    //         plus_or_minus = 1;
    //     } else {
    //         printf("plus minus -1\n");
    //         plus_or_minus = -1;
    //     }
    // case 1:

    //     break;
    // // case 4:
    // //     g_leftBatteryState = state;
    // //     break;
    // // case 5:
    // //     g_rightBatteryState = state;
    // //     break;
    // // case 8:
    // //     g_handState = state;
    // //     break;
    // default:
    //     break;
    // }
}
