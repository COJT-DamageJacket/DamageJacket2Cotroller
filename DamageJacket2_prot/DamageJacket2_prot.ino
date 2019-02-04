// Relay Control

byte receive = 0;

int waiting = 0; //timer counter

const int reA_0 = 5;
const int reA_1 = 6;
const int reB_0 = 7;
const int reB_1 = 8;

const int motor_1 = 9;
const int motor_2 = 10;

const int coil_1 = 11;
const int coil_2 = 12;

const int front_sens = 2;
const int back_sens = 3;

int state = 0; //0:ready,1:release,2:move forward,3:magnetise,4:move back

void setup()
{
  Serial.begin(9600);
  
  pinMode(reA_0, OUTPUT);
  pinMode(reA_1, OUTPUT);
  pinMode(reB_0, OUTPUT);
  pinMode(reB_1, OUTPUT);

  pinMode(motor_1,OUTPUT);
  pinMode(motor_2,OUTPUT);

  pinMode(coil_1,OUTPUT);
  pinMode(coil_2,OUTPUT);

  pinMode(front_sens,INPUT_PULLUP);
  pinMode(back_sens,INPUT_PULLUP);


  //motor and coil off
  digitalWrite(motor_1,0);
  digitalWrite(motor_2,0);

  digitalWrite(coil_1,0);
  digitalWrite(coil_2,0); 

  //reset to released
  state=1;
}

void loop()
{
  if(Serial.available()>=1){
    receive = Serial.read();
    while(Serial.available())Serial.read();
  }else{
    waiting++;
    if(waiting>50){
      waiting=0;
      //Serial.write(0); //0=.?
      //digitalWrite(ledpin,LOW); //turn off LED
    }
  }

  digitalWrite(reA_0,bitRead(receive, 0));
  digitalWrite(reA_1,bitRead(receive, 1));
  
  digitalWrite(reB_0,bitRead(receive, 2));
  digitalWrite(reB_1,bitRead(receive, 3));
  

  if(state!=0) bitWrite(receive, 4, 0);;
  
  switch (state) {
    case 0://0:ready
      if(!bitRead(receive, 4)){//Do Nothing
        //Serial.write((byte)0x00);
      }else{//Do anything
        coil_drive(2);//DEmagnetise
        delay(3000);
        coil_drive(0);//stop
        bitWrite(receive, 4, 0);
        state++;
      }
      
      break;
  
    case 1://1:released
      motor_drive(1);//forward
      state++;
      break;
      
    case 2://2:move forward
      if(digitalRead(front_sens)==HIGH) {
        state++;
        motor_drive(0);//stop
      }
      break;
      
    case 3://3:magnetise
      coil_drive(1);//magnetise
      delay(3000);
      coil_drive(0);//stop
      motor_drive(2);//back
      state++;
      break;
      
    case 4://4:move back
      if(digitalRead(back_sens)==HIGH) {
        state=0;
        motor_drive(0);//stop
      }
      break;
      
    //default:
    }

    Serial.write((byte)state);

  //Serial.write(receive); //send raw data like echo
}

void motor_drive (int n) //0:stop,1:forward,2:back,3:break
{
  switch (n) {
    case 0://0:stop
      digitalWrite(motor_1,0);
      digitalWrite(motor_2,0);
      break;

    case 1://1:forward
      digitalWrite(motor_1,1);
      digitalWrite(motor_2,0);
      break;
      
    case 2://2:back
      digitalWrite(motor_1,0);
      digitalWrite(motor_2,1);
      break;
      
    case 3://3:break
      digitalWrite(motor_1,1);
      digitalWrite(motor_2,1);
      break;
      
    default://stop for any bad case
      digitalWrite(motor_1,0);
      digitalWrite(motor_2,0);
      break;
  }
}

void coil_drive (int n) //0:stop,1:forward,2:back,3:break
{
  switch (n) {
    case 0://0:stop
      digitalWrite(coil_1,0);
      digitalWrite(coil_2,0);
      break;

    case 1://1:forward
      digitalWrite(coil_1,1);
      digitalWrite(coil_2,0);
      break;
      
    case 2://2:back
      digitalWrite(coil_1,0);
      digitalWrite(coil_2,1);
      break;
      
    case 3://3:break
      digitalWrite(coil_1,1);
      digitalWrite(coil_2,1);
      break;
      
    default://stop for any bad case
      digitalWrite(coil_1,0);
      digitalWrite(coil_2,0);
      break;
  }
}
