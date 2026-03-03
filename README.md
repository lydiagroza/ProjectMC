# 7 Wonders Duel - Proiect C++

Acest proiect reprezintă implementarea jocului de societate **7 Wonders Duel**. Aplicația include atât logica de bază a jocului (Game Engine), cât și o interfață grafică (GUI) realizată în Qt, alături de un oponent AI.

## Echipa

**Studenți:**
* lydiagroza
* Alexandra
* ale0050
* laurinho

**Mentori Coordonatori:**
* Andrei Rozmarin (@offuru)
* Vlad Vrabie (@vladvrabie)
* Iulian Popa (@iulian-unitbv)

---

## Detalii Tehnice

### Tehnologii Utilizate
* **Limbaj:** C++ (Standard C++17 / C++20)
* **Framework GUI:** Qt 6
* **IDE:** Visual Studio 2022 / Qt Creator
* **Arhitectură:** Engine-ul este separat într-un DLL (`GameBase`), interfațat cu UI-ul (`gameUI`) și consola (`ConsoleGame`).

### Elemente de Modern C++ Implementate
Proiectul respectă cerințele de utilizare a funcționalităților moderne ale limbajului:

* **Smart Pointers:** Utilizare extensivă a `std::shared_ptr` și `std::unique_ptr` pentru managementul memoriei (ex: gestionarea cărților și a efectelor).
* **Lambda Expressions:** Folosite intens în `UniversalCardLoader` pentru definirea efectelor cărților.
* **Move Semantics:** Optimizarea transferului de obiecte (ex: `CardsPyramid`, `Wonder`).
* **Templates:** Utilizate pentru funcționalități generice în clasele de bază.
* **DLL Export:** Logica jocului este încapsulată într-o bibliotecă dinamică (`__declspec(dllexport)`).
* **Const Correctness & References:** Utilizare strictă a `const ref` pentru parametri și metode.

---

## Instrucțiuni de Compilare și Rulare

### Cerințe Preliminare
* Visual Studio 2022 (cu extensia Qt Tools) SAU Qt Creator.
* Qt 6.10 (sau o versiune compatibilă) instalat.

### Pași pentru Build

1. **Clonare Repository:**
   ```bash
   git clone https://github.com/lydiagroza/ProjectMC.git
   ```

2. **Configurare:**
   * Deschideți fișierul `7Wonders.sln` în Visual Studio.
   * Asigurați-vă că aveți configurat kit-ul de Qt corespunzător în setările proiectului (Qt VS Tools).

3. **Compilare:**
   * Selectați configurația `Debug` sau `Release` și platforma `x64`.
   * Build Solution (Ctrl+Shift+B).

4. **Rulare:**
   * Setați `gameUI` ca "Startup Project" pentru a lansa interfața grafică.
   * Setați `ConsoleGame` ca "Startup Project" pentru versiunea in terminal.

---

## 🎥 Video Demo

Prezentarea funcționalității proiectului și a elementelor de cod poate fi vizionată în fișierul video atașat în repository

---

## 📂 Structura Proiectului

* `7Wonders/GameBase` - Biblioteca statică/DLL cu logica jocului.
* `7Wonders/gameUI` - Interfața grafică bazată pe Qt.
* `7Wonders/gameAI` - Implementarea logicii pentru oponentul AI.
* `7Wonders/ConsoleGame` - Punct de intrare pentru testarea în consolă.
* `7Wonders/UI` - Resurse grafice (imagini, sunete, fișiere .ui).

---

*Proiect realizat pentru sesiunea Ianuarie 2026.*
