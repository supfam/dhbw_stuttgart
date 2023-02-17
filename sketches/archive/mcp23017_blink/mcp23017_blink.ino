#include <Wire.h>
#include <Adafruit_MCP23X17.h>


Adafruit_MCP23X17 mcp;

void setup() {  

  mcp.begin_I2C(); // mcp_begin() existiert für MCP23X17 Instanzen nicht -> evtl. veraltet
  
  mcp.pinMode(0, OUTPUT);  // expander pin GPA0 als output
  mcp.pinMode(1, OUTPUT);  // expander pin GPA1 als output
  
}


void loop() {

  delay(200);
  mcp.digitalWrite(0, HIGH); // LED an GPA0 an
  delay(200);
  mcp.digitalWrite(1, HIGH); //LED an GPA1 an
  delay(200);
  mcp.digitalWrite(0, LOW); //LED an GPA0 aus
  delay(200);
  mcp.digitalWrite(1, LOW); //LED an GPA1 aus

}
