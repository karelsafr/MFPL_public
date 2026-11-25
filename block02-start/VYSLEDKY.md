# Blok 02 – Resolver: vlastní příklady a výsledky

Autor: Denisa Linh Phí

## Sestavení a ověření

```sh
cmake -S . -B build
cmake --build build
python3 overit.py --prekladac build/minic
```

Ověřovač vypsal `Všechny kontroly prošly.` – všech 10 dodaných příkladů OK.

## Vlastní příklady

Napsala jsem šest vlastních příkladů do složky `vlastni_priklady/`. Chtěla jsem
vyzkoušet věci, které dodaná sada neřeší – hlubší zanoření, víc závorek za sebou,
prázdný blok a duplicitu uvnitř bloku.

### v01_trojite_zanoreni.mini

Tři úrovně zastínění stejného jména. Chtěla jsem vidět, jestli se hledání opravdu
vrací ven po jedné vrstvě.

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

`./build/minic run vlastni_priklady/v01_trojite_zanoreni.mini`

Čekala jsem `3`, `2`, `1`. Vyšlo `3`, `2`, `1`.

Pak jsem se ještě podívala přes `./build/minic cpp`, jaká dostaly ID. Ve
vygenerovaném C++ jsou tři různé proměnné `mini_variable_0`, `mini_variable_1`
a `mini_variable_2` a každý `print` čte jinou z nich. Takže každá deklarace
dostala svoje vlastní ID a nic se nepřepsalo.

### v02_hluboke_zavorky.mini

Trojité závorky kolem jména – chtěla jsem ověřit, že se resolver zanoří až
dovnitř.

```mini
fun main() {
  var x = 7;
  print (((x)));
}
```

`./build/minic run vlastni_priklady/v02_hluboke_zavorky.mini`

Čekala jsem `7`. Vyšlo `7`.

### v03_prazdny_blok.mini

Blok, ve kterém nic není. Zajímalo mě, jestli to nespadne, když se otevře
a zavře prázdný scope.

```mini
fun main() {
  var x = 5;
  {
  }
  print x;
}
```

`./build/minic run vlastni_priklady/v03_prazdny_blok.mini`

Čekala jsem `5`. Vyšlo `5`.

### v04_prirazeni_do_vnejsi.mini

Přiřazení do proměnné z vnějšího bloku a potom její přečtení, až blok skončí.

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

`./build/minic run vlastni_priklady/v04_prirazeni_do_vnejsi.mini`

Čekala jsem `2`, `2`. Vyšlo `2`, `2`.

Druhý výpis je důležitý – kdyby přiřazení uvnitř bloku vyrobilo novou proměnnou
místo toho, aby změnilo tu vnější, vypsalo by se `1`.

### v05_duplicita_ve_vnorenem.mini

Dvakrát stejné jméno, ale ve vnořeném bloku. Dodaný příklad `04_duplicate.mini`
to zkouší jen přímo v těle `main`.

```mini
fun main() {
  var x = 1;
  {
    var y = 2;
    var y = 3;
  }
}
```

`./build/minic check vlastni_priklady/v05_duplicita_ve_vnorenem.mini`

Čekala jsem chybu o duplicitě. Vypsalo se:

```
vlastni_priklady/v05_duplicita_ve_vnorenem.mini:5:9: resolve: duplicate variable 'y' in the same scope
```

### v06_vlastni_inicializator_bez_vnejsi.mini

`var pocet = pocet;` v situaci, kdy žádné jiné `pocet` neexistuje. Dodaný
příklad `05_self_initializer.mini` má venku ještě jedno `x`, takže jsem chtěla
vidět i variantu bez něj.

```mini
fun main() {
  var pocet = pocet;
}
```

`./build/minic check vlastni_priklady/v06_vlastni_inicializator_bez_vnejsi.mini`

Čekala jsem chybu o vlastním inicializátoru. Vypsalo se:

```
vlastni_priklady/v06_vlastni_inicializator_bez_vnejsi.mini:2:15: resolve: cannot read variable 'pocet' in its own initializer
```

Tenhle příklad mi pomohl pochopit, proč na pořadí v `declare` záleží. Jméno se
musí zapsat do mapy ještě před zpracováním inicializátoru – kdyby se zapsalo až
potom, nenašlo by se a hlásilo by se `unknown variable 'pocet'`. Jsou to dvě
různé hlášky, takže to jde poznat.

## Shrnutí

Dodané příklady: 10 z 10 OK podle `overit.py`.
Vlastní příklady: 6 z 6 vyšlo tak, jak jsem čekala.

Testy ale pokrývají jen to, co jsem vymyslela, a jen tuhle verzi AST – nejsou
tam funkce s parametry, cykly ani logické operátory, protože je zadaný jazyk
zatím nemá.
