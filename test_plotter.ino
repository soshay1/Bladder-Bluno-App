void setup() {
  pinMode(5, OUTPUT);
  pinMode(A0, INPUT);
  Serial.begin(115200);               //initial the Serial
}
 
void loop()
{
   // char bt = Serial.read();
    delay(2000);
    //if(Serial.available())
    //{
      int SDA = digitalRead(A4); //represents SDA pin
      //Serial.write(SDA);
      Serial.print("SDA: ");
      Serial.print(SDA);
      int SCL = digitalRead(A5); // represents SCL
      //Serial.write(SCL);
      Serial.print(" SCL: ");
      Serial.print(SCL);
      Serial.print("\n");
  //}
}  
