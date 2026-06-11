# Tutorato Sistemi Operativi

Materiale per il tutorato di Sistemi Operativi e per gli esempi delle lezioni di laboratorio.

La struttura corrente separa il materiale del tutorato 2025 dagli esempi delle lezioni attuali:

- `Tutorato2025/`: esercizi, prove d'esame, soluzioni e slide del tutorato A.A. 2024-2025.
- `Esempi-Lezioni-Lab2026/`: esempi organizzati per data delle lezioni di laboratorio 2026. Sono presenti esempi presi direttamente dagli esempi forniti dal professore Mario Di Raimondo, per una preparazione adeguata della materia si dovrebbe fare sempre affidamento agli esempi forniti dal docente e non questi qui che sono secondari

## Prerequisiti

- Git >= 2.20
- GCC o Clang per compilare gli esempi in C
- Ambiente Linux, macOS o WSL2. Per gli esercizi che usano API POSIX, Linux/WSL2 e' l'ambiente consigliato.

## Uso rapido

Clona la repo:

```bash
git clone https://github.com/timeassassinRG/Tutorato-Sistemi-Operativi-2025.git
cd Tutorato-Sistemi-Operativi-2025
```

Per lavorare sul materiale del tutorato 2025:

```bash
cd "Tutorato2025/Esercizio 1"
gcc main.c -o main
./main
```

Per lavorare su una prova d'esame:

```bash
cd "Tutorato2025/Esame-[YYYY-MM-DD]"
gcc main.c -o main
./main
```

Se nella cartella e' presente `run.sh`, puoi usare direttamente:

```bash
chmod +x run.sh
./run.sh
```

Per aggiungere o consultare gli esempi delle lezioni correnti:

```bash
cd "Esempi-Lezioni-Lab2026/Esempi-11-06"
```

## Convenzioni

- Il materiale storico del tutorato 2025 resta sotto `Tutorato2025/`.
- I nuovi esempi di laboratorio vanno sotto `Esempi-Lezioni-Lab2026/`, preferibilmente in una sottocartella per data.
- Ogni esercizio dovrebbe contenere il testo in PDF, i file di input necessari e la soluzione in C.
- Le soluzioni alternative possono essere aggiunte in una cartella `Soluzione_alt_[COGNOME o GitHub username]/`, includendo un file `Changes.md` o `Changes.txt` che spieghi le differenze rispetto alla soluzione proposta.

## Come contribuire

1. Fai un fork del progetto.
2. Crea un nuovo branch:

   ```bash
   git checkout -b feature/nome-feature
   ```

3. Aggiungi il materiale o correggi un esercizio.
4. Apri una Pull Request descrivendo le modifiche.
