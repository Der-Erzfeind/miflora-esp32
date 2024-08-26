#define CMD_PUMP1_OFF "101"
#define CMD_PUMP2_OFF "102"
#define CMD_PUMP3_OFF "103"
#define CMD_PUMP4_OFF "104"
#define CMD_PUMP5_OFF "105"
#define CMD_PUMP6_OFF "106"
#define CMD_PUMP1_ON "111"
#define CMD_PUMP2_ON "112"
#define CMD_PUMP3_ON "113"
#define CMD_PUMP4_ON "114"
#define CMD_PUMP5_ON "115"
#define CMD_PUMP6_ON "116"
#define ACK_PUMP1_OFF "201"
#define ACK_PUMP2_OFF "202"
#define ACK_PUMP3_OFF "203"
#define ACK_PUMP4_OFF "204"
#define ACK_PUMP5_OFF "205"
#define ACK_PUMP6_OFF "206"
#define ACK_PUMP1_ON "211"
#define ACK_PUMP2_ON "212"
#define ACK_PUMP3_ON "213"
#define ACK_PUMP4_ON "214"
#define ACK_PUMP5_ON "215"
#define ACK_PUMP6_ON "216"


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

  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, LOW);
  digitalWrite(pin5, LOW);
  digitalWrite(pin6, LOW);

  Serial.begin(115200);
}

String receivedCommand = ""; 

void loop() {
  while (Serial.available() > 0) {
    char inChar = Serial.read();   // Read each character

    // Append each character to the receivedCommand string
    if (inChar != '\n') {  // Until a newline is received
      receivedCommand += inChar;
    } else {
      // Handle the received command
      receivedCommand.trim();
      handleCommand(receivedCommand);

      // Clear the command string after processing
      receivedCommand = "";
    }
  }
}

// Function to handle received commands

void handleCommand(String command) {
      Serial.println(command);

      if(command ==  CMD_PUMP1_ON){// Turn pin 2 ON
        digitalWrite(pin1, HIGH);
        Serial.println(ACK_PUMP1_ON);
      }

      else if(command ==  CMD_PUMP1_OFF){ // Turn pin 2 OFF
        digitalWrite(pin1, LOW);
        Serial.println(ACK_PUMP1_OFF);
      }

      else if(command ==  CMD_PUMP2_ON){// Turn pin 3 ON
        digitalWrite(pin2, HIGH);
        Serial.println(ACK_PUMP2_ON);
      }

      else if(command ==  CMD_PUMP2_OFF){ // Turn pin 3 OFF
        digitalWrite(pin2, LOW);
        Serial.println(ACK_PUMP2_OFF);
      }

      else if(command ==  CMD_PUMP3_ON){// Turn pin 4 ON
        digitalWrite(pin3, HIGH);
        Serial.println(ACK_PUMP3_ON);
      }

      else if(command ==  CMD_PUMP3_OFF){ // Turn pin 4 OFF
        digitalWrite(pin3, LOW);
        Serial.println(ACK_PUMP3_OFF);
      }

      else if(command ==  CMD_PUMP4_ON){// Turn pin 5 ON
        digitalWrite(pin4, HIGH);
        Serial.println(ACK_PUMP4_ON);
      }

      else if(command ==  CMD_PUMP4_OFF){ // Turn pin 5 OFF
        digitalWrite(pin4, LOW);
        Serial.println(ACK_PUMP4_OFF);
      }

      else if(command ==  CMD_PUMP5_ON){// Turn pin 6 ON
        digitalWrite(pin5, HIGH);
        Serial.println(ACK_PUMP5_ON);
      }

      else if(command ==  CMD_PUMP5_OFF){ // Turn pin 6 OFF
        digitalWrite(pin5, LOW);
        Serial.println(ACK_PUMP5_OFF);
      }

      else if(command ==  CMD_PUMP6_ON){// Turn pin 7 ON
        digitalWrite(pin6, HIGH);
        Serial.println(ACK_PUMP6_ON);
      }

      else if(command ==  CMD_PUMP6_OFF){ // Turn pin 7 OFF
        digitalWrite(pin6, LOW);
        Serial.println(ACK_PUMP6_OFF);
      }

      //else
        //Serial.println(-1);
    }

