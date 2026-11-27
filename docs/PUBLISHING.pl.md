# Przygotowanie dwóch powiązanych PR-ów

## Repozytoria

| Część | Fork roboczy | Cel PR-a |
| --- | --- | --- |
| Interfejs | [Bezimienny111/SpacerNET_Interface](https://github.com/Bezimienny111/SpacerNET_Interface) | [postm1/SpacerNET_Interface](https://github.com/postm1/SpacerNET_Interface), `master` |
| Backend | [Bezimienny111/SpacerNET_Union](https://github.com/Bezimienny111/SpacerNET_Union) | [postm1/SpacerNET_Union](https://github.com/postm1/SpacerNET_Union), `master` |

W obecnych lokalnych checkoutach `origin` wskazuje repozytorium autora, a `fork` — własne repozytorium.
Sprawdź to przez `git remote -v` w każdym katalogu. Nie trzeba zmieniać nazw istniejących zdalnych repozytoriów.
Poniższe polecenia opisują przyszłą publikację; sama obecność tego pliku niczego nie wysyła na GitHuba.
Odnośniki między dokumentami obu forków wskazują gałąź `feature/group-selection`.
Udostępnią dokumenty po wysłaniu obu gałęzi pod tą nazwą; wcześniej pliki są dostępne lokalnie.

## Zapis zmian i wysłanie gałęzi

Po sprawdzeniu nowej pary DLL w grze, w katalogu `SpacerNET_Interface`:

```powershell
git switch -c feature/group-selection
git add README.md docs SpacerNET_Interface tests
git diff --cached --stat
git commit -m "Add group-selection controls and Objects tree selection"
git push -u fork feature/group-selection
```

Następnie w katalogu `SpacerNET_Union`:

```powershell
git switch -c feature/group-selection
git add README.md docs SpacerNET_Union tests
git diff --cached --stat
git commit -m "Add group transforms and Objects tree selection API"
git push -u fork feature/group-selection
```

Jeśli gałąź już istnieje, użyj `git switch feature/group-selection`.
Przed commitem przejrzyj staged diff. Powinien obejmować źródła, konfigurację, dokumentację i testy funkcji.
DLL-e, katalogi `bin`/`obj`, SDK i lokalny katalog `.compat` nie są częścią zmian źródłowych do PR-a.
Wysłanie na własny fork wymaga zalogowanego konta z prawem zapisu do niego.

## Otwieranie PR-ów w przeglądarce

W każdym oryginalnym repozytorium wybierz **Pull requests → New pull request → compare across forks**:

| Pole | Wartość |
| --- | --- |
| Base repository | Odpowiednie repozytorium `postm1` |
| Base branch | `master` |
| Head repository | Odpowiedni fork `Bezimienny111` |
| Compare branch | `feature/group-selection` |

Tytuły i angielskie opisy są przygotowane w `docs/PULL_REQUEST.md` każdego repozytorium.
Po utworzeniu obu PR-ów dodaj w każdym bezpośredni link do drugiego i uzupełnij wynik ręcznych prób.
Obie części wymagają wspólnego wydania DLL; samo scalanie dwóch repozytoriów nie jest operacją atomową.

Gałęzie można opublikować jako draft, gdy trwają jeszcze sprawdzenia na rzeczywistym silniku.
[Oficjalna instrukcja GitHub](https://docs.github.com/en/pull-requests/how-tos/create-pull-requests/creating-a-pull-request-from-a-fork)
opisuje wybór gałęzi bazowej i porównywanej oraz tryb draft.

## Zakres i wyniki do przekazania autorowi

- Nowe okno grupy, 22 filtry, wybór w Objects/Fast access, przesuwanie, obrót i cofanie.
- Ochrona zaznaczenia przy powrocie do widoku i aktualizacja obiektów zależnych/hierarchii.
- Zachowane .NET Framework 4.0, v140_xp i Union SDK 1.0l; opcjonalne własne ścieżki SDK.
- 119 scenariuszy backendu i 70 sprawdzeń interfejsu oraz jawnie opisane [pozostałe próby](VALIDATION.md).
- Poprawka max-FPS pochodzi już z oryginalnej gałęzi bazowej; nie jest częścią naszego wkładu.
