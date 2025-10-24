
# 7WondersDuel / The Game — Project Repository

> **Proiect**: Implementare joc — *7 Wonders Duel* (Informatică Aplicată) **sau** *The Game* (Informatică Teoretică)

---

## Scop

Acest repository conține specificațiile, structura și fișiere utile pentru implementarea proiectului de licență/semestru cerut de curs. Conținutul este organizat astfel încât echipa să poată începe rapid dezvoltarea, să respecte regulile de commit și baremul cerut, și să pregătească materialele pentru predare și prezentare.

> Alegeți fie folderul `7WondersDuel/` (pentru Informatică Aplicată) fie `TheGame/` (pentru Informatică Teoretică), conform specializării.

---

## Mentori (adăugați ca membri pe GitHub)

* Andrei Rozmarin — `offuru`
* Vlad Vrabie — `vladvrabie`
* Iulian Popa — `iulian-unitbv`

---

## Timeline proiect

* **Perioada proiect**: 27 octombrie → 18 ianuarie
* **Durata totală**: 10 săptămâni
* Perioade speciale de commit: 15 decembrie - 30 decembrie și 1 ianuarie - 11 ianuarie (fiecare numără o săptămână de commituri)

### Deadline importante

* Video demo final (max 5–7 minute): **18 ianuarie, 23:59** — videoul trebuie comis în repo
* Prezentare intermediară: între **24 noiembrie** și **19 decembrie**

---

## Reguli de commit (script: *Commit Counter*)

* Fiecare commit trebuie să aibă *sens* (cod funcțional, fișiere de documentație/asset, teste)
* După fiecare commit, trebuie să existe o variantă compilabilă a aplicației

### Cerințe pe săptămână (echipă)

* Echipe de 4: minim **12 commits/săptămână/echipă**
* Echipe de 5: minim **15 commits/săptămână/echipă**
* Echipe < 4: minim **9 commits/săptămână/echipă**
* Dublu commit ca penalizare dacă lipsă o săptămână (o singură excepție acceptată)

### Cerințe per persoană

* Minim **30 commits sens/persoană** până la prezentare
* Dacă proiectul are un singur membru: **3 commits/săptămână** individual și **30** până la prezentare

> **Notă**: Doar commit-urile validate de scriptul `Commit Counter` sunt luate în calcul.

---

## Barem minimal și avansat

* Respectarea baremului minimal oferă maxim **nota 8**
* Partea avansată (basic + advanced) adaugă puncte suplimentare (valabilitate 2p pentru advanced + 1p element custom)

### Elemente Modern C++ (va fi evaluat la prezentare)

Must justify presence/absence of:

* const reference
* move semantics
* data structures moderne + ranges
* modern algorithms (Algorithms library)
* smart pointers
* templates / variadic templates
* regex
* DLL (implementare proprie)
* lambdas
* modules (basic) — bonus
* alte minim 2 elemente C++ >= C++20

> Totalul punctelor pentru MC++: **4 puncte**

---

## Cerințe de bază (rezumat tehnic)

### 7 Wonders Duel (Informatică Aplicată)

* Implementare completă în consolă (toate cărțile și efectele, jetoane de progres științific, aranjament card tree)
* Start nou / Continue dintr-un save
* Descriere textuală scurtă pentru fiecare carte (cost, efect, tip)

**Opțional/Avansat (2p fiecare)**

* GUI (afişare arbore cărți, board, tooltip card)
* AI / sistem recomandare
* Mod rețea (play online)

### The Game (Informatică Teoretică)

* Arhitectură client-server (HTTP + Crow) cu minim 2 clienți + 1 server
* Login/Register (username unic) — email/parolă opțional
* DB: SQLite folosind SQLite ORM
* Consolă pentru joc; opțional GUI

**Opțional/Avansat (2p total)**

* Multigaming (matching + timeout 30s)
* GUI personalizată

---

## Structură recomandată a repository-ului

```
/ (root)
├─ .gitignore
├─ README.md (acesta)
├─ CONTRIBUTING.md
├─ LICENSE
├─ 7WondersDuel/              # dacă alegeți varianta aplicată
│  ├─ docs/
│  ├─ src/
│  │  ├─ main.cpp
│  │  ├─ game_engine/
│  │  ├─ models/
│  │  └─ ai/
│  ├─ assets/
│  └─ tests/
├─ TheGame/                  # dacă alegeți varianta teoretică
│  ├─ server/
│  ├─ client/
│  ├─ db/
│  └─ tests/
└─ scripts/
   ├─ commit_counter_check.sh
   └─ build_all.sh
```

---

## Fișiere utile (template)

* **.gitignore** (C++, Visual Studio, build folders, binaries)
* **CONTRIBUTING.md** — convenții de commit, stil de cod (Clean Code Tips), code review flow
* **ISSUE_TEMPLATE.md** — pentru bug / feature request
* **PULL_REQUEST_TEMPLATE.md** — ce trebuie inclus într-un PR
* **scripts/commit_counter_check.sh** — wrapper local pentru a valida commit-urile înainte de push (opțional)

---

## Exemplu rapid .gitignore (C++)

```
# Build
/build/
/bin/
/obj/
*.exe
*.dll
*.lib
*.pdb

# VS
.vscode/
.vs/

# CMake
CMakeCache.txt
CMakeFiles/
cmake_install.cmake
Makefile

# OS
.DS_Store
Thumbs.db

# IDE
.idea/

# user-specific
*.user
```

---

## Build & Run (exemplu generic C++)

1. Clone repo

```bash
git clone <repo-url>
cd <project-folder>
```

2. Build (CMake)

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

3. Run

```bash
./bin/game  # sau executabilul generat
```

---

## Cerinţe pentru prezentare finală

1. Demonstrarea componentelor din barem (MC++ + cerinţe de bază)
2. Prezentarea elementelor avansate (dacă există)
3. Video demo (max 5–7 minute) comis în repo înainte de deadline
4. Proiectul trebuie clonabil și compilabil pe o mașină curată

---

## Sugestii practice (Clean Code & workflow)

* Folosiți branch-uri feature/bugfix; merge prin Pull Request cu cel puțin 1 review
* Folosiți mesaje de commit clare: `feat(engine): add turn processing loop` / `fix(ui): correct resource display`
* Adăugați teste unitare și rulate în CI
* Documentați deciziile tehnice în `docs/ARCHITECTURE.md`

---

## Contribuție și responsabilități

* Stabiliți un `leader` responsabil de integrare și versiunea de release
* Distribuiți task-urile clar (engine, UI, AI, DB, network, tests)
* Respectați cerințele de commit și păstrați repo-ul curat

---
