# EasyChargeSystem

## About the Project
This project is a comprehensive **Charging Station Infrastructure Management System** designed to handle complex relationships between charging stations, available ports, and vehicle queues.

The system is built with a focus on efficient memory management and advanced data structures to ensure scalability and performance.

### Key Technical Features
* **Advanced Data Structures:** * **Binary Search Trees (BST):** Used for managing Stations and Cars to allow fast searching and sorted data retrieval.
    * **Queues:** Implemented for car waitlists at charging ports to ensure a First-In-First-Out (FIFO) charging sequence.
    * **Linked Lists:** Managing dynamic lists of charging ports within each station.
* **File Persistence:** Full support for loading and saving system states from/to external data files (`Stations.txt`, `Cars.txt`, `Ports.txt`).
* **Memory Management:** Implements deep-cleaning recursive functions (`freeAll`) to prevent memory leaks in complex nested structures.
* **Interactive CLI:** A modular menu-driven interface for real-time system interaction.

### Academic Context
Developed as part of the engineering curriculum at **Ruppin Academic Center**, this project demonstrates proficiency in low-level programming, manual memory allocation, and system logic design.
