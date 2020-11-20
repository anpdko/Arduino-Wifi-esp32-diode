/*********
  Complete project details at https://randomnerdtutorials.com
*********/

// Загрузить библиотеку Wi-Fi
#include <WiFi.h>

TaskHandle_t Task1;
TaskHandle_t Task2;

// Замените на свои сетевые учетные данные
const char* ssid     = "Project";
const char* password = "1111";

// Установите номер порта веб-сервера на 80
WiFiServer server(80);

// Переменная для хранения HTTP-запроса
String header;

// Назначьте выходные переменные контактам GPIO
const int output26 = 26;
const int output27 = 27;


// Пременые для чтения задержки
String valueString;
String tempString;
int pos1 = 0;
int pos2 = 0;

int delayone = 0;
int delaytwo = 0;

void parsingData() {
  if (header.indexOf("?delayone=&") >= 0 && header.indexOf("&delaytwo=") >= 0) {
    pos1 = header.indexOf('=');
    pos2 = header.indexOf('&');

    valueString = header.substring(pos1 + 1);
    pos1 = valueString.indexOf('=');

    tempString = valueString.substring(pos1 + 1);
    delaytwo = tempString.toInt();
  }
  else if (header.indexOf("?delayone=") >= 0 && header.indexOf("&delaytwo=") >= 0) {
    pos1 = header.indexOf('=');
    pos2 = header.indexOf('&');

    tempString = header.substring(pos1 + 1, pos2);
    delayone = tempString.toInt();

    valueString = header.substring(pos1 + 1);

    pos1 = valueString.indexOf('=');

    tempString = valueString.substring(pos1 + 1);

    if (tempString.toInt() != 0) {
      delaytwo = tempString.toInt();
    }
  }
}

void setup() {
  Serial.begin(115200);

  //Запустить функцию в 1 ярде
  xTaskCreatePinnedToCore( Task1code, "Task1", 10000, NULL, 1, &Task1, 0);
  delay(500);
  xTaskCreatePinnedToCore(Task2code, "Task2", 10000, NULL, 1, &Task2, 1);
  delay(500);


  // Инициализировать выходные переменные как выходы
  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  // Установите выходы на НИЗКИЙ
  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);

  // Подключитесь к сети Wi-Fi с SSID и паролем
  Serial.print("Setting AP (Access Point)…");
  // Удалите параметр пароля, если хотите, чтобы AP (точка доступа) была открыта
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("IP-адрес точки доступа: ");
  Serial.println(IP);

  server.begin();
}

void Task1code(void * pvParameters) {
  while (true) {
    WiFiClient client = server.available();   // Слушайте входящих клиентов

    if (client) {                             // Если подключается новый клиент,
      Serial.println("New Client.");          // распечатать сообщение в последовательном порту
      String currentLine = "";                // сделать String для хранения входящих данных от клиента
      while (client.connected()) {            // цикл, пока клиент подключен
        if (client.available()) {             // если есть байты для чтения от клиента,
          char c = client.read();             // прочтите байт, затем
          Serial.write(c);                    // распечатать серийный монитор
          header += c;
          if (c == '\n') {                    // если байт является символом новой строки
            // если текущая строка пуста, у вас есть два символа новой строки подряд.
            // это конец клиентского HTTP-запроса, поэтому отправьте ответ:
            if (currentLine.length() == 0) {
              // Заголовки HTTP всегда начинаются с кода ответа (например, HTTP / 1.1 200 OK)
              // и тип содержимого, чтобы клиент знал, что будет дальше, затем пустая строка:
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();
              Serial.println("-----------------");
              Serial.println("-----------------");
              // включает и выключает GPIO

              if (header.indexOf("GET /?delayone=&") >= 0 && header.indexOf("&delaytwo=") >= 0) {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');

                valueString = header.substring(pos1 + 1);
                pos1 = valueString.indexOf('=');

                tempString = valueString.substring(pos1 + 1);
                delaytwo = tempString.toInt();
              }
              else if (header.indexOf("GET /?delayone=") >= 0 && header.indexOf("&delaytwo=") >= 0) {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');

                tempString = header.substring(pos1 + 1, pos2);
                delayone = tempString.toInt();

                valueString = header.substring(pos1 + 1);

                pos1 = valueString.indexOf('=');

                tempString = valueString.substring(pos1 + 1);

                if (tempString.toInt() != 0) {
                  delaytwo = tempString.toInt();
                }
              }
              Serial.println("!!!!");
              Serial.print("delayone: ");
              Serial.println(delayone);
              Serial.print("delaytwo: ");
              Serial.println(delaytwo);

              // Отображение веб-страницы HTML
              client.println("<!DOCTYPE html><html>");
              client.println("<head><meta charset=\"utf-8\" name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
              client.println("<link rel=\"icon\" href=\"data:,\">");

              client.println("<style type=\"text/css\">{font-family: sans-serif;}");
              client.println("fieldset{width: 250px;margin-bottom: 20px; }");
              client.println(".text{width: 70px;}.submit{margin-top: 17px;}</style></head>");
              client.println("<body><center><form>");
              client.println("<h2>Управление светодиода</h2><span>Частота зеленого мигания:</span> ");
              client.println("<input type=\"text\" name=\"delayone\" class=\"text\"> (мс)<br><br>");
              client.println("<span>Частота красного мигания:</span> ");
              client.println("<input type=\"text\" name=\"delaytwo\" class=\"text\"> (мс)<br>");
              client.println("<input class=\"submit\" type=\"submit\" value=\"Отправить\">");
              client.println("</form></center></body></html>");

              // HTTP-ответ заканчивается еще одной пустой строкой
              client.println();
              // Выйти из цикла while
              break;
            } else { // если у вас есть новая строка, очистите currentLine
              currentLine = "";
            }
          } else if (c != '\r') {  //если у вас есть что-то еще, кроме символа возврата каретки,
            currentLine += c;      // добавляем в конец currentLine
          }
        }
      }
      // Очистить переменную заголовка
      header = "";
      // Закрываем соединение
      client.stop();
      Serial.println("Client disconnected.");
      Serial.println("");
    }
  }
}


unsigned long currentTime;
unsigned long currentTime2;
bool ledState = LOW;
bool ledState2 = LOW;

void Task2code(void * pvParameters) {

  while (true) {
    if (millis() - currentTime > delayone)
    {
      currentTime = millis();
      ledState = !ledState;
      digitalWrite(26, ledState);
    }
    if (millis() - currentTime2 > delaytwo)
    {
      currentTime2 = millis();
      ledState2 = !ledState2;
      digitalWrite(27, ledState2);
    }
  }
}



void loop() {

}
