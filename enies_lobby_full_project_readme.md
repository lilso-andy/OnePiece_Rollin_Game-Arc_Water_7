# 🏴‍☠️ Enies Lobby C++ Project

## 📜 Introduction

This repository contains the complete source code for the **Enies Lobby** coursework project, divided into two distinct parts. Developed entirely in **C++**, this project demonstrates a progression from foundational algorithmic problem-solving to advanced Object-Oriented Programming (OOP) and Data Structures.

---

## 📂 Part 1: [Replace with Part 1 Title, e.g., Basic Rescue Mission]

### Overview
*Briefly describe what Part 1 is about. E.g., "In Part 1, the objective was to implement fundamental algorithms to calculate the travel path to Enies Lobby, process strings of character names, and manage basic arrays."*

### Key Features
*   **Feature 1:** [e.g., Password decryption using string manipulation]
*   **Feature 2:** [e.g., Shortest path calculation using 2D arrays]
*   **Feature 3:** [e.g., Basic procedural combat logic]

### How to Run (Part 1)
Navigate to the `Part1` directory and compile the source code:
```bash
g++ -o part1 main.cpp
./part1
```

---

## ⚔️ Part 2: OOP Battle Simulator

### Overview
Part 2 elevates the project into a complex **Turn-Based Strategy** game, simulating the fierce battle between the Straw Hat Pirates and the CP9 organization at the Judicial Island. It heavily utilizes Object-Oriented Programming concepts (Inheritance, Polymorphism) and Linked Lists.

### Key Features

* **Lore-Accurate Character System:**
  * **Straw Hat Pirates:** Luffy, Zoro, Sanji, Nami, Usopp, Chopper, and Franky. Each possesses unique stats and Special Skills (e.g., Luffy's Gear 2 drains HP for massive damage; Nami reduces enemy speed).
  * **CP9 Organization:** Lucci, Kaku, Jabra, Blueno, Kalifa, Kumadori, and Fukurou. They utilize bonus damage mechanics based on their Doriki (Power Level).

* **Building System & Environmental Interaction:**
  * `MainGate` & `Courthouse`: Destroying these boosts Morale and Rescue Progress.
  * `TowerOfJustice` & `BridgeOfHesitation`: Opening these paves the way for the escape.
  * `BusterCallShip`: A deadly countdown. Failure to destroy it triggers the Buster Call (Game Over).

* **Turn-Based Engine:**
  * **Turn Order (Linked List):** Character action sequences are strictly managed and rotated using a Linked List structure.
  * **Battle Context:** Globally tracks `Morale`, `Alarm Level`, `Rescue Progress`, `Escape Progress`, and the `Buster Call Timer`.

### Win / Loss Conditions
*   **STRAW_HAT_WIN:** Achieve 100% Rescue Progress and 100% Escape Progress.
*   **STRAW_HAT_WIN_BY_DEFEAT_CP9:** Wipe out all CP9 agents.
*   **CP9_WIN:** All Straw Hat crew members are defeated.
*   **BUSTER_CALL:** The Buster Call timer reaches 0.
*   **TIME_OUT:** The maximum allowed turns are reached without a conclusive result.

### How to Run (Part 2)
Navigate to the `Part2` directory and compile the source code:
```bash
g++ -o battle_sim main.cpp eniesLobby.cpp
./battle_sim
```

---

## 💻 Tech Stack & Skills Demonstrated

*   **Language:** C++ (Standard Template Library)
*   **Core Concepts:** 
    *   Object-Oriented Programming (OOP): Abstraction, Encapsulation, Inheritance, Polymorphism.
    *   Dynamic Memory Management & Pointers.
    *   Data Structures: Custom Singly Linked Lists.
    *   Algorithmic Logic: Game loop implementation, conditional state tracking.

*Developed with ☕, sleepless nights, and the spirit of a true Pirate!* ⛵