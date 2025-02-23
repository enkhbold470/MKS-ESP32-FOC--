// MKS ESP32 FOC 开环位置控制例程 测试库：SimpleFOC 2.1.1 测试硬件：MKS ESP32 FOC V1.0
// 串口中输入"T+数字"设定两个电机的位置，如设置电机转到到180度，输入 "T3.14"（弧度制的180度）
// 在使用自己的电机时，请一定记得修改默认极对数，即 BLDCMotor(7) 中的值，设置为自己的极对数数字
// 程序默认设置的供电电压为 12V,用其他电压供电请记得修改 voltage_power_supply , voltage_limit 变量中的值

#include <SimpleFOC.h>

BLDCMotor motor = BLDCMotor(7);         //根据选用的电机，修改正确的极对数，即BLDCMotor()中的值
BLDCDriver3PWM driver = BLDCDriver3PWM(32, 33, 25, 22);

BLDCMotor motor1 = BLDCMotor(7);        //同样修改此处极对数的值
BLDCDriver3PWM driver1 = BLDCDriver3PWM(26, 27, 14, 12);

//目标变量
float target_velocity = 0;

//串口指令设置
Commander command = Commander(Serial);
void doTarget(char* cmd) { command.scalar(&target_velocity, cmd); }

void setup() {

  driver.voltage_power_supply = 12;       //根据电机的供电电压，修改voltage_power_supply变量的值
  driver.init();
  motor.linkDriver(&driver);
  motor.voltage_limit = 12;   // [V]      //根据电机的供电电压，修改voltage_limit变量的值
  motor.velocity_limit = 15; // [rad/s]
  
  driver1.voltage_power_supply = 12;
  driver1.init();
  motor1.linkDriver(&driver1);
  motor1.voltage_limit = 12;   // [V]
  motor1.velocity_limit = 15; // [rad/s]

 
  //开环控制模式设置
  motor.controller = MotionControlType::angle_openloop;
  motor1.controller = MotionControlType::angle_openloop;

  //初始化硬件
  motor.init();
  motor1.init();


  //增加 T 指令
  //在串口监视器中输入"T+数字"的指令并点击发送，可以控制两个电机转动到指定的位置
  //例如：T3.14就是转动到正方向180°的位置
  command.add('T', doTarget, "target velocity");

  Serial.begin(115200);
  Serial.println("Motor ready!");
  Serial.println("Set target velocity [rad/s]");
  _delay(1000);
}

void loop() {
  motor.move(target_velocity);
  motor1.move(target_velocity);

  //用户通讯
  command.run();
}
