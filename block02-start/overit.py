#!/usr/bin/env python3
"""Ověřovač příkladů samostatného výukového balíčku Mini (Python 3.10+)."""
from pathlib import Path
import argparse
import json
import os
import signal
import subprocess


def spustit(prikaz, limit):
    prostredi = dict(os.environ)
    prostredi.pop('MINI_CXX', None)
    prostredi.pop('CXX', None)
    proces = subprocess.Popen(prikaz, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                              text=True, encoding='utf-8', errors='replace',
                              env=prostredi, start_new_session=os.name != 'nt')
    try:
        vystup, chyba = proces.communicate(timeout=limit)
    except subprocess.TimeoutExpired:
        if os.name == 'nt':
            proces.kill()
        else:
            os.killpg(proces.pid, signal.SIGKILL)
        proces.communicate()
        raise RuntimeError('Překročen časový limit; zkontrolujte podmínku smyčky.')
    return proces.returncode, vystup, chyba


def main():
    volby = argparse.ArgumentParser(description=__doc__)
    volby.add_argument('--prekladac', required=True, type=Path,
                       help='cesta k sestavenému minic, například build/minic')
    volby.add_argument('--vychozi', action='store_true',
                       help='ověřit nedoplněný start: dostupné fáze a očekávané TODO')
    volby.add_argument('--limit', type=int, default=30,
                       help='nejvyšší počet sekund na jeden příkaz (výchozí 30)')
    args = volby.parse_args()
    if args.limit <= 0:
        volby.error('--limit musí být kladný')
    koren = Path(__file__).resolve().parent
    minic = args.prekladac.resolve()
    if not minic.is_file():
        volby.error('Překladač neexistuje; nejprve proveďte sestavení podle README.md.')
    # Jeden společný skript ve všech balíčcích; etapu určuje příbalová evidence.
    metadata = json.loads((koren / 'balicek.json').read_text(encoding='utf-8'))
    blok = metadata['blok']
    pripady = json.loads((koren / 'priklady/cases.json').read_text(encoding='utf-8'))
    vsechny = {p.name for p in (koren / 'priklady').glob('*.mini')}
    if vsechny != {p['file'] for p in pripady}:
        raise RuntimeError('Seznam kontrol neodpovídá přiloženým příkladům.')
    chyby = 0
    if args.vychozi:
        prvni = next(p for p in pripady if p['exit'] == 0)
        zdroj = koren / 'priklady' / prvni['file']
        prikazy = ['tokens', 'ast', 'lower']
        if blok >= 3:
            prikazy.append('check')
        if blok == 4:
            prikazy.append('cpp')
        for faze in prikazy:
            kod, vystup, chyba = spustit([str(minic), faze, str(zdroj)], args.limit)
            uspel = kod == 0 and not chyba and (vystup == '' if faze == 'check' else bool(vystup))
            print(('OK' if uspel else 'CHYBA'), faze)
            chyby += not uspel
        faze = {2: 'check', 3: 'cpp', 4: 'run'}[blok]
        kod, vystup, chyba = spustit([str(minic), faze, str(zdroj)], args.limit)
        uspel = kod == 1 and not vystup and f'TODO BLOCK{blok:02d}' in chyba
        print(('OK' if uspel else 'CHYBA'), 'očekávaná nedoplněná část:', faze)
        chyby += not uspel
        print('Kontrola startu neprokazuje správnost řešení úlohy.')
    else:
        for pripad in pripady:
            zdroj = koren / 'priklady' / pripad['file']
            kod, vystup, chyba = spustit(
                [str(minic), pripad['command'], str(zdroj)], args.limit)
            jadro = pripad['stderr_contains']
            uspel = (kod == pripad['exit'] and vystup == pripad['stdout'] and
                     (jadro in chyba if jadro else chyba == ''))
            print(('OK' if uspel else 'CHYBA'), pripad['file'])
            if not uspel:
                print('  Návratový kód:', kod, '(očekáván', pripad['exit'], ')')
                print('  Očekávaný standardní výstup:', repr(pripad['stdout']))
                print('  Skutečný standardní výstup:', repr(vystup))
                print('  Očekávané jádro chyby:', repr(jadro))
                print('  Skutečná diagnostika:', repr(chyba))
            chyby += not uspel
        print('Kontrola přiložených příkladů nenahrazuje vysvětlení a vlastní testy.')
    print('Všechny kontroly prošly.' if not chyby else f'Počet neúspěšných kontrol: {chyby}')
    return 0 if not chyby else 1


if __name__ == '__main__':
    try:
        raise SystemExit(main())
    except (OSError, ValueError, RuntimeError) as chyba:
        print('Kontrolu nelze dokončit:', chyba)
        raise SystemExit(1)
