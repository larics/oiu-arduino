String sensorData;
String tempSensorData;

int Str2int(String str_val) // funkcija pretvara objekt String u integer
{
  char buffer[10];
  str_val.toCharArray(buffer, 10);
  int int_val = atoi(buffer);
  return int_val;
}

// funkcija za razvdajanje objekta String na mjestu separator_char

void split(String array_str[], int len, String input_str, char separator_char)
{
  String temp = input_str;
  for (int i=0; i<len; i++)
  {
    int index_separator_char = temp.indexOf(separator_char);
    array_str[i] = temp.substring(0, index_separator_char);
    temp = temp.substring(index_separator_char + 1);
  }
}

void Version() // za sigurnije povezivanje sa python programom
{
  Serial.println("version");
}


void DigitalCallback(int mode, String data) // rad sa digitalnim nacinom rada pinova
{
  int pin = Str2int(data);
  if (mode <= 0)
  {
    Serial.println(digitalRead(pin));
  }
  else
  {
    if (pin < 0)
    {
      digitalWrite(-pin, LOW);
    }
    else
    {
      digitalWrite(pin, HIGH);
    }
  }
}


void AnalogCallback(int mode, String data) // rad sa analognim nacinom rada pinova
{
  if (mode <= 0)
  {
    int pin = Str2int(data);
    Serial.println(analogRead(pin));
  }
  else
  {
    String str_data[2];
    split(str_data, 2, data, '%');
    int pin = Str2int(str_data[0]);
    int pin_value = Str2int(str_data[1]);
    analogWrite(pin, pin_value);
  }
}

void ConfigurePinCallback(String data) // odabir moda rada pojedinog pina
{
  int pin = Str2int(data);
  if (pin <= 0)
  {
    pinMode(-pin, INPUT);
  }
  else
  {
    pinMode(pin, OUTPUT);
  }
}

void SensorCallback(void) // slanje podataka sa senzora
{
  char thisChar;
  Serial1.write('s');
  Serial1.flush();
  delay(25);
  while (Serial1.available() > 0)
  {
    thisChar = Serial1.read();
    Serial.write(thisChar);
    Serial.flush();
  }
}


void SerialParser(void) // parser naredbi serijske komunikacije sa python programom
{
  char readChar[64];
  if (Serial.available() > 0)
  {
    Serial.readBytesUntil(33, readChar, 64);
  }
  else
  {
    return;
  }
  String read_str = String(readChar);
  int index_1 = read_str.indexOf('%');
  int index_2 = read_str.indexOf('$');
  String command = read_str.substring(1, index_1);
  String data = read_str.substring(index_1 + 1, index_2);
  read_str = String("");
  
  if  (command == "dw")
  {
    DigitalCallback(1, data);
  }
  else if (command == "dr")
  {
    DigitalCallback(0, data);
  }
  else if (command == "aw")
  {
    AnalogCallback(1, data);
  }
  else if (command == "ar")
  {
    AnalogCallback(0, data);
  }
  else if (command == "pm")
  {
    ConfigurePinCallback(data);
  }
  else if (command == "version")
  {
    Version();
  }
  else if (command == "sd")
  {
    SensorCallback();
  }
}
  
void setup()
{
  Serial.begin(9600);
  while (!Serial)
  {
    ;
  }
  Serial1.begin(57600);
}

void loop()
{
  if (Serial && Serial1)
  {
    SerialParser();
  }
  delay(15);
}
