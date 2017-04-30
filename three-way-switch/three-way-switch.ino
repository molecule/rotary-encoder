const int switchRight = 13;
const int switchCenter = 12;
const int switchLeft = 11;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(switchRight, INPUT);
  pinMode(switchCenter, INPUT);
  pinMode(switchLeft, INPUT);

  digitalWrite(switchRight, HIGH);
  digitalWrite(switchCenter, HIGH);
  digitalWrite(switchLeft, HIGH);

}

void loop() {
  // put your main code here, to run repeatedly:
  int rightVal = digitalRead(switchRight);
  int centerVal = digitalRead(switchCenter);
  int leftVal = digitalRead(switchLeft);
  
  Serial.print("right: ");
  Serial.println(rightVal);
  Serial.print("center: ");
  Serial.println(centerVal);
  Serial.print("left: ");
  Serial.println(leftVal);

  delay(1000);

}
