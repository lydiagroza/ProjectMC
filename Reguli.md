# 7 Wonders Duel — Reguli oficiale și cerințe de proiect

## 🎯 Descriere generală

**7 Wonders Duel** este un joc de strategie pentru 2 jucători, în care fiecare participant își dezvoltă civilizația de-a lungul a trei ere, încercând să acumuleze mai multe puncte de victorie decât adversarul.  
Pe parcursul jocului, jucătorii adună **resurse** pentru a construi **clădiri** și **minuni** care oferă avantaje economice, militare sau științifice.

---

## 🏗️ Componentele jocului

- 3 pachete de cărți (câte unul pentru fiecare eră)
- 7 minuni
- Jetoane științifice (progres)
- Tablele jucătorilor (orașe)
- Tabla militară (pionul conflictului)
- Monede

---

## 🔄 Desfășurarea jocului

### 1️⃣ Pregătirea
- Fiecare jucător primește **7 bănuți**.
- Se aleg **4 minuni per jucător** conform procedurii (pagina 7 din regulile oficiale).
- Jocul se desfășoară în **3 ere**.

### 2️⃣ Runda (Era)
- Fiecare eră folosește un **set specific de 20 de cărți**.
- Cărțile se aranjează conform **structurii de pe pagina 20** din regulile jocului.
- Ordinea vizibilității cărților respectă “arborele” (unele sunt ascunse, altele vizibile).

### 3️⃣ Tura unui jucător
La tura sa, un jucător alege o carte disponibilă și alege UNA dintre acțiunile posibile:

#### 🏛️ a) Construiește cartea
- Plătește costul în resurse (sau cumpără resurse de la adversar).
- Adaugă cartea în orașul propriu.
- Aplică efectul (resurse, puncte, simboluri științifice etc.).

#### 💰 b) Decartează cartea
- O elimină și primește **monede** (pagina 10 din regulile oficiale).

#### 🏗️ c) Construiește o minune
- Folosește cartea pentru a construi una dintre minuni.
- Aplică efectul special al minunii (pagina 11 și 17).

> ⚠️ Doar **7 minuni pot fi construite** în total (nu per jucător).

---

## ⚔️ Condiții de victorie

Există **3 moduri** de a câștiga jocul:

### 1. Victorie militară
- Diferența de **9 scuturi** între jucători.
- Jucătorul cu cele mai multe scuturi câștigă imediat.

### 2. Victorie științifică
- Un jucător obține **6 simboluri științifice diferite**.

### 3. Victorie civilă (puncte)
- Dacă nu există victorie militară sau științifică, la finalul erei 3:
  - Se numără punctele câștigătoare (din clădiri, minuni, jetoane, bani).
  - Câștigă jucătorul cu **cele mai multe puncte**.

---

## ⚙️ Cerințe de implementare (Informatică Aplicată)

### Cerințe de bază
- Implementarea completă în consolă:
  - Toate cărțile și efectele (pag. 14–17).
  - Jetoanele de progres științific.
  - Aranjamentul cărților (arborele erei).
- Posibilitatea de:
  - Începere joc nou.
  - Continuare joc dintr-un **save** anterior.
- Afișarea descrierilor concise:
  - Cost, efect, tip (ex. „12G, Repeat turn”, „1WP+1G per Blue per Player”).
- Afișarea tipului de carte prin **culori** (Maro, Albastru, Verde etc.).

### Componente avansate (2p fiecare)
- **GUI:** reprezentare grafică a arborelui, jetoanelor, resurselor, minuni etc.
- **AI:** recomandări sau mod Player vs Computer.
- **Mod rețea:** joc între două instanțe diferite conectate prin IP.

### Componente opționale (bonus)
- Rezumat vizual al resurselor active.
- Tooltip detaliat la hover peste carte.
- Implementarea unei expansiuni („Pantheon” sau „Agora”).
- Mod „turneu” (AI vs AI).

---

## 💾 Salvare / Încărcare

Jocul trebuie să permită:
- Salvarea stării curente (jucători, cărți disponibile, resurse, minuni construite).
- Încărcarea unei stări salvate pentru a continua ulterior.

---

## 🧠 Elemente Modern C++ cerute

- `const` & `reference`
- `smart pointers`
- `move semantics`
- STL moderne: `ranges`, `algorithms`
- `lambda` expressions
- `regex`
- `templates`
- `unique_ptr` / `shared_ptr`
- (opțional) `modules`, `concepts`, `std::variant`, `std::optional`

---

## 🧾 Surse utile

- [Regulile complete 7 Wonders Duel (Repos Production)](https://www.rprod.com/en/games/7-wonders-duel)
- [Lista completă de cărți și efecte (pag. 14–17)](https://boardgamegeek.com/filepage/104227/7-wonders-duel-rulebook)
