void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  for(int i = 0; i < 7; i++){
    Serial.print("#0");
    Serial.println(i);
    delay(1000);
    Serial.println("q");
    delay(100);
  }
  
  // put your main code here, to run repeatedly:

}
