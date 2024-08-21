#define CMD_PUMP1_OFF 010
#define CMD_PUMP2_OFF 020
#define CMD_PUMP3_OFF 030
#define CMD_PUMP4_OFF 040
#define CMD_PUMP5_OFF 050
#define CMD_PUMP6_OFF 060

#define CMD_PUMP1_ON 011
#define CMD_PUMP2_ON 021
#define CMD_PUMP3_ON 031
#define CMD_PUMP4_ON 041
#define CMD_PUMP5_ON 051
#define CMD_PUMP6_ON 061

#define ACK_PUMP1_OFF 110
#define ACK_PUMP2_OFF 120
#define ACK_PUMP3_OFF 130
#define ACK_PUMP4_OFF 140
#define ACK_PUMP5_OFF 150
#define ACK_PUMP6_OFF 160

#define ACK_PUMP1_ON 111
#define ACK_PUMP2_ON 121
#define ACK_PUMP3_ON 131
#define ACK_PUMP4_ON 141
#define ACK_PUMP5_ON 151
#define ACK_PUMP6_ON 161


// Define the pins to control
const int pin1 = 2;
const int pin2 = 3;
const int pin3 = 4;
const int pin4 = 5;
const int pin5 = 6;
const int pin6 = 7;

void setup() {

  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
  pinMode(pin4, OUTPUT);
  pinMode(pin5, OUTPUT);
  pinMode(pin6, OUTPUT);

  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, HIGH);
  digitalWrite(pin3, HIGH);
  digitalWrite(pin4, HIGH);
  digitalWrite(pin5, HIGH);
  digitalWrite(pin6, HIGH);

  Serial.begin(115200);
}

void loop() {
  // Check if data is available to read
  if (Serial.available() > 0) {
    // Read the incoming byte
    int command = Serial.read();

    // Handle the command
    switch (command) {
      case CMD_PUMP1_ON: // Turn pin 2 ON
        digitalWrite(pin1, LOW);
        Serial.println(ACK_PUMP1_ON);
        break;

      case CMD_PUMP1_OFF: // Turn pin 2 OFF
        digitalWrite(pin1, HIGH);
        Serial.println(ACK_PUMP1_OFF);
        break;

      case CMD_PUMP2_ON: // Turn pin 3 ON
        digitalWrite(pin2, LOW);
        Serial.println(ACK_PUMP2_ON);
        break;

      case CMD_PUMP2_OFF: // Turn pin 3 OFF
        digitalWrite(pin2, HIGH);
        Serial.println(ACK_PUMP2_OFF);
        break;

      case CMD_PUMP3_ON: // Turn pin 4 ON
        digitalWrite(pin3, LOW);
        Serial.println(ACK_PUMP3_ON);
        break;

      case CMD_PUMP3_OFF: // Turn pin 4 OFF
        digitalWrite(pin3, HIGH);
        Serial.println(ACK_PUMP3_OFF);
        break;

      case CMD_PUMP4_ON: // Turn pin 5 ON
        digitalWrite(pin4, LOW);
        Serial.println(ACK_PUMP4_ON);
        break;

      case CMD_PUMP4_OFF: // Turn pin 5 OFF
        digitalWrite(pin4, HIGH);
        Serial.println(ACK_PUMP4_OFF);
        break;

      case CMD_PUMP5_ON: // Turn pin 6 ON
        digitalWrite(pin5, LOW);
        Serial.println(ACK_PUMP5_ON);
        break;

      case CMD_PUMP5_OFF: // Turn pin 6 OFF
        digitalWrite(pin5, HIGH);
        Serial.println(ACK_PUMP5_OFF);
        break;

      case CMD_PUMP6_ON: // Turn pin 7 ON
        digitalWrite(pin6, LOW);
        Serial.println(ACK_PUMP6_ON);
        break;

      case CMD_PUMP6_OFF: // Turn pin 7 OFF
        digitalWrite(pin6, HIGH);
        Serial.println(ACK_PUMP6_OFF);
        break;

      default:
        Serial.println(-1);
        break;
    }
  }
}
