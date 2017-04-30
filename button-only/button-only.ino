const int r = 2;
const int g = 2;
const int b = 4;

const int button = 7;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(button, INPUT);
  pinMode(r, INPUT);
  pinMode(g, INPUT);
  pinMode(b, INPUT);
  
  digitalWrite(button, LOW);
  digitalWrite(r, LOW);
  digitalWrite(g, LOW);
  digitalWrite(b, LOW);

}

void loop() {
  // put your main code here, to run repeatedly:
  int buttonVal = digitalRead(button);
  Serial.print("buttonVal: ");
  Serial.println(buttonVal);
}
