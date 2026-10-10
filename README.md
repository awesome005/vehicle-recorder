# 汽车行驶状态记录仪 (Vehicle State Recorder)

基于 STM32 的双节点车载嵌入式系统，模拟汽车 ECU + 仪表架构。
使用 CAN 总线进行节点间通信，实时采集车辆姿态、加速度、环境数据，
并判断行驶状态（正常/加速/刹车/转弯）。

本科毕业设计项目 · V1.1

---

## 项目简介

本项目采用双节点架构：

- **ZET6 节点（ECU）**：采集 MPU6050、DHT22 数据，进行姿态解算、状态判断，通过 CAN 总线发送数据，蓝牙作为调试接口。基于 FreeRTOS 多任务调度。
- **C8T6 节点（仪表）**：接收 CAN 报文，通过 OLED 分页显示数据。

CAN 总线模拟汽车内部 ECU 通信，两节点间使用 SN65HVD230 收发器，波特率 500kbps，标准帧格式。

---

## 功能特性

- MPU6050 六轴姿态采集（加速度 + 角速度）
- 互补滤波姿态解算（Roll / Pitch / Yaw）
- 陀螺仪零偏自动校准
- DHT22 温湿度采集
- 行驶状态判断（正常 / 加速 / 刹车 / 转弯）
- CAN 总线自定义应用层协议
- **FreeRTOS 多任务调度（V1.1 新增）**
- OLED 自动翻页显示（3 页循环）
- 蓝牙调试输出

---

## 系统架构

**ZET6 节点（ECU / 采集节点）**
- MPU6050 通过 I2C2 读取数据
- DHT22 通过 GPIO 读取数据
- 数据处理 + 姿态解算 + 状态判断
- CAN1 (500kbps) 发送到总线
- HC-05 蓝牙通过 USART1 调试输出

**C8T6 节点（仪表 / 显示节点）**
- CAN1 (500kbps) 从总线接收
- 报文解析 + 数据缓存
- OLED 通过 I2C1 显示，自动翻页 3 页

两个节点通过 CANH / CANL / GND 连接。

---

## FreeRTOS 任务架构（V1.1）

大板（ZET6）基于 FreeRTOS CMSIS_V2，拆分为 4 个独立任务：

| 任务 | 周期 | 优先级 | 功能 |
|---|---|---|---|
| MPUTask | 10ms | High | 读取 MPU6050、姿态解算、状态判断 |
| CANTask | 20ms | AboveNormal | 发送姿态/加速度（20ms）、状态（100ms） |
| DHTTask | 5s | Low | 读取 DHT22、发送温湿度 CAN 报文 |
| DEBUGTask | 1s | Normal | 蓝牙调试输出 |

HAL 时基源使用 TIM6，避免与 FreeRTOS 的 SysTick 冲突。

---

## 硬件清单

**主控**
- STM32F103ZET6（正点原子精英板）× 1，ECU 节点
- STM32F103C8T6（最小系统板）× 1，仪表节点

**传感器与模块**
- MPU6050 × 1，六轴姿态传感器
- DHT22 × 1，温湿度传感器
- OLED 0.96 寸 × 1，I2C 显示屏（SSD1306）
- HC-05 × 1，蓝牙调试模块
- SN65HVD230 × 2，CAN 收发器

**其他**
- 120Ω 终端电阻 × 2
- 面包板、杜邦线、双绞线若干
- CMSIS-DAP 调试器

---

## 引脚分配

完整引脚配置见 `BLUE.ioc`（CubeMX 工程文件）。核心引脚：

| 功能 | 引脚 |
|---|---|
| MPU6050 | PB10 (SCL) / PB11 (SDA)，I2C2 |
| DHT22 | PB0 |
| HC-05 | PA9 / PA10，USART1 |
| CAN（两节点） | PA11 (RX) / PA12 (TX) |
| OLED（C8T6） | PB6 (SCL) / PB7 (SDA)，I2C1 |

---

## CAN 应用层协议

本项目自定义的应用层协议，非标准汽车 CAN 协议。

**0x100 姿态数据**，DLC 8，周期 20ms
- Byte0-1：Pitch ×100，int16
- Byte2-3：Roll ×100，int16
- Byte4-5：Yaw ×100，int16
- Byte6-7：保留

**0x101 加速度数据**，DLC 8，周期 20ms
- Byte0-1：Ax，int16，LSB
- Byte2-3：Ay，int16，LSB
- Byte4-5：Az，int16，LSB
- Byte6-7：保留

**0x200 温湿度**，DLC 8，周期 5s
- Byte0-1：Temp ×10，int16
- Byte2-3：Humi ×10，int16
- Byte4-7：保留

**0x300 车辆状态**，DLC 8，周期 100ms
- Byte0：State，uint8
- Byte1-7：保留

**状态定义（Byte0）**
- 0：NORMAL，静止或平稳行驶
- 1：ACCEL，Ax > 0.15g
- 2：BRAKE，Ax < -0.15g
- 3：TURN，|Ay| > 0.15g

---

## 工程结构

**Core/Inc/ 头文件**
- main.h
- mpu6050.h（MPU6050 驱动）
- dht22.h（DHT22 驱动）
- oled.h（OLED 驱动）
- my_can.h（CAN 应用层）
- sensor.h（传感器数据打包）
- dwt.h（DWT 微秒延时）

**Core/Src/ 源文件**
- main.c（主程序）
- freertos.c（FreeRTOS 任务）
- mpu6050.c / dht22.c / oled.c / my_can.c / sensor.c / dwt.c

**Drivers/**：HAL 库（CubeMX 生成）

**MDK-ARM/**：Keil 工程（BLUE.uvprojx、BLUE.sct、启动文件）

**BLUE.ioc**：CubeMX 工程文件

---

## 开发环境

- STM32CubeMX 6.x
- Keil MDK 5.x（ARMCC V5.06）
- HAL 库：STM32F1xx HAL Driver
- FreeRTOS CMSIS_V2
- 调试器：CMSIS-DAP

---

## 编译与烧录

1. 克隆仓库：
git clone https://gitee.com/awesome05/vehicle-recorder.git
2. 使用 STM32CubeMX 打开对应 `.ioc`，生成代码（可选）
3. 使用 Keil MDK 打开 `ZET6_ECU/MDK-ARM/BLUE.uvprojx` 或 `C8T6_Display/MDK-ARM/BLUEC8T6.uvprojx`
4. 编译工程（Rebuild）
5. 使用 CMSIS-DAP 连接目标板，烧录

---

## 使用说明

1. 两节点通过 SN65HVD230 连接 CANH / CANL / GND
2. 总线上接 120Ω 终端电阻（两端各一）
3. 上电后：
- ZET6 开始采集数据并通过 CAN 发送
- C8T6 接收数据并显示在 OLED 上
- OLED 每 3 秒自动翻页
4. 通过蓝牙串口查看 ZET6 调试信息（9600 bps）

**OLED 页面**
- 第 1 页：State / Roll / Pitch
- 第 2 页：State / Temp / Humi
- 第 3 页：State / Ax / Ay / Az

---

## 版本规划

| 版本 | 状态 | 内容 |
|---|---|---|
| **V1.0** | ✅ 已完成 | 裸机双节点 CAN + MPU6050 + DHT22 + OLED + 蓝牙 |
| **V1.1** | ✅ 已完成 | 大板迁移到 FreeRTOS 多任务架构 |
| V1.2 | 🔄 计划中 | SD 卡数据记录（FatFs） |
| V2.0 | 🔄 计划中 | UDS 诊断（ISO-TP） |

---

## 项目亮点

- **双节点 ECU 架构**：模拟真实汽车 ECU 与仪表的关系
- **CAN 总线通信**：自定义应用层协议，分层 ID 规划
- **姿态解算**：互补滤波融合加速度计与陀螺仪
- **FreeRTOS 多任务调度**：按数据重要性分配任务优先级与周期
- **模块化设计**：驱动层、应用层分离

---

## 许可

本项目为本科毕业设计，仅用于学习与交流。

---

## 作者

**awesome**

- Gitee: https://gitee.com/awesome05
- GitHub: https://github.com/awesome005
- 邮箱: z1225691192@163.com

---

## 致谢

- STMicroelectronics（HAL 库）