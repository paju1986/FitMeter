"# FitMeter (H9 Heart Rate Monitor)

FitMeter is a lightweight application designed to monitor and display heart rate data from an H9 heart rate sensor via Bluetooth. It is built using C++ and the Qt framework, providing a clean and modern user interface through QML.

## 🚀 Features
- **Real-time Heart Rate Monitoring**: Connects to H9 sensors via Bluetooth Low Energy (BLE).
- **Modern UI**: A responsive interface developed with Qt Quick/QML.
- **Cross-platform Potential**: Built with CMake for easy compilation across different Linux-based environments.

## 🛠️ Tech Stack
- **Language**: C++17
- **Framework**: Qt 6.2 (Core, Gui, Qml, Quick, Bluetooth)
- **Build System**: CMake 3.16+
- **UI**: QML

## 📋 Prerequisites
Before building the project, ensure you have the following installed:
- **CMake** (3.16 or higher)
- **Qt 6.2** (including the Bluetooth module)
- **C++17 compatible compiler** (e.g., GCC or Clang)
- **Bluetooth hardware** and appropriate drivers installed on your system.

## ⚙️ Installation & Build

1. **Clone the repository:**
   ```bash
   git clone https://github.com/paju1986/FitMeter.git
   cd FitMeter
   ```

2. **Create a build directory:**
   ```bash
   mkdir build && cd build
   ```

3. **Configure and build:**
   ```bash
   cmake ..
   make
   ```

4. **Run the application:**
   ```bash
   ./h9-heartrate
   ```

## 📂 Project Structure
- `main.cpp`: Application entry point.
- `HeartRateManager.cpp/h`: Logic for Bluetooth communication and heart rate data processing.
- `Main.qml`: User interface definition.
- `CMakeLists.txt`: Build configuration.

## 📄 License
This project is licensed under the MIT License - see the LICENSE file for details.
"