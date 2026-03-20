# Sensor Actuator Node (Modbus + FreeRTOS)

Embedded firmware for **STM32F407 Discovery** implementing a **sensor and actuator node** using **FreeRTOS** and **Modbus RTU**.

The system acquires sensor data, controls actuators via PWM, and provides both a CLI interface and Modbus registers for monitoring and configuration.

The firmware follows a **DMA-first, interrupt-driven architecture** to minimize CPU load and reflect industrial embedded system design practices.

---

## 📦 Hardware

### 🧩 Target Board

* STM32F407 Discovery

### 📡 Onboard Sensors

* LIS3DSH Accelerometer (SPI)
* Internal Temperature Sensor

### 🔌 External Components

* Potentiometer (ADC input)
* USB-to-TTL converter (CLI – USART3)
* USB-to-TTL converter (Modbus – USART2)
* LED / PWM output device

---

## 🚀 System Features

* FreeRTOS-based multitasking firmware
* Modbus RTU slave implementation
* CLI command interface over UART
* ADC sensor acquisition using DMA
* SPI accelerometer acquisition
* PWM control via timer
* Watchdog-based fault recovery
* System health monitoring

---

## 🧠 Task Architecture

| Task               | Function                          | Priority |
| ------------------ | --------------------------------- | -------- |
| Communication Task | Handles Modbus RTU requests       | 4        |
| ADC Task           | Reads potentiometer & temperature | 3        |
| Accelerometer Task | Reads accelerometer data          | 3        |
| Control Task       | Updates PWM output                | 3        |
| CLI Task           | Parses CLI commands               | 2        |
| Print Task         | Handles UART logging              | 2        |
| Monitor Task       | System health monitoring          | 1        |

---

## 🔗 Modbus Support

The firmware operates as a **Modbus RTU Slave**.

### Supported Function Codes

| Code | Function                 |
| ---- | ------------------------ |
| 0x03 | Read Holding Registers   |
| 0x04 | Read Input Registers     |
| 0x06 | Write Single Register    |
| 0x10 | Write Multiple Registers |

---

### 📥 Holding Registers (Configuration)

| Register         | Description      |
| ---------------- | ---------------- |
| LED Duty         | PWM duty cycle   |
| DO State         | Digital output   |
| Modbus ID        | Device slave ID  |
| Baud Rate        | UART baud rate   |
| Firmware Version | Firmware version |

---

### 📤 Input Registers (Sensor Data)

| Register    | Description          |
| ----------- | -------------------- |
| Accel X     | X-axis acceleration  |
| Accel Y     | Y-axis acceleration  |
| Accel Z     | Z-axis acceleration  |
| Temperature | Internal temperature |
| Voltage     | ADC voltage          |
| DI State    | Digital input state  |
| Uptime      | System uptime        |

---

## 💻 CLI Commands

The CLI runs on **USART3 (115200 baud)**.

| Command         | Description                  |
| --------------- | ---------------------------- |
| help            | List available commands      |
| set_pwm <value> | Set PWM duty cycle           |
| get_temp        | Read temperature             |
| get_accel       | Read accelerometer values    |
| get_info        | Display system information   |
| do <0/1>        | Set digital output           |
| modbus          | Display Modbus configuration |

---

## 🔌 Hardware Connections

### ADC

| Sensor Pin | STM32 Pin |
| ---------- | --------- |
| VCC        | 3.3V      |
| GND        | GND       |
| ADC OUT    | PA0       |

---

### CLI UART (USART3)

| STM32 Pin | TTL Module |
| --------- | ---------- |
| PD8 (TX)  | RX         |
| PB11 (RX) | TX         |
| GND       | GND        |

---

### Modbus UART (USART2)

| STM32 Pin | TTL Module |
| --------- | ---------- |
| PA2 (TX)  | RX         |
| PA3 (RX)  | TX         |
| GND       | GND        |

---

### PWM Output

| Device Pin | STM32 Pin |
| ---------- | --------- |
| VCC        | PC6       |
| GND        | GND       |

---

### Digital Output

| Device Pin | STM32 Pin |
| ---------- | --------- |
| VCC        | PC9       |
| GND        | GND       |

---

## 🛠️ Build and Flash

### Requirements

* VS Code
* CMake
* Ninja
* ARM GCC Toolchain
* OpenOCD
* QModMaster (for Modbus testing)

---

### 🔨 Build Firmware

```bash
cmake -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=toolchain-arm-none-eabi.cmake
cd build
ninja
```

---

### ⚡ Flash Firmware

```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program firmware.elf verify reset exit"
```

---

## 🧪 Testing

### 1️⃣ Power Up

* Connect board using ST-Link USB

**Expected Behavior:**

* Firmware boots successfully
* Heartbeat LED (PD12) starts blinking

---

### 2️⃣ CLI Test

Serial settings:

| Parameter | Value  |
| --------- | ------ |
| Baud Rate | 115200 |
| Data Bits | 8      |
| Stop Bits | 1      |
| Parity    | None   |

Example:

```bash
help
get_temp
get_accel
set_pwm 50
```

---

### 3️⃣ PWM Test

```bash
set_pwm 20
set_pwm 80
```

**Expected Result:**

* LED brightness changes

---

### 4️⃣ Sensor Test

```bash
get_temp
get_accel
```

**Expected Result:**

* Sensor values printed in terminal

---

### 5️⃣ Modbus Test

Use **QModMaster** or any Modbus master tool.

#### Read Input Registers (0x04)

* Start Address: 0
* Quantity: 7

#### Read Holding Registers (0x03)

* Start Address: 0
* Quantity: 5

#### Write Single Register (0x06)

* Register: LED Duty
* Value: New duty cycle

**Expected Behavior:**

* Registers update correctly
* PWM duty cycle changes

---

## 📊 System Monitoring

The Monitor Task periodically updates:

* System uptime
* Free heap memory
* Number of active tasks
* Watchdog refresh
* Heartbeat LED

---

## 🤝 Contribution

Contributions are welcome!

### Steps:

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Submit a pull request

---

## 👩‍💻 Author

Akanksha Bhaisare
