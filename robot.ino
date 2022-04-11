#include <WiFi.h>

// В двух строчках ниже пишем SSID и пароль для своей WiFi-сети:
const char* ssid     = "ESP32-Robot";
const char* password = "123456789";

// Создаем объект сервера и задаем ему порт «80»:
WiFiServer server(80);

// Переменная для хранения HTTP-запроса:
String header;

// Мотор 1:
int motor1Pin1 = 27; 
int motor1Pin2 = 26; 
int enable1Pin = 14; 

// Мотор 2:
int motor2Pin1 = 33; 
int motor2Pin2 = 25; 
int enable2Pin = 32;

// Переменные для свойств ШИМ:
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 0;

// Переменные для расшифровки HTTP-запроса GET:
String valueString = "0";
int pos1 = 0;
int pos2 = 0;

void setup() {
  Serial.begin(115200);
  
  // Переключаем контакты моторов в режим «OUTPUT»:
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);

  // Задаем настройки для ШИМ-канала:
  ledcSetup(pwmChannel, freq, resolution);
  
  // Подключаем ШИМ-канал 0 к контактам ENA и ENB,
  // т.е. к GPIO-контактам для управления скоростью моторов: 
  ledcAttachPin(enable1Pin, pwmChannel);
  ledcAttachPin(enable2Pin, pwmChannel); 

  // Генерируем ШИМ-сигнал для контактов ENA и ENB
  // с коэффициентом заполнения «0»:
  ledcWrite(pwmChannel, dutyCycle);
  
  // Подключаемся к WiFi-сети
  // при помощи заданных выше SSID и пароля: 
  Serial.print("Setting AP (Access Point)...");
           //  "Настраиваем точку доступа..."
  // Если вы хотите, чтобы ваша точка доступа была открытой,
  // удалите параметр «password» из метода ниже:
  WiFi.softAP(ssid, password);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");  //  "IP-адрес точки доступа"
  Serial.println(IP);

  server.begin();
}

void loop(){
  WiFiClient client = server.available();  // Запускаем прослушку 
                                           // входящих клиентов.

  if (client) {                            // Если подключился 
                                           // новый клиент,
    Serial.println("New Client.");         // печатаем в монитор порта  
                                           // сообщение об этом.
    String currentLine = "";               // Создаем строку
                                           // для хранения данных,
                                           // пришедших от клиента.
    while (client.connected()) {           // Запускаем цикл while(), 
                                           // который будет работать,
                                           // пока клиент подключен.
      if (client.available()) {            // Если у клиента
                                           // есть байты, которые
                                           // можно прочесть,
        char c = client.read();            // считываем байт
        Serial.write(c);                   // и печатаем его 
                                           // в мониторе порта.
        header += c;
        if (c == '\n') {                   // Если полученный байт – 
                                           // это символ новой строки.
          // Если мы получили два символа новой строки подряд,
          // то это значит, что текущая строка пуста.
          // Это конец HTTP-запроса клиента, поэтому шлём ответ:
          if (currentLine.length() == 0) {
            // HTTP-заголовки всегда начинаются с кода ответа
            // (например, «HTTP/1.1 200 OK») и данных о типе контента,
            // чтобы клиент знал, что получает,
            // а после этого пишем пустую строчку:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
                       //  "Соединение: отключено"
            client.println();
            
            // Включаем и выключаем GPIO-контакты:
            if (header.indexOf("GET /forward") >= 0) {
              Serial.println("Forward");  //  "Вперед"
              digitalWrite(motor1Pin1, LOW);
              digitalWrite(motor1Pin2, HIGH); 
              digitalWrite(motor2Pin1, LOW);
              digitalWrite(motor2Pin2, HIGH);
            }  else if (header.indexOf("GET /left") >= 0) {
              Serial.println("Left");  //  "Влево"
              digitalWrite(motor1Pin1, LOW); 
              digitalWrite(motor1Pin2, LOW); 
              digitalWrite(motor2Pin1, LOW);
              digitalWrite(motor2Pin2, HIGH);
            }  else if (header.indexOf("GET /stop") >= 0) {
              Serial.println("Stop");  //  "Стоп"
              digitalWrite(motor1Pin1, LOW); 
              digitalWrite(motor1Pin2, LOW); 
              digitalWrite(motor2Pin1, LOW);
              digitalWrite(motor2Pin2, LOW);             
            } else if (header.indexOf("GET /right") >= 0) {
              Serial.println("Right");  //  "Вправо"
              digitalWrite(motor1Pin1, LOW); 
              digitalWrite(motor1Pin2, HIGH); 
              digitalWrite(motor2Pin1, LOW);
              digitalWrite(motor2Pin2, LOW);    
            } else if (header.indexOf("GET /reverse") >= 0) {
              Serial.println("Reverse");  //  "Назад"
              digitalWrite(motor1Pin1, HIGH);
              digitalWrite(motor1Pin2, LOW); 
              digitalWrite(motor2Pin1, HIGH);
              digitalWrite(motor2Pin2, LOW);          
            }
            // Показываем веб-страницу:
            client.println("<!DOCTYPE HTML><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // При помощи CSS задаем стиль кнопок. 
            // Попробуйте поэкспериментировать 
            // с атрибутами «background-color» и «font-size»,
            // чтобы стилизовать кнопки согласно своим предпочтениям:
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { -webkit-user-select: none; -moz-user-select: none; -ms-user-select: none; user-select: none; background-color: #4CAF50;");
            client.println("border: none; color: white; padding: 12px 28px; text-decoration: none; font-size: 26px; margin: 1px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Веб-страница:        
            client.println("<p><a href=\"/forward\"><button class=\"button\" onclick=\"moveForward()\">FORWARD</button></a></p>");
            client.println("<div style=\"clear: both;\"><p><a href=\"/left\"><button class=\"button\" onclick=\"moveLeft()\">LEFT </button></a>");
            client.println("<a href=\"/stop\"><button class=\"button button2\" onclick=\"stopRobot()\">STOP</button></a>");
            client.println("<a href=\"/right\"><button class=\"button\" onclick=\"moveRight()\">RIGHT</button></a></p></div>");
            client.println("<p><a href=\"/reverse\"><button class=\"button\" onclick=\"moveReverse()\">REVERSE</button></a></p>");        
            client.println("<input type=\"range\" min=\"0\" max=\"100\" step=\"25\" id=\"motorSlider\" onchange=\"motorSpeed(this.value)\" value=\"" + valueString + "\"/>");
            
            client.println("<script> function motorSpeed(pos) { ");
            client.println("var xhr = new XMLHttpRequest();"); 
            client.println("xhr.open('GET', \"/?value=\" + pos + \"&\", true);"); 
            client.println("xhr.send(); } </script>");
           
            client.println("</html>");
            
            // Пример запроса: «GET /?value=100& HTTP/1.1»;
            // Он задает коэффициент заполнения ШИМ на 100%:
            if(header.indexOf("GET /?value=")>=0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              valueString = header.substring(pos1+1, pos2);
              // Задаем скорость мотора:
              if (valueString == "0") {
                ledcWrite(pwmChannel, 0);
                digitalWrite(motor1Pin1, LOW); 
                digitalWrite(motor1Pin2, LOW); 
                digitalWrite(motor2Pin1, LOW);
                digitalWrite(motor2Pin2, LOW);   
              }
              else { 
                dutyCycle = map(valueString.toInt(), 25, 100, 200, 255);
                ledcWrite(pwmChannel, dutyCycle);
                Serial.println(valueString);
              } 
            }         
            // HTTP-ответ заканчивается еще одной пустой строкой:
            client.println();
            // Выходим из цикла while():
            break;
          } else {  // Если получили символ новой строки,
                    // очищаем переменную «currentLine»
            currentLine = "";
          }
        } else if (c != '\r') {  // Если получили что-либо,
                                 // кроме символа возврата каретки,
          currentLine += c;      // добавляем эти данные
                                 // в конец переменной «currentLine»
        }
      }
    }
    // Очищаем переменную «header»:
    header = "";
    // Отключаем соединение:
    client.stop();
    Serial.println("Client disconnected.");  //  "Клиент отключился."
    Serial.println("");
  }
}
