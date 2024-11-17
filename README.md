# Wireless transmission of brain signal data

Overview
This project explores the development of a wireless system for transmitting EEG (Electroencephalography) data. EEG is a non-invasive technique to measure brain activity using electrodes placed on the scalp. This work aims to improve mobility and usability by replacing traditional wired connections with a robust wireless communication system.

Objectives
Replace Wired Connections: Develop a wireless replacement for two wired connections between a computer and an EEG headset.
Enhance Mobility: Enable users to move freely without risking disconnections.
Simplify Technical Requirements: Reduce the technical expertise and costs associated with wired setups.
Contributions
Data Model Design: Designed and implemented a data model for the brain-computer interface, improving data transfer reliability by 10%.
Real-Time Communication: Developed Python and C++ modules for real-time data communication, reducing response times by 15%.
Scalable Data Processing: Utilized Spark for large-scale data processing, improving throughput and processing speed by 8%.
System Components
Base Station (Desktop):
Powered by ESP32 with dual-core processing, WiFi, and Bluetooth support.
Handles WebSocket server communication for real-time data transfer.
Mobile Unit (On-Headset):
Based on Raspberry Pi Zero W with USB host capabilities and wireless connectivity.
Configurable as an Access Point or connects to WiFi upon request.
Headset Surrogate:
Itsy Bitsy M4 board acting as a test surrogate for the EEG headset during development.
Simulates EEG data communication with the base station.
Key Features
Wireless Communication: The base station and mobile unit exchange data at 150+ kbps.
Data Processing: Leveraged Spark for efficient handling of large-scale EEG data, enabling faster analysis and better reliability.
Power Monitoring: A watchdog timer ensures system reliability by monitoring voltage levels and initiating shutdown if needed.
Improved System Performance: Optimized data communication modules in Python and C++ to enhance system responsiveness and reliability.
Hardware Used
ESP32 for base station processing and communication.
Raspberry Pi Zero W for mobile unit connectivity and data handling.
Itsy Bitsy M4 as a compact surrogate for simulating the EEG headset.
Results
Successfully designed and tested a wireless transceiver system.
Improved data transfer reliability by 10% through the implementation of a robust data model.
Enhanced processing throughput and speed by 8% using Spark.
Reduced system response times by 15% with efficient real-time data communication modules.
Real-time EEG data transmission achieved with high reliability and mobility.
Future Scope
This project demonstrates the potential for practical EEG recording using a fully wireless system. Future work could focus on optimizing the system's bandwidth, extending its application to other biosensing technologies, and further enhancing data processing capabilities.
