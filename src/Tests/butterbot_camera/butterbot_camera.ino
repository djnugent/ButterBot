#define START_FRAME 0xFF

void setup() {
  // put your setup code here, to run once:
  Serial.begin(38400);
}

void loop() {
  if(Serial.available()>4){
    while(Serial.read() != START_FRAME);
    int x = Serial.read();
    int y = Serial.read();
    int size = Serial.read();
    int fps = Serial.read();
    Serial.print(x);
    Serial.print(',');
    Serial.print(y);
    Serial.print(',');
    Serial.print(size);
    Serial.print(',');
    Serial.println(fps);
    
  }
  delay(10);
}
