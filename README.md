# <img src="assets/scheduim-logo.png" alt="scheduim" width="440">

[Русский](#русский) · [English](#english)

## Русский

Schedium — небольшая C++23-библиотека для хранения расписаний и оценки их качества. Проект находится на ранней стадии разработки: сейчас доступны события, менеджеры расписаний и валидатор с бонусами за свободные промежутки, штрафами за пересечения и режимами EASY, MEDIUM, HARD. Автоматическое построение расписания и дополнительные ограничения для групп и преподавателей пока не реализованы.

### Требования

- Git, CMake **3.22+** и компилятор со стандартной библиотекой, поддерживающей используемые возможности C++23 (`std::chrono`, ranges).
- Windows: MSVC из Visual Studio 2022 с компонентом **Desktop development with C++**, либо MinGW-w64. Linux: GCC; CI использует Ubuntu 24.04. macOS в текущую матрицу CI не входит.
- Ninja — для команд с `-G Ninja` и локального запуска clang-tidy. Для обычной сборки можно использовать генератор CMake по умолчанию.
- Python и инструменты LLVM нужны только для проверок форматирования и статического анализа перед PR.

Сторонние C++-библиотеки и отдельный тестовый фреймворк не требуются. На Windows команды должны выполняться в среде выбранного компилятора: Developer PowerShell для MSVC или терминале с `bin` MinGW в `PATH`. CMake и Ninja также должны быть доступны в `PATH`.

### Получение и сборка

```sh
git clone https://github.com/Lappland-Julia/Schedium.git
cd Schedium
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DSCHEDIUM_BUILD_BENCHMARKS=OFF
cmake --build build --config Debug --parallel 2
```

Эти команды подходят для Bash и PowerShell. `--config Debug` выбирает конфигурацию в Visual Studio и других генераторах с несколькими конфигурациями; для Ninja/Makefiles конфигурацию задаёт `CMAKE_BUILD_TYPE` при настройке.

Параметры проекта:

| Параметр | По умолчанию | Назначение |
| --- | --- | --- |
| `BUILD_TESTING` | `ON` | Собрать и зарегистрировать `CMakeTests` в CTest. |
| `BUILD_SHARED_LIBS` | `OFF` | `ON` — динамическая библиотека; `OFF` — статическая. |
| `SCHEDIUM_BUILD_BENCHMARKS` | `ON` на Windows, иначе `OFF` | Собрать Windows-бенчмарк `SchediumBenchmarks`. |
| `CMAKE_BUILD_TYPE` | Не задан | Для Ninja/Makefiles задайте `Debug` или `Release`. |
| `SCHEDIUM_CLANG_FORMAT` | Автопоиск | Путь к clang-format для целей `format` и `format-check`. |
| `SCHEDIUM_CLANG_TIDY` | Автопоиск | Путь к clang-tidy; цель анализа требует Ninja/Makefiles. |

Для сборки только библиотеки:

```sh
cmake -S . -B build-library -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DSCHEDIUM_BUILD_BENCHMARKS=OFF
cmake --build build-library --config Release --target Schedium --parallel 2
```

Сейчас проект не предоставляет правил `cmake --install` и пакета для `find_package(Schedium)`. Подключайте исходники через `add_subdirectory` или `FetchContent`.

### Подключение к приложению

Пример структуры вашего проекта:

```text
my-app/
├── CMakeLists.txt
├── main.cpp
└── external/
    └── Schedium/        # клон этого репозитория
```

Из каталога `my-app`:

```sh
git clone https://github.com/Lappland-Julia/Schedium.git external/Schedium
```

`my-app/CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.22)
project(MyScheduleApp LANGUAGES CXX)

set(BUILD_TESTING OFF CACHE BOOL "Build dependency tests" FORCE)
set(SCHEDIUM_BUILD_BENCHMARKS OFF CACHE BOOL "Build Schedium benchmarks" FORCE)
add_subdirectory(external/Schedium)

add_executable(schedule-demo main.cpp)
target_link_libraries(schedule-demo PRIVATE Schedium)
```

Цель `Schedium` передаёт приложению путь к заголовкам и требование C++23. Здесь `BUILD_TESTING` отключён для всего примера; в проекте с собственными тестами настройте эту общую опцию под свои нужды.

Сохраните следующий пример как `main.cpp`:

```cpp
#include <Schedium.hpp>

#include <chrono>
#include <iostream>

int main() {
    using namespace std::chrono;

    const sys_seconds start = sys_days{year{2026} / October / 12} + hours{9};

    ScheduleManager schedule;
    schedule.set_name("Monday");
    schedule.add_schedule_entry(ScheduleEntry{"Math", start, minutes{45}, "Group A", "Room 101"});
    schedule.add_schedule_entry(
        ScheduleEntry{"Physics", start + minutes{45}, minutes{45}, "Group A", "Room 101"});

    const Validator initial{schedule};
    std::cout << std::boolalpha;
    std::cout << "Valid: " << initial.is_valid() << ", score: " << initial.score() << '\n';

    schedule.add_schedule_entry(
        ScheduleEntry{"Meeting", start + minutes{30}, minutes{10}, "Group B", "Room 101"});

    const Validator updated{schedule};
    std::cout << "Valid: " << updated.is_valid() << ", score: " << updated.score() << '\n';
}
```

Соберите и запустите из `my-app`:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug --parallel 2
```

Запуск: Linux — `./build/schedule-demo`; PowerShell с Ninja/MinGW — `./build/schedule-demo.exe`; с Visual Studio — `./build/Debug/schedule-demo.exe`.

Ожидаемый вывод:

```text
Valid: true, score: 0
Valid: false, score: -10000
```

### Как работает API

| Тип | Назначение |
| --- | --- |
| `ScheduleEntry` | Имя события, начало (`std::chrono::sys_seconds`), длительность (`std::chrono::minutes`), группа и помещение. |
| `ScheduleManager` | Добавление событий, поиск по имени, переименование и чтение расписания. |
| `Validator` | Проверка конфликтов одного или нескольких менеджеров через `is_valid()` и `score()`. |

- Длительность — от **1 до 6000 минут** включительно; недопустимое значение вызывает `std::invalid_argument`.
- Имена событий должны быть уникальны внутри менеджера. Дубликат, поиск отсутствующего имени и недопустимое переименование вызывают `std::runtime_error`.
- Обе перегрузки `set_schedule(...)` **добавляют** события к текущим. Векторная перегрузка не заменяет расписание; если одно добавление завершится исключением, ранее добавленные элементы останутся.
- Валидатор объединяет события по `place_name` из всех переданных менеджеров. `group_name` хранится в событии, но отдельная проверка занятости группы пока отсутствует. Пустые названия помещений тоже попадают в одну общую группу.
- Интервалы имеют вид `[начало, конец)`: следующее событие может начаться точно в момент окончания предыдущего. Проверка учитывает полную дату и секунды, в том числе переход через полночь. Преобразование локального времени и часовых поясов выполняет приложение.
- `Validator` хранит **копии** менеджеров. После изменения расписания создайте новый валидатор; изменение исходного менеджера не обновляет уже созданный.
- `score()` возвращает `std::int64_t`: сумма бонусов и отрицательных штрафов, больше — лучше. За каждую пересекающуюся пару в одном помещении добавляется `overlap_penalty`, по умолчанию **−10000**. Вложенные интервалы и одинаковое время начала учитываются полностью; порядок добавления событий не влияет на счёт.
- За каждый свободный промежуток в помещении **строго больше** `gap_duration` начисляется один `gap_score`, по умолчанию **+5** за промежуток больше **15 минут**. При перекрывающихся занятиях промежуток отсчитывается от самого позднего окончания. Длинный промежуток, в том числе через полночь, тоже даёт один бонус; между разными помещениями бонус не начисляется.
- `is_valid()` сравнивает `score()` с минимальным счётом выбранного режима. По умолчанию `HARD` требует **0**, `MEDIUM` — **−5000**, `EASY` — **−10000**. Равенство порогу допустимо; бонусы могут компенсировать штрафы. Пустое расписание имеет счёт `0` и тоже проверяется по порогу.
- Непредставимый конец интервала или переполнение при накоплении счёта вызывают `std::overflow_error` из `score()` и `is_valid()`.
- `get_entry()` возвращает ссылку на элемент менеджера. Храните менеджер живым и не удерживайте ссылку через добавления, способные перераспределить его вектор; при необходимости используйте копию: `auto entry = schedule.get_entry("Math");`.

### Настройка score

| Параметр | По умолчанию | Чтение и изменение |
| --- | --- | --- |
| Бонус за промежуток | `5` | `get_gap_score()`, `update_gap_score(value)`; значение неотрицательное. |
| Минимальная длительность промежутка | `std::chrono::minutes{15}` | `get_gap_duration()`, `update_gap_duration(value)`; длительность неотрицательная и должна представляться в секундах. |
| Штраф за пересекающуюся пару | `-10000` | `get_overlap_penalty()`, `update_overlap_penalty(value)`; значение неположительное. |
| Пороги EASY, MEDIUM, HARD | `-10000`, `-5000`, `0` | `update_min_scores(easy, medium, hard)`; требуется `easy <= medium <= hard`. `get_min_score()` возвращает порог выбранного режима. |
| Режим | `"MEDIUM"` | `get_validating_level()`, `set_validating_level("EASY" / "MEDIUM" / "HARD")`. |

Все значения счёта и порогов имеют тип `std::int64_t`. Нулевой бонус или штраф отключает соответствующий вклад. Недопустимая настройка вызывает `std::invalid_argument` и сохраняет прежнее значение.

```cpp
Validator validator{schedule};
validator.update_gap_duration(std::chrono::minutes{20});
validator.update_gap_score(10);
validator.update_overlap_penalty(-100);
validator.update_min_scores(-100, -50, 0);
validator.set_validating_level("HARD");

std::cout << "Score: " << validator.score()
          << ", required: " << validator.get_min_score()
          << ", valid: " << std::boolalpha << validator.is_valid() << '\n';
```

### Тесты

[tests/CMakeTests.cpp](tests/CMakeTests.cpp) содержит собственный небольшой runner: 48 сценариев проверяют события, менеджеры, исключения, владение данными, бонусы, штрафы, пороги режимов и границы времени и счёта. CTest видит их как **один** тест `CMakeTests`; число сценариев выводится внутри него.

Запуск после сборки с `BUILD_TESTING=ON`:

```sh
cmake --build build --config Debug --target CMakeTests --parallel 2
ctest --test-dir build -C Debug --tests-regex "^CMakeTests$" --verbose --output-on-failure --no-tests=error
```

Для текущего набора ожидаются `48/48 cases passed`, отсутствие `FAIL` и нулевой код завершения. `--no-tests=error` предотвращает ложный успех при отсутствии зарегистрированного теста. При падении смотрите сообщение с файлом, строкой и выражением, а также `build/Testing/Temporary/LastTest.log`.

### Подготовка Pull Request

1. Создайте fork, клонируйте его и заведите отдельную ветку, например `git switch -c fix/room-conflict`. Для прямого доступа к репозиторию fork не обязателен.
2. Делайте изменение с ясной целью. При исправлении ошибки добавьте сценарий, воспроизводящий её; при добавлении API проверьте обычные случаи, границы и исключения.
3. Добавляйте тестовую функцию и её запись в список `test::run(...)` в `tests/CMakeTests.cpp`. Если число сценариев изменилось, обновите последний аргумент runner (сейчас `48`) и подпись шага в `.github/workflows/tests.yml`; при необходимости обновите README. Runner намеренно завершается с ошибкой при несовпадении счётчика и списка.
4. Выполните тесты и проверки LLVM ниже. Для изменения поведения проверьте Debug/Release и `BUILD_SHARED_LIBS=OFF/ON`. CI повторит эту матрицу на обеих ОС.
5. Проверьте `git diff --check` и diff перед отправкой. Сохраняйте стиль из `.clang-format`; сборочные каталоги, бинарники и настройки IDE не включайте в PR.
6. В описании PR укажите проблему, итоговое поведение, изменения публичного API и команды с результатами проверки. Разделяйте выполненные и пропущенные проверки. После push проверьте оба workflow в GitHub и исправьте ошибки перед запросом review.

**Что проверяет CI**

| Workflow | Проверки |
| --- | --- |
| [Tests](.github/workflows/tests.yml) | 8 сочетаний: `ubuntu-24.04` / `windows-2022` × Debug / Release × static / shared. Собирает `CMakeTests`, запускает CTest; бенчмарк отключён. |
| [Clang tools](.github/workflows/clang.yml) | Ubuntu 24.04, Ninja, Debug: проверка конфигурации clang-tidy, `format-check`, сборка `Schedium`, `clang-tidy`. Тесты и бенчмарк отключены. |

Локальная матрица на вашей ОС, **Bash** (Ninja и компилятор должны быть доступны):

```bash
for config in Debug Release; do
    for shared in OFF ON; do
        dir="build-${config}-${shared}"
        cmake -S . -B "$dir" -G Ninja -DCMAKE_BUILD_TYPE="$config" -DBUILD_SHARED_LIBS="$shared" -DBUILD_TESTING=ON -DSCHEDIUM_BUILD_BENCHMARKS=OFF || exit 1
        cmake --build "$dir" --config "$config" --target CMakeTests --parallel 2 || exit 1
        ctest --test-dir "$dir" -C "$config" --tests-regex "^CMakeTests$" --verbose --output-on-failure --no-tests=error || exit 1
    done
done
```

То же для **PowerShell**:

```powershell
foreach ($config in @('Debug', 'Release')) {
    foreach ($shared in @('OFF', 'ON')) {
        $buildDir = "build-$config-$shared"
        cmake -S . -B $buildDir -G Ninja "-DCMAKE_BUILD_TYPE=$config" "-DBUILD_SHARED_LIBS=$shared" -DBUILD_TESTING=ON -DSCHEDIUM_BUILD_BENCHMARKS=OFF
        if ($LASTEXITCODE -ne 0) { throw 'Configure failed' }
        cmake --build $buildDir --config $config --target CMakeTests --parallel 2
        if ($LASTEXITCODE -ne 0) { throw 'Build failed' }
        ctest --test-dir $buildDir -C $config --tests-regex '^CMakeTests$' --verbose --output-on-failure --no-tests=error
        if ($LASTEXITCODE -ne 0) { throw 'Tests failed' }
    }
}
```

**Форматирование и статический анализ**

Для воспроизведения проверки Clang tools используйте Linux или WSL с GCC, Python 3 и `pip`. Следующие версии совпадают с закреплёнными в workflow:

```sh
python3 -m venv .venv
. .venv/bin/activate
python3 -m pip install 'clang-format==18.1.8' 'clang-tidy==18.1.8' 'cmake==4.2.3' 'ninja==1.13.2'
cmake -S . -B build-clang -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=OFF -DSCHEDIUM_BUILD_BENCHMARKS=OFF -DSCHEDIUM_CLANG_FORMAT=clang-format -DSCHEDIUM_CLANG_TIDY=clang-tidy
clang-tidy --verify-config
cmake --build build-clang --target format-check
cmake --build build-clang --target Schedium
cmake --build build-clang --target clang-tidy
git diff --check
```

Окружение `.venv` сейчас не исключено в `.gitignore`: не добавляйте его в коммит. Для автоматического исправления форматирования запустите `cmake --build build-clang --target format`, просмотрите diff и повторите `format-check`. Форматирование охватывает `src` и `tests`, а при включённых бенчмарках — также `benchmarks`. Цель `clang-tidy` анализирует `.cpp` библиотеки; предупреждения `clang-analyzer-*` считаются ошибками согласно `.clang-tidy`.

Пример описания PR:

```text
Проблема: <условие, при котором возникает ошибка>
Изменение: <наблюдаемое поведение после исправления>
API: <изменения совместимости или «без изменений»>
Проверка: <ОС, компилятор, конфигурации, команды и результаты>
Не выполнено: <проверки, которые не запускались, и причина>
```

### Бенчмарк и CLion

Бенчмарк использует Windows API и `Psapi`; на Linux его необходимо отключать. Он измеряет создание 1000 событий и отдельно построение менеджера, копирование в валидатор и `score()` в 50 повторениях. Для оценки есть два сценария: все события пересекаются либо разделены промежутками по 16 минут. Каждое повторение проверяет ожидаемый счёт (**−4995000000** и **4995**) и валидность вне измеряемого участка. Бенчмарк выводит время и память, не заменяет функциональные тесты и не входит в CI.

PowerShell на Windows:

```powershell
cmake -S . -B build-bench -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DSCHEDIUM_BUILD_BENCHMARKS=ON
cmake --build build-bench --config Release --target SchediumBenchmarks --parallel 2
```

Запуск с Ninja/MinGW: `./build-bench/SchediumBenchmarks.exe`; с Visual Studio: `./build-bench/Release/SchediumBenchmarks.exe`.

В CLion откройте корень проекта и выберите toolchain. В CMake options задайте нужные параметры, затем Reload CMake Project. Для функциональных тестов используйте `CMakeTests`, для бенчмарка — `SchediumBenchmarks`; цель `Schedium` собирает саму библиотеку.

### Частые проблемы

- **Компилятор/CMake/Ninja не найден:** проверьте `PATH` и выбранную среду компилятора. Для Visual Studio откройте Developer PowerShell.
- **Конфликт генератора или компилятора:** создайте новый каталог сборки; не переключайте Ninja и Visual Studio в одном CMake cache.
- **`No tests were found` / отсутствует `CMakeTests`:** повторно настройте проект с `BUILD_TESTING=ON`, соберите `CMakeTests` и выберите ту же конфигурацию через `-C`.
- **Отсутствуют `format-check` / `clang-tidy`:** установите инструменты до настройки CMake либо задайте их полные пути через `SCHEDIUM_CLANG_FORMAT` и `SCHEDIUM_CLANG_TIDY`. Для clang-tidy используйте Ninja/Makefiles: генератор Visual Studio не создаёт эту цель.
- **Не найдена DLL при запуске приложения:** для shared-сборки Windows разместите DLL Schedium рядом с приложением или добавьте её каталог в `PATH`; для MinGW также нужны runtime DLL компилятора. Тестовая цель проекта уже настраивает расположение своей DLL и путь MinGW.
- **`SchediumBenchmarks requires Windows`:** задайте `SCHEDIUM_BUILD_BENCHMARKS=OFF`.

### Лицензия

[MIT](LICENSE). При распространении сохраняйте текст лицензии и уведомление об авторских правах.

---

## English

Schedium is a small C++23 library for storing schedules and scoring their quality. It is at an early stage: events, schedule managers, and a validator with free-gap bonuses, overlap penalties, and EASY, MEDIUM, HARD levels are available. Automatic schedule generation and additional constraints for groups and teachers are not implemented yet.

### Requirements

- Git, CMake **3.22+**, and a compiler with a standard library supporting the C++23 features used here (`std::chrono`, ranges).
- Windows: Visual Studio 2022 MSVC with **Desktop development with C++**, or MinGW-w64. Linux: GCC; CI uses Ubuntu 24.04. macOS is outside the current CI matrix.
- Ninja for commands using `-G Ninja` and local clang-tidy checks. Ordinary builds can use CMake's default generator.
- Python and LLVM tools are needed only for formatting and static analysis before a PR.

No third-party C++ libraries or separate test framework are required. On Windows, use the selected compiler's environment: Developer PowerShell for MSVC, or a terminal with MinGW's `bin` directory in `PATH`. CMake and Ninja must also be available in `PATH`.

### Download and build

```sh
git clone https://github.com/Lappland-Julia/Schedium.git
cd Schedium
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DSCHEDIUM_BUILD_BENCHMARKS=OFF
cmake --build build --config Debug --parallel 2
```

These commands work in Bash and PowerShell. `--config Debug` selects the configuration for Visual Studio and other multi-configuration generators; Ninja/Makefiles use `CMAKE_BUILD_TYPE` at configure time.

Project options:

| Option | Default | Purpose |
| --- | --- | --- |
| `BUILD_TESTING` | `ON` | Build `CMakeTests` and register it with CTest. |
| `BUILD_SHARED_LIBS` | `OFF` | `ON` builds a shared library; `OFF` builds a static library. |
| `SCHEDIUM_BUILD_BENCHMARKS` | `ON` on Windows, otherwise `OFF` | Build the Windows-only `SchediumBenchmarks` executable. |
| `CMAKE_BUILD_TYPE` | Unset | Set `Debug` or `Release` for Ninja/Makefiles. |
| `SCHEDIUM_CLANG_FORMAT` | Auto-detected | clang-format path for the `format` and `format-check` targets. |
| `SCHEDIUM_CLANG_TIDY` | Auto-detected | clang-tidy path; the analysis target requires Ninja/Makefiles. |

To build only the library:

```sh
cmake -S . -B build-library -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DSCHEDIUM_BUILD_BENCHMARKS=OFF
cmake --build build-library --config Release --target Schedium --parallel 2
```

The project currently provides neither `cmake --install` rules nor a `find_package(Schedium)` package. Integrate the source with `add_subdirectory` or `FetchContent`.

### Integrate with an application

Example application layout:

```text
my-app/
├── CMakeLists.txt
├── main.cpp
└── external/
    └── Schedium/        # clone of this repository
```

From `my-app`:

```sh
git clone https://github.com/Lappland-Julia/Schedium.git external/Schedium
```

`my-app/CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.22)
project(MyScheduleApp LANGUAGES CXX)

set(BUILD_TESTING OFF CACHE BOOL "Build dependency tests" FORCE)
set(SCHEDIUM_BUILD_BENCHMARKS OFF CACHE BOOL "Build Schedium benchmarks" FORCE)
add_subdirectory(external/Schedium)

add_executable(schedule-demo main.cpp)
target_link_libraries(schedule-demo PRIVATE Schedium)
```

The `Schedium` target propagates its header search path and C++23 requirement. This example disables `BUILD_TESTING` globally; adjust this shared option if your application has its own tests.

Save the following as `main.cpp`:

```cpp
#include <Schedium.hpp>

#include <chrono>
#include <iostream>

int main() {
    using namespace std::chrono;

    const sys_seconds start = sys_days{year{2026} / October / 12} + hours{9};

    ScheduleManager schedule;
    schedule.set_name("Monday");
    schedule.add_schedule_entry(ScheduleEntry{"Math", start, minutes{45}, "Group A", "Room 101"});
    schedule.add_schedule_entry(
        ScheduleEntry{"Physics", start + minutes{45}, minutes{45}, "Group A", "Room 101"});

    const Validator initial{schedule};
    std::cout << std::boolalpha;
    std::cout << "Valid: " << initial.is_valid() << ", score: " << initial.score() << '\n';

    schedule.add_schedule_entry(
        ScheduleEntry{"Meeting", start + minutes{30}, minutes{10}, "Group B", "Room 101"});

    const Validator updated{schedule};
    std::cout << "Valid: " << updated.is_valid() << ", score: " << updated.score() << '\n';
}
```

Build from `my-app`:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug --parallel 2
```

Run on Linux with `./build/schedule-demo`; in PowerShell with Ninja/MinGW, use `./build/schedule-demo.exe`; with Visual Studio, use `./build/Debug/schedule-demo.exe`.

Expected output:

```text
Valid: true, score: 0
Valid: false, score: -10000
```

### API behavior

| Type | Purpose |
| --- | --- |
| `ScheduleEntry` | Event name, start (`std::chrono::sys_seconds`), duration (`std::chrono::minutes`), group, and room. |
| `ScheduleManager` | Add entries, look them up by name, rename them, and read the schedule. |
| `Validator` | Check conflicts across one or more managers using `is_valid()` and `score()`. |

- Duration must be **1–6000 minutes**, inclusive; invalid values throw `std::invalid_argument`.
- Entry names must be unique within a manager. Duplicate names, missing lookups, and invalid renames throw `std::runtime_error`.
- Both `set_schedule(...)` overloads **append** entries. The vector overload does not replace the schedule; if an insertion throws, entries added earlier in the call remain.
- The validator groups entries by `place_name` across all supplied managers. `group_name` is stored, but group availability is not checked separately yet. Empty room names also share a single group.
- Intervals are `[start, end)`: an entry may start exactly when the previous entry ends. Validation uses the full date and seconds, including intervals crossing midnight. Local-time and time-zone conversion belongs to the application.
- `Validator` stores **copies** of managers. Create a new validator after changing a schedule; modifying the original manager does not update an existing validator.
- `score()` returns `std::int64_t`: the sum of bonuses and negative penalties; higher is better. Every overlapping pair in a room adds `overlap_penalty`, which defaults to **−10000**. Nested intervals and equal start times are fully counted, and insertion order does not affect the score.
- Each free gap in a room **strictly longer** than `gap_duration` earns one `gap_score`: by default, **+5** for a gap longer than **15 minutes**. With overlapping entries, the gap starts at the latest end time. A long gap, including one across midnight, still earns one bonus; entries in different rooms do not earn a shared gap bonus.
- `is_valid()` compares `score()` with the selected level's minimum score. Defaults are **0** for `HARD`, **−5000** for `MEDIUM`, and **−10000** for `EASY`. Equality is accepted, and bonuses can compensate for penalties. Empty schedules score `0` and are also checked against the threshold.
- An unrepresentable interval end or overflow while accumulating the score throws `std::overflow_error` from `score()` and `is_valid()`.
- `get_entry()` returns a reference into the manager. Keep the manager alive and do not retain the reference across additions that may reallocate its vector; copy when needed: `auto entry = schedule.get_entry("Math");`.

### Configure score

| Parameter | Default | Read and update |
| --- | --- | --- |
| Gap bonus | `5` | `get_gap_score()`, `update_gap_score(value)`; must be nonnegative. |
| Minimum gap duration | `std::chrono::minutes{15}` | `get_gap_duration()`, `update_gap_duration(value)`; must be nonnegative and representable in seconds. |
| Penalty per overlapping pair | `-10000` | `get_overlap_penalty()`, `update_overlap_penalty(value)`; must be nonpositive. |
| EASY, MEDIUM, HARD thresholds | `-10000`, `-5000`, `0` | `update_min_scores(easy, medium, hard)`; requires `easy <= medium <= hard`. `get_min_score()` returns the selected level's threshold. |
| Level | `"MEDIUM"` | `get_validating_level()`, `set_validating_level("EASY" / "MEDIUM" / "HARD")`. |

All score values and thresholds use `std::int64_t`. A zero bonus or penalty disables that contribution. Invalid settings throw `std::invalid_argument` and preserve the previous value.

```cpp
Validator validator{schedule};
validator.update_gap_duration(std::chrono::minutes{20});
validator.update_gap_score(10);
validator.update_overlap_penalty(-100);
validator.update_min_scores(-100, -50, 0);
validator.set_validating_level("HARD");

std::cout << "Score: " << validator.score()
          << ", required: " << validator.get_min_score()
          << ", valid: " << std::boolalpha << validator.is_valid() << '\n';
```

### Tests

[tests/CMakeTests.cpp](tests/CMakeTests.cpp) contains a small custom runner with 48 cases covering entries, managers, exceptions, ownership, bonuses, penalties, level thresholds, and time and score boundaries. CTest sees these as **one** test named `CMakeTests`; the internal runner prints the case count.

Run after configuring with `BUILD_TESTING=ON`:

```sh
cmake --build build --config Debug --target CMakeTests --parallel 2
ctest --test-dir build -C Debug --tests-regex "^CMakeTests$" --verbose --output-on-failure --no-tests=error
```

For the current suite, expect `48/48 cases passed`, no `FAIL` messages, and exit code zero. `--no-tests=error` prevents a false success when no test is registered. For failures, inspect the file, line, and expression in the message, and `build/Testing/Temporary/LastTest.log`.

### Preparing a Pull Request

1. Fork and clone the repository, then create a dedicated branch, for example `git switch -c fix/room-conflict`. A fork is optional if you have direct repository access.
2. Keep the change focused. For a bug fix, add a case reproducing the failure; for new API, cover ordinary behavior, boundaries, and exceptions.
3. Add the test function and its registration in `test::run(...)` in `tests/CMakeTests.cpp`. If the case count changes, update the runner's final argument (currently `48`) and the step label in `.github/workflows/tests.yml`; update the README where needed. The runner deliberately fails if its expected count differs from the registered list.
4. Run the tests and LLVM checks below. For behavior changes, check Debug/Release with `BUILD_SHARED_LIBS=OFF/ON`. CI repeats this matrix on both operating systems.
5. Run `git diff --check` and inspect the diff before pushing. Follow `.clang-format`; exclude build directories, binaries, and IDE settings from the PR.
6. Describe the problem, resulting behavior, public API changes, and validation commands with results. Distinguish completed checks from skipped checks. After pushing, check both GitHub workflows and address failures before requesting review.

**CI checks**

| Workflow | Checks |
| --- | --- |
| [Tests](.github/workflows/tests.yml) | 8 combinations: `ubuntu-24.04` / `windows-2022` × Debug / Release × static / shared. Builds `CMakeTests` and runs CTest; benchmarks are disabled. |
| [Clang tools](.github/workflows/clang.yml) | Ubuntu 24.04, Ninja, Debug: clang-tidy configuration validation, `format-check`, `Schedium` build, and `clang-tidy`. Tests and benchmarks are disabled. |

Local matrix for your OS, **Bash** (Ninja and a compiler must be available):

```bash
for config in Debug Release; do
    for shared in OFF ON; do
        dir="build-${config}-${shared}"
        cmake -S . -B "$dir" -G Ninja -DCMAKE_BUILD_TYPE="$config" -DBUILD_SHARED_LIBS="$shared" -DBUILD_TESTING=ON -DSCHEDIUM_BUILD_BENCHMARKS=OFF || exit 1
        cmake --build "$dir" --config "$config" --target CMakeTests --parallel 2 || exit 1
        ctest --test-dir "$dir" -C "$config" --tests-regex "^CMakeTests$" --verbose --output-on-failure --no-tests=error || exit 1
    done
done
```

Equivalent **PowerShell** commands:

```powershell
foreach ($config in @('Debug', 'Release')) {
    foreach ($shared in @('OFF', 'ON')) {
        $buildDir = "build-$config-$shared"
        cmake -S . -B $buildDir -G Ninja "-DCMAKE_BUILD_TYPE=$config" "-DBUILD_SHARED_LIBS=$shared" -DBUILD_TESTING=ON -DSCHEDIUM_BUILD_BENCHMARKS=OFF
        if ($LASTEXITCODE -ne 0) { throw 'Configure failed' }
        cmake --build $buildDir --config $config --target CMakeTests --parallel 2
        if ($LASTEXITCODE -ne 0) { throw 'Build failed' }
        ctest --test-dir $buildDir -C $config --tests-regex '^CMakeTests$' --verbose --output-on-failure --no-tests=error
        if ($LASTEXITCODE -ne 0) { throw 'Tests failed' }
    }
}
```

**Formatting and static analysis**

To reproduce Clang tools checks, use Linux or WSL with GCC, Python 3, and `pip`. These versions match those pinned in the workflow:

```sh
python3 -m venv .venv
. .venv/bin/activate
python3 -m pip install 'clang-format==18.1.8' 'clang-tidy==18.1.8' 'cmake==4.2.3' 'ninja==1.13.2'
cmake -S . -B build-clang -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=OFF -DSCHEDIUM_BUILD_BENCHMARKS=OFF -DSCHEDIUM_CLANG_FORMAT=clang-format -DSCHEDIUM_CLANG_TIDY=clang-tidy
clang-tidy --verify-config
cmake --build build-clang --target format-check
cmake --build build-clang --target Schedium
cmake --build build-clang --target clang-tidy
git diff --check
```

The `.venv` directory is currently not excluded by `.gitignore`: keep it out of your commits. To apply formatting automatically, run `cmake --build build-clang --target format`, inspect the diff, and repeat `format-check`. Formatting covers `src` and `tests`, plus `benchmarks` when enabled. The `clang-tidy` target analyzes the library's `.cpp` files; `.clang-tidy` treats `clang-analyzer-*` warnings as errors.

Example PR description:

```text
Problem: <conditions that trigger the issue>
Change: <observable behavior after the fix>
API: <compatibility changes or "unchanged">
Validation: <OS, compiler, configurations, commands, and results>
Not run: <skipped checks and reasons>
```

### Benchmark and CLion

The benchmark uses Windows APIs and `Psapi`; disable it on Linux. It measures creation of 1000 entries and, separately, manager population, validator copying, and `score()` over 50 repetitions. Scoring has two scenarios: all entries overlap, or entries are separated by 16-minute gaps. Each repetition checks the expected score (**−4995000000** and **4995**) and validity outside the timed section. It reports time and memory, does not replace functional tests, and is not part of CI.

On Windows, in PowerShell:

```powershell
cmake -S . -B build-bench -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DSCHEDIUM_BUILD_BENCHMARKS=ON
cmake --build build-bench --config Release --target SchediumBenchmarks --parallel 2
```

Run with Ninja/MinGW using `./build-bench/SchediumBenchmarks.exe`; with Visual Studio, use `./build-bench/Release/SchediumBenchmarks.exe`.

In CLion, open the project root and select a toolchain. Set the required CMake options, then Reload CMake Project. Choose `CMakeTests` for functional tests or `SchediumBenchmarks` for the benchmark; the `Schedium` target builds the library itself.

### Troubleshooting

- **Compiler/CMake/Ninja not found:** check `PATH` and the compiler environment. For Visual Studio, open Developer PowerShell.
- **Generator or compiler mismatch:** use a new build directory; do not switch between Ninja and Visual Studio in the same CMake cache.
- **`No tests were found` / missing `CMakeTests`:** reconfigure with `BUILD_TESTING=ON`, build `CMakeTests`, and select the matching configuration with `-C`.
- **Missing `format-check` / `clang-tidy`:** install the tools before configuring or provide their full paths using `SCHEDIUM_CLANG_FORMAT` and `SCHEDIUM_CLANG_TIDY`. Use Ninja/Makefiles for clang-tidy: the Visual Studio generator does not create that target.
- **Application cannot find a DLL:** for Windows shared builds, place Schedium's DLL beside the application or add its directory to `PATH`; MinGW also requires its compiler runtime DLLs. The project's test target already configures DLL placement and the MinGW runtime path.
- **`SchediumBenchmarks requires Windows`:** set `SCHEDIUM_BUILD_BENCHMARKS=OFF`.

### License

[MIT](LICENSE). Preserve the license text and copyright notice when redistributing.
