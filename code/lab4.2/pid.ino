const int MOTOR_0 = 5;
const int MOTOR_1 = 6;
int LEDC_0 = 0;
int LEDC_1 = 1;
int LEDC_FREQ = 5000;
int LEDC_RES = 12;

const int ENCODER_0 = 0;
const int ENCODER_1 = 1;

int encoderCount_0 = 0;
int encoderCount_1 = 0;
volatile unsigned long prevTime_0 = 0;
volatile unsigned long prevTime_1 = 0;

float Kp = 2, Ki = 0.3, Kd = 0.001;
float Setpoint_0 = 50.0;  // seems unused
float last_RPM_0 = 0;
float last_RPM_1 = 0;
float integral_0 = 0;
float integral_1 = 0;
float derivative_0 = 0;
float derivative_1 = 0;

int Output_0 = 0;  // seems unused
float RPM_0 = 0.0;
float RPM_1 = 0.0;
float deltaTime_0 = 0.0;
float deltaTime_1 = 0.0;

unsigned long lastTime_0 = 0;
unsigned long lastTime_1 = 0;
unsigned long currentTime_0 = 0;  // seems usused
unsigned long currentTime_1 = 0;

int call_PID_flag_0 = 0;
int call_PID_flag_1 = 0;


int calculatePID_0(float setpoint, float Kp, float Ki, float Kd, float &lastInput, unsigned long &lastTime, float RPM_val)
{
  // Serial.print("RPM: ");
  // Serial.println(RPM);
  // Serial.print("DeltaTime: ");
  // Serial.println(deltaTime);

  float error = setpoint - RPM_val;
  // float integral = integral + error;
  integral_0 = integral_0 + error;
  derivative_0 = 1000 * (RPM_val - last_RPM_0) / 1;
  int output = Kp * error + Ki * integral_0 + constrain(Kd * derivative_0, -0.1 * Kp * error, 0.1 * Kp * error);
  output = constrain(output, -80, 40);

  // Print individual components for debugging
  // Serial.print("Error: ");
  // Serial.println(error);
  // Serial.print("P Term: ");
  // Serial.println(Kp * error);
  // Serial.print("I Term: ");
  // Serial.println(Ki * integral);
  // Serial.print("D Term: ");
  // Serial.println(Kd * derivative);
  // Serial.print("Output: ");
  // Serial.println(output);
  // Serial.println(error);

  last_RPM_0 = RPM_val;
  return output;
}


void setup() {
  Serial.begin(115200);

  // 初始化LEDC用于电机控制
  ledcSetup(LEDC_0, LEDC_FREQ, LEDC_RES);
  ledcAttachPin(MOTOR_0, LEDC_0);

  // Set encoder and photointerrupter pins as inputs
  pinMode(ENCODER_0, INPUT_PULLUP);
  // Attach an interrupt for the encoder signal
  attachInterrupt(digitalPinToInterrupt(ENCODER_0), handleEncoderInterrupt, HIGH);
}

void loop() {
  int setpoint = 70;

  if (call_PID_flag_0 == 1) {
    int output = calculatePID_0(setpoint, Kp, Ki, Kd, last_RPM_0, lastTime_0, RPM_0);
    int motorSpeed = map(output, -80, 40, 4095, 0);

    ledcWrite(LEDC_0, motorSpeed);
    // Serial.print("PWM: ");
    // Serial.println(motorSpeed);
    call_PID_flag_0 = 0;
    encoderCount_0 = 0;
  }
  //delay(50);
}


void handleEncoderInterrupt() {
  encoderCount_0++;
  if (encoderCount_0 == 1) {
    call_PID_flag_0 = 1;
    unsigned long currentTime = millis();
    deltaTime_0 = (currentTime - lastTime_0);  // Convert milliseconds to seconds
    RPM_0 = (60 * 1 * 1000) / (20 * deltaTime_0);
    lastTime_0 = currentTime;
  }
}
