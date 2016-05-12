#include <Cmd.h>

const char *cmd_banner = "** TUFF Command Line Tester Ready **";
const char *cmd_prompt = "TUFF>> ";
const char *cmd_unrecog = "Commands: on, off, update, synchronize, reset, cmd, clock, data";

// Change these for any other board.
// pin 40 (P2_5) is clock
// pin 39 (P2_4) is data
#define CLOCK_PIN 40
#define DATA_PIN 39

void data_cmd(int arg_cnt, char **args) {
  unsigned int val;
  arg_cnt--;
  args++;
  if (!arg_cnt) {
      Serial.println("clock needs 1 argument - bit to clock in");
      Serial.print(cmd_prompt);
      return;
  }
  val = atoi(*args);
  if (!val) {
    digitalWrite(DATA_PIN, 0);
  } else {
    digitalWrite(DATA_PIN, 1);
  }
}
  
void clock_cmd(int arg_cnt, char **args) {
  unsigned int val;
  arg_cnt--;
  args++;
  if (!arg_cnt) {
      Serial.println("clock needs 1 argument - bit to clock in");
      Serial.print(cmd_prompt);
      return;
  }
  val = atoi(*args);
  if (!val) {
    digitalWrite(CLOCK_PIN, 0);
  } else {
    digitalWrite(CLOCK_PIN, 1);
  }
}

void sendCommand(unsigned int val) {
  // Lower clock first. It should idle high.
  digitalWrite(CLOCK_PIN, 0);
  digitalWrite(DATA_PIN, 0);
  for (unsigned int i=0;i<16;i++) {
      digitalWrite(CLOCK_PIN, 0);
      delayMicroseconds(10);
      if (val & 0x8000) digitalWrite(DATA_PIN, 1);
      else digitalWrite(DATA_PIN, 0);
      delayMicroseconds(10);
      digitalWrite(CLOCK_PIN, 1);
      delayMicroseconds(20);
      val = val << 1;
  }
  // Leave clock high.
}
void cmd_cmd(int arg_cnt, char **args) {
  unsigned int cmd;
  arg_cnt--;
  args++;
  
  if (arg_cnt < 1) {
    Serial.println("cmd needs 1 argument - command to send");
    Serial.print(cmd_prompt);
    return;
  }
  cmd = atoi(*args);
  Serial.print("Sending command ");
  Serial.println(cmd, HEX);
  sendCommand(cmd);
}

void reset_cmd(int arg_cnt, char **args) {
  sendCommand(0xFFFF);
  delay(100);
//  sendCommand(0xFFFF);
//  delay(100);
  Serial.println("Reset.");
}

void synchronize(int arg_cnt, char **args) {
  sendCommand(0xD00D);
  delay(100);
  Serial.println("Synchronized.");
  Serial.print(cmd_prompt);
}

void update(int arg_cnt, char **args) {
  unsigned int channel;
  unsigned int notch;
  unsigned int value;
  unsigned int cmd;
  unsigned int store;
  
  arg_cnt--;
  args++;
  if (arg_cnt < 3) {
    Serial.println("update usage: update channel notch value [store]");
    Serial.println("            : channel between 1-6");
    Serial.println("            : notch between 1-3");
    Serial.println("            : value between 1-31");
    Serial.println("            : if store is non-zero, also copy all values to default");
    return;
  }
  channel = atoi(*args);
  args++;
  arg_cnt--;
  notch = atoi(*args);
  args++;
  arg_cnt--;
  value = atoi(*args);
  args++;
  arg_cnt--;
  if (arg_cnt) {
    store = atoi(*args);
  } else {
    store = 0;
  }
  if (channel < 1 || channel > 6) {
    Serial.println("channel must be between 1 and 6");
    return;
  }
  if (notch < 1 || notch > 3) {
    Serial.println("notch must be between 1 and 3");
    return;
  }
  if (value > 31) {
    Serial.println("value must be between 0 and 31");
    return;
  }
  cmd = 0;
  if (store != 0) cmd |= 0x20;
  cmd |= value;
  cmd |= (channel << 12) | ((notch-1) << 8);
  cmd |= 0x40;
  Serial.print("Sending cmd ");
  Serial.println(cmd, HEX);
  sendCommand(cmd);
}

void notchOn(int arg_cnt, char **args) {
  unsigned int channel;
  unsigned int notch;
  unsigned int value;
  unsigned int cmd;
  
  arg_cnt--;
  args++;
  // arg_cnt needs 3 arguments: channel, notch #, and value
  if (arg_cnt < 2) {
    Serial.println("on usage: on channel notch");
    Serial.println("        : channel between 1-6");
    Serial.println("        : notch between 1-3");
    return;
  }
  channel = atoi(*args);
  args++;
  notch = atoi(*args);
  if (channel < 1 || channel > 6) {
    Serial.println("channel must be between 1 and 6");
    return;
  }
  if (notch < 1 || notch > 3) {
    Serial.println("notch must be between 1 and 3");
    return;
  }
  cmd = (channel << 12) | ((notch - 1) << 8) | 0x20;
  Serial.print("Sending cmd ");
  Serial.println(cmd, HEX);
  sendCommand(cmd);
}

void notchOff(int arg_cnt, char **args) {
  unsigned int channel;
  unsigned int notch;
  unsigned int cmd;
  
  arg_cnt--;
  args++;
  
  if (arg_cnt < 2) {
    Serial.println("off usage: off channel notch");
    Serial.println("         : channel between 1-6");
    Serial.println("         : notch between 1-3");
    return;
  }
  channel = atoi(*args);
  args++;
  notch = atoi(*args);
  if (channel < 1 || channel > 6) {
    Serial.println("channel must be between 1 and 6");
    return;
  }
  if (notch < 1 || notch > 3) {
    Serial.println("notch must be between 1 and 3");
    return;
  }
  cmd = (channel << 12) | ((notch-1) << 8);
  Serial.print("Sending cmd ");
  Serial.println(cmd, HEX);
  sendCommand(cmd);
}

void setup()
{
  // put your setup code here, to run once:
  cmdInit(9600);
  digitalWrite(CLOCK_PIN, 1);
  digitalWrite(DATA_PIN, 0);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  cmdAdd("on", notchOn);
  cmdAdd("off", notchOff);
  cmdAdd("update", update);
  cmdAdd("synchronize", synchronize);
  cmdAdd("reset", reset_cmd);
  cmdAdd("cmd", cmd_cmd);
  cmdAdd("clock", clock_cmd);
  cmdAdd("data", data_cmd);
}

void loop()
{
  cmdPoll();
}
