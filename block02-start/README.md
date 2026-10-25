# Týden Mini 1 / seminář 2: vazby lokálních jmen

**Výchozí studentská verze.** Tento balíček patří k tříetapové části Mini v blokovém
kurzu Programovací jazyky. Autorem jazyka Mini a původní implementace je
Karel Šafr, PhD. Složka je samostatná: nepotřebuje jiné týdny ani původní
nadřazený projekt.

## Domácí práce k bloku 2

V hodině probíhá teoretický výklad; implementaci tohoto týdne Mini
zpracujete **samostatně doma**. Číslo `block02` označuje 2. blok
celého kurzu, tedy 1. týden části Mini. Nejde o stejně očíslovaný
balíček z původní osmitýdenní řady `TeachingWeeks`.

Začněte vlastní kopií tohoto balíčku. Hotové `solution` slouží až
k porovnání po vlastní práci; termín a způsob odevzdání určí vyučující.

## Co máte udělat

Úloha se týká souboru [`src/resolver.cpp`](src/resolver.cpp): implementujete **celou třídu `Resolver`**.
Přesný kontrakt, příklady a odevzdání určuje [ZADANI.md](ZADANI.md).
Referenční řešení má 96 neprázdných / 116 fyzických řádků; nejde o povinnou délku odevzdání.

V kódu je jedna ucelená chybějící část označená `TODO BLOCK02`.
Před jejím doplněním fungují příkazy `tokens`, `ast` a `lower`. Příkazy `check`, `cpp` a `run`
na platném neprázdném programu narazí na zamýšlenou výzvu k doplnění.
Úspěšné sestavení samo neznamená splnění úlohy.

## Sestavení a první spuštění

Pracujte uvnitř této složky vedle `CMakeLists.txt`. Potřebujete CMake
alespoň 3.20 a překladač C++17; Python není potřeba pro vlastní sestavení.
Následující cesty odpovídají ověřovanému macOS prostředí. U generátoru
s více konfiguracemi může být binárka například `build/Debug/minic.exe`.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
./build/minic tokens priklady/01_shadowing.mini
./build/minic check priklady/01_shadowing.mini
./build/minic run priklady/01_shadowing.mini
```

Před doplněním očekávejte u neimplementované fáze zprávu `TODO BLOCK02`.
Po doplnění a novém sestavení má poslední příkaz vypsat:

```text
20
30
```

Příkaz `run` překládá Mini do C++ a spouští vzniklý program. Příkaz
`cpp` pouze vypíše vygenerované C++. Při změně C++ implementace překladače
znovu spusťte `cmake --build build --config Debug`. Změněná runtime hlavička
se použije při příštím překladu programu pomocí `run`.

## Volitelná kontrola v češtině

S Pythonem 3.10 nebo novějším lze nejprve ověřit nedoplněný základ:

```sh
python3 overit.py --prekladac build/minic --vychozi
```

Po doplnění a sestavení spusťte režim pro hotové řešení:

```sh
python3 overit.py --prekladac build/minic
```

Úspěch režimu `--vychozi` potvrzuje právě očekávanou nedokončenost.
Odevzdané řešení musí projít kontrolou bez tohoto přepínače.

## Obsah balíčku a další práce

- `src/`: zdroje překladače v C++17.
- `runtime/`: běhová vrstva pro vygenerovaný program.
- [ZADANI.md](ZADANI.md): české zadání, očekávání a hranice úlohy.
- [priklady/](priklady/): malé vstupy Mini a jejich očekávané výsledky.
- `overit.py`: volitelná automatická kontrola s českým výpisem.
- `balicek.json`: určení týdne Mini, semináře a varianty pro ověřovač; neupravujte jej.

Každý týden začíná novým připraveným projektem. Předchozí soubory do něj
mechanicky nekopírujte; učitel mezi etapami doplňuje další infrastrukturu.
Původní řešení si uschovejte a porovnávejte principy i pozorované výsledky.
Číslo tohoto týdne Mini je **1**, číslo semináře celého kurzu **2**.

Výklad je česky. Technické identifikátory a příkazy zachovávají názvy
použité ve zdrojích; význam očekávaných diagnostik vysvětluje zadání.
