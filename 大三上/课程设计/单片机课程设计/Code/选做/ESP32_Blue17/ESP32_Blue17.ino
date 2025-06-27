#include "BluetoothSerial.h"  // 引入蓝牙串口库

BluetoothSerial SerialBT;  // 创建 BluetoothSerial 对象

const int led = 2;  // 定义 LED 引脚
char receivedPassword[11];  // 存储用户输入的密码
int passwordIndex = 0;  // 密码索引
const char correctPassword[] = "1234";  // 预设的正确密码
int attemptCount = 0;  // 错误次数计数
const int maxAttempts = 5;  // 最大尝试次数

unsigned long lastAttemptTime = 0;  // 上次错误输入的时间
const unsigned long lockDuration = 30000;  // 锁定持续时间（毫秒）

bool bluetoothConnected = false;  // 蓝牙连接状态标志
unsigned long lastSendTime = 0;  // 上次发送状态的时间

void setup() {
  pinMode(led, OUTPUT);  // 设置 LED 引脚为输出
  digitalWrite(led, LOW);  // 初始化 LED 为关闭

  SerialBT.begin("ESP32_Blue");  // 启动蓝牙串口并设置蓝牙名称
  Serial.begin(115200);  // 初始化串口，用于调试输出
  Serial1.begin(9600, SERIAL_8N1, 16, 17);  // 初始化串口1（D16为RX，D17为TX）

  Serial.println("ESP32 蓝牙串口启动，等待蓝牙连接...");
}

void checkBluetoothStatus() {
  // 检查蓝牙连接状态
  if (SerialBT.connected()) {
    if (!bluetoothConnected) {
      bluetoothConnected = true;  // 蓝牙刚刚连接成功
      Serial.println("蓝牙连接成功！");
      SerialBT.println("蓝牙已连接，请输入密码：");
      // 不再发送 'A'
    }
  } else {
    if (bluetoothConnected) {
      bluetoothConnected = false;  // 蓝牙断开连接
      Serial.println("蓝牙连接断开！");
    }
    // 不再发送 'B'
  }
}

void handlePasswordInput() {
  // 检查是否处于锁定状态
  if (attemptCount >= maxAttempts) {
    if (millis() - lastAttemptTime < lockDuration) {
      // 锁定中，不处理输入
      return;
    } else {
      // 锁定结束，重置计数
      attemptCount = 0;
      SerialBT.println("锁定已解除，请重新输入密码：");
    }
  }

  if (!bluetoothConnected) return;  // 如果蓝牙未连接，不处理密码输入

  // 检查蓝牙串口是否有数据可读
  while (SerialBT.available()) {
    char receivedChar = SerialBT.read();

    // 检测输入结束符（空格）
    if (receivedChar == ' ') {
      receivedPassword[passwordIndex] = '\0';  // 添加字符串结束符

      // 显示接收到的密码
      Serial.print("接收到的密码：");
      Serial.println(receivedPassword);

      // 密码验证
      if (strcmp(receivedPassword, correctPassword) == 0) {
        Serial.println("密码正确");
        digitalWrite(led, HIGH);
        SerialBT.println("密码正确，欢迎！");
        attemptCount = 0;  // 重置错误计数
      } else {
        Serial.println("密码错误");
        digitalWrite(led, LOW);
        attemptCount++;

        if (attemptCount >= maxAttempts) {
          lastAttemptTime = millis();
          SerialBT.println("错误次数过多，已锁定输入30秒！");
        } else {
          SerialBT.print("密码错误，剩余尝试次数：");
          SerialBT.println(maxAttempts - attemptCount);
        }
      }

      // 将密码发送给单片机
      Serial1.print(receivedPassword);  // 使用 Serial1.print 确保逐字发送
      Serial1.write(' ');  // 追加发送空格作为结束符

      // 清空缓冲区
      memset(receivedPassword, 0, sizeof(receivedPassword));
      passwordIndex = 0;
    } else if (passwordIndex < sizeof(receivedPassword) - 1) {
      // 存储用户输入字符
      receivedPassword[passwordIndex++] = receivedChar;
    }
  }
}

void loop() {
  checkBluetoothStatus();  // 检查蓝牙连接状态
  handlePasswordInput();  // 处理密码输入
  delay(10);               // 添加小延时以降低CPU使用率
}
