# Blok 02 – Resolver: vlastní příklady a výsledky

Autor: Denisa Linh Phí
Upravený soubor: `src/resolver.cpp` (pouze třída `Resolver`, infrastruktura beze změny)

## Sestavení

```sh
cmake -S . -B build
cmake --build build
```

Ověření dodaných příkladů:

```sh
python3 overit.py --prekladac build/minic
```

Výstup: `Všechny kontroly prošly.` (10× OK)

## Vlastní příklady

Příklady jsou v adresáři `vlastni_priklady/`. Testují případy, které dodaná
sada nepokrývá: hlubší zanoření, rekurzi v závorkách, prázdný blok, přiřazení
do vnější vazby s následným čtením, duplicitu uvnitř vnořeného bloku
a vlastní inicializátor bez existující vnější proměnné.

### v01_trojite_zanoreni.mini

Tři úrovně zastínění téhož jména. Ověřuje, že vyhledávání jde od nejvnitřnějšího
scope ven a že se po uzavření bloku obnoví vnější vazba.

```mini
fun main() {
  var x = 1;
  {
    var x = 2;
    {
      var x = 3;
      print x;
    }
    print x;
  }
  print x;
}
```

Příkaz: `./build/minic run vlastni_priklady/v01_trojite_zanoreni.mini`

| Očekáváno | Skutečnost |
|---|---|
| `3`, `2`, `1` | `3`, `2`, `1` ✔ |

Kontrola přidělených ID přes `./build/minic cpp`:

```cpp
mini::Value mini_variable_0 = ...   // vnější x = 1
  mini::Value mini_variable_1 = ... // prostřední x = 2
    mini::Value mini_variable_2 = ... // vnitřní x = 3
    ... = mini_variable_2;          // print uvnitř čte vnitřní x
  ... = mini_variable_1;            // print uprostřed čte prostřední x
... = mini_variable_0;              // print vně čte vnější x
```

Každé `print x` je navázané na jinou deklaraci – shadowing funguje správně
a každá deklarace má vlastní `SymbolId`.

### v02_hluboke_zavorky.mini

Trojité závorky kolem jména. Ověřuje rekurzi v `resolveExpression` přes
`GroupingExpression`.

```mini
fun main() {
  var x = 7;
  print (((x)));
}
```

Příkaz: `./build/minic run vlastni_priklady/v02_hluboke_zavorky.mini`

| Očekáváno | Skutečnost |
|---|---|
| `7` | `7` ✔ |

### v03_prazdny_blok.mini

Blok bez jediného příkazu. Ověřuje, že `pushScope`/`popScope` fungují i pro
prázdný seznam příkazů a že vnější vazba zůstane dostupná.

```mini
fun main() {
  var x = 5;
  {
  }
  print x;
}
```

Příkaz: `./build/minic run vlastni_priklady/v03_prazdny_blok.mini`

| Očekáváno | Skutečnost |
|---|---|
| `5` | `5` ✔ |

### v04_prirazeni_do_vnejsi.mini

Přiřazení do vnější proměnné z vnořeného bloku a čtení po uzavření bloku.
Ověřuje, že přiřazení nezakládá novou vazbu a že změna přetrvá i po `popScope`.

```mini
fun main() {
  var x = 1;
  {
    x = 2;
    print x;
  }
  print x;
}
```

Příkaz: `./build/minic run vlastni_priklady/v04_prirazeni_do_vnejsi.mini`

| Očekáváno | Skutečnost |
|---|---|
| `2`, `2` | `2`, `2` ✔ |

Kdyby přiřazení zakládalo novou proměnnou v bloku, druhý výpis by dal `1`.

### v05_duplicita_ve_vnorenem.mini

Duplicitní deklarace uvnitř vnořeného bloku (dodaný `04_duplicate.mini` testuje
jen tělo `main`). Ověřuje, že kontrola duplicity se dívá do aktuálního scope,
ne jen do toho vnějšího.

```mini
fun main() {
  var x = 1;
  {
    var y = 2;
    var y = 3;
  }
}
```

Příkaz: `./build/minic check vlastni_priklady/v05_duplicita_ve_vnorenem.mini`

| Očekáváno | Skutečnost |
|---|---|
| chyba `duplicate variable 'y' in the same scope` | `...v05_duplicita_ve_vnorenem.mini:5:9: resolve: duplicate variable 'y' in the same scope` ✔ |

### v06_vlastni_inicializator_bez_vnejsi.mini

Vlastní inicializátor v situaci, kdy žádná stejnojmenná vnější proměnná
neexistuje (dodaný `05_self_initializer.mini` jednu má). Ověřuje, že chyba
nevzniká kvůli zastínění, ale kvůli příznaku `defined = false`.

```mini
fun main() {
  var pocet = pocet;
}
```

Příkaz: `./build/minic check vlastni_priklady/v06_vlastni_inicializator_bez_vnejsi.mini`

| Očekáváno | Skutečnost |
|---|---|
| chyba `cannot read variable 'pocet' in its own initializer` | `...v06_vlastni_inicializator_bez_vnejsi.mini:2:15: resolve: cannot read variable 'pocet' in its own initializer` ✔ |

Kdyby `declare` zapsalo jméno až po zpracování inicializátoru, hlásilo by se
`unknown variable 'pocet'` – tedy jiná chyba. Pořadí declare → inicializátor →
define je tedy skutečně rozlišitelné testem.

## Shrnutí

| Sada | Počet | Výsledek |
|---|---|---|
| Dodané příklady (`priklady/`) | 10 | 10× OK (`overit.py`) |
| Vlastní příklady (`vlastni_priklady/`) | 6 | 6× odpovídá očekávání |

## Meze testů

Testy ověřují jen zvolenou rodinu vstupů a tuto implementaci. Neověřují
chování pro konstrukce, které dodaná verze AST nemá (funkce s parametry,
cykly, logické operátory), ani výkon na rozsáhlých vstupech.
