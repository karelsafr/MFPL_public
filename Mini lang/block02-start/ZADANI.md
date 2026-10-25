# Domácí úloha — týden Mini 1 / blok 2 — celý resolver lokálních jmen

Tato samostatná domácí úloha navazuje na teoretický výklad kapitol 4–6.
V hodině se probírají principy zpracování programu, jména, vazby a typy;
celý resolver implementujete doma. Píšete **C++ kód překladače Mini**,
nikoli nový program v jazyce Mini. Vstupní `.mini` programy ověřují výsledek.
Pracujte ve vlastní kopii `block02-start`; termín a způsob odevzdání určí
vyučující. `block02-solution` je reference k porovnání po vlastní práci.

Předpokládá se základní orientace v C++17: třídy a reference,
`std::variant` a rozlišování jeho alternativ přes `std::get_if`,
`std::vector` jako zásobník rozsahů a mapa jmen (zde `std::unordered_map`).
Je třeba umět rekurzivně projít AST, tedy strom příkazů a výrazů.
Význam jednotlivých uzlů si před psaním ověřte v místním `src/ast.hpp`.

## Jedna ucelená úloha

V [`src/resolver.cpp`](src/resolver.cpp)
nahraďte celou zástupnou třídu `Resolver` funkčním resolverem pro dodanou
verzi AST. Veřejné rozhraní zachovejte:

```cpp
Resolver(const SourceText &source, Program &program);
Resolution run();
```

Referenční třída má **96 neprázdných řádků / 116 fyzických řádků včetně
prázdných řádků**. Jde o odhad rozsahu, ne povinný počet řádků vašeho
řešení. Implementujete jeden objekt; nevytváříte osm oddělených drobných TODO.

## Co již dostáváte

- Načítání souborů, lexer, parser, AST, identitní lowering, generování C++,
  runtime, příkazové rozhraní a CMake jsou hotové.
- `Binding` se složkami `id` a `defined` je definovaný nad třídou.
- `SymbolId`, `Resolution`, `Program`, uzly AST, `SourceText`, `SourceSpan`
  a `CompileError` jsou dodané typy. Veřejná funkce `resolve` pod třídou
  už vytváří resolver a volá `run`.
- `Program` této verze obsahuje `main_body`. Deklarace i použití proměnné
  mají pole `resolved_id`, které má doplnit váš objekt.

Upravujte pouze třídu `Resolver` v uvedeném souboru. Neměňte parser,
runtime, AST ani generátor a neobcházejte chyby vracením libovolného ID.
Uvnitř třídy si zvolte pomocné metody. Rozumné členění je průchod seznamem
příkazů, průchod příkazem a výrazem, otevření/uzavření scope, deklarace,
dokončení definice a vyhledání jména.

## Kontrakt resolveru

1. `run` založí scope těla `main`, projde příkazy v pořadí zdroje,
   scope uzavře a vrátí `Resolution{}`.
2. Každá deklarace dostane v rámci celého programu vlastní `SymbolId`.
   Čítač ID při uzavření bloku nevracejte zpět a již přidělená ID
   nerecyklujte ani v následujícím sousedním bloku. Stejné ID patří všem
   následným čtením a přiřazením, která se vážou k této deklaraci.
3. Blok otevírá nový scope. Po jeho skončení už jeho jména nejsou viditelná.
   Vyhledávání začíná v nejvnitřnějším scope a pokračuje směrem ven.
4. Druhá deklarace stejného jména ve stejném scope je chyba. Zastínění
   jména z vnějšího scope je povolené.
5. Deklaraci nejprve zaveďte jako dosud nedefinovanou, potom zkontrolujte
   inicializátor a teprve poté ji označte jako definovanou. Pokud hledání
   narazí na takovou nedefinovanou vazbu, skončí chybou; nesmí ji přeskočit
   a použít stejně pojmenovanou vnější proměnnou.
6. U přiřazení nejprve vyhledejte cílové jméno a zapište jeho `resolved_id`,
   potom zkontrolujte pravou stranu. Přiřazení
   nezakládá novou proměnnou. Čtení i přiřazení neznámého jména je chyba.
7. Projděte `PrintStatement`, `VarStatement`, `AssignmentStatement`,
   `ExpressionStatement` a `BlockStatement`. Ve výrazech řešte jména
   a vnořené závorky; literály samy žádnou vazbu nevyžadují.
8. Chyby hlaste jako `CompileError` s `Phase::Resolve`, názvem souboru
   a příslušným úsekem zdroje (`span`). Neočekávaný uzel příkazu odmítněte
   zprávou `unsupported statement node`. Níže uvedená diagnostická jádra jsou součástí
   kontraktu cvičení; prefix se souborem a pozicí doplní nástroj.

Tabulka scope zde obsahuje **vazby jmen při překladu**, nikoli aktuální
hodnoty za běhu. Změna hodnoty `x` z čísla na bool proto nezakládá novou vazbu.

## Sestavení a první pozorování

Příkazy spouštějte uvnitř svého rozbaleného balíčku, vedle `CMakeLists.txt`.
Potřebujete CMake alespoň 3.20
a překladač C++17; konkrétní příkazy níže používají rozložení binárky
v ověřovaném macOS prostředí. U vícekonfiguračního generátoru může být
binárka například `build/Debug/minic.exe`.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
./build/minic tokens priklady/01_shadowing.mini
./build/minic ast priklady/01_shadowing.mini
./build/minic lower priklady/01_shadowing.mini
./build/minic check priklady/01_shadowing.mini
```

Před doplněním třídy jsou `tokens`, `ast` a `lower` funkční. Příkaz `check`
končí záměrnou chybou obsahující `TODO BLOCK02: implement the complete Resolver`.
Totéž chybějící místo brání `cpp` a `run`; úspěšné sestavení překladače
samo neznamená dokončený resolver.

Po změně C++ zdroje překladač znovu sestavte:

```sh
cmake --build build --config Debug
./build/minic check priklady/01_shadowing.mini
./build/minic cpp priklady/01_shadowing.mini
./build/minic run priklady/01_shadowing.mini
```

Úspěšný `check` je tichý. `run` první ukázky vypíše každý výsledek na
samostatný řádek:

```text
20
30
```

## Příklady a očekávané výsledky

V tabulce je název souboru ve složce [`priklady/`](priklady/).
Úspěšné případy spouštějte pomocí `run`; chybné pomocí `check`.

| Soubor | Očekávaný výsledek | Co vysvětlit |
|---|---|---|
| `01_shadowing.mini` | `20`, potom `30` | Vnitřní deklarace zastíní vnější; jiný blok může přiřadit do vnější vazby. |
| `02_copy.mini` | `10`, potom `30` | Hodnota se kopíruje; změna `x` nemění hodnotu dříve uloženou do `y`. |
| `03_literals.mini` | `nil`, `true`, `Mini` | Stejná vazba může nést hodnoty různých druhů; výraz v závorkách se také kontroluje. |
| `04_duplicate.mini` | `duplicate variable 'x' in the same scope` | Duplicita ve stejném scope je zakázaná. |
| `05_self_initializer.mini` | `cannot read variable 'x' in its own initializer` | Vnitřní nedefinovaná vazba se nesmí přeskočit. |
| `06_unknown_read.mini` | `unknown variable 'missing'` | Čtení musí mít deklaraci. |
| `07_scope_escape.mini` | `unknown variable 'local'` | Jméno z ukončeného bloku není dostupné. |
| `08_before_declaration.mini` | `unknown variable 'later'` | Pozdější deklarace nepůsobí zpětně. |
| `09_unknown_assignment.mini` | `unknown variable 'missing'` | Přiřazení samo nedeklaruje proměnnou. |
| `10_sibling_scopes.mini` | `1`, potom `2` | Dva sousední bloky mají odlišné scope. |

Například:

```sh
./build/minic check priklady/05_self_initializer.mini
```

Poslední příkaz musí skončit nenulovým návratovým kódem a diagnostikou
ve fázi `resolve`. Pouhé vytištění zprávy a pokračování není správné řešení.

## Dokončení a návaznost

Odevzdejte upravený `src/resolver.cpp`, vlastní kladný a záporný příklad
a krátké vysvětlení, proč se v `05_self_initializer.mini` nepoužije vnější `x`.
U kladného příkladu předem napište očekávaný výstup; u záporného pravidlo,
které musí odmítnout. Očekávání musí souhlasit se skutečným během.

Tato výuková verze má pouze `fun main()`, literály, závorky, proměnné,
přiřazení, výpis a bloky. **Nemá ještě operátory, podmínky, smyčky ani
obecné funkce a volání.** Jde o omezení etapy, nikoli finálního jazyka Mini.

Pro následující domácí úlohu dostanete nový `block03-start` s dodanou infrastrukturou
a již rozšířeným resolverem. Svůj soubor do něj automaticky nekopírujte:
například struktura `Program` se mezitím rozšíří na seznam funkcí.
Svou práci si uchovejte odděleně a porovnejte, které principy
vazeb zůstaly stejné.

## Kontrola před odevzdáním domácí úlohy

- Odevzdávaná změna je omezena na předepsanou třídu, metodu nebo funkci.
  Okolní připravená infrastruktura a `balicek.json` zůstávají zachované.
- Projekt jde sestavit a dodané příklady mají přesně uvedené výsledky,
  včetně očekávaných chyb. Výchozí TODO už při jejich spuštění nenastává.
- Přiložte vlastní příklad či příklady a vysvětlení požadované v oddílu
  o odevzdání. Zapište příkazy, očekávané i skutečné výsledky; samotný
  screenshot úspěšného sestavení neprokazuje správnost řešení.
- Odevzdávejte upravený zdrojový soubor, vlastní `.mini` příklady a stručný
  text výsledků. Build adresář ani spustitelné soubory nejsou potřeba.

## Jazyk dokumentace a technických výstupů

Zadání, vysvětlení a pokyny jsou česky. Názvy funkcí, druhů uzlů,
příkazy `minic` a klíčová slova Mini jsou součástí rozhraní a nepřekládají se.
Přesné původní diagnostiky jsou uvedené spolu s českým vysvětlením,
aby šlo očekávanou chybu porovnat se skutečným výstupem.

## Volitelná automatická kontrola

Přiložený český ověřovač vyžaduje Python 3.10 nebo novější. Pro sestavení
překladače ani ruční příkazy výše není Python potřebný. Před doplněním
studentské části lze ověřit, že výchozí projekt hlásí očekávané TODO:

```sh
python3 overit.py --prekladac build/minic --vychozi
```

Po doplnění a novém sestavení spusťte kontrolu řešení:

```sh
python3 overit.py --prekladac build/minic
```

Režim `--vychozi` je kontrola nedoplněné studentské verze; úspěch v tomto
režimu není úspěchem řešení. V balíčku s hotovým řešením jej nepoužívejte.
Kontrola řešení spouští příklady z `priklady/` a porovnává jejich výstupy
s `priklady/cases.json`. Záporný příklad uspěje právě tehdy, když program
skončí očekávanou chybou. Vlastní testy a vysvětlení nenahrazuje.
