/*
MKS ESP32 FOC 闭环位置控制例程 测试库：SimpleFOC 2.1.1 测试硬件：MKS ESP32 FOC V1.0
在串口窗口中输入：T+位置，就可以使得两个电机闭环转动
比如让两个电机都转动180°，则输入其弧度制：T3.14
在使用自己的电机时，请一定记得修改默认极对数，即 BLDCMotor(7) 中的值，设置为自己的极对数数字
程序默认设置的供电电压为 12V,用其他电压供电请记得修改 voltage_power_supply , voltage_limit 变量中的值
默认PID针对的电机是 2804云台电机 ，使用自己的电机需要修改PID参数，才能实现更好效果
*/

#include <SimpleFOC.h>


MagneticSensorI2C sensor = MagneticSensorI2C(AS5600_I2C);
MagneticSensorI2C sensor1 = MagneticSensorI2C(AS5600_I2C);
TwoWire I2Cone = TwoWire(0);
TwoWire I2Ctwo = TwoWire(1);

//电机参数
BLDCMotor motor = BLDCMotor(7);                               //根据选用电机的极对数，修改此处BLDCMotor()的值
BLDCDriver3PWM driver = BLDCDriver3PWM(32,33,25,22);

BLDCMotor motor1 = BLDCMotor(7);                              //同样修改此处BLDCMotor()的值
BLDCDriver3PWM driver1 = BLDCDriver3PWM(26,27,14,12);

//命令设置
//在串口窗口中输入：T+位置，就可以使得两个电机闭环转动
//比如让两个电机都转动180°，则输入其弧度制：T3.14
float target_velocity = 0;
Commander command = Commander(Serial);
void doTarget(char* cmd) { command.scalar(&target_velocity, cmd); }

void setup() {
  I2Cone.begin(19,18, 400000); 
  I2Ctwo.begin(23,5, 400000);
  sensor.init(&I2Cone);
  sensor1.init(&I2Ctwo);
  //连接motor对象与传感器对象
  motor.linkSensor(&sensor);
  motor1.linkSensor(&sensor1);

  //供电电压设置 [V]
  driver.voltage_power_supply = 12;                   //根据供电电压，修改此处voltage_power_supply的值
  driver.init();

  driver1.voltage_power_supply = 12;                  //同样修改此处voltage_power_supply的值
  driver1.init();
  //连接电机和driver对象
  motor.linkDriver(&driver);
  motor1.linkDriver(&driver1);
  
  //FOC模型选择
  motor.foc_modulation = FOCModulationType::SpaceVectorPWM;
  motor1.foc_modulation = FOCModulationType::SpaceVectorPWM;
  //运动控制模式设置
  motor.controller = MotionControlType::angle;
  motor1.controller = MotionControlType::angle;

  //速度PI环设置                                     
  motor.PID_velocity.P = 0.1;             //根据选用的电机，修改此处的PID参数，以实现更好的效果
  motor1.PID_velocity.P = 0.1;
  motor.PID_velocity.I = 1;
  motor1.PID_velocity.I = 1;
  //角度P环设置 
  motor.P_angle.P = 20;
  motor1.P_angle.P = 20;
  //最大电机限制电机
  motor.voltage_limit = 1;                //根据供电电压，修改此处voltage_limit的值
  motor1.voltage_limit = 1;               //同样修改此处voltage_limit的值
  
  //速度低通滤波时间常数
  motor.LPF_velocity.Tf = 0.01;
  motor1.LPF_velocity.Tf = 0.01;

  //设置最大速度限制
  motor.velocity_limit = 20;
  motor1.velocity_limit = 20;

  Serial.begin(115200);
  motor.useMonitoring(Serial);
  motor1.useMonitoring(Serial);

  
  //初始化电机
  motor.init();
  motor1.init();
  //初始化 FOC
  motor.initFOC();
  motor1.initFOC();
  command.add('T', doTarget, "target velocity");

  Serial.println(F("Motor ready."));
  Serial.println(F("Set the target velocity using serial terminal:"));
  
}



void loop() {
  Serial.print(sensor.getAngle()); 
  Serial.print(" - "); 
  Serial.print(sensor1.getAngle());
  Serial.println();
  motor.loopFOC();
  motor1.loopFOC();

  motor.move(target_velocity);
  motor1.move(target_velocity);
  
  command.run();
}
