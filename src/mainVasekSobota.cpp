// // #include "BluetoothSerial.h"
// #include "WiFi.h"
// #include "robotka.h"
// #include <thread>
// #include "SmartServoBus.hpp"
// #include "arm_commands.h"

// #define DELAY 50
// #define DELAY_BLINK 250
// #define LED_COUNT 8

// constexpr std::size_t bufferSize = 64;

// constexpr size_t axisOpCode = 0x80;
// constexpr size_t axisCount = 5;
// // cislo os je o dve vyssi, nez pise Lorris, protoze prvni byte je hlavicka a druhy je pocet os a az potom jsou hodnoty os
// constexpr size_t xAxisPosition = 2; // pravy nahoru dolu
// constexpr size_t yAxisPosition = 4; // levy doprava doleva

// constexpr size_t armAxisPosition = 5;

// constexpr size_t buttonOpCode = 0x81;
// constexpr size_t buttonCount = 14;
// constexpr size_t buttonIdPosition = 1;
// constexpr size_t buttonStatePosition = 2;

// void handleAxes(const char buffer[bufferSize]);
// void handleButton(const char buffer[bufferSize]);

// int Pozice_KlepetoJedna01_Zavrena = 21;
// int Pozice_KlepetoJedna01_Otevrena = 126;

// int Pozice_KlepetoJedna02_Zavrena = 79;
// int Pozice_KlepetoJedna02_Otevrena = 177;



// int PolohaSerbo = 0;

// // podle potreby proved kalibraci
// // vyrobte klepeta a nahradni baterky
// // sezeneme krabici

// bool BTworks = true; // jede bluetooth a pouzivame ho?
// #include "joystick.h"  // na lorris je to COM5 - musi byt pripojeny analyzer i terminal
// void print() {
//     while(true) {
//         Serial.println();
//         Serial.println(a);
//         Serial.println();
//         //udp.println(a);
//         a++;
//         delay(1000);
//     }
// }

// void setup() {
//     rkConfig cfg;
//     cfg.owner = "vasek"; // Ujistěte se, že v aplikace RBController máte nastavené stejné
//     cfg.name = "SokolSus";
//     cfg.motor_max_power_pct = 100; // limit výkonu motorů na xx %

//     cfg.motor_enable_failsafe = false;
//     cfg.rbcontroller_app_enable = false; // nepoužívám mobilní aplikaci (lze ji vypnout - kód se zrychlí, ale nelze ji odstranit z kódu -> kód se nezmenší)
//     rkSetup(cfg);
//     Serial.begin(115200);  // COM port 25 pocitac Burda


// //-----------------------------WIFI-----------------
//     WiFi.softAP("vasek", "sokolska");
//     WiFiUDP udp;
//     udp.begin(80);
// //--------------------------------------------------


// //--------------------------Bluethooth-NEPOUZIVAT-----

//     // if (!SerialBT.begin("TCA-BSokolSus")) //Bluetooth device name; zapnutí BT musí být až za rkSetup(cfg); jinak to nebude fungovat a bude to tvořit reset ESP32
//     // {
//     //     Serial.println("!!! Bluetooth initialization failed!");
//     // } else {
//     //     SerialBT.println("!!! Bluetooth work!\n");
//     //     Serial.println("!!! Bluetooth work!\n");
//     //     rkLedBlue(true);
//     //     delay(300);
//     //     rkLedBlue(false);
//     // }
// //-----------------------------------------------------



//     std::thread t1(print);

//     delay(300);
//     //fmt::print("{}'s SokolSus '{}' with {} mV started!\n", cfg.owner, cfg.name, rkBatteryVoltageMv());
//     rkLedYellow(true); // robot je připraven

//     servoBus.begin(4, UART_NUM_1, GPIO_NUM_27);

//     // servoBus.setAutoStop(0, true);
//     // -------- ??? -----printf("Start\n");
    


//     int count = 0;

    

//     while (true) {
//         char buffer[bufferSize] = { 0 };
//             memset(buffer, 0, bufferSize);

//         udp.parsePacket();
//         if ((count = udp.read(buffer, bufferSize - 1)) > 0) {
//                 //fwrite(buffer, 1, count, stdout);
//                 //wfflush(stdout);
                
//         }
//         if (buffer[0] == axisOpCode) {
//             handleAxes(buffer); // pohyb robota
//         }
//         else if (buffer[0] == buttonOpCode) {
//             handleButton(buffer); // tlacitka
//         }
//         else {
//             ////// tady zastav robota
//             rkMotorsSetPower(0, 0); //rkMotorsSetSpeed jede sotva polovicni rychlosti !!
//             }
//         }
//         delay(10);

//        //vTaskDelay(100 / portTICK_PERIOD_MS);
//     }
//     /*
//     // stary while
//     while(true) {
//         if ( read_joystick() ){

//             float axis_0 = (abs(axis[0]) < 10) ? 0 : -axis[0] /128.0;
//             //axis_0 = axis_0*axis_0*axis_0;
//             float axis_2 = (abs(axis[2]) < 10) ? 0 : -axis[2] /128.0;
//             // axis_1 = axis_1*axis_1*axis_1;

//             int levy_m = ((axis_2 * -plus_or_minus) - (axis_0 /2 )) * speed_coef;  // hodnota pro levy motor
//             int pravy_m = ((axis_2 * -plus_or_minus) + (axis_0 /2 )) * speed_coef; // hodnota pro pravy motor

//             if(levy_m + pravy_m > 160 || levy_m + pravy_m < -160) {
//                 levy_m = ((axis_2 * -plus_or_minus) - (axis_0 /4 )) * speed_coef;
//                 pravy_m = ((axis_2 * -plus_or_minus) + (axis_0 /4 )) * speed_coef;
//             }
//             //.................................jizda..dopredu..jizda..dozadu................
//             if (btn[0] == true) {
//                 plus_or_minus = 1;
//                 printf("Zmena smeru 1\n");
//             }

//             if (btn[0] == false) {
//                 plus_or_minus = -1;
//                 printf("Zmena smeru -1\n");
//             }
//             //................................ovladani..drapaku.............

//             if(btn[1] == true){
//                 if(btn[4] == false || btn[3] == false){
//                     PoziceNahore = PoziceNahore; // Ja vim ze je to blbost ale nech me byt
//                 }
//                 if(btn[4] == true){
//                     PoziceNahore = PoziceNahore + OKolikPosouvatPozici;
//                 }
//                 if(btn[3] == true){
//                     PoziceNahore = PoziceNahore - OKolikPosouvatPozici;
//                 }
//                 PoziceDrapaku = PoziceNahore;
//             }

//             if(btn[1] == false){
//                 if(btn[4] == false || btn[3] == false){
//                     PoziceDole = PoziceDole; // Ja vim ze je to blbost ale nech me byt
//                 }
//                 if(btn[4] == true){
//                     PoziceDole = PoziceDole + OKolikPosouvatPozici;
//                 }
//                 if(btn[3] == true){
//                     PoziceDole = PoziceDole - OKolikPosouvatPozici;
//                 }
//                 PoziceDrapaku = PoziceDole;
//             }

//         if(btn[0] == true){
//             Pozice_KlepetoJedna01 = Pozice_KlepetoJedna01_Otevrena;
//             delay(300);
//             Pozice_KlepetoJedna02 = Pozice_KlepetoJedna02_Otevrena;
//         }
//         if(btn[0] == false){
//             Pozice_KlepetoJedna02 = Pozice_KlepetoJedna02_Zavrena;
//             delay(300);
//             Pozice_KlepetoJedna01 = Pozice_KlepetoJedna01_Zavrena;
//         }
//             servoBus.set(0, Angle::deg(Pozice_KlepetoJedna01));
//             servoBus.set(1, Angle::deg(Pozice_KlepetoJedna02));

//             servoBus.set(2, Angle::deg(PoziceDrapaku));
//             servoBus.set(3, Angle::deg(PoziceDrapaku));

//             if (BTworks) {
//                 SerialBT.print(levy_m); SerialBT.print(" "); SerialBT.println(pravy_m);
//                 fmt::print("levy: {}, pravy: {} \n ", levy_m, pravy_m );
//             }
//             else {
//                 fmt::print("levy: {}, pravy: {} \n ", levy_m, pravy_m);
//             }


//             rkMotorsSetPower(levy_m, pravy_m); //rkMotorsSetSpeed jede sotva polovicni rychlosti !!

//             }
//         delay(1);
//     } */




// void handleAxes(const char buffer[bufferSize]) {
//     // auto& man = rb::Manager::get();

//     if (buffer[1] != axisCount) {
//         ESP_LOGE("UDP Parser", "Wrong axis count");
//     }
//     //---------------------------vypisuje-osy-------------------
//     printf("Axes:");
//     for(int i = 0; i < axisCount; i++) {
//         int axis = static_cast<int8_t>(buffer[i]);
//         printf("[%d]: %d, ", i, axis);
//     }
//     printf("\n");
//     //------------------------------------------------------------

//     // printf("orig x: %i\ty: %i\n", buffer[xAxisPosition], buffer[yAxisPosition]);
//     int x = static_cast<int8_t>(buffer[xAxisPosition]);
//     int y = -static_cast<int8_t>(buffer[yAxisPosition]);

//     float axis_0 = (abs(x) < 10) ? 0 : -x /128.0;
//     //axis_0 = axis_0*axis_0*axis_0;
//     float axis_2 = (abs(y) < 10) ? 0 : -y /128.0;
//     // axis_1 = axis_1*axis_1*axis_1;

//     int levy_m = ((axis_2 * -1) - (axis_0 /2 )) * speed_coef;  // hodnota pro levy motor
//     int pravy_m = ((axis_2 * -1) + (axis_0 /2 )) * speed_coef; // hodnota pro pravy motor

//     if(levy_m + pravy_m > 160 || levy_m + pravy_m < -160) {
//          levy_m = ((axis_2 * -1) - (axis_0 /4 )) * speed_coef;
//          pravy_m = ((axis_2 * -1) + (axis_0 /4 )) * speed_coef;
//     }

//     rkMotorsSetPower(levy_m, pravy_m); //rkMotorsSetSpeed jede sotva polovicni rychlosti !!


//     // tlacitka

// //     if (g_leftBatteryState) {
// //         int leftBatteryAngle = static_cast<int8_t>(buffer[armAxisPosition]);
// //         leftBatteryAngle = map(leftBatteryAngle, -128, 128, leftBatteryDown.deg(), leftBatteryUp.deg());
// //         leftBatteryAngle = rb::clamp(leftBatteryAngle, (int)leftBatteryDown.deg(), (int)leftBatteryUp.deg());

// //         man.servoBus().set(leftBatteryId, Angle::deg(leftBatteryAngle));
// //     }

// //     if (g_rightBatteryState) {
// //         int rightBatteryAngle = static_cast<int8_t>(buffer[armAxisPosition]);
// //         rightBatteryAngle = map(rightBatteryAngle, -128, 128, rightBatteryUp.deg(), rightBatteryDown.deg());
// //         rightBatteryAngle = rb::clamp(rightBatteryAngle, (int)rightBatteryDown.deg(), (int)rightBatteryUp.deg());

// //         man.servoBus().set(rightBatteryId, Angle::deg(rightBatteryAngle));
// //     }

// //    // if (!g_leftBatteryState && !g_rightBatteryState) {
// //     if (g_handState) { //pridano
// //         int armAngle = static_cast<int8_t>(buffer[armAxisPosition]);
// //         armAngle = map(armAngle, -128, 128, armDown.deg(), armUp.deg());
// //         armAngle = rb::clamp(armAngle, (int)armDown.deg(), (int)armUp.deg());

// //         man.servoBus().set(armId, Angle::deg(armAngle));
// //     }
//     // if (static_cast<int8_t>(buffer[armAxisPosition]) > 0) { // zapne mod, kdy je ruka pouze v krajnich polohach
//     //     man.servoBus().set(armId, armUp);
//     // }
//     // else
//     //     man.servoBus().set(armId, armDown);
// }

// void handleButton(const char btn[bufferSize]) {
//     size_t id = btn[buttonIdPosition];
//     uint8_t state = btn[buttonStatePosition];

//     if (id >= buttonCount) {
//         ESP_LOGE("UDP Parser", "Button id out of bounds");
//     }

//     // ESP_LOGI("UDP Parser", "Button %u changed to %u\n", id, state);
//     printf("Button %u changed to %u\n", id, state);
    
//     if (btn[4] == false || btn[3] == false) {
//         PolohaSerbo = PolohaSerbo;
//     }

//     if (btn[3] == true) {
//         PolohaSerbo = PolohaSerbo - 5;
//     }
//     if (btn[4] == true) {
//         PolohaSerbo = PolohaSerbo + 5;
//     }

//     servoBus.set(0, Angle::deg(PolohaSerbo));
//     servoBus.set(1, Angle::deg(PolohaSerbo));
//     servoBus.set(2, Angle::deg(PolohaSerbo));
//     servoBus.set(3, Angle::deg(PolohaSerbo));


// }
