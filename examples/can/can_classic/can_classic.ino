#include <VBCoreG4_arduino_system.h>


//в VBCoreG4_arduino_system.h пин PA5 определен как LED2 

//запустить can c распберри - sudo ip link set can0 up txqueuelen 65535 type can bitrate 1000000 dbitrate 8000000 fd on
//отправить сообщение c распберри - cansend can0 00000123#DEADBEEF, ID всегда содержит 8 цифр
//прочитать все сообщения в can -  candump can0
//прочитать сообщение can по его ID -  candump can0,ID:7ff

//в libraries добавить библиотеку VBCoreG4_arduino_system
//функция can_init() запускает can
//функция get_hfdcan() возвращает переменную типа FDCAN_HandleTypeDef, без которой невозможно взаимодействие с can
//функция create_header(uint8_t ID) создает хидер для отправки сообщения, в нее нужно передать переменную типа uint8_t - ID сообщения


uint8_t data[4] = {222, 173, 190, 239}; //DE AD BE EF
FDCAN_HandleTypeDef*  hfdcan1; // создаем указатель на переменную типа FDCAN_HandleTypeDef
CanFD* can;
FDCAN_TxHeaderTypeDef TxHeader; //FDCAN_TxHeaderTypeDef TxHeader;
void setup() {
  Serial.begin(115200);
  pinMode(LED2, OUTPUT);
  /* Настройка FD CAN  */
  SystemClock_Config();  // Настройка тактирования
  can = new CanFD();  // Создаем управляющий класс
  can->init();  // Инициализация CAN
  can->write_default_params_classic();  // Записываем дефолтные параметры для classic CAN
  can->apply_config();  // Применяем их
  can->default_start();  // Запуск FDCAN модуля
  hfdcan1 = can->get_hfdcan();  // Сохраняем конфиг
  
  TxHeader.Identifier = 0x68;
  TxHeader.DataLength = FDCAN_DLC_BYTES_4;
  TxHeader.IdType = FDCAN_EXTENDED_ID;

}


void loop() {
  //------Отправка сообщения в can------
  
  if (HAL_FDCAN_GetTxFifoFreeLevel(hfdcan1) != 0){
    
    if (HAL_FDCAN_AddMessageToTxFifoQ(hfdcan1, &TxHeader, data) != HAL_OK){ Error_Handler();} 
    else{digitalWrite(LED2, !digitalRead(LED2));} //помигаем светодиодом, если все ок
    
  }
  
  // -------Получение сообщений из can-------
  while(HAL_FDCAN_GetRxFifoFillLevel(hfdcan1, FDCAN_RX_FIFO0) > 0 )
    {
      FDCAN_RxHeaderTypeDef Header;  // хидер для входящего сообщения
      uint8_t RxData[4]; // максимальная длина сообщения - 64 байта 
      if (HAL_FDCAN_GetRxMessage(hfdcan1, FDCAN_RX_FIFO0, &Header, RxData) != HAL_OK){ Serial.println("error"); Error_Handler(); }  
      else{ // напечатаем первые 4 байта входящего сообщения, если все ок. Пример отправки сообщения cansend can0 00000123#DEADBEEF 
      Serial.print("ID ");
      Serial.print(Header.Identifier); // ID сообщения 
      Serial.print(" data: ");
      Serial.print(RxData[0]);
      Serial.print("  ");
      Serial.print(RxData[1]);
      Serial.print("  ");
      Serial.print(RxData[2]);
      Serial.print("  ");
      Serial.print(RxData[3]);
      Serial.println("  ");
    }
    }
  delay(100);
  
}